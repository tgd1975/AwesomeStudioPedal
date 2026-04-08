#include <Arduino.h>
#include <memory>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "FunctionalInterrupt.h"

#include "platform.h"
#include "ble_keyboard_adapter.h"

#include "button.h"
#include "send.h"
#include "led_controller.h"
#include "bank_manager.h"
#include "pedal_config.h"
#include "config.h"
#include "event_dispatcher.h"

/**
 * @file main.cpp
 * @brief Main application entry point for Awesome Guitar Pedal
 * 
 * This file contains the setup, loop, and core application logic
 * for the ESP32-based guitar pedal controller.
 */

/**
 * @brief Bluetooth keyboard instance
 * 
 * Represents the BLE keyboard that appears as "Strix-Pedal" in device menus.
 */
BleKeyboard bleKeyboard("Strix-Pedal", "Strix");

/**
 * @brief BLE keyboard adapter
 * 
 * Wrapper around BleKeyboard for easier integration with pedal logic.
 */
BleKeyboardAdapter bleKeyboardAdapter(bleKeyboard);

/**
 * @brief Bluetooth connection status flag
 * 
 * Tracks whether the pedal is currently connected to a host device.
 */
bool connected = false;

/**
 * @brief LED controllers for visual feedback
 * 
 * Each LED controller manages a specific indicator light on the pedal.
 */
LEDController ledBluetooth(hardwareConfig.ledBluetooth); /**< Bluetooth status LED */
LEDController ledPower(hardwareConfig.ledPower);      /**< Power indicator LED */
LEDController ledSelect1(hardwareConfig.ledSelect1);  /**< Bank 1 selection LED */
LEDController ledSelect2(hardwareConfig.ledSelect2);  /**< Bank 2 selection LED */
LEDController ledSelect3(hardwareConfig.ledSelect3);  /**< Bank 3 selection LED */

/**
 * @brief Button instances for user input
 * 
 * Each button is configured with its respective GPIO pin and handles
 * interrupt-based press detection with debouncing.
 */
Button BUTTON_SELECT = Button(hardwareConfig.buttonSelect); /**< Bank selection button */
Button BUTTON_A = Button(hardwareConfig.buttonA);       /**< Action button A */
Button BUTTON_B = Button(hardwareConfig.buttonB);       /**< Action button B */
Button BUTTON_C = Button(hardwareConfig.buttonC);       /**< Action button C */
Button BUTTON_D = Button(hardwareConfig.buttonD);       /**< Action button D */

/**
 * @brief Bank management system
 * 
 * Handles the three configurable button banks and visual feedback.
 */
BankManager bankManager(ledSelect1, ledSelect2, ledSelect3);

/**
 * @brief Event handling system
 * 
 * Dispatches button press events to appropriate handlers.
 */
EventDispatcher eventDispatcher;

/**
 * @brief Interrupt service routine for Button A
 * 
 * Called when Button A GPIO pin detects a falling edge.
 * Only processes the interrupt if Bluetooth is connected.
 */
void IRAM_ATTR isr_a() {
  if (connected) {BUTTON_A.isr();}
}

/**
 * @brief Interrupt service routine for Button B
 * 
 * Called when Button B GPIO pin detects a falling edge.
 * Only processes the interrupt if Bluetooth is connected.
 */
void IRAM_ATTR isr_b() {
  if (connected) {BUTTON_B.isr();}
}

/**
 * @brief Interrupt service routine for Button C
 * 
 * Called when Button C GPIO pin detects a falling edge.
 * Only processes the interrupt if Bluetooth is connected.
 */
void IRAM_ATTR isr_c() {
  if (connected) {BUTTON_C.isr();}
}

/**
 * @brief Interrupt service routine for Button D
 * 
 * Called when Button D GPIO pin detects a falling edge.
 * Only processes the interrupt if Bluetooth is connected.
 */
void IRAM_ATTR isr_d() {
  if (connected) {BUTTON_D.isr();}
}

/**
 * @brief Interrupt service routine for Select Button
 * 
 * Called when Select Button GPIO pin detects a falling edge.
 * Processes the interrupt regardless of Bluetooth connection status
 * since bank switching should work even when disconnected.
 */
void IRAM_ATTR isr_select() {
  BUTTON_SELECT.isr();
}

/**
 * @brief Initializes all hardware components
 * 
 * Configures GPIO pins for LEDs and buttons, sets initial LED states.
 */
void setup_hardware() {
    // Setup LEDs
    ledPower.setup(1);      // Turn on power LED
    ledBluetooth.setup(0);   // Turn off Bluetooth LED initially
    ledSelect1.setup(1);    // Turn on Bank 1 LED (default bank)
    ledSelect2.setup(1);    // Turn off Bank 2 LED
    ledSelect3.setup(1);    // Turn off Bank 3 LED
    
    // Setup buttons (configure GPIO pins as input with pull-up)
    BUTTON_SELECT.setup();
    BUTTON_A.setup();
    BUTTON_B.setup();
    BUTTON_C.setup();
    BUTTON_D.setup();
}

/**
 * @brief Configures event handlers for all buttons
 * 
 * Sets up callbacks that are triggered when buttons are pressed.
 * Handles both action buttons (A-D) and bank selection button.
 */
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
        uint8_t bank = bankManager.switchBank();
        Serial.printf("Switched to Bank %d\n", bank + 1);
    });
}

/**
 * @brief Arduino setup function - runs once at startup
 * 
 * Initializes serial communication, hardware components, event handlers,
 * Bluetooth keyboard, and button configurations.
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("started");

  setup_hardware();
  setup_event_handlers();

  bleKeyboardAdapter.begin();

  configureBanks(bankManager, &bleKeyboardAdapter);

}

/**
 * @brief Attaches interrupt handlers to all buttons
 * 
 * Configures hardware interrupts for falling edge detection on all button pins.
 * Called when Bluetooth connection is established.
 */
void attachInterrupts() {
  BUTTON_A.reset();
  attachInterrupt(hardwareConfig.buttonA, isr_a, FALLING);
  BUTTON_B.reset();
  attachInterrupt(hardwareConfig.buttonB, isr_b, FALLING);
  BUTTON_C.reset();
  attachInterrupt(hardwareConfig.buttonC, isr_c, FALLING);
  BUTTON_D.reset();
  attachInterrupt(hardwareConfig.buttonD, isr_d, FALLING);
  BUTTON_SELECT.reset();
  attachInterrupt(hardwareConfig.buttonSelect, isr_select, FALLING);
}

/**
 * @brief Detaches interrupt handlers from all buttons
 * 
 * Removes hardware interrupts for all button pins.
 * Called when Bluetooth connection is lost.
 */
void detachInterrupts() {
  BUTTON_A.reset();
  detachInterrupt(hardwareConfig.buttonA);
  BUTTON_B.reset();
  detachInterrupt(hardwareConfig.buttonB);
  BUTTON_C.reset();
  detachInterrupt(hardwareConfig.buttonC);
  BUTTON_D.reset();
  detachInterrupt(hardwareConfig.buttonD);
  BUTTON_SELECT.reset();
  detachInterrupt(hardwareConfig.buttonSelect);
}

/**
 * @brief Processes button events and dispatches them to handlers
 * 
 * Checks each button for press events and triggers the appropriate
 * event handler through the event dispatcher.
 */
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

/**
 * @brief Arduino main loop - runs repeatedly
 * 
 * Handles Bluetooth connection state changes, processes button events,
 * and manages interrupt attachment/detachment based on connection status.
 */
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
