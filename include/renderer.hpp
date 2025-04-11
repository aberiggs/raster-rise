#pragma once

#include "camera.hpp"
#include "types/frame_buffer.hpp"
#include "types/model.hpp"

class Renderer {
public:
    enum class Mode { Wireframe, Shaded, Normals };

    static void draw(const Model& model, const Camera& camera, FrameBuffer& frame_buffer, Mode mode);
};