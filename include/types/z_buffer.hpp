#pragma once

#include <algorithm>
#include <limits>
#include <mutex>
#include <vector>

class ZBuffer {
public:
    ZBuffer(int width, int height)
        : m_width(width), m_height(height), m_buffer(width * height, -std::numeric_limits<float>::infinity()),
          m_mutexes(width * height) {}

    float& operator[](int x, int y) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            throw std::runtime_error("Requested coordinates to access were outside of the ZBuffer: (" +
                                     std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        int index = y * m_width + x;
        return m_buffer[index];
    }

    void lock(int x, int y) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            throw std::runtime_error("Requested coordinates to lock were outside of the ZBuffer: (" +
                                     std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        int index = y * m_width + x;
        m_mutexes[index].lock();
    }

    void unlock(int x, int y) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            throw std::runtime_error("Requested coordinates to unlock were outside of the ZBuffer: (" +
                                     std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        int index = y * m_width + x;
        m_mutexes[index].unlock();
    }

    void clear() { std::fill(m_buffer.begin(), m_buffer.end(), -std::numeric_limits<float>::infinity()); }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int size() const { return m_width * m_height; }

private:
    int m_width{0};
    int m_height{0};

    std::vector<float> m_buffer;

    std::vector<std::mutex> m_mutexes;
};