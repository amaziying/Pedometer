#include <pebble_worker.h>
#include "pedometer.h"
#define PERSIST_KEY_STEPS 0

static int s_step_count;
static void init() {
  // Initialize your worker here
}

static void deinit() {
  // Deinitialize your worker here
}

static void save_data() {
  // Save the step count to Persistent Storage
  persist_write_int(PERSIST_KEY_STEPS, s_step_count);
}

int main(void) {
  init();
  worker_event_loop();
  deinit();
  save_data();
}


