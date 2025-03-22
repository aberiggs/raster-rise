#include <iostream>

#include "types/frame_buffer.hpp"
#include "utils/colors.hpp"

void draw_line(int x1, int y1, int x2, int y2, FrameBuffer &frame_buffer,
               Color3 color)
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

    for (int x = x1; x <= x2; x++)
    {
        float t = (x - x1) / static_cast<float>(x2 - x1);
        int y = y1 + t * (y2 - y1);

        if (transpose)
        {
            frame_buffer.set_pixel(y, x, color);
            continue;
        }
        frame_buffer.set_pixel(x, y, color);
    }
}

int main() {
    std::cout << "Starting raster-rise!" << std::endl;

    FrameBuffer frame_buffer{64, 64};

    auto x1 = 7;
    auto y1 = 3;
    auto x2 = 62;
    auto y2 = 53;
    auto x3 = 12;
    auto y3 = 37;

    draw_line(x1, y1, x2, y2, frame_buffer, Colors::blue);
    draw_line(x2, y2, x3, y3, frame_buffer, Colors::green);
    draw_line(x3, y3, x1, y1, frame_buffer, Colors::yellow);
    draw_line(x1, y1, x3, y3, frame_buffer, Colors::red);

    frame_buffer.set_pixel(7, 3, Colors::white);
    frame_buffer.set_pixel(62, 53, Colors::white);
    frame_buffer.set_pixel(12, 37, Colors::white);

    frame_buffer.write("output.png");

    return 0;
}
