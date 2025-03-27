#pragma once

#include "types/vec3.hpp"
#include "types/frame_buffer.hpp"

// Convenience function to draw a horizontal line - faster than draw_line due to the assumptions we can make
void draw_line_horizontal(int x1, int x2, int y, FrameBuffer &frame_buffer, const Color3 &color);

// Draws a line using Bresenham's algorithm
void draw_line(int x1, int y1, int x2, int y2, FrameBuffer &frame_buffer,
               const Color3 &color);

// TODO: These Vec3's should really be integers
void draw_triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, FrameBuffer &frame_buffer,
                   const Color3 &color);

// TODO: These Vec3's should really be integers
// Draws filled rectangle using scanline rendering
void draw_triangle_filled(const Vec3& v0, const Vec3& v1, const Vec3& v2, FrameBuffer& frame_buffer,
                          const Color3& color); 