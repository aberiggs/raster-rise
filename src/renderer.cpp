#include "renderer.hpp"
#include "primitives.hpp"
#include "utils/timer.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace {

std::vector<Vec3i> vertices_in_screen_space(const std::vector<Vec3f>& object_vertices, const Vec2i& window_size,
                                            float zoom) {
    // TODO: These transformations should probably be handled somewhere else (maybe a camera?)
    std::vector<Vec3i> vertices_in_camera_space;
    vertices_in_camera_space.reserve(object_vertices.size());
    for (const auto& vertex : object_vertices) {
        // Ensure that vertices in the range [0, 1] are mapped to the range [0, min(width, height) - 1]
        // It has to be -1 because of the 0-based indexing
        const float scale = zoom * (std::min(window_size.x(), window_size.y()) - 1);
        // Scale vertices to fit screen
        auto v = (vertex + Vec3f({1.0, 1.0, 1.0})) / 2;
        // Please stay in the range [0, 1] :D
        assert(v.x() >= 0 && v.x() <= 1);
        assert(v.y() >= 0 && v.y() <= 1);
        v = v * scale;
        // Flip y to match screen coordinates (where the origin is at the top-left corner instead of the bottom-left)
        v = Vec3i({static_cast<int>(v.x()), window_size.y() - 1 - static_cast<int>(v.y()), static_cast<int>(v.z())});

        vertices_in_camera_space.emplace_back(v);
    }

    return vertices_in_camera_space;
}

} // namespace

void Renderer::draw(const Model& model, const Camera& camera, FrameBuffer& frame_buffer, Mode mode) {
    Timer timer("Renderer::draw"); // For profiling

    const float zoom = 0.9f; // TODO: Revisit. This should probably be something like scale.

    // Until we implement backface culling, we'll just draw the faces according to their max z values
    std::vector sorted_faces{model.faces()};
    std::vector<Vec3f> model_space_vertices = model.vertices();
    std::sort(sorted_faces.begin(), sorted_faces.end(),
              [&vertices = model_space_vertices](const Model::Face& a, const Model::Face& b) {
                  auto max_z_a = std::max({vertices[a[0]].z(), vertices[a[1]].z(), vertices[a[2]].z()});
                  auto max_z_b = std::max({vertices[b[0]].z(), vertices[b[1]].z(), vertices[b[2]].z()});

                  return max_z_a < max_z_b;
              });

    // TODO: Complete entire sequence of coordinate transformations
    // 1. Convert to world space
    // 2. Convert to view space (camera space)
    // 3. Convert to clip space
    // 4. Convert to normalized device coordinates (NDC)
    // 5. Convert to screen space

    auto screen_space_vertices = vertices_in_screen_space(model_space_vertices, frame_buffer.size(), zoom);

    for (const auto& face : sorted_faces) {
        Vec3i v0_screen = screen_space_vertices[face[0]];
        Vec3i v1_screen = screen_space_vertices[face[1]];
        Vec3i v2_screen = screen_space_vertices[face[2]];

        switch (mode) {
            case Mode::Wireframe: {

                Color3 color = Colors::green;
                draw_triangle(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            case Mode::Shaded: {

                // Calculate the normal of the face
                Vec3f v0 = model_space_vertices[face[0]];
                Vec3f v1 = model_space_vertices[face[1]];
                Vec3f v2 = model_space_vertices[face[2]];

                Vec3f normal = (v1 - v0).cross(v2 - v0);
                Vec3f unit_normal = normal.unit();

                // Calculate the light intensity based on the angle between the normal and the light direction
                Vec3f light_direction = Vec3f({1.f, 1.f, 1.f}).unit(); // Light points into the screen
                const float intensity = std::max(0.01f, unit_normal.dot(light_direction)); // Some ambient light

                // Use the intensity to shade the color
                Color3 color = {intensity, intensity, intensity};

                draw_triangle_filled(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            case Mode::Rainbow: {

                // Pick a random color
                // TODO: Consider fixing this awful RNG
                float r = static_cast<float>(rand()) / RAND_MAX;
                float g = static_cast<float>(rand()) / RAND_MAX;
                float b = static_cast<float>(rand()) / RAND_MAX;

                // Factor that washes out the colors
                constexpr float pastelization_factor = 1.3;
                r = (r + (pastelization_factor - 1)) / pastelization_factor;
                g = (g + (pastelization_factor - 1)) / pastelization_factor;
                b = (b + (pastelization_factor - 1)) / pastelization_factor;

                Color3 color{r, g, b};

                draw_triangle_filled(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            default: {
                throw std::invalid_argument("Invalid renderer mode");
            }
        }
    }
}