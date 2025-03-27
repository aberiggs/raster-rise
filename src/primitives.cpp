#include "primitives.hpp" // self

#include <algorithm> // std::sort
#include <iostream> // std::cout

void draw_line_horizontal(int x1, int x2, int y, FrameBuffer &frame_buffer, const Color3 &color)
{
    if (x1 > x2)
    {
        std::swap(x1, x2);
    }

    for (int x = x1; x <= x2; ++x)
    {
        frame_buffer.set_pixel(x, y, color);
    }
}

void draw_line(int x1, int y1, int x2, int y2, FrameBuffer &frame_buffer,
                const Color3 &color)
{
    bool transpose = std::abs(y2 - y1) > std::abs(x2 - x1); // Line is steep
    if (transpose)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    if (x1 > x2)
    {
        // Always draw from left to right
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    for (int x = x1; x <= x2; ++x)
    {
        float t = (x - x1) / static_cast<float>(x2 - x1);
        int y = y1 + t * (y2 - y1);

        if (transpose)
        {
            frame_buffer.set_pixel(y, x, color);
        } else {
            frame_buffer.set_pixel(x, y, color);
        }
    }
}

void draw_triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, FrameBuffer& frame_buffer,
                    const Color3& color)
{
    draw_line(v0.x, v0.y, v1.x, v1.y, frame_buffer, color);
    draw_line(v1.x, v1.y, v2.x, v2.y, frame_buffer, color);
    draw_line(v2.x, v2.y, v0.x, v0.y, frame_buffer, color);
}

void draw_triangle_filled(const Vec3& v0, const Vec3& v1, const Vec3& v2, FrameBuffer& frame_buffer,
                          const Color3& color)
{
    std::array vertices{v0, v1, v2};
    // Sort vertices by y coordinates
    std::sort(vertices.begin(), vertices.end(), [](const Vec3& a, const Vec3& b) {
        return a.y < b.y;
    });
    
    // Slope of the line on the longest side (in terms of y)
    // We'll consider this the "major side"
    float slope0{0};
    if (static_cast<int>(vertices[2].y - vertices[0].y) > 0)
    {
        slope0 = (vertices[2].x - vertices[0].x) / static_cast<int>(vertices[2].y - vertices[0].y);
    }

    // Slopes on the opposite sides
    // We'll consider these the "minor sides"
    float slope1{0}; 
    if (static_cast<int>(vertices[1].y - vertices[0].y) > 0) {
        slope1 = (vertices[1].x - vertices[0].x) / static_cast<int>(vertices[1].y - vertices[0].y);
    }

    float slope2{0};
    if (static_cast<int>(vertices[2].y - vertices[1].y) > 0) {
        slope2 = (vertices[2].x - vertices[1].x) / static_cast<int>(vertices[2].y - vertices[1].y);
    }

    // Draw lower part of the triangle
    for (int y = vertices[0].y; y <= vertices[1].y; ++y)
    {
        // x value on major side
        int x0 = vertices[0].x + slope0 * (y - vertices[0].y);
        // x value on minor side
        int x1 = vertices[1].x + slope1 * (y - vertices[1].y);
        if (x0 < 0 || x1 < 0 || x0 >= frame_buffer.width() || x1 >= frame_buffer.width())
        {
            continue;
        }
        draw_line_horizontal(x0, x1, y, frame_buffer, color);
    }

    // Draw upper part of the triangle
    for (int y = vertices[1].y; y <= vertices[2].y; ++y)
    {
        // x value on major side
        int x0 = vertices[0].x + slope0 * (y - vertices[0].y);
        // x value on minor side
        int x1 = vertices[2].x + slope2 * (y - vertices[2].y);

        if (x0 < 0 || x1 < 0 || x0 >= frame_buffer.width() || x1 >= frame_buffer.width())
        {
            continue;
        }

        draw_line_horizontal(x0, x1, y, frame_buffer, color);
    }

} 