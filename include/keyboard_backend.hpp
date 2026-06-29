#pragma once

#include "types.hpp"

class IKeyboardBackend {
public:
    virtual ~IKeyboardBackend() = default;

    virtual FreezeResult freeze() = 0;
    virtual void unfreeze() = 0;
    virtual AppState state() const = 0;
};
