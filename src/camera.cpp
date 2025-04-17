#include "camera.hpp" // self

Matrix4x4f Camera::view_matrix() const {
    Vec3f z = (m_position - m_target).unit();
    Vec3f x = m_up.cross(z).unit();
    Vec3f y = z.cross(x).unit();

    auto view = Matrix4x4f{
        Vec4f{x.x(), y.x(), z.x(), -m_position.x()},
        Vec4f({x.y(), y.y(), z.y(), -m_position.y()}),
        Vec4f({x.z(), y.z(), z.z(), -m_position.z()}),
        Vec4f({0, 0, 0, 1}),
    };

    return view;
}

Matrix4x4f Camera::projection_matrix(float aspect_ratio) const {
    float fov_rad = m_fov * (M_PI / 180.0f);
    float f = 1.0f / std::tan(fov_rad / 2.0f);

    return {
        Vec4f({f / aspect_ratio, 0, 0, 0}),
        Vec4f({0, f, 0, 0}),
        Vec4f({0, 0, (m_far + m_near) / (m_near - m_far), (2 * m_far * m_near) / (m_near - m_far)}),
        Vec4f({0, 0, 1, 0}),
    };
}