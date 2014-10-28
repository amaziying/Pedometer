#include <pebble.h>
#define PERSIST_KEY_STEPS 0
  
Window *my_window;
TextLayer *count_layer;
long int worker_steps;

static void get_step_count() {
  // Get the data from Persistent Storage
  worker_steps = persist_read_int(PERSIST_KEY_STEPS);
  static char buf[] = "123456890abcdefghijkl";
  snprintf(buf, sizeof(buf), "%ld", worker_steps);
  text_layer_set_text(count_layer, buf);
}

static void worker_message_handler(uint16_t type, AppWorkerMessage *data) {
  // Get the step data
  worker_steps = data->data0;

  // Do something with the data
  static char buf[] = "123456890abcdefghijkl";
  snprintf(buf, sizeof(buf), "%ld", worker_steps);
  text_layer_set_text(count_layer, buf);
}

static void main_window_load(){  
  //Create the TextLayer...
  count_layer = text_layer_create(GRect(0, 0, 144, 20));
  text_layer_set_background_color(count_layer, GColorClear);
  text_layer_set_text_color(count_layer, GColorBlack);  
  
  //add it to the windows root layer
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(count_layer));
  get_step_count();
}

static void main_window_unload(Window *Window){
  text_layer_destroy(count_layer);
}

void handle_init(void) {
  app_worker_launch();
  app_worker_message_subscribe(worker_message_handler);
  my_window = window_create();
  

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
