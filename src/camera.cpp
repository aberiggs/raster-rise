#include "camera.hpp" // self

Matrix4x4f Camera::view_matrix() const {
    constexpr bool enabled = false;

    if (!enabled) {
        return Matrix4x4f::identity();
    }

    Vec3f z = (m_target - m_position).unit();
    Vec3f x = z.cross(m_up).unit();
    Vec3f y = x.cross(z).unit();

    auto view = Matrix4x4f{
        Vec4f{x.x(), x.y(), x.z(), -(x.dot(m_position))},
        Vec4f({y.x(), y.y(), y.z(), -(y.dot(m_position))}),
        Vec4f({z.x(), z.y(), z.z(), z.dot(m_position)}),
        Vec4f({0, 0, 0, 1}),
    };

    return view;
}

Matrix4x4f Camera::projection_matrix(float aspect_ratio) const {
    constexpr bool enabled = false;

    if (!enabled) {
        return Matrix4x4f::identity();
    }

    // float fov_rad = m_fov * (M_PI / 180.0f);
    // float f = 1.0f / std::tan(fov_rad / 2.0f);

    return {
        Vec4f({1, 0, 0, 0}),
        Vec4f({0, 1, 0, 0}),
        Vec4f({0, 0, 1, 0}),
        Vec4f({0, 0, -1 / (m_position.z()), 1}),
    };
}