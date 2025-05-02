#include <iostream>

#include "camera.hpp"
#include "renderer.hpp"
#include "types/frame_buffer.hpp"
#include "types/object.hpp"

#include <tracy/Tracy.hpp>

FrameBuffer some_triangles();
FrameBuffer some_filled_triangles();
FrameBuffer body_model(Renderer::Mode mode = Renderer::Mode::Normals);
FrameBuffer diablo_model(Renderer::Mode mode = Renderer::Mode::Normals);
FrameBuffer other(const std::string& name, Renderer::Mode mode = Renderer::Mode::Normals);

int main(int argc, char* argv[]) {
    int return_code = 0;

    std::cout << "[ -- Starting raster-rise -- ]\n" << std::endl;

    try {
        [[maybe_unused]] constexpr Renderer::Mode mode = Renderer::Mode::Normals;

        FrameBuffer frame_buffer{some_triangles()};

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
    Camera camera{};
    camera.set_position({0.f, 0.f, -4.f});

    Object triangle_a = Object::triangle({0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f});
    Object triangle_b = Object::triangle({1.f, 1.f, 0.f}, {0.2f, 1.f, 0.f}, {1.f, 0.2f, 0.f});
    Renderer::draw({triangle_a, triangle_b}, camera, frame_buffer, Renderer::Mode::Wireframe);

    return frame_buffer;
}

FrameBuffer some_filled_triangles() {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({0.f, 0.f, -4.f});

    Object triangle_a = Object::triangle({0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f});
    Object triangle_b = Object::triangle({1.f, 1.f, 0.f}, {0.2f, 1.f, 0.f}, {1.f, 0.2f, 0.f});
    Renderer::draw({triangle_a, triangle_b}, camera, frame_buffer, Renderer::Mode::Normals);

    return frame_buffer;
}

FrameBuffer body_model(Renderer::Mode mode) {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({0.f, 0.f, -4.f});

    Object model{"objects/body.obj"};
    Renderer::draw(model, camera, frame_buffer, mode);

    return frame_buffer;
}

FrameBuffer diablo_model(Renderer::Mode mode) {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({0.f, 0.f, -4.f});

    Object model{"objects/diablo3_post.obj"};
    Renderer::draw(model, camera, frame_buffer, mode);

    return frame_buffer;
}

FrameBuffer other(const std::string& name, Renderer::Mode mode) {
    FrameBuffer frame_buffer{1500, 1500};
    Camera camera{};
    camera.set_position({0.f, 1.f, -3.f});
    camera.set_target({0.f, 1.f, 0.f});

    Object model{name};
    Renderer::draw(model, camera, frame_buffer, mode);

    return frame_buffer;
}
