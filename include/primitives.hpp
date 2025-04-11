#pragma once

#include "types/color.hpp"
#include "types/frame_buffer.hpp"
#include "types/vec.hpp"

// TODO: Convert these all to use Vec2i - they only operate on screen space coordinates

// Convenience function to draw a horizontal line - faster than draw_line due to the assumptions we can make
void draw_line_horizontal(int a_x, int b_x, int y, FrameBuffer& frame_buffer, const Color3& color);

// Draws a line using Bresenham's algorithm
void draw_line(Vec3i a, Vec3i b, FrameBuffer& frame_buffer, const Color3& color);

// Draws the 3 edges that connect the vertices
void draw_triangle(const Vec3i& a, const Vec3i& b, const Vec3i& c, FrameBuffer& frame_buffer, const Color3& color);

// Draws filled rectangle using scanline rendering
void draw_triangle_filled(const Vec3i& a, const Vec3i& b, const Vec3i& c, FrameBuffer& frame_buffer,
                          const Color3& color);