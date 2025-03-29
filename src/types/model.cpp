#include "types/model.hpp"
#include "primitives.hpp"
#include "types/frame_buffer.hpp"
#include "types/vec.hpp"
#include "utils/timer.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const std::string& filename) { load(filename); }

void Model::load(const std::string& filename) {
    // Load the model from the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        char type;
        iss >> type;
        // Check that there's a space after the 'v' (to avoid reading 'vt' or 'vn')
        if (type == 'v' && iss.peek() == ' ') {
            Vec3f vertex{};
            iss >> vertex.x() >> vertex.y() >> vertex.z();
            m_vertices.emplace_back(vertex);
        } else if (type == 'f') {
            Face face{};
            for (int i = 0; i < 3; i++) {
                std::string vertex_data;
                // Read up to the slash (only vertex index)
                std::getline(iss, vertex_data, '/');
                // Subtract 1 to convert to 0-based indexing
                face[i] = std::stoi(vertex_data) - 1;
                // Skip the rest of the data
                std::getline(iss, vertex_data, ' ');
            }

            m_faces.emplace_back(face);
        }
    }
}

std::vector<Vec3i> Model::vertices_in_camera_space(const Vec2i& window_size, float zoom) const {
    // TODO: These transformations should probably be handled somewhere else (maybe a camera?)
    std::vector<Vec3i> vertices_in_camera_space;
    vertices_in_camera_space.reserve(m_vertices.size());
    for (const auto& vertex : m_vertices) {
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

void Model::draw_with_lighting(FrameBuffer& frame_buffer, const float zoom) const {
    Timer timer("Model::draw_with_lighting()"); // For profiling

    // Until we implement backface culling, we'll just draw the faces according to their max z values
    std::vector sorted_faces{m_faces};
    std::sort(sorted_faces.begin(), sorted_faces.end(), [this](const Face& a, const Face& b) {
        auto max_z_a = std::max({m_vertices[a[0]].z(), m_vertices[a[1]].z(), m_vertices[a[2]].z()});
        auto max_z_b = std::max({m_vertices[b[0]].z(), m_vertices[b[1]].z(), m_vertices[b[2]].z()});

        return max_z_a < max_z_b;
    });

    auto vertices = vertices_in_camera_space(frame_buffer.size(), zoom);
    for (const auto& face : sorted_faces) {
        // Calculate the normal of the face
        Vec3f v0 = m_vertices[face[0]];
        Vec3f v1 = m_vertices[face[1]];
        Vec3f v2 = m_vertices[face[2]];

        Vec3f normal = (v1 - v0).cross(v2 - v0);
        Vec3f unit_normal = normal.unit();

        // Calculate the light intensity based on the angle between the normal and the light direction
        Vec3f light_direction = Vec3f({1.f, 1.f, 1.f}).unit();                     // Light points into the screen
        const float intensity = std::max(0.01f, unit_normal.dot(light_direction)); // Some ambient light

        // Use the intensity to shade the color
        Color3 color{intensity, intensity, intensity};

        auto v0_screen = vertices[face[0]];
        auto v1_screen = vertices[face[1]];
        auto v2_screen = vertices[face[2]];

        draw_triangle_filled(v0_screen, v1_screen, v2_screen, frame_buffer, color);
    }
}

void Model::draw_rainbow(FrameBuffer& frame_buffer, const float zoom) const {
    Timer timer("Model::draw_rainbow()"); // For profiling

    // Until we implement backface culling, we'll just draw the faces according to their max z values
    std::vector sorted_faces{m_faces};
    std::sort(sorted_faces.begin(), sorted_faces.end(), [this](const Face& a, const Face& b) {
        auto max_z_a = std::max({m_vertices[a[0]].z(), m_vertices[a[1]].z(), m_vertices[a[2]].z()});
        auto max_z_b = std::max({m_vertices[b[0]].z(), m_vertices[b[1]].z(), m_vertices[b[2]].z()});

        return max_z_a < max_z_b;
    });

    auto vertices = vertices_in_camera_space(frame_buffer.size(), zoom);
    for (const auto& face : sorted_faces) {
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

        auto v0 = vertices[face[0]];
        auto v1 = vertices[face[1]];
        auto v2 = vertices[face[2]];

        draw_triangle_filled(v0, v1, v2, frame_buffer, color);
    }
}

void Model::draw_wireframe(FrameBuffer& frame_buffer, const Color3& color, const float zoom) const {
    Timer timer("Model::draw_wireframe()"); // For profiling

    auto vertices = vertices_in_camera_space(frame_buffer.size(), zoom);
    for (const auto& face : m_faces) {

        auto v0 = vertices[face[0]];
        auto v1 = vertices[face[1]];
        auto v2 = vertices[face[2]];

        draw_triangle(v0, v1, v2, frame_buffer, color);
    }
}
