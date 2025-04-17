#pragma once

#include "types/color.hpp"
#include "types/frame_buffer.hpp"
#include "types/vec.hpp"
#include "types/z_buffer.hpp"

// Convenience function to draw a horizontal line - faster than draw_line due to the assumptions we can make
void draw_line_horizontal(int a_x, int b_x, int y, float z0, float z1, FrameBuffer& frame_buffer, ZBuffer& z_buffer,
                          const Color3& color);

// Draws a line using Bresenham's algorithm
void draw_line(Vec3f a, Vec3f b, FrameBuffer& frame_buffer, const Color3& color);

// Draws the 3 edges that connect the vertices
void draw_triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, FrameBuffer& frame_buffer, const Color3& color);

// Draws filled rectangle using scanline rendering
void draw_triangle_filled(const Vec3f& a, const Vec3f& b, const Vec3f& c, FrameBuffer& frame_buffer, ZBuffer& z_buffer,
                          const Color3& color);