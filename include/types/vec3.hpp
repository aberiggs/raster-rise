#pragma once

class Vec3 {
public:
    float x{0};
    float y{0};
    float z{0};

    Vec3() = default;
    Vec3(float x, float y, float z);

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const; 
};

// Channel ordering: rgb -> xyz
using Color3 = Vec3;