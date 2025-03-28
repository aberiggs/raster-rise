#include "types/frame_buffer.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.hpp"

#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, const Color4& color)
    : m_width{width}, m_height{height}, m_buffer_ptr{std::make_shared<std::vector<Color4>>(width * height, color)} {}

FrameBuffer FrameBuffer::clone() const {
    FrameBuffer clone{m_width, m_height};
    clone.m_buffer_ptr = std::make_shared<std::vector<Color4>>(*m_buffer_ptr);
    return clone;
}

void FrameBuffer::write(const std::string& filename) {
    // TODO: Refactor
    std::vector<std::uint8_t> data(m_width * m_height * 4);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Vec2i pixel({x, y});                  // Create a pixel coordinate from x and y
            const Color4& color = (*this)[pixel]; // Get the color at the pixel coordinate
            data[(y * m_width + x) * 4 + 0] = static_cast<std::uint8_t>(color.r() * 255);
            data[(y * m_width + x) * 4 + 1] = static_cast<std::uint8_t>(color.g() * 255);
            data[(y * m_width + x) * 4 + 2] = static_cast<std::uint8_t>(color.b() * 255);
            data[(y * m_width + x) * 4 + 3] = static_cast<std::uint8_t>(color.a() * 255);
        }
    }

    stbi_write_png(filename.c_str(), m_width, m_height, 4, data.data(), m_width * 4);
    std::cout << "Wrote image to " << filename << std::endl;
}
