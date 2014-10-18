#include <pebble.h>
#define PERSIST_KEY_STEPS 0
  
Window *my_window;
TextLayer *text_layer;
static int s_worker_steps;

void handle_init(void) {
  app_worker_launch();
  my_window = window_create();

  text_layer = text_layer_create(GRect(0, 0, 144, 20));
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

static void get_step_count() {
  // Get the data from Persistent Storage
  s_worker_steps = persist_read_int(PERSIST_KEY_STEPS);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
  get_step_count();
}
