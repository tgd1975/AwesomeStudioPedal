#include <Arduino.h>

#include "esp32_pedal_app.h"

static Esp32PedalApp app;

void setup() { app.setup(); }
void loop() { app.loop(); }
