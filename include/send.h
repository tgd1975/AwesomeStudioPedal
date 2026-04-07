#include <Arduino.h>
#include <BleKeyboard.h>

class Send {
  public:
    virtual void send() = 0;
    virtual ~Send() = default;  // Add virtual destructor for proper cleanup
    BleKeyboard* bleKeyboard;
    Send(BleKeyboard* bleKeyboard);
};

class SendChar : public Send {
  private:
    char key;

  public:
    void send() override;
    SendChar(BleKeyboard* bleKeyboard, char k);
};

class SendString : public Send {
  private:
    String text;

  public:
    void send() override;
    SendString(BleKeyboard* bleKeyboard, String t);
};

class SendKey : public Send {
  private:
    uint8_t key;

  public:
    void send() override;
    SendKey(BleKeyboard* bleKeyboard, uint8_t k);
};

class SendMediaKey : public Send {
  private:
    MediaKeyReport key;

  public: 
    void send() override;
    SendMediaKey(BleKeyboard* bleKeyboard, const MediaKeyReport k);
};
