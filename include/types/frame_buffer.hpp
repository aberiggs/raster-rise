#pragma once

#include "types/vec4.hpp"

#include <vector>

class FrameBuffer {
public:
    FrameBuffer(int width, int height, const Color4& color = Color3{0, 0, 0});

    void set_pixel(int x, int y, const Color4& color);
    Color4 get_pixel(int x, int y) const;
    
    FrameBuffer clone() const;
    void write(const std::string& filename);

private:
    int m_width{0};
    int m_height{0};
    std::shared_ptr<std::vector<Color4>> m_buffer_ptr{nullptr};
};