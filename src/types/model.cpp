#include "types/model.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <cassert>

namespace
{

    void draw_line(int x1, int y1, int x2, int y2, FrameBuffer &frame_buffer,
                   const Color3 &color)
    {
        bool transpose = std::abs(y2 - y1) > std::abs(x2 - x1); // Line is steep
        if (transpose)
        {
            std::swap(x1, y1);
            std::swap(x2, y2);
        }
        if (x1 > x2)
        {
            // Always draw from left to right
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for (int x = x1; x <= x2; x++)
        {
            float t = (x - x1) / static_cast<float>(x2 - x1);
            int y = y1 + t * (y2 - y1);

            if (transpose)
            {
                frame_buffer.set_pixel(y, x, color);
            } else {
                frame_buffer.set_pixel(x, y, color);
            }
        }
    }

}

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

void Model::draw_wireframe(FrameBuffer &frame_buffer, const Color3 &color) const
{
    // Ensure that vertices in the range [0, 1] are mapped to the range [0, min(width, height) - 1]
    // It has to be -1 because of the 0-based indexing
    float scale = 1.0f * (std::min(frame_buffer.width(), frame_buffer.height()) - 1);
    for (const auto &face : m_faces)
    {
        // Draw the edges of the triangle
        for (int i = 0; i < 3; i++)
        {
            // Normalize the vertices to the range [0, 1]
            auto v1 = (m_vertices[face[i]] + Vec3{1.0, 1.0, 1.0}) / 2;
            auto v2 = (m_vertices[face[(i + 1) % 3]] + Vec3{1.0, 1.0, 1.0}) / 2;

            // Please stay within [0, 1] :D
            assert(v1.x >= 0 && v1.x <= 1);
            assert(v1.y >= 0 && v1.y <= 1);
            assert(v2.x >= 0 && v2.x <= 1);
            assert(v2.y >= 0 && v2.y <= 1);

            v1 = v1 * scale;
            v2 = v2 * scale;

            // Flip y to match screen coordinates (where the origin is at the top-left corner instead of the bottom-left)
            v1 = {v1.x, static_cast<float>(frame_buffer.height() - 1) - v1.y, v1.z};
            v2 = {v2.x, static_cast<float>(frame_buffer.height() - 1) - v2.y, v2.z};

            draw_line(v1.x, v1.y, v2.x, v2.y, frame_buffer, color);
        }
    }
}