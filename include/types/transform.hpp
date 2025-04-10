#pragma once

#include "types/vec.hpp"

struct Transform {
    Vec3f position = std::array{0.f, 0.f, 0.f};
    Vec3f rotation = std::array{0.f, 0.f, 0.f};
    Vec3f scale = std::array{1.f, 1.f, 1.f};
};