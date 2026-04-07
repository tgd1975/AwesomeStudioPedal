#include <Arduino.h>
#include <memory>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
//#include "FunctionalInterrupt.h"

#include <BleKeyboard.h>

#include "button.h"
#include "send.h"
#include "hardware/led_controller.h"
#include "hardware/button_controller.h"

//Se the name of the bluetooth keyboard (that shows up in the bluetooth menu of your device)
BleKeyboard bleKeyboard("Strix-Pedal", "Strix");
boolean connected = false;

// Hardware abstraction instances
LEDController ledBluetooth(GPIO_NUM_26);
LEDController ledPower(GPIO_NUM_25);
LEDController ledSelect1(GPIO_NUM_5);
LEDController ledSelect2(GPIO_NUM_18);
LEDController ledSelect3(GPIO_NUM_19);

ButtonController buttonSelect(GPIO_NUM_21);
ButtonController buttonA(GPIO_NUM_13);
ButtonController buttonB(GPIO_NUM_12);
ButtonController buttonC(GPIO_NUM_27);
ButtonController buttonD(GPIO_NUM_14);

#define SHIFT 0x80

//SendMediaKey SEND_A = SendMediaKey(&bleKeyboard, KEY_MEDIA_PLAY_PAUSE);
SendString SEND_A = SendString(&bleKeyboard, " ");
SendMediaKey SEND_B = SendMediaKey(&bleKeyboard, KEY_MEDIA_STOP);
SendChar SEND_C = SendChar(&bleKeyboard, KEY_LEFT_ARROW);  // '-'
SendChar SEND_D = SendChar(&bleKeyboard, KEY_RIGHT_ARROW);  // '+'

// Arrays to hold pointers to the different actions for each bank
std::unique_ptr<Send> bankActionsA[3];
std::unique_ptr<Send> bankActionsB[3];
std::unique_ptr<Send> bankActionsC[3];
std::unique_ptr<Send> bankActionsD[3];

int currentBank = 0; // Tracks active bank: 0, 1, or 2

void IRAM_ATTR isr_a() {
  if (connected) {BUTTON_A.isr();}
}
void IRAM_ATTR isr_b() {
  if (connected) {BUTTON_B.isr();}
}
void IRAM_ATTR isr_c() {
  if (connected) {BUTTON_C.isr();}
}
void IRAM_ATTR isr_d() {
  if (connected) {BUTTON_D.isr();}
}

void IRAM_ATTR isr_select() {
  BUTTON_SELECT.isr();
}

void setup_hardware() {
    // Setup LEDs
    ledPower.setup(1);
    ledBluetooth.setup(0);
    ledSelect1.setup(1);
    ledSelect2.setup(1);
    ledSelect3.setup(1);
    
    // Setup buttons
    buttonA.setup();
    buttonB.setup();
    buttonC.setup();
    buttonD.setup();
    buttonSelect.setup();
}

void updateBankLEDs() {
    ledSelect1.setState(currentBank == 0);
    ledSelect2.setState(currentBank == 1);
    ledSelect3.setState(currentBank == 2);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("started");

  setup_hardware();

  bleKeyboard.begin();

  // Initialize bank actions
  // Bank 0
  bankActionsA[0] = std::make_unique<SendString>(&bleKeyboard, " ");
  bankActionsB[0] = std::make_unique<SendMediaKey>(&bleKeyboard, KEY_MEDIA_STOP);
  bankActionsC[0] = std::make_unique<SendChar>(&bleKeyboard, KEY_LEFT_ARROW);
  bankActionsD[0] = std::make_unique<SendChar>(&bleKeyboard, KEY_RIGHT_ARROW);

  // Bank 1
  bankActionsA[1] = std::make_unique<SendString>(&bleKeyboard, "Hello");
  bankActionsB[1] = std::make_unique<SendString>(&bleKeyboard, "World");
  bankActionsC[1] = std::make_unique<SendKey>(&bleKeyboard, KEY_UP_ARROW);
  bankActionsD[1] = std::make_unique<SendKey>(&bleKeyboard, KEY_DOWN_ARROW);

  // Bank 2
  bankActionsA[2] = std::make_unique<SendString>(&bleKeyboard, "Bank 2 A");
  bankActionsB[2] = std::make_unique<SendString>(&bleKeyboard, "Bank 2 B");
  bankActionsC[2] = std::make_unique<SendString>(&bleKeyboard, "Bank 2 C");
  bankActionsD[2] = std::make_unique<SendString>(&bleKeyboard, "Bank 2 D");
  
  currentBank = 0;
  updateBankLEDs();

}

void attachInterrupts() {
  BUTTON_A.pressed = false;
  attachInterrupt(BUTTON_A.PIN, isr_a, FALLING);
  BUTTON_B.pressed = false;
  attachInterrupt(BUTTON_B.PIN, isr_b, FALLING);
  BUTTON_C.pressed = false;
  attachInterrupt(BUTTON_C.PIN, isr_c, FALLING);
  BUTTON_D.pressed = false;
  attachInterrupt(BUTTON_D.PIN, isr_d, FALLING);
  BUTTON_SELECT.pressed = false;
  attachInterrupt(BUTTON_SELECT.PIN, isr_select, FALLING);
}

void detachInterrupts() {
  BUTTON_A.pressed = false;
  detachInterrupt(BUTTON_A.PIN);
  BUTTON_B.pressed = false;
  detachInterrupt(BUTTON_B.PIN);
  BUTTON_C.pressed = false;
  detachInterrupt(BUTTON_C.PIN);
  BUTTON_D.pressed = false;
  detachInterrupt(BUTTON_D.PIN);
  BUTTON_SELECT.pressed = false;
  detachInterrupt(BUTTON_SELECT.PIN);
}

void process_events() {
    if (BUTTON_A.event()) {
      bankActionsA[currentBank]->send();
    }
    if (BUTTON_B.event()) {
      bankActionsB[currentBank]->send();
    }
    if (BUTTON_C.event()) {
      bankActionsC[currentBank]->send();
    }
    if (BUTTON_D.event()) {
      bankActionsD[currentBank]->send();
    }

    if (BUTTON_SELECT.event()) {
      currentBank = (currentBank + 1) % 3;
      updateBankLEDs();
      Serial.printf("Switched to Bank %d\n", currentBank + 1);
    }

}

void loop() {
  // put your main code here, to run repeatedly:
  if (bleKeyboard.isConnected()) {
    if (!connected) {
      Serial.println("connected");
      attachInterrupts();
      ledBluetooth.setState(1);
      connected = true;
    }

    process_events();

  } else {
    if (connected) {
      Serial.println("disconnected");
      connected = false;
      detachInterrupts();
      ledBluetooth.setState(0);
    }

  }
  delay(10);
}
