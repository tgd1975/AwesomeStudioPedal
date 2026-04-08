#include "pedal_config.h"
#include "send.h"
#include <memory>

/**
 * @brief Configures the three button banks with default actions
 *
 * Sets up the pedal's button banks with predefined actions:
 * - Bank 0: Space, Media Stop, Left Arrow, Right Arrow
 * - Bank 1: "Hello", "World", Up Arrow, Down Arrow
 * - Bank 2: "Bank 2 A", "Bank 2 B", "Bank 2 C", "Bank 2 D"
 *
 * @param bankManager Reference to the BankManager instance
 * @param keyboard Pointer to the BLE keyboard interface
 */
void configureBanks(BankManager& bankManager, IBleKeyboard* keyboard)
{
    // Bank 0
    bankManager.addAction(0, 0, std::unique_ptr<Send>(new SendString(keyboard, " ")));
    bankManager.addAction(0, 1, std::unique_ptr<Send>(new SendMediaKey(keyboard, KEY_MEDIA_STOP)));
    bankManager.addAction(0, 2, std::unique_ptr<Send>(new SendChar(keyboard, KEY_LEFT_ARROW)));
    bankManager.addAction(0, 3, std::unique_ptr<Send>(new SendChar(keyboard, KEY_RIGHT_ARROW)));

    // Bank 1
    bankManager.addAction(1, 0, std::unique_ptr<Send>(new SendString(keyboard, "Hello")));
    bankManager.addAction(1, 1, std::unique_ptr<Send>(new SendString(keyboard, "World")));
    bankManager.addAction(1, 2, std::unique_ptr<Send>(new SendKey(keyboard, KEY_UP_ARROW)));
    bankManager.addAction(1, 3, std::unique_ptr<Send>(new SendKey(keyboard, KEY_DOWN_ARROW)));

    // Bank 2
    bankManager.addAction(2, 0, std::unique_ptr<Send>(new SendString(keyboard, "Bank 2 A")));
    bankManager.addAction(2, 1, std::unique_ptr<Send>(new SendString(keyboard, "Bank 2 B")));
    bankManager.addAction(2, 2, std::unique_ptr<Send>(new SendString(keyboard, "Bank 2 C")));
    bankManager.addAction(2, 3, std::unique_ptr<Send>(new SendString(keyboard, "Bank 2 D")));
}
