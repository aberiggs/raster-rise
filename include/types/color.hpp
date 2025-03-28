#pragma once

#include "types/vec.hpp"

class Color3 : public Vec3f {
public:
    Color3() = default;

    /// @param array {r, g, b}
    Color3(float r, float g, float b) : Vec3f({r, g, b}) {}

    /// @param vec3 {r, g, b}
    Color3(const Vec3f& vec3f) : Vec3f(vec3f) {}

    // Const accessors
    [[nodiscard]] const float& r() const { return x(); }
    [[nodiscard]] const float& g() const { return y(); }
    [[nodiscard]] const float& b() const { return z(); }

    // Non-const accessors
    [[nodiscard]] float& r() { return x(); }
    [[nodiscard]] float& g() { return y(); }
    [[nodiscard]] float& b() { return z(); }
};

class Color4 : public Vec4f {
public:
    Color4() : Vec4f({0.f, 0.f, 0.f, 1.f}){};

    Color4(float r, float g, float b, float a = 1.f) : Vec4f({r, g, b, a}) {}

    /// @param vec3f {r, g, b}
    Color4(const Vec3f& vec3f) : Vec4f({vec3f.x(), vec3f.y(), vec3f.z(), 1.f}) {}

    /// @param vec4f {r, g, b, a}
    Color4(const Vec4f& vec4f) : Vec4f(vec4f) {}

    // Const accessors
    [[nodiscard]] const float& r() const { return x(); }
    [[nodiscard]] const float& g() const { return y(); }
    [[nodiscard]] const float& b() const { return z(); }
    [[nodiscard]] const float& a() const { return w(); }

    // Non-const accessors
    [[nodiscard]] float& r() { return x(); }
    [[nodiscard]] float& g() { return y(); }
    [[nodiscard]] float& b() { return z(); }
    [[nodiscard]] float& a() { return w(); }
};