#include "event_dispatcher.h"

void EventDispatcher::registerHandler(uint8_t button, EventCallback callback) {
    if (button < handlers.size()) {
        handlers[button] = callback;
    }
}

void EventDispatcher::dispatch(uint8_t button) {
    if (button < handlers.size() && handlers[button]) {
        handlers[button]();
    }
}

void EventDispatcher::clearHandlers() {
    for (auto& handler : handlers) {
        handler = nullptr;
    }
}
