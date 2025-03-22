#pragma once

#include "types/vec3.hpp"

class Vec4 {
public:
    float x{0};
    float y{0};
    float z{0};
    float w{1};

    Vec4() = default;
    Vec4(float x, float y, float z);
    Vec4(float x, float y, float z, float w);
    Vec4(const Vec3& vec3, float w = 1);

    Vec3 vec3() const;

    Vec4 operator+(const Vec4& other) const;
    Vec4 operator-(const Vec4& other) const;
    Vec4 operator*(float scalar) const;
    Vec4 operator/(float scalar) const; 
};

// Channel ordering: bgra -> xyzw 
using Color4 = Vec4; 