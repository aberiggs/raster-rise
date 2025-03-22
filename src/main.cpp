#include <iostream>

#include "types/frame_buffer.hpp"
#include "utils/colors.hpp"
#include "types/model.hpp"

int main(int argc, char *argv[])
{
    int return_code = 0;

    std::cout << "Starting raster-rise!" << std::endl;

    try
    {
        FrameBuffer frame_buffer{1000, 1000};

        Model model{"objects/body.obj"};

        model.draw_wireframe(frame_buffer, Colors::yellow);

        frame_buffer.write("output.png");
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return_code = 1;
    }

    std::cout << "Shutting down raster-rise!" << std::endl;

    return return_code;
}
