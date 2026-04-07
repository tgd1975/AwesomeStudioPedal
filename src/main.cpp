#include <Arduino.h>
#include <memory>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
//#include "FunctionalInterrupt.h"

#include <BleKeyboard.h>
#include "ble_keyboard_adapter.h"

#include "button.h"
#include "send.h"
#include "led_controller.h"
#include "button_controller.h"
#include "bank_manager.h"
#include "pedal_config.h"
#include "config.h"
#include "event_dispatcher.h"

//Se the name of the bluetooth keyboard (that shows up in the bluetooth menu of your device)
BleKeyboard bleKeyboard("Strix-Pedal", "Strix");
BleKeyboardAdapter bleKeyboardAdapter(bleKeyboard);
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

// Button instances for interrupt handling
Button BUTTON_SELECT = Button(hardwareConfig.buttonSelect);
Button BUTTON_A = Button(hardwareConfig.buttonA);
Button BUTTON_B = Button(hardwareConfig.buttonB);
Button BUTTON_C = Button(hardwareConfig.buttonC);
Button BUTTON_D = Button(hardwareConfig.buttonD);

// Bank management system
BankManager bankManager(ledSelect1, ledSelect2, ledSelect3);

// Event handling system
EventDispatcher eventDispatcher;

#define SHIFT 0x80

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

void setup_event_handlers() {
    // Register button event handlers
    eventDispatcher.registerHandler(0, []() {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 0)) {
            action->send();
        }
    });
    
    eventDispatcher.registerHandler(1, []() {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 1)) {
            action->send();
        }
    });
    
    eventDispatcher.registerHandler(2, []() {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 2)) {
            action->send();
        }
    });
    
    eventDispatcher.registerHandler(3, []() {
        if (auto action = bankManager.getAction(bankManager.getCurrentBank(), 3)) {
            action->send();
        }
    });
    
    eventDispatcher.registerHandler(4, []() {
        bankManager.switchBank();
    });
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("started");

  setup_hardware();
  setup_event_handlers();

  bleKeyboardAdapter.begin();

  configureBanks(bankManager, &bleKeyboardAdapter);

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
        eventDispatcher.dispatch(0);
    }
    if (BUTTON_B.event()) {
        eventDispatcher.dispatch(1);
    }
    if (BUTTON_C.event()) {
        eventDispatcher.dispatch(2);
    }
    if (BUTTON_D.event()) {
        eventDispatcher.dispatch(3);
    }

    if (BUTTON_SELECT.event()) {
        eventDispatcher.dispatch(4);
    }

}

void loop() {
  // put your main code here, to run repeatedly:
  if (bleKeyboardAdapter.isConnected()) {
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
