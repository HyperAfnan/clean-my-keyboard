#pragma once

#include "keyboard_backend.hpp"
#include "types.hpp"

#include <glib.h>

class PolkitBackend : public IKeyboardBackend {
public:
    PolkitBackend();
    ~PolkitBackend() override;

    FreezeResult freeze() override;
    void unfreeze() override;
    AppState state() const override;

private:
    AppState state_ = AppState::Active;
    GPid child_pid_ = 0;
    int stdin_fd_ = -1;
    int stdout_fd_ = -1;
};
