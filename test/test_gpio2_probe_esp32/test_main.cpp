/**
 * GPIO 2 onboard-LED probe — ESP32 only.
 *
 * Some ESP32 dev boards (e.g. WeMos D1 R32, certain DOIT clones) carry a
 * blue user LED on GPIO 2; the Joy-IT SBC-NodeMCU-ESP32 reference design
 * does not. This probe answers "does my specific board have one?".
 *
 * Behaviour: drives GPIO 2 HIGH for 500 ms, LOW for 500 ms, forever.
 * Each transition prints a line to serial so you can correlate the
 * blink (if any) with the firmware actually running.
 *
 * Strapping-pin note: GPIO 2 must be LOW or floating at reset for normal
 * boot. We only set OUTPUT after Serial is up (well past reset), so this
 * probe does not interfere with flashing. If you ever wire an external
 * pull-up to GPIO 2, expect bootloader-mode entry on power-on.
 *
 * Flash with: make test-esp32-gpio2-probe
 */

#include <Arduino.h>

static constexpr uint8_t PROBE_PIN = 2;
static constexpr uint32_t STEP_MS = 500;

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("[GPIO2_PROBE] toggling GPIO 2 every 500 ms");
    Serial.println("[GPIO2_PROBE] watch for a blue LED on the dev board");
    pinMode(PROBE_PIN, OUTPUT);
    digitalWrite(PROBE_PIN, LOW);
}

void loop()
{
    digitalWrite(PROBE_PIN, HIGH);
    Serial.println("GPIO 2 = HIGH");
    delay(STEP_MS);
    digitalWrite(PROBE_PIN, LOW);
    Serial.println("GPIO 2 = LOW");
    delay(STEP_MS);
}
