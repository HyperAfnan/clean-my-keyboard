#pragma once

#include "keyboard_backend.hpp"
#include "types.hpp"

#include <memory>

class KeyboardService {
public:
    explicit KeyboardService(std::unique_ptr<IKeyboardBackend> backend);

    FreezeResult freeze();
    void unfreeze();
    AppState state() const;

private:
    std::unique_ptr<IKeyboardBackend> backend_;
};
