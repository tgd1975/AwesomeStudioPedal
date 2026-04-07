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
#include "bank_manager.h"
#include "config.h"

//Se the name of the bluetooth keyboard (that shows up in the bluetooth menu of your device)
BleKeyboard bleKeyboard("Strix-Pedal", "Strix");
boolean connected = false;

// Hardware abstraction instances using config
LEDController ledBluetooth(hardwareConfig.ledBluetooth);
LEDController ledPower(hardwareConfig.ledPower);
LEDController ledSelect1(hardwareConfig.ledSelect1);
LEDController ledSelect2(hardwareConfig.ledSelect2);
LEDController ledSelect3(hardwareConfig.ledSelect3);

ButtonController buttonSelect(hardwareConfig.buttonSelect);
ButtonController buttonA(hardwareConfig.buttonA);
ButtonController buttonB(hardwareConfig.buttonB);
ButtonController buttonC(hardwareConfig.buttonC);
ButtonController buttonD(hardwareConfig.buttonD);

// Bank management system
BankManager bankManager(ledSelect1, ledSelect2, ledSelect3);

#define SHIFT 0x80

//SendMediaKey SEND_A = SendMediaKey(&bleKeyboard, KEY_MEDIA_PLAY_PAUSE);
SendString SEND_A = SendString(&bleKeyboard, " ");
SendMediaKey SEND_B = SendMediaKey(&bleKeyboard, KEY_MEDIA_STOP);
SendChar SEND_C = SendChar(&bleKeyboard, KEY_LEFT_ARROW);  // '-'
SendChar SEND_D = SendChar(&bleKeyboard, KEY_RIGHT_ARROW);  // '+'

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("started");

  setup_hardware();

  bleKeyboard.begin();

  // Initialize bank actions using BankManager
  // Bank 0
  bankManager.addAction(0, 0, std::make_unique<SendString>(&bleKeyboard, " "));
  bankManager.addAction(0, 1, std::make_unique<SendMediaKey>(&bleKeyboard, KEY_MEDIA_STOP));
  bankManager.addAction(0, 2, std::make_unique<SendChar>(&bleKeyboard, KEY_LEFT_ARROW));
  bankManager.addAction(0, 3, std::make_unique<SendChar>(&bleKeyboard, KEY_RIGHT_ARROW));

  // Bank 1
  bankManager.addAction(1, 0, std::make_unique<SendString>(&bleKeyboard, "Hello"));
  bankManager.addAction(1, 1, std::make_unique<SendString>(&bleKeyboard, "World"));
  bankManager.addAction(1, 2, std::make_unique<SendKey>(&bleKeyboard, KEY_UP_ARROW));
  bankManager.addAction(1, 3, std::make_unique<SendKey>(&bleKeyboard, KEY_DOWN_ARROW));

  // Bank 2
  bankManager.addAction(2, 0, std::make_unique<SendString>(&bleKeyboard, "Bank 2 A"));
  bankManager.addAction(2, 1, std::make_unique<SendString>(&bleKeyboard, "Bank 2 B"));
  bankManager.addAction(2, 2, std::make_unique<SendString>(&bleKeyboard, "Bank 2 C"));
  bankManager.addAction(2, 3, std::make_unique<SendString>(&bleKeyboard, "Bank 2 D"));
  
  bankManager.updateLEDs();

}

void attachInterrupts() {
  BUTTON_A.pressed = false;
  attachInterrupt(hardwareConfig.buttonA, isr_a, FALLING);
  BUTTON_B.pressed = false;
  attachInterrupt(hardwareConfig.buttonB, isr_b, FALLING);
  BUTTON_C.pressed = false;
  attachInterrupt(hardwareConfig.buttonC, isr_c, FALLING);
  BUTTON_D.pressed = false;
  attachInterrupt(hardwareConfig.buttonD, isr_d, FALLING);
  BUTTON_SELECT.pressed = false;
  attachInterrupt(hardwareConfig.buttonSelect, isr_select, FALLING);
}

void detachInterrupts() {
  BUTTON_A.pressed = false;
  detachInterrupt(hardwareConfig.buttonA);
  BUTTON_B.pressed = false;
  detachInterrupt(hardwareConfig.buttonB);
  BUTTON_C.pressed = false;
  detachInterrupt(hardwareConfig.buttonC);
  BUTTON_D.pressed = false;
  detachInterrupt(hardwareConfig.buttonD);
  BUTTON_SELECT.pressed = false;
  detachInterrupt(hardwareConfig.buttonSelect);
}

void process_events() {
    if (BUTTON_A.event()) {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 0)) {
            action->send();
        }
    }
    if (BUTTON_B.event()) {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 1)) {
            action->send();
        }
    }
    if (BUTTON_C.event()) {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 2)) {
            action->send();
        }
    }
    if (BUTTON_D.event()) {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 3)) {
            action->send();
        }
    }

    if (BUTTON_SELECT.event()) {
      bankManager.switchBank();
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
