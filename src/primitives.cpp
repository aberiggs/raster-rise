#include "primitives.hpp" // self
#include "types/vec.hpp"

#include <tracy/Tracy.hpp> // Tracy profiling

#include <algorithm>  // std::sort
#include <functional> // For std::hash
#include <unordered_map>
namespace std {
template <> struct hash<Vec3f> {
    size_t operator()(const Vec3f& vec) const {
        size_t h1 = std::hash<float>{}(vec.x());
        size_t h2 = std::hash<float>{}(vec.y());
        size_t h3 = std::hash<float>{}(vec.z());
        return h1 ^ (h2 << 1) ^ (h3 << 2); // Combine hashes
    }
};
} // namespace std
namespace {

Vec2i to_screen_space(const Vec3f& ndc, int width, int height) {
    ZoneScopedN("to_screen_space"); // Add Tracy profiling for this function

    // Convert to screen space
    int x = static_cast<int>((ndc.x() + 1.0f) * 0.5f * width);
    int y = static_cast<int>((-ndc.y() + 1.0f) * 0.5f * height); // Flip y-axis
    return {x, y};
}

std::pair<Vec2i, Vec2i> find_bounding_box(Vec2i a, Vec2i b, Vec2i c) {
    ZoneScopedN("find_bounding_box"); // Add Tracy profiling for this function

    // Find the bounding box of the triangle
    int min_x = std::min({a.x(), b.x(), c.x()});
    int max_x = std::max({a.x(), b.x(), c.x()});
    int min_y = std::min({a.y(), b.y(), c.y()});
    int max_y = std::max({a.y(), b.y(), c.y()});

    return {{min_x, min_y}, {max_x, max_y}};
}

double signed_triangle_area(Vec2i a, Vec2i b, Vec2i c) {
    ZoneScopedN("signed_triangle_area"); // Add Tracy profiling for this function

    // The shoelace formula
    return 0.5 *
           ((b.y() - a.y()) * (b.x() + a.x()) + (c.y() - b.y()) * (c.x() + b.x()) + (a.y() - c.y()) * (a.x() + c.x()));
}

} // namespace

void draw_line(Vec3f a, Vec3f b, FrameBuffer& frame_buffer, const Color3& color) {
    ZoneScopedN("draw_line"); // Add Tracy profiling for this function

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
    ZoneScopedN("draw_triangle"); // Add Tracy profiling for this function
    draw_line(a, b, frame_buffer, color);
    draw_line(b, c, frame_buffer, color);
    draw_line(c, a, frame_buffer, color);
}

void draw_triangle_filled(const Vec3f& a, const Vec3f& b, const Vec3f& c, FrameBuffer& frame_buffer, ZBuffer& z_buffer,
                          const Color3& color) {
    ZoneScopedN("draw_triangle_filled"); // Add Tracy profiling for this function

    // TODO: Consider moving these statics somewhere else
    static std::mutex screen_cache_mutex;
    static std::unordered_map<Vec3f, Vec2i> screen_cache{};
    // TODO: Make this an option in a config class
    constexpr bool use_cache = true; // Used to compare performance with and without cache

    // Convert to screen space
    screen_cache_mutex.lock();

    if (!use_cache) {
        screen_cache.clear();
    }

    if (screen_cache.find(a) == screen_cache.end()) {
        screen_cache[a] = to_screen_space(a, frame_buffer.width(), frame_buffer.height());
    }
    if (screen_cache.find(b) == screen_cache.end()) {
        screen_cache[b] = to_screen_space(b, frame_buffer.width(), frame_buffer.height());
    }
    if (screen_cache.find(c) == screen_cache.end()) {
        screen_cache[c] = to_screen_space(c, frame_buffer.width(), frame_buffer.height());
    }

    Vec2i& a_screen = screen_cache[a];
    Vec2i& b_screen = screen_cache[b];
    Vec2i& c_screen = screen_cache[c];

    screen_cache_mutex.unlock();

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
                z_buffer.lock(x, y);
                if (z > z_buffer[x, y]) {
                    // Z buffer test
                    frame_buffer[x, y] = color;
                    z_buffer[x, y] = z;
                }
                z_buffer.unlock(x, y);
            }
        }
    }
}