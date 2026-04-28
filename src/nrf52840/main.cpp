#include <Arduino.h>

#include "nrf52840_pedal_app.h"

static Nrf52840PedalApp app;

void setup() { app.setup(); }
void loop() { app.loop(); }
