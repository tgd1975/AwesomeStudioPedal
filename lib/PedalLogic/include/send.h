#pragma once
#include "i_ble_keyboard.h"
#include <string>

class Send {
  public:
    virtual void send() = 0;
    virtual ~Send() = default;

  protected:
    IBleKeyboard* bleKeyboard;
    Send(IBleKeyboard* bleKeyboard);
};

class SendChar : public Send {
  private:
    char key;

  public:
    void send() override;
    SendChar(IBleKeyboard* bleKeyboard, char k);
};

class SendString : public Send {
  private:
    std::string text;

  public:
    void send() override;
    SendString(IBleKeyboard* bleKeyboard, std::string t);
};

class SendKey : public Send {
  private:
    uint8_t key;

  public:
    void send() override;
    SendKey(IBleKeyboard* bleKeyboard, uint8_t k);
};

class SendMediaKey : public Send {
  private:
    MediaKeyReport key;

  public:
    void send() override;
    SendMediaKey(IBleKeyboard* bleKeyboard, const MediaKeyReport k);
};
