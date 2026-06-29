#include "keyboard_service.hpp"

KeyboardService::KeyboardService(std::unique_ptr<IKeyboardBackend> backend)
    : backend_(std::move(backend)) {}

FreezeResult KeyboardService::freeze() {
    return backend_->freeze();
}

void KeyboardService::unfreeze() {
    backend_->unfreeze();
}

AppState KeyboardService::state() const {
    return backend_->state();
}
