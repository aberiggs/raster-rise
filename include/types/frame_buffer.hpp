#pragma once

#include "types/color.hpp"
#include "types/vec.hpp"
#include "utils/colors.hpp" // For default color

#include <vector>

// A simple ref counted frame buffer
class FrameBuffer {
public:
    // (0, 0) is the top-left corner
    FrameBuffer(int width, int height, const Color3& color = Colors::black);

    // Const accessor
    const Color3& operator[](const Vec2i& pixel) const {
        if (pixel.x() < 0 || pixel.x() >= m_width || pixel.y() < 0 || pixel.y() >= m_height) {
            throw std::runtime_error("Requested coordinates to access were outside of the FrameBuffer: (" +
                                     std::to_string(pixel.x()) + ", " + std::to_string(pixel.y()) + ")");
        }

        // Convert the 2D index to a 1D index for the underlying vector
        int index = pixel.y() * m_width + pixel.x();
        return (*m_buffer_ptr)[index];
    }

    // Non-const accessor
    Color3& operator[](const Vec2i& pixel) {
        if (pixel.x() < 0 || pixel.x() >= m_width || pixel.y() < 0 || pixel.y() >= m_height) {
            throw std::runtime_error("Requested coordinates to access were outside of the FrameBuffer: (" +
                                     std::to_string(pixel.x()) + ", " + std::to_string(pixel.y()) + ")");
        }
        // Convert the 2D index to a 1D index for the underlying vector
        int index = pixel.y() * m_width + pixel.x();
        return (*m_buffer_ptr)[index];
    }

    Color3& operator[](int x, int y) const {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            throw std::runtime_error("Requested coordinates to access were outside of the FrameBuffer: (" +
                                     std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        // Convert the 2D index to a 1D index for the underlying vector
        int index = y * m_width + x;
        return (*m_buffer_ptr)[index];
    }

    // Explicitly produces a clone of the buffer
    [[nodiscard]] FrameBuffer clone() const;

    // Writes the frame buffer to a file
    void write(const std::string& filename);

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }
    inline Vec2i size() const { return Vec2i({m_width, m_height}); }

    // Iterator begin
    inline auto begin() { return m_buffer_ptr->begin(); }
    // Iterator end
    inline auto end() { return m_buffer_ptr->end(); }

private:
    int m_width{0};
    int m_height{0};
    // This buffering being shared means that copies of a FrameBuffer will share ownership of image data
    std::shared_ptr<std::vector<Color3>> m_buffer_ptr{nullptr};
};