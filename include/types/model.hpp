#pragma once

#include "types/vec3.hpp"
#include "types/frame_buffer.hpp"

#include <vector>

class Model {
public:
    Model() = default;
    Model(const std::string &filename);

    void load(const std::string &filename);

    void draw(FrameBuffer &frame_buffer, const float zoom = 1.f) const;
    void draw_wireframe(FrameBuffer &frame_buffer, const Color3 &color, const float zoom = 1.f) const;

private:
    std::vector<Vec3> m_vertices{};

    using Face = std::array<int, 3>;
    std::vector<Face> m_faces{};
};