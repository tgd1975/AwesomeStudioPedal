#pragma once
#include "i_ble_keyboard.h"
#include <string>

/**
 * @class Send
 * @brief Base class for all sendable actions
 * 
 * Abstract base class that defines the interface for actions
 * that can be sent via BLE keyboard.
 */
class Send {
  public:
    /**
     * @brief Executes the send action
     * 
     * Sends the configured input to the BLE keyboard.
     */
    virtual void send() = 0;
    virtual ~Send() = default;

  protected:
    IBleKeyboard* bleKeyboard; /**< Pointer to BLE keyboard interface */
    
    /**
     * @brief Constructs a Send action
     * 
     * @param bleKeyboard Pointer to BLE keyboard interface
     */
    Send(IBleKeyboard* bleKeyboard);
};

/**
 * @class SendChar
 * @brief Sends a single character via BLE keyboard
 */
class SendChar : public Send {
  private:
    char key; /**< Character to send */

  public:
    /**
     * @brief Executes the character send action
     */
    void send() override;
    
    /**
     * @brief Constructs a SendChar action
     * 
     * @param bleKeyboard Pointer to BLE keyboard interface
     * @param k Character to send
     */
    SendChar(IBleKeyboard* bleKeyboard, char k);
};

/**
 * @class SendString
 * @brief Sends a text string via BLE keyboard
 */
class SendString : public Send {
  private:
    std::string text; /**< Text string to send */

  public:
    /**
     * @brief Executes the string send action
     */
    void send() override;
    
    /**
     * @brief Constructs a SendString action
     * 
     * @param bleKeyboard Pointer to BLE keyboard interface
     * @param t Text string to send
     */
    SendString(IBleKeyboard* bleKeyboard, std::string t);
};

/**
 * @class SendKey
 * @brief Sends a USB HID key code via BLE keyboard
 */
class SendKey : public Send {
  private:
    uint8_t key; /**< USB HID key code to send */

  public:
    /**
     * @brief Executes the key send action
     */
    void send() override;
    
    /**
     * @brief Constructs a SendKey action
     * 
     * @param bleKeyboard Pointer to BLE keyboard interface
     * @param k USB HID key code to send
     */
    SendKey(IBleKeyboard* bleKeyboard, uint8_t k);
};

/**
 * @class SendMediaKey
 * @brief Sends a media key report via BLE keyboard
 */
class SendMediaKey : public Send {
  private:
    MediaKeyReport key; /**< Media key report to send */

  public:
    /**
     * @brief Executes the media key send action
     */
    void send() override;
    
    /**
     * @brief Constructs a SendMediaKey action
     * 
     * @param bleKeyboard Pointer to BLE keyboard interface
     * @param k Media key report to send
     */
    SendMediaKey(IBleKeyboard* bleKeyboard, const MediaKeyReport k);
};
