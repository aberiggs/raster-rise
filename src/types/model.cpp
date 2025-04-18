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
