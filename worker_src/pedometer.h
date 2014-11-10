#pragma once
#include <pebble.h>

void pedometer_update();
void resetUpdate();
void update_ui_callback();
static void timer_callback(void *data);

 