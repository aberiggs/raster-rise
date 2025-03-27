#include "types/model.hpp"
#include "primitives.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <cassert>
#include <algorithm>

Model::Model(const std::string &filename)
{
    load(filename);
}

void Model::load(const std::string &filename)
{
    // Load the model from the file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);
        char type;
        iss >> type;
        // Check that there's a space after the 'v' (to avoid reading 'vt' or 'vn')
        if (type == 'v' && iss.peek() == ' ')
        {
            Vec3 vertex{};
            iss >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.emplace_back(vertex);
        }
        else if (type == 'f')
        {
            Face face{};
            for (int i = 0; i < 3; i++)
            {
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

void Model::draw(FrameBuffer &frame_buffer, const float zoom) const
{
    // TODO: These transformations really should be handled somewhere else (maybe a camera?)
    // Same with drawing the wireframe

    // Until we implement backface culling, we'll just draw the faces according to their max z values
    std::vector sorted_faces{m_faces};
    std::sort(sorted_faces.begin(), sorted_faces.end(), [this](const Face &a, const Face &b)
              {
                auto max_z_a = std::max({m_vertices[a[0]].z, m_vertices[a[1]].z, m_vertices[a[2]].z});
                auto max_z_b = std::max({m_vertices[b[0]].z, m_vertices[b[1]].z, m_vertices[b[2]].z});

                return max_z_a < max_z_b; });

    // Ensure that vertices in the range [0, 1] are mapped to the range [0, min(width, height) - 1]
    // It has to be -1 because of the 0-based indexing
    float scale = zoom * (std::min(frame_buffer.width(), frame_buffer.height()) - 1);
    for (const auto &face : sorted_faces)
    {
        // Scale vertices to fit screen
        auto v0 = (m_vertices[face[0]] + Vec3{1.0, 1.0, 1.0}) / 2;
        auto v1 = (m_vertices[face[1]] + Vec3{1.0, 1.0, 1.0}) / 2;
        auto v2 = (m_vertices[face[2]] + Vec3{1.0, 1.0, 1.0}) / 2;

        // Please stay in the range [0, 1] :D
        assert(v0.x >= 0 && v0.x <= 1);
        assert(v0.y >= 0 && v0.y <= 1);
        assert(v1.x >= 0 && v1.x <= 1);
        assert(v1.y >= 0 && v1.y <= 1);
        assert(v2.x >= 0 && v2.x <= 1);
        assert(v2.y >= 0 && v2.y <= 1);

        v0 = v0 * scale;
        v1 = v1 * scale;
        v2 = v2 * scale;

        // Flip y to match screen coordinates (where the origin is at the top-left corner instead of the bottom-left)
        v0 = {std::round(v0.x), std::round(static_cast<float>(frame_buffer.height() - 1) - v0.y), v0.z};
        v1 = {std::round(v1.x), std::round(static_cast<float>(frame_buffer.height() - 1) - v1.y), v1.z};
        v2 = {std::round(v2.x), std::round(static_cast<float>(frame_buffer.height() - 1) - v2.y), v2.z};

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

        draw_triangle_filled(v0, v1, v2, frame_buffer, color);
    }
}

void Model::draw_wireframe(FrameBuffer &frame_buffer, const Color3 &color, const float zoom) const
{
    // Ensure that vertices in the range [0, 1] are mapped to the range [0, min(width, height) - 1]
    // It has to be -1 because of the 0-based indexing
    float scale = zoom * (std::min(frame_buffer.width(), frame_buffer.height()) - 1);
    for (const auto &face : m_faces)
    {
        // Scale vertices to fit screen
        auto v0 = (m_vertices[face[0]] + Vec3{1.0, 1.0, 1.0}) / 2;
        auto v1 = (m_vertices[face[1]] + Vec3{1.0, 1.0, 1.0}) / 2;
        auto v2 = (m_vertices[face[2]] + Vec3{1.0, 1.0, 1.0}) / 2;

        // Please stay in the range [0, 1] :D
        assert(v0.x >= 0 && v0.x <= 1);
        assert(v0.y >= 0 && v0.y <= 1);
        assert(v1.x >= 0 && v1.x <= 1);
        assert(v1.y >= 0 && v1.y <= 1);
        assert(v2.x >= 0 && v2.x <= 1);
        assert(v2.y >= 0 && v2.y <= 1);

        v0 = v0 * scale;
        v1 = v1 * scale;
        v2 = v2 * scale;

        // Flip y to match screen coordinates (where the origin is at the top-left corner instead of the bottom-left)
        v0 = {v0.x, static_cast<float>(frame_buffer.height() - 1) - v0.y, v0.z};
        v1 = {v1.x, static_cast<float>(frame_buffer.height() - 1) - v1.y, v1.z};
        v2 = {v2.x, static_cast<float>(frame_buffer.height() - 1) - v2.y, v2.z};

        draw_triangle(v0, v1, v2, frame_buffer, color);
    }
}
