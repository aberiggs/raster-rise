#include <iostream>

#include "camera.hpp"
#include "primitives.hpp"
#include "renderer.hpp"
#include "types/frame_buffer.hpp"
#include "types/model.hpp"
#include "utils/colors.hpp"
#include "utils/timer.hpp"

FrameBuffer some_triangles();
FrameBuffer some_filled_triangles();
FrameBuffer body_model(Renderer::Mode mode = Renderer::Mode::Normals);
FrameBuffer diablo_model(Renderer::Mode mode = Renderer::Mode::Normals);

int main(int argc, char* argv[]) {
    int return_code = 0;

    std::cout << "[ -- Starting raster-rise -- ]\n" << std::endl;

    Timer::enabled = true;

    try {
        constexpr Renderer::Mode mode = Renderer::Mode::Shaded;

        FrameBuffer frame_buffer{diablo_model(mode)};

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

    draw_triangle(Vec3i{7, 45, 0} * 10, Vec3i{35, 100, 0} * 10, Vec3i{45, 60, 0} * 10, frame_buffer, Colors::red);
    draw_triangle(Vec3i{120, 35, 0} * 10, Vec3i{90, 5, 0} * 10, Vec3i{45, 110, 0} * 10, frame_buffer, Colors::white);
    draw_triangle(Vec3i{115, 83, 0} * 10, Vec3i{80, 90, 0} * 10, Vec3i{85, 120, 0} * 10, frame_buffer, Colors::green);

    return frame_buffer;
}

FrameBuffer some_filled_triangles() {
    FrameBuffer frame_buffer{1500, 1500};

    draw_triangle_filled(Vec3i{7, 45, 0} * 10, Vec3i{35, 100, 0} * 10, Vec3i{45, 60, 0} * 10, frame_buffer,
                         Colors::red);
    draw_triangle_filled(Vec3i{120, 35, 0} * 10, Vec3i{90, 5, 0} * 10, Vec3i{45, 110, 0} * 10, frame_buffer,
                         Colors::white);
    draw_triangle_filled(Vec3i{115, 83, 0} * 10, Vec3i{80, 90, 0} * 10, Vec3i{85, 120, 0} * 10, frame_buffer,
                         Colors::green);

    return frame_buffer;
}

FrameBuffer body_model(Renderer::Mode mode) {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({-1.f, 0.f, -2.f});

    Model model{"objects/body.obj"};
    Renderer::draw(model, camera, frame_buffer, mode);

    return frame_buffer;
}

FrameBuffer diablo_model(Renderer::Mode mode) {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({-1.f, 0.f, -2.f});
    camera.set_target({-1.2f, 0.f, 0.f});

    Model model{"objects/diablo3_post.obj"};
    Renderer::draw(model, camera, frame_buffer, mode);

    return frame_buffer;
}
