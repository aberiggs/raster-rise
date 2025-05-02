#include "types/object.hpp"
#include "types/vec.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Object::Object(const std::string& filename) { load_obj(filename); }

void Object::load_obj(const std::string& filename) {
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

    std::cout << "Loaded " << m_vertices.size() << " vertices and " << m_faces.size() << " faces from " << filename
              << std::endl;
}

Object Object::triangle(Vec3f a, Vec3f b, Vec3f c) {
    Object object{};
    object.m_vertices = {a, b, c};
    object.m_faces = {{0, 1, 2}};
    return object;
}
