#pragma once

// Project lib
#include "vector.hpp"

void draw_base(Vector o, Vector x, Vector y, Vector z);
void draw_boid(Vector front, Vector left, Vector up, Vector o, float size);
void draw_line(Vector p1, Vector p2);
void draw_point(Vector p1, float r, float g, float b);
void draw_box(Vector p1, Vector p2, float r, float g, float b);
