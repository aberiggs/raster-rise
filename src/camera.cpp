#include "camera.hpp" // self

Matrix4x4f Camera::view_matrix() const {
    Vec3f z = (m_target - m_position).unit();
    Vec3f x = z.cross(m_up).unit();
    Vec3f y = x.cross(z);

    auto view = Matrix4x4f{{
        Vec4f({x.x(), y.x(), z.x(), -x.dot(m_position)}),
        Vec4f({x.y(), y.y(), z.y(), -y.dot(m_position)}),
        Vec4f({x.z(), y.z(), -z.z(), -z.dot(m_position)}),
        Vec4f({0, 0, 0, 1}),
    }};

    return view;
}

Matrix4x4f Camera::projection_matrix(float aspect_ratio) const {

    float f = 1.0f / std::tan(m_fov / 2.0f);

    return Matrix4x4f({
        Vec4f({f / aspect_ratio, 0, 0, 0}),
        Vec4f({0, f, 0, 0}),
        Vec4f({0, 0, (m_far + m_near) / (m_near - m_far), (2 * m_far * m_near) / (m_near - m_far)}),
        Vec4f({0, 0, -1, 0}),
    });
}