#pragma once

#include <vector>

class ZBuffer {
public:
    // (0, 0) is the top-left corner
    ZBuffer(int width, int height) : m_width(width), m_height(height) {
        // Initialize the ZBuffer with a large value (infinity)
        m_buffer.resize(width * height, -std::numeric_limits<float>::infinity());
    }

    // Accessor for the ZBuffer
    float& operator[](int x, int y) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            throw std::runtime_error("Requested coordinates to access were outside of the ZBuffer: (" +
                                     std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        // Convert the 2D index to a 1D index for the underlying vector
        int index = y * m_width + x;
        return m_buffer[index];
    }

private:
    // The width and height of the ZBuffer
    int m_width{0};
    int m_height{0};

    // The ZBuffer itself, stored as a vector of floats
    std::vector<float> m_buffer;
};