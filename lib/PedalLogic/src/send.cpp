#include "send.h"

/**
 * @brief Constructs a Send action
 * 
 * @param bleKeyboard Pointer to BLE keyboard interface
 */
Send::Send(IBleKeyboard* bleKeyboard) {
    this->bleKeyboard = bleKeyboard;
}

/**
 * @brief Constructs a SendKey action
 * 
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k USB HID key code to send
 */
SendKey::SendKey(IBleKeyboard* bleKeyboard, uint8_t k) : Send(bleKeyboard) {
    key = k;
}

/**
 * @brief Executes the key send action
 * 
 * Sends the configured USB HID key code to the BLE keyboard.
 */
void SendKey::send() {
    bleKeyboard->write(key);
}


/**
 * @brief Constructs a SendMediaKey action
 * 
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k Media key report to send
 */
SendMediaKey::SendMediaKey(IBleKeyboard* bleKeyboard, const MediaKeyReport k) : Send(bleKeyboard) {
    key[0]=k[0];
    key[1]=k[1];
}

/**
 * @brief Executes the media key send action
 * 
 * Sends the configured media key report to the BLE keyboard.
 */
void SendMediaKey::send() {
    bleKeyboard->write(key);
}


/**
 * @brief Constructs a SendChar action
 * 
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param k Character to send
 */
SendChar::SendChar(IBleKeyboard* bleKeyboard, char k) : Send(bleKeyboard) {
    key = k;
}

/**
 * @brief Executes the character send action
 * 
 * Sends the configured character to the BLE keyboard.
 */
void SendChar::send() {
    bleKeyboard->write(key);
}

/**
 * @brief Constructs a SendString action
 * 
 * @param bleKeyboard Pointer to BLE keyboard interface
 * @param t Text string to send
 */
SendString::SendString(IBleKeyboard* bleKeyboard, std::string t) : Send(bleKeyboard) {
    text = t;
}

/**
 * @brief Executes the string send action
 * 
 * Sends the configured text string to the BLE keyboard.
 */
void SendString::send() {
    bleKeyboard->print(text.c_str());
}
