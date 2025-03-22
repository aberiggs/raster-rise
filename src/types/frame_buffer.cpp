#include "types/frame_buffer.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.hpp"

#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, const Color4& color)
    : m_width{width}, m_height{height}, m_buffer_ptr{std::make_shared<std::vector<Color4>>(width * height, color)} {}

void FrameBuffer::set_pixel(int x, int y, const Color4& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        throw std::runtime_error("Requested coordinates to set were outside of the FrameBuffer: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    (*m_buffer_ptr)[y * m_width + x] = color;
}

Color4 FrameBuffer::get_pixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        throw std::runtime_error("Requested coordinates to get were outside of the FrameBuffer: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    return (*m_buffer_ptr)[y * m_width + x];
}

FrameBuffer FrameBuffer::clone() const {
    FrameBuffer clone{m_width, m_height};
    clone.m_buffer_ptr = std::make_shared<std::vector<Color4>>(*m_buffer_ptr);
    return clone;
}

void FrameBuffer::write(const std::string& filename) {
    std::vector<std::uint8_t> data(m_width * m_height * 4);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Color4 color = get_pixel(x, y);
            data[(y * m_width + x) * 4 + 0] = static_cast<std::uint8_t>(color.x * 255);
            data[(y * m_width + x) * 4 + 1] = static_cast<std::uint8_t>(color.y * 255);
            data[(y * m_width + x) * 4 + 2] = static_cast<std::uint8_t>(color.z * 255);
            data[(y * m_width + x) * 4 + 3] = static_cast<std::uint8_t>(color.w * 255);
        }
    }

    stbi_write_png(filename.c_str(), m_width, m_height, 4, data.data(), m_width * 4);
    std::cout << "Wrote image to " << filename << std::endl;
}
    