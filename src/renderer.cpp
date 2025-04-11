#include "renderer.hpp"
#include "primitives.hpp"
#include "types/matrix.hpp"
#include "utils/timer.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace {

Matrix4x4f look_at(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (center - eye).unit();
    Vec3f x = z.cross(up).unit();
    Vec3f y = x.cross(z);

    auto view = Matrix4x4f{{
        Vec4f({x.x(), y.x(), z.x(), -x.dot(eye)}),
        Vec4f({x.y(), y.y(), z.y(), -y.dot(eye)}),
        Vec4f({x.z(), y.z(), -z.z(), -z.dot(eye)}),
        Vec4f({0, 0, 0, 1}),
    }};

    return view;
}

Matrix4x4f perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / std::tan(fov / 2.0f);
    return Matrix4x4f({
        Vec4f({f / aspect, 0, 0, 0}),
        Vec4f({0, f, 0, 0}),
        Vec4f({0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)}),
        Vec4f({0, 0, -1, 0}),
    });
}

std::vector<Vec3f> apply_vertex_shader(const std::vector<Vec3f>& object_vertices, const Vec2i& window_size) {
    std::vector<Vec3f> ndc_vertices{};
    ndc_vertices.reserve(object_vertices.size());
    for (const auto& vertex : object_vertices) {
        // 1. Convert to world space
        // Put at origin
        Matrix4x4f transform_matrix = Matrix4x4f::identity();
        // 2. Convert to view space (camera space)
        Matrix4x4f view_matrix = look_at(Vec3f({-1.f, 0.f, -3.f}), Vec3f({0.f, 0.f, 0.f}), Vec3f({0.f, 1.f, 0.f}));
        // 3. Convert to clip space
        Matrix4x4f projection_matrix =
            perspective(45.0f, static_cast<float>(window_size.x()) / window_size.y(), 0.1f, 100.f);

        Matrix<float, 4, 1> vertex_matrix =
            std::array{std::array{vertex.x()}, std::array{vertex.y()}, std::array{vertex.z()}, std::array{1.f}};

        Matrix<float, 4, 1> clip_matrix = projection_matrix * view_matrix * transform_matrix * vertex_matrix;

        Vec4f clip = Vec4f({clip_matrix.at(0, 0), clip_matrix.at(1, 0), clip_matrix.at(2, 0), clip_matrix.at(3, 0)});

        // 4. Convert to normalized device coordinates (NDC)
        Vec3f ndc = Vec3f({clip.x(), clip.y(), clip.z()}) / clip.w();

        ndc_vertices.emplace_back(ndc);
    }

    return ndc_vertices;
}

} // namespace

void Renderer::draw(const Model& model, const Camera& camera, FrameBuffer& frame_buffer, Mode mode) {
    Timer timer("Renderer::draw"); // For profiling

    std::vector<Vec3f> model_vertices = model.vertices();
    std::vector<Vec3f> ndc_vertices = apply_vertex_shader(model_vertices, frame_buffer.size());

    // For simplicity, we'll just draw the closer faces on top of the farther ones
    std::vector sorted_faces{model.faces()};
    std::sort(sorted_faces.begin(), sorted_faces.end(),
              [&vertices = ndc_vertices](const Model::Face& a, const Model::Face& b) {
                  auto max_z_a = std::max({vertices[a[0]].z(), vertices[a[1]].z(), vertices[a[2]].z()});
                  auto max_z_b = std::max({vertices[b[0]].z(), vertices[b[1]].z(), vertices[b[2]].z()});

                  return max_z_a < max_z_b;
              });

    for (const auto& face : sorted_faces) {

        // Convert to screen space
        int x0 = static_cast<int>((ndc_vertices[face[0]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y0 = static_cast<int>((ndc_vertices[face[0]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v0_screen({x0, y0, 0});

        int x1 = static_cast<int>((ndc_vertices[face[1]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y1 = static_cast<int>((ndc_vertices[face[1]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v1_screen({x1, y1, 0});

        int x2 = static_cast<int>((ndc_vertices[face[2]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y2 = static_cast<int>((ndc_vertices[face[2]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v2_screen({x2, y2, 0});

        switch (mode) {
            case Mode::Wireframe: {

                Color3 color = Colors::green;
                draw_triangle(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            case Mode::Shaded: {

                // Calculate the normal of the face
                Vec3f v0 = model_vertices[face[0]];
                Vec3f v1 = model_vertices[face[1]];
                Vec3f v2 = model_vertices[face[2]];

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