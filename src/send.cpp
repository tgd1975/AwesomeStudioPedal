#include "send.h"

Send::Send(IBleKeyboard* bleKeyboard) {
    this->bleKeyboard = bleKeyboard;
}

SendKey::SendKey(IBleKeyboard* bleKeyboard, uint8_t k) : Send(bleKeyboard) {
    key = k;
}
void SendKey::send() {
    bleKeyboard->write(key);
}


SendMediaKey::SendMediaKey(IBleKeyboard* bleKeyboard, const MediaKeyReport k) : Send(bleKeyboard) {
    key[0]=k[0];
    key[1]=k[1];
}
void SendMediaKey::send() {
    bleKeyboard->write(key);
}


SendChar::SendChar(IBleKeyboard* bleKeyboard, char k) : Send(bleKeyboard) {
    key = k;
}
void SendChar::send() {
    bleKeyboard->write(key);
}

SendString::SendString(IBleKeyboard* bleKeyboard, std::string t) : Send(bleKeyboard) {
    text = t;
}
void SendString::send() {
    bleKeyboard->print(text.c_str());
}
