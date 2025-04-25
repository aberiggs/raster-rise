#pragma once

#include "types/frame_buffer.hpp"
#include "types/matrix.hpp"
#include "types/vec.hpp"

#include <vector>

class Object {
public:
    using Face = std::array<int, 3>;

    Object() = default;
    Object(const std::string& filename);

    void load_obj(const std::string& filename);

    const std::vector<Face>& faces() const { return m_faces; }
    const std::vector<Vec3f>& vertices() const { return m_vertices; }

    // TODO: Add ability to transform objects - for now we'll just use the identity matrix
    const Matrix4x4f transform_matrix() const { return Matrix4x4f::identity(); }

    // TODO: Impl
    static Object sphere(int slices, int stacks);
    // TODO: Impl
    static Object cube();

    static Object triangle(Vec3f a, Vec3f b, Vec3f c);

private:
    std::vector<Vec3f> m_vertices{};

    // The indexes of 3 vertices in `m_vertices` that make up a face
    std::vector<Face> m_faces{};
};