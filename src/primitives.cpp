#include "primitives.hpp" // self
#include "types/vec.hpp"

#include <algorithm> // std::sort
#include <iostream>

namespace {

Vec2i to_screen_space(const Vec3f& ndc, int width, int height) {
    // Convert to screen space
    int x = static_cast<int>((ndc.x() + 1.0f) * 0.5f * width);
    int y = static_cast<int>((-ndc.y() + 1.0f) * 0.5f * height); // Flip y-axis
    return {x, y};
}

std::pair<Vec2i, Vec2i> find_bounding_box(Vec2i a, Vec2i b, Vec2i c) {
    // Find the bounding box of the triangle
    int min_x = std::min({a.x(), b.x(), c.x()});
    int max_x = std::max({a.x(), b.x(), c.x()});
    int min_y = std::min({a.y(), b.y(), c.y()});
    int max_y = std::max({a.y(), b.y(), c.y()});

    return {{min_x, min_y}, {max_x, max_y}};
}

double signed_triangle_area(Vec2i a, Vec2i b, Vec2i c) {
    // The shoelace formula
    return 0.5 *
           ((b.y() - a.y()) * (b.x() + a.x()) + (c.y() - b.y()) * (c.x() + b.x()) + (a.y() - c.y()) * (a.x() + c.x()));
}

} // namespace

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

    // Convert to screen space
    Vec2i a_screen = to_screen_space(a, frame_buffer.width(), frame_buffer.height());
    Vec2i b_screen = to_screen_space(b, frame_buffer.width(), frame_buffer.height());
    Vec2i c_screen = to_screen_space(c, frame_buffer.width(), frame_buffer.height());

    auto [top_left, bottom_right] = find_bounding_box(a_screen, b_screen, c_screen);

    double total_area = signed_triangle_area(a_screen, b_screen, c_screen);

    for (int x = std::max(0, top_left.x()); x <= std::min(bottom_right.x(), frame_buffer.width() - 1); ++x) {
        for (int y = std::max(0, top_left.y()); y <= std::min(bottom_right.y(), frame_buffer.height() - 1); ++y) {
            Vec2i p{x, y};

            // Check if the point is inside the triangle using barycentric coordinates
            double alpha = signed_triangle_area(p, b_screen, c_screen);
            double beta = signed_triangle_area(a_screen, p, c_screen);
            double gamma = signed_triangle_area(a_screen, b_screen, p);

            // Normalize the barycentric coordinates
            alpha /= total_area;
            beta /= total_area;
            gamma /= total_area;

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                // Point is in the triangle
                float z = alpha * a.z() + beta * b.z() + gamma * c.z();
                if (z > z_buffer[x, y]) {
                    // Z buffer test
                    frame_buffer[x, y] = color;
                    z_buffer[x, y] = z;
                }
            }
        }
    }
}