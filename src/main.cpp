#include <iostream>

#include "primitives.hpp"
#include "types/frame_buffer.hpp"
#include "types/model.hpp"
#include "utils/colors.hpp"
#include "utils/timer.hpp"

FrameBuffer some_triangles();
FrameBuffer some_filled_triangles();
FrameBuffer body_wireframe();
FrameBuffer body_model();
FrameBuffer diablo_model();

int main(int argc, char* argv[]) {
    int return_code = 0;

    std::cout << "[ -- Starting raster-rise -- ]\n" << std::endl;

    Timer::enabled = false;

    try {
        FrameBuffer frame_buffer{body_model()};

        frame_buffer.write("output.png");
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return_code = 1;
    }

    std::cout << "\n[ -- Stopping raster-rise -- ]" << std::endl;

    return return_code;
}

FrameBuffer some_triangles() {
    FrameBuffer frame_buffer{1500, 1500};

    draw_triangle(Vec3i({7, 45, 0}) * 10, Vec3i({35, 100, 0}) * 10, Vec3i({45, 60, 0}) * 10, frame_buffer, Colors::red);
    draw_triangle(Vec3i({120, 35, 0}) * 10, Vec3i({90, 5, 0}) * 10, Vec3i({45, 110, 0}) * 10, frame_buffer,
                  Colors::white);
    draw_triangle(Vec3i({115, 83, 0}) * 10, Vec3i({80, 90, 0}) * 10, Vec3i({85, 120, 0}) * 10, frame_buffer,
                  Colors::green);

    return frame_buffer;
}

FrameBuffer some_filled_triangles() {
    FrameBuffer frame_buffer{1500, 1500};

    draw_triangle_filled(Vec3i({7, 45, 0}) * 10, Vec3i({35, 100, 0}) * 10, Vec3i({45, 60, 0}) * 10, frame_buffer,
                         Colors::red);
    draw_triangle_filled(Vec3i({120, 35, 0}) * 10, Vec3i({90, 5, 0}) * 10, Vec3i({45, 110, 0}) * 10, frame_buffer,
                         Colors::white);
    draw_triangle_filled(Vec3i({115, 83, 0}) * 10, Vec3i({80, 90, 0}) * 10, Vec3i({85, 120, 0}) * 10, frame_buffer,
                         Colors::green);

    return frame_buffer;
}

FrameBuffer body_wireframe() {
    FrameBuffer frame_buffer{1500, 1500};

    Model model{"objects/body.obj"};
    model.draw_wireframe(frame_buffer, Colors::yellow);

    return frame_buffer;
}

FrameBuffer body_model() {
    FrameBuffer frame_buffer{1500, 1500};

    Model model{"objects/body.obj"};
    model.draw(frame_buffer);

    return frame_buffer;
}

FrameBuffer diablo_model() {
    FrameBuffer frame_buffer{1500, 1500};

    Model model{"objects/diablo3_post.obj"};
    model.draw(frame_buffer);

    return frame_buffer;
}
