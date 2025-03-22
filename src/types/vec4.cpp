#include "types/vec4.hpp" // self

Vec4::Vec4(float x, float y, float z) : x{x}, y{y}, z{z} {}
Vec4::Vec4(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w} {}
Vec4::Vec4(const Vec3& vec3, float w) : x{vec3.x}, y{vec3.y}, z{vec3.z}, w{w} {}

Vec4 Vec4::operator+(const Vec4& other) const {
    return Vec4{x + other.x, y + other.y, z + other.z, w + other.w};
}

Vec4 Vec4::operator-(const Vec4& other) const {
    return Vec4{x - other.x, y - other.y, z - other.z, w - other.w};
}
