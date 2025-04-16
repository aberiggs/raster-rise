#include "renderer.hpp"
#include "primitives.hpp"
#include "types/matrix.hpp"
#include "utils/timer.hpp"

#include <algorithm>
#include <future>
#include <vector>

namespace {

template <typename F> void async_for(std::size_t start, std::size_t end, F func) {
    std::vector<std::future<void>> futures{};
    // TODO: Re-enable when z-buffer is implemented
    constexpr bool parallelize = false;
    const std::size_t num_threads = parallelize ? std::thread::hardware_concurrency() : 1;
    const std::size_t chunk_size = (end - start) / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        std::size_t chunk_start = start + t * chunk_size;
        std::size_t chunk_end = (t == num_threads - 1) ? end : chunk_start + chunk_size;

        futures.emplace_back(std::async(std::launch::async, [&, chunk_start, chunk_end]() {
            for (std::size_t i = chunk_start; i < chunk_end; ++i) {
                func(i);
            }
        }));
    }

    for (auto& future : futures) {
        future.get();
    }
}

// TODO: Reorganize
std::vector<Vec4f> to_view_space(const std::vector<Vec3f>& object_vertices, const Matrix4x4f& transform_mat,
                                 const Matrix4x4f& view_mat) {

    Timer timer("Convert to View Space"); // For profiling

    std::vector<Vec4f> view_space_vertices{};
    view_space_vertices.resize(object_vertices.size());
    auto task = [&](std::size_t i) {
        const auto& vertex = object_vertices[i];
        Matrix<float, 4, 1> vertex_matrix{vertex.x(), vertex.y(), vertex.z(), 1.f};
        Matrix<float, 4, 1> view_space = view_mat * vertex_matrix;
        view_space_vertices[i] = view_space.col(0);
    };

    async_for(0, object_vertices.size(), task);

    return view_space_vertices;
}

// TODO: Reorganize
std::vector<Vec3f> apply_vertex_shader(const std::vector<Vec4f>& view_space, const Matrix4x4f& projection_mat) {

    Timer timer("Apply Vertex Shader"); // For profiling

    std::vector<Vec3f> ndc_vertices{};
    ndc_vertices.resize(view_space.size());
    auto task = [&](std::size_t i) {
        // 3. Convert to clip space
        Matrix<float, 4, 1> view_space_mat{view_space[i].x(), view_space[i].y(), view_space[i].z(), view_space[i].w()};
        Matrix<float, 4, 1> clip_matrix = projection_mat * view_space_mat;
        Vec4f clip_space = clip_matrix.col(0);

        // 4. Convert to normalized device coordinates (NDC)
        Vec3f ndc = Vec3f{clip_space.x(), clip_space.y(), clip_space.z()} / clip_space.w();

        ndc_vertices[i] = std::move(ndc);
    };

    async_for(0, view_space.size(), task);

    return ndc_vertices;
}

} // namespace

void Renderer::draw(const Model& model, const Camera& camera, FrameBuffer& frame_buffer, Mode mode) {
    Timer timer("Renderer::draw"); // For profiling

    float aspect_ratio = static_cast<float>(frame_buffer.width()) / static_cast<float>(frame_buffer.height());

    std::vector<Vec3f> model_vertices = model.vertices();

    // Put at origin until object transforms are implemented
    constexpr auto transform_matrix = Matrix4x4f::identity();
    // 1. Transform to view space
    std::vector<Vec4f> view_space_vertices = to_view_space(model_vertices, transform_matrix, camera.view_matrix());
    // 2. Transform to normalized device coordinates (NDC)
    std::vector<Vec3f> ndc_vertices = apply_vertex_shader(view_space_vertices, camera.projection_matrix(aspect_ratio));

    // For simplicity, we'll just draw the closer faces on top of the farther ones (Painters algorithm)
    std::vector sorted_faces{model.faces()};
    std::sort(sorted_faces.begin(), sorted_faces.end(),
              [&vertices = ndc_vertices](const Model::Face& a, const Model::Face& b) {
                  auto max_z_a = std::max({vertices[a[0]].z(), vertices[a[1]].z(), vertices[a[2]].z()});
                  auto max_z_b = std::max({vertices[b[0]].z(), vertices[b[1]].z(), vertices[b[2]].z()});

                  return max_z_a < max_z_b;
              });

    auto task = [&](std::size_t i) {
        const auto& face = sorted_faces[i];

        // Get the vertices of the triangle in view space
        Vec3f v0_view{view_space_vertices[face[0]]};
        Vec3f v1_view{view_space_vertices[face[1]]};
        Vec3f v2_view{view_space_vertices[face[2]]};

        // Calculate the normal of the face - flip since we're in a left-handed coordinate system
        Vec3f normal = (v1_view - v0_view).cross(v2_view - v0_view) * -1.f;

        if (normal.z() < 0) {
            // Cull the backface
            return;
        }

        // Convert to screen space
        int x0 = static_cast<int>((ndc_vertices[face[0]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y0 = static_cast<int>((ndc_vertices[face[0]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v0_screen{x0, y0, 0};

        int x1 = static_cast<int>((ndc_vertices[face[1]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y1 = static_cast<int>((ndc_vertices[face[1]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v1_screen{x1, y1, 0};

        int x2 = static_cast<int>((ndc_vertices[face[2]].x() + 1.0f) * 0.5f * frame_buffer.width());
        int y2 = static_cast<int>((ndc_vertices[face[2]].y() + 1.0f) * 0.5f * frame_buffer.height());
        Vec3i v2_screen{x2, y2, 0};

        switch (mode) {
            case Mode::Wireframe: {

                Color3 color = Colors::green;
                draw_triangle(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            case Mode::Shaded: {
                Vec3f unit_normal = normal.unit();

                // Calculate the light intensity based on the angle between the normal and the light direction
                Vec3f light_direction = Vec3f{1.f, 1.f, 1.f}.unit(); // Light points into the screen
                const float intensity = std::max(0.01f, unit_normal.dot(light_direction)); // Some ambient light

                // Use the intensity to shade the color
                Color3 color = {intensity, intensity, intensity};
                draw_triangle_filled(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            case Mode::Normals: {
                Vec3f unit_normal = normal.unit();

                float r = (unit_normal.x() + 1.0f) * 0.5f;
                float g = (unit_normal.y() + 1.0f) * 0.5f;
                float b = (unit_normal.z() + 1.0f) * 0.5f;

                Color3 color{r, g, b};
                draw_triangle_filled(v0_screen, v1_screen, v2_screen, frame_buffer, color);
                break;
            }
            default: {
                throw std::invalid_argument("Invalid renderer mode");
            }
        }
    };

    Timer timer2("Apply Fragment Shader"); // For profiling
    async_for(0, sorted_faces.size(), task);
}