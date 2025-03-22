#include <iostream>

#include "types/frame_buffer.hpp"

int main() {
    std::cout << "Starting raster-rise!" << std::endl;

    Color4 background = {0.0, 0.71, 1.0};
    FrameBuffer frame_buffer{800, 800, background};
    frame_buffer.write("output.png");

    return 0;
}
