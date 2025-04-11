#pragma once

#include "types/frame_buffer.hpp"
#include "types/matrix.hpp"
#include "types/vec.hpp"

#include <vector>

class Camera {
public:
    Camera() = default;

    Matrix4x4f view_matrix() const;
    Matrix4x4f projection_matrix(float aspect_ratio) const;

    void set_position(const Vec3f& position) { m_position = position; }
    void set_target(const Vec3f& target) { m_target = target; }
    void set_up(const Vec3f& up) { m_up = up; }
    void set_fov(float fov) { m_fov = fov; }

private:
    Vec3f m_position = Vec3f({0.f, 0.f, -2.f});
    Vec3f m_target = Vec3f({0.f, 0.f, 0.f});
    Vec3f m_up = Vec3f({0.f, 1.f, 0.f});
    float m_fov{45.0f};
    float m_near{0.1f};
    float m_far{100.0f};
};