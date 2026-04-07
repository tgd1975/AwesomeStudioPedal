#pragma once
#include <functional>
#include <array>

class EventDispatcher {
public:
    using EventCallback = std::function<void()>;
    
    void registerHandler(uint8_t button, EventCallback callback);
    void dispatch(uint8_t button);
    void clearHandlers();
    
private:
    std::array<EventCallback, 5> handlers{}; // 0-3 for buttons A-D, 4 for select
};EOF