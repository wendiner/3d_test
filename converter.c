// converter.c
// by wendiner, 2026
// GNU General Public License v3.0
//
// Converts .obj files to an array of line structs,
// readable by wireframe.c.
// By nature, only supports "v" and "f" entries.

#include <stdio.h>
#include <stdlib.h>
#include "wireframe.h"

struct vertex {
  double x, y, z;
  struct vertex* next;
};

struct vertex* seek(struct vertex* start, unsigned int ind) {
  struct vertex* ptr = start;
  for (unsigned int i = 0; i < ind; i++)
    ptr = ptr->next;

  return ptr;
}

struct triangle {
  struct line line1, line2, line3;
  struct triangle* next;
};

int main(int argc, char** argv) {
  FILE* fp_inp = fopen(argv[1], "r");
  FILE* fp_out = fopen(argv[2], "w+");

  struct vertex *start, *old, *new;
  struct triangle *tri_start, *tri_old, *tri_new;
  unsigned int numLines;
  
  char prefix;
  double num1, num2, num3;

  while (1) {
    fscanf(fp_inp, "%c %lf %lf %lf\n", &prefix, &num1, &num2, &num3);

    switch (prefix) {
      case 'v':
        if (!start) {
          start = (struct vertex*) malloc(sizeof(struct vertex));
          start->x = num1;
          start->y = num2;
          start->z = num3;
          new = start;
        } else {
          old = new;
          new = (struct vertex*) malloc(sizeof(struct vertex));
          old->next = new;
          new->x = num1;
          new->y = num2;
          new->z = num3;
        }

        break;
      case 'f':
        struct vertex* vert1 = seek(start, (unsigned int) (num1 - 1));
        struct vertex* vert2 = seek(start, (unsigned int) (num2 - 1));
        struct vertex* vert3 = seek(start, (unsigned int) (num3 - 1));

        struct triangle* current = (struct triangle*) malloc(sizeof(struct triangle));

        if (tri_new) {
          tri_old = tri_new;
          tri_old->next = current;
        } else {
          tri_start = current;
        }

        struct line lines[3] = {
          {
            .start = {vert1->x, vert1->y, vert1->z},
            .end = {vert2->x, vert2->y, vert2->z},
            .color = al_map_rgb(0, 255, 0)
          },
          {
            .start = {vert2->x, vert2->y, vert2->z},
            .end = {vert3->x, vert3->y, vert3->z},
            .color = al_map_rgb(0, 255, 0)
          },
          {
            .start = {vert3->x, vert3->y, vert3->z},
            .end = {vert1->x, vert1->y, vert1->z},
            .color = al_map_rgb(0, 255, 0)
          }
        };

        current->line1 = lines[0];
        current->line2 = lines[1];
        current->line3 = lines[2];

        tri_new = current;
        numLines += 3;
    }

    if (feof(fp_inp))
      break;
  }

  fprintf(fp_out, "MODEL");
  fwrite(&numLines, sizeof(unsigned int), 1, fp_out);

  struct triangle* ptr = tri_start;
  while (ptr) {
    fwrite(&(ptr->line1), sizeof(struct line), 1, fp_out);
    fwrite(&(ptr->line2), sizeof(struct line), 1, fp_out);
    fwrite(&(ptr->line3), sizeof(struct line), 1, fp_out);
    
    ptr = ptr->next;
  }

  return 0;
}
