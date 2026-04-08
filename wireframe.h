#ifndef _WIREFRAME_H_
#define _WIREFRAME_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

struct xyz {
  double x, y, z;
};

struct line {
  struct xyz start, end;
  ALLEGRO_COLOR color;
};

struct model {
  unsigned int numLines;
  struct line* path;
};

extern void project(double* result, struct xyz* a, struct xyz* c, struct xyz* t, struct xyz* e);
extern void rotate(struct xyz* inp, struct xyz* out, double rx, double ry, double rz);

#endif
