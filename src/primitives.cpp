#include "primitives.hpp" // self

#include <algorithm> // std::sort
#include <iostream>

namespace {

Vec3i to_screen_space(const Vec3f& ndc, int width, int height) {
    // Convert to screen space
    int x = static_cast<int>((ndc.x() + 1.0f) * 0.5f * width);
    int y = static_cast<int>((ndc.y() + 1.0f) * 0.5f * height) * -1 + height; // Flip y-axis;
    return Vec3i{x, y, 0};
}

} // namespace

void draw_line_horizontal(int a_x, int b_x, int y, float z0, float z1, FrameBuffer& frame_buffer, ZBuffer& z_buffer,
                          const Color3& color) {
    if (y < 0 || y >= frame_buffer.height() || a_x >= frame_buffer.width() || b_x < 0) {
        return; // Out of bounds
    }

    // Clamp the coordinates to the frame buffer
    a_x = std::max(0, a_x);
    b_x = std::min(frame_buffer.width() - 1, b_x);

    if (a_x > b_x) {
        std::swap(a_x, b_x);
        std::swap(z0, z1);
    }

    float dz = 0;
    if (b_x - a_x > 0) {
        dz = (z1 - z0) / static_cast<float>(b_x - a_x);
    }

    float z = z0;
    for (int curr_x = a_x; curr_x <= b_x; curr_x += 1) {
        // Z buffer test
        if (z > z_buffer[curr_x, y]) {
            frame_buffer[curr_x, y] = color;
            z_buffer[curr_x, y] = z;
        }
        z += dz;
    }
}

void draw_line(Vec3f a, Vec3f b, FrameBuffer& frame_buffer, const Color3& color) {

    // Convert to screen space
    Vec3i a_screen = to_screen_space(a, frame_buffer.width(), frame_buffer.height());
    Vec3i b_screen = to_screen_space(b, frame_buffer.width(), frame_buffer.height());

    bool transpose = std::abs(a.y() - b.y()) > std::abs(a.x() - b.x()); // Line is steep
    if (transpose) {
        std::swap(a_screen.x(), a_screen.y());
        std::swap(b_screen.x(), b_screen.y());
    }
    if (a_screen.x() > b_screen.x()) {
        // Always draw from left to right
        std::swap(a_screen.x(), b_screen.x());
        std::swap(a_screen.y(), b_screen.y());
    }

    for (int x = a_screen.x(); x <= b_screen.x(); ++x) {
        float t = (x - a_screen.x()) / static_cast<float>(b_screen.x() - a_screen.x());
        int y = a_screen.y() + t * (b_screen.y() - a_screen.y());

        if (transpose) {
            frame_buffer[y, x] = color;
        } else {
            frame_buffer[x, y] = color;
        }
    }
}

void draw_triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, FrameBuffer& frame_buffer, const Color3& color) {
    draw_line(a, b, frame_buffer, color);
    draw_line(b, c, frame_buffer, color);
    draw_line(c, a, frame_buffer, color);
}

void draw_triangle_filled(const Vec3f& a, const Vec3f& b, const Vec3f& c, FrameBuffer& frame_buffer, ZBuffer& z_buffer,
                          const Color3& color) {

    std::cout << "a_z: " << a.z() << std::endl;
    // Convert to screen space
    Vec3i a_screen = to_screen_space(a, frame_buffer.width(), frame_buffer.height());
    Vec3i b_screen = to_screen_space(b, frame_buffer.width(), frame_buffer.height());
    Vec3i c_screen = to_screen_space(c, frame_buffer.width(), frame_buffer.height());

    // A face made of three vertices
    std::array face{a_screen, b_screen, c_screen};
    // Sort vertices by y coordinates
    std::sort(face.begin(), face.end(), [](const Vec3i& a, const Vec3i& b) { return a.y() < b.y(); });

    // Slope of the line on the longest side (in terms of y)
    // We'll consider this the "major side"
    float dxdy0{0};
    float dzdy0{0};
    if (face[2].y() - face[0].y() > 0) {
        dxdy0 = static_cast<float>(face[2].x() - face[0].x()) / (face[2].y() - face[0].y());
        dzdy0 = static_cast<float>(c.z() - a.z()) / (face[2].y() - face[0].y());
    }

    // Slopes on the opposite sides
    // We'll consider these the "minor sides"
    float dxdy1{0};
    float dzdy1{0};
    if (face[1].y() - face[0].y() > 0) {
        dxdy1 = static_cast<float>(face[1].x() - face[0].x()) / (face[1].y() - face[0].y());
        dzdy1 = static_cast<float>(b.z() - a.z()) / (face[1].y() - face[0].y());
    }

    float dxdy2{0};
    float dzdy2{0};
    if (face[2].y() - face[1].y() > 0) {
        dxdy2 = static_cast<float>(face[2].x() - face[1].x()) / (face[2].y() - face[1].y());
        dzdy2 = static_cast<float>(c.z() - b.z()) / (face[2].y() - face[1].y());
    }

    // Draw lower part of the triangle
    for (int y = face[0].y(); y < face[1].y(); ++y) {
        // x value on major side
        int x0 = face[0].x() + dxdy0 * (y - face[0].y());
        // z value on major side
        float z0 = face[0].z() + dzdy0 * (y - face[0].y());

        // x value on minor side
        int x1 = face[1].x() + dxdy1 * (y - face[1].y());
        // z value on minor side
        float z1 = face[1].z() + dzdy1 * (y - face[1].y());

        draw_line_horizontal(x0, x1, y, z0, z1, frame_buffer, z_buffer, color);
    }

    // Draw upper part of the triangle
    for (int y = face[1].y(); y <= face[2].y(); ++y) {
        // x value on major side
        // TODO: Do not compute this twice...
        int x0 = face[0].x() + dxdy0 * (y - face[0].y());
        // z value on major side
        float z0 = face[0].z() + dzdy0 * (y - face[0].y());

        // x value on minor side
        int x1 = face[2].x() + dxdy2 * (y - face[2].y());
        // z value on minor side
        float z1 = face[2].z() + dzdy2 * (y - face[2].y());

        draw_line_horizontal(x0, x1, y, z0, z1, frame_buffer, z_buffer, color);
    }
}