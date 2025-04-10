#pragma once

#include "types/frame_buffer.hpp"
#include "types/vec.hpp"

#include <vector>

class Model {
public:
    using Face = std::array<int, 3>;

    Model() = default;
    Model(const std::string& filename);

    void load(const std::string& filename);

    std::vector<Face> faces() const { return m_faces; }
    std::vector<Vec3f> vertices() const { return m_vertices; }

    // TODO: Impl
    static Model sphere(int slices, int stacks);
    // TODO: Impl
    static Model cube();

private:
    std::vector<Vec3f> m_vertices{};

    // The indexes of 3 vertices in `m_vertices` that make up a face
    std::vector<Face> m_faces{};
};