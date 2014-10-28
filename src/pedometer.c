#include <pebble_worker.h>
//#include "pedometer.h"
#define PERSIST_KEY_STEPS 52

long int step_count;

static AppTimer *timer;

int X_DELTA = 35;
int Y_DELTA, Z_DELTA = 185;
int YZ_DELTA_MIN = 175;
int YZ_DELTA_MAX = 195; 
int X_DELTA_TEMP, Y_DELTA_TEMP, Z_DELTA_TEMP = 0;
int lastX, lastY, lastZ, currX, currY, currZ = 0;
int sensitivity = 1;

bool did_pebble_vibrate = false;
bool validX, validY, validZ = false;
bool startedSession = false;

// interval to check for next step (in ms)
const int ACCEL_STEP_MS = 475;
// value to auto adjust step acceptance 
const int PED_ADJUST = 2;

static void send_to_foreground() {
  // Construct a data packet
  AppWorkerMessage msg_data = {
    .data0 = step_count
  };

  // Send the data to the foreground app
  app_worker_send_message(PERSIST_KEY_STEPS, &msg_data);
}

static void save_data() {
  // Save the step count to Persistent Storage
  persist_write_int(PERSIST_KEY_STEPS, step_count);
}

void autoCorrectZ(){
  if (Z_DELTA > YZ_DELTA_MAX){
    Z_DELTA = YZ_DELTA_MAX; 
  } else if (Z_DELTA < YZ_DELTA_MIN){
    Z_DELTA = YZ_DELTA_MIN;
  }
}

void autoCorrectY(){
  if (Y_DELTA > YZ_DELTA_MAX){
    Y_DELTA = YZ_DELTA_MAX; 
  } else if (Y_DELTA < YZ_DELTA_MIN){
    Y_DELTA = YZ_DELTA_MIN;
  }
}

void pedometer_update() {
  if (startedSession) {
    X_DELTA_TEMP = abs(abs(currX) - abs(lastX));
    if (X_DELTA_TEMP >= X_DELTA) {
      validX = true;
    }
    Y_DELTA_TEMP = abs(abs(currY) - abs(lastY));
    if (Y_DELTA_TEMP >= Y_DELTA) {
      validY = true;
      if (Y_DELTA_TEMP - Y_DELTA > 200){
        autoCorrectY();
        Y_DELTA = (Y_DELTA < YZ_DELTA_MAX) ? Y_DELTA + PED_ADJUST : Y_DELTA;
      } else if (Y_DELTA - Y_DELTA_TEMP > 175){
        autoCorrectY();
        Y_DELTA = (Y_DELTA > YZ_DELTA_MIN) ? Y_DELTA - PED_ADJUST : Y_DELTA;
      }
    }
    Z_DELTA_TEMP = abs(abs(currZ) - abs(lastZ));
    if (abs(abs(currZ) - abs(lastZ)) >= Z_DELTA) {
      validZ = true;
      if (Z_DELTA_TEMP - Z_DELTA > 200){
        autoCorrectZ();
        Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
      } else if (Z_DELTA - Z_DELTA_TEMP > 175){
        autoCorrectZ();
        Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
      }
    }
  } else {
    startedSession = true;
  }
}

void resetUpdate() {
  lastX = currX;
  lastY = currY;
  lastZ = currZ;
  validX = false;
  validY = false;
  validZ = false;
}

void update_ui_callback() {
  if ((validX && validY && !did_pebble_vibrate) || (validX && validZ && !did_pebble_vibrate)) {
    step_count++;
    send_to_foreground();
    save_data();
  }
  resetUpdate();
}

static void timer_callback(void *data) {
  AccelData accel = (AccelData ) { .x = 0, .y = 0, .z = 0 };
  accel_service_peek(&accel);

  if (!startedSession) {
    lastX = accel.x;
    lastY = accel.y;
    lastZ = accel.z;
  } else {
    currX = accel.x;
    currY = accel.y;
    currZ = accel.z;
  }
  
  did_pebble_vibrate = accel.did_vibrate;

  pedometer_update();
  update_ui_callback();

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void init() {
  // Initialize your worker here
  step_count = persist_read_int(PERSIST_KEY_STEPS);
  accel_data_service_subscribe(0, NULL);
  
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);

}

static void deinit() {
  // Deinitialize your worker here
}

int main(void) {
  init();
  worker_event_loop();
  deinit();
}


