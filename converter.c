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
  for (unsigned int i = 0; i <= ind; i++)
    ptr = ptr->next;

  return ptr;
}

int main(int argc, char** argv) {
  FILE* fp_inp = fopen(argv[1], "r");
  FILE* fp_out = fopen(argv[2], "w+");

  unsigned int numVerts = 0;
  struct vertex *start, *old, *new;
  
  char prefix;
  double num1, num2, num3;

  bool headerWritten = false;

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

        numVerts++;
        break;
      case 'f':
        if (!headerWritten) {
          headerWritten = true;
          fprintf(fp_out, "MODEL");
          printf("I counted %i vertices!\n", numVerts);
          numVerts /= 3;
          fwrite(&numVerts, sizeof(unsigned int), 1, fp_out);
        }

        printf("%i %i %i\n", (unsigned int) num1, (unsigned int) num2, (unsigned int) num3);

        struct vertex* vert1 = seek(start, (unsigned int) (num1 - 1));
        struct vertex* vert2 = seek(start, (unsigned int) (num2 - 1));
        struct vertex* vert3 = seek(start, (unsigned int) (num3 - 1));

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

        fwrite(lines, sizeof(struct line), 3, fp_out);
    }

    if (feof(fp_inp))
      break;
  }

  return 0;
}
