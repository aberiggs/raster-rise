#include "primitives.hpp" // self

#include <algorithm> // std::sort

void draw_line_horizontal(int a_x, int b_x, int y, FrameBuffer& frame_buffer, const Color3& color) {
    if (a_x > b_x) {
        std::swap(a_x, b_x);
    }

    for (int x = a_x; x <= b_x; ++x) {
        Vec2i pixel({x, y}); // Create a pixel coordinate from x and y
        frame_buffer[pixel] = color;
    }
}

void draw_line(Vec3i a, Vec3i b, FrameBuffer& frame_buffer, const Color3& color) {
    bool transpose = std::abs(a.y() - b.y()) > std::abs(a.x() - b.x()); // Line is steep
    if (transpose) {
        std::swap(a.x(), a.y());
        std::swap(b.x(), b.y());
    }
    if (a.x() > b.x()) {
        // Always draw from left to right
        std::swap(a.x(), b.x());
        std::swap(a.y(), b.y());
    }

    for (int x = a.x(); x <= b.x(); ++x) {
        float t = (x - a.x()) / static_cast<float>(b.x() - a.x());
        int y = a.y() + t * (b.y() - a.y());

        if (transpose) {
            Vec2i pixel({y, x});
            frame_buffer[pixel] = color;
        } else {
            Vec2i pixel({x, y});
            frame_buffer[pixel] = color;
        }
    }
}

void draw_triangle(const Vec3i& a, const Vec3i& b, const Vec3i& c, FrameBuffer& frame_buffer, const Color3& color) {
    draw_line(a, b, frame_buffer, color);
    draw_line(b, c, frame_buffer, color);
    draw_line(c, a, frame_buffer, color);
}

void draw_triangle_filled(const Vec3i& a, const Vec3i& b, const Vec3i& c, FrameBuffer& frame_buffer,
                          const Color3& color) {
    // A face made of three vertices
    std::array face{a, b, c};
    // Sort vertices by y coordinates
    std::sort(face.begin(), face.end(), [](const Vec3i& a, const Vec3i& b) { return a.y() < b.y(); });

    // Slope of the line on the longest side (in terms of y)
    // We'll consider this the "major side"
    float slope0{0};
    if (face[2].y() - face[0].y() > 0) {
        slope0 = static_cast<float>(face[2].x() - face[0].x()) / (face[2].y() - face[0].y());
    }

    // Slopes on the opposite sides
    // We'll consider these the "minor sides"
    float slope1{0};
    if (face[1].y() - face[0].y() > 0) {
        slope1 = static_cast<float>(face[1].x() - face[0].x()) / (face[1].y() - face[0].y());
    }

    float slope2{0};
    if (face[2].y() - face[1].y() > 0) {
        slope2 = static_cast<float>(face[2].x() - face[1].x()) / (face[2].y() - face[1].y());
    }

    // Draw lower part of the triangle
    for (int y = face[0].y(); y < face[1].y(); ++y) {
        // x value on major side
        int x0 = face[0].x() + slope0 * (y - face[0].y());
        // x value on minor side
        int x1 = face[1].x() + slope1 * (y - face[1].y());

        draw_line_horizontal(x0, x1, y, frame_buffer, color);
    }

    // Draw upper part of the triangle
    for (int y = face[1].y(); y <= face[2].y(); ++y) {
        // x value on major side
        int x0 = face[0].x() + slope0 * (y - face[0].y());
        // x value on minor side
        int x1 = face[2].x() + slope2 * (y - face[2].y());

        draw_line_horizontal(x0, x1, y, frame_buffer, color);
    }
}