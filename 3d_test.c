// 3d_test
// by wendiner
// GNU General Public License v3.0
//
// My first 3D program.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "wireframe.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

int main(int argc, char** argv) {
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

  struct xyz camera = {0, 101, -100};
  struct xyz camera_direc = {M_PI / 4, 0, 0};
  struct xyz plane = {0, 0, 100};  
  double rotx = M_PI / 100;
  double scale = 100;

  struct model ex;

  FILE* fp = fopen(argv[1], "rb");
  for (int i = 0; i < 5; i++)
    fgetc(fp);
  fread(&(ex.numLines), sizeof(unsigned int), 1, fp);
  printf("I received %i lines!\n", ex.numLines);
  ex.path = (struct line*) malloc(ex.numLines * sizeof(struct line));
  fread(ex.path, sizeof(struct line), ex.numLines, fp);

  al_start_timer(timer);
  while(1) {
    al_wait_for_event(queue, &event);

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ex.numLines; i++) {
          struct line old = ex.path[i];
          rotate(&(old.start), &(ex.path[i].start), 0, rotx, 0);
          rotate(&(old.end), &(ex.path[i].end), 0, rotx, 0);
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

      for (unsigned int i = 0; i < ex.numLines; i++) {
        double start_xy[2];
        project(start_xy, &(ex.path[i].start), &camera, &camera_direc, &plane);
        double end_xy[2];
        project(end_xy, &(ex.path[i].end), &camera, &camera_direc, &plane);

        al_draw_line(start_xy[0] * scale + 256, start_xy[1] * -scale + 256, end_xy[0] * scale + 256, end_xy[1] * -scale + 256, ex.path[i].color, 1.0);
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
