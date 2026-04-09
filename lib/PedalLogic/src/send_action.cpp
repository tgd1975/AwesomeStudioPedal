#include "send_action.h"

/**
 * @brief Constructs a SendAction
 *
 * @param bleKeyboard Pointer to BLE keyboard interface
 */
SendAction::SendAction(IBleKeyboard* bleKeyboard) { this->bleKeyboard = bleKeyboard; }

/**
 * @brief Constructs a SendKeyAction
 *
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k USB HID key code to send
 */
SendKeyAction::SendKeyAction(IBleKeyboard* bleKeyboard, uint8_t k) : SendAction(bleKeyboard)
{
    key = k;
}

/**
 * @brief Executes the key send action
 *
 * Sends the configured USB HID key code to the BLE keyboard.
 */
void SendKeyAction::send() { bleKeyboard->write(key); }

/**
 * @brief Constructs a SendMediaKeyAction
 *
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k Media key report to send
 */
SendMediaKeyAction::SendMediaKeyAction(IBleKeyboard* bleKeyboard, const MediaKeyReport k)
    : SendAction(bleKeyboard)
{
    key[0] = k[0];
    key[1] = k[1];
}

/**
 * @brief Executes the media key send action
 *
 * Sends the configured media key report to the BLE keyboard.
 */
void SendMediaKeyAction::send() { bleKeyboard->write(key); }

/**
 * @brief Constructs a SendCharAction
 *
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k Character to send
 */
SendCharAction::SendCharAction(IBleKeyboard* bleKeyboard, char k) : SendAction(bleKeyboard)
{
    key = k;
}

/**
 * @brief Executes the character send action
 *
 * Sends the configured character to the BLE keyboard.
 */
void SendCharAction::send() { bleKeyboard->write(key); }

/**
 * @brief Constructs a SendStringAction
 *
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param t Text string to send
 */
SendStringAction::SendStringAction(IBleKeyboard* bleKeyboard, std::string t)
    : SendAction(bleKeyboard)
{
    text = t;
}

/**
 * @brief Executes the string send action
 *
 * Sends the configured text string to the BLE keyboard.
 */
void SendStringAction::send() { bleKeyboard->print(text.c_str()); }
