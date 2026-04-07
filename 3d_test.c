// 3d_test
// by wendiner
// GNU General Public License v3.0
//
// My first 3D program.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

void project(double* result, struct xyz* a, struct xyz* c, struct xyz* t, struct xyz* e) {
  // we first perform the "camera transform" to describe the target point relative to the camera
  struct xyz diff = {a->x - c->x, a->y - c->y, a->z - c->z};
  struct xyz cost = {cos(t->x), cos(t->y), cos(t->z)};
  struct xyz sint = {sin(t->x), sin(t->y), sin(t->z)};
  struct xyz d = {
    cost.y * (sint.z * diff.y + cost.z * diff.x) - sint.y * diff.z,
    sint.x * (cost.y * diff.z + sint.y * (sint.z * diff.y + cost.z * diff.x)) + cost.x * (cost.z * diff.y - sint.z * diff.x),
    cost.x * (cost.y * diff.z + sint.y * (sint.z * diff.y + cost.z * diff.x)) - sint.x * (cost.z * diff.y - sint.z * diff.x)
  };

  // then we find the intersection between the target vector and the 2D plane
  result[0] = e->z / d.z * d.x + e->x;
  result[1] = e->z / d.z * d.y + e->y;
}

void rotate(struct xyz* inp, struct xyz* out, double rx, double ry, double rz) {
  // rotation matrices for each axis
  double rmx[3][3] = {
    {1, 0, 0},
    {0, cos(rx), -sin(rx)},
    {0, sin(rx), cos(rx)}
  };
  double rmy[3][3] = {
    {cos(ry), 0, sin(ry)},
    {0, 1, 0},
    {-sin(ry), 0, cos(ry)}
  };
  double rmz[3][3] = {
    {cos(rz), -sin(rz), 0},
    {sin(rz), cos(rz), 0},
    {0, 0, 1}
  };

  struct xyz old, new;
  
  old.x = inp->x;
  old.y = inp->y;
  old.z = inp->z;

  new.x = old.x * rmx[0][0] + old.y * rmx[0][1] + old.z * rmx[0][2];
  new.y = old.x * rmx[1][0] + old.y * rmx[1][1] + old.z * rmx[1][2];
  new.z = old.x * rmx[2][0] + old.y * rmx[2][1] + old.z * rmx[2][2];

  old.x = new.x;
  old.y = new.y;
  old.z = new.z;
  
  new.x = old.x * rmy[0][0] + old.y * rmy[0][1] + old.z * rmy[0][2];
  new.y = old.x * rmy[1][0] + old.y * rmy[1][1] + old.z * rmy[1][2];
  new.z = old.x * rmy[2][0] + old.y * rmy[2][1] + old.z * rmy[2][2];
  
  old.x = new.x;
  old.y = new.y;
  old.z = new.z;
  
  new.x = old.x * rmz[0][0] + old.y * rmz[0][1] + old.z * rmz[0][2];
  new.y = old.x * rmz[1][0] + old.y * rmz[1][1] + old.z * rmz[1][2];
  new.z = old.x * rmz[2][0] + old.y * rmz[2][1] + old.z * rmz[2][2];

  out->x = new.x;
  out->y = new.y;
  out->z = new.z;
}

int main() {
  al_init();
  al_init_primitives_addon();

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  ALLEGRO_DISPLAY* disp = al_create_display(512, 512);

  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  bool redraw = true;
  bool done = false;
  ALLEGRO_EVENT event;

  struct model triangle;
  triangle.numLines = 3;
  struct line path[3] = {
    {
      .start = {0, 50, 0},
      .end = {50, -50, 0},
      .color = al_map_rgb(0, 255, 0)
    },
    {
      .start = {50, -50, 0},
      .end = {-50, -50, 0},
      .color = al_map_rgb(0, 255, 0)
    },
    {
      .start = {-50, -50, 0},
      .end = {0, 50, 0},
      .color = al_map_rgb(0, 255, 0)
    }
  };
  triangle.path = path;

  struct xyz camera = {0, 0, -200};
  struct xyz camera_direc = {0, 0, 0};
  struct xyz plane = {0, 0, 100};
  
  double rotx = M_PI / 100;

  al_start_timer(timer);
  while(1) {
    al_wait_for_event(queue, &event);

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < triangle.numLines; i++) {
          struct line old = triangle.path[i];
          rotate(&(old.start), &(triangle.path[i].start), 0, rotx, 0);
          rotate(&(old.end), &(triangle.path[i].end), 0, rotx, 0);
        }
        redraw = true;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }

    if (done)
      break;

    if (redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      for (unsigned int i = 0; i < triangle.numLines; i++) {
        double start_xy[2];
        project(start_xy, &(triangle.path[i].start), &camera, &camera_direc, &plane);
        double end_xy[2];
        project(end_xy, &(triangle.path[i].end), &camera, &camera_direc, &plane);

        al_draw_line(start_xy[0] + 256, start_xy[1] * -1 + 256, end_xy[0] + 256, end_xy[1] * -1 + 256, triangle.path[i].color, 1.0);
      }

      al_flip_display();

      redraw = false;
    }
  }

  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}
