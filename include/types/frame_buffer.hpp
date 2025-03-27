#pragma once

#include "types/vec4.hpp"

#include <vector>

// A simple class to represent a frame buffer
class FrameBuffer {
public:
    // (0, 0) is the top-left corner
    FrameBuffer(int width, int height, const Color4 &color = Color3{0, 0, 0});

    void set_pixel(int x, int y, const Color4 &color);
    Color4 get_pixel(int x, int y) const;

    // Explicitly produces a clone the buffer
    FrameBuffer clone() const;

    // Writes the frame buffer to a file
    void write(const std::string &filename);

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

private:
    int m_width{0};
    int m_height{0};
    // This buffering being shared means that copies of a FrameBuffer will share ownership of image data
    std::shared_ptr<std::vector<Color4>> m_buffer_ptr{nullptr};
};