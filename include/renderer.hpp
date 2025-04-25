#pragma once

#include "camera.hpp"
#include "types/frame_buffer.hpp"
#include "types/object.hpp"

class Renderer {
public:
    enum class Mode { Wireframe, Shaded, Normals };

    static void draw(const Object& object, const Camera& camera, FrameBuffer& frame_buffer, Mode mode);
    static void draw(const std::vector<Object>& object, const Camera& camera, FrameBuffer& frame_buffer, Mode mode);
};