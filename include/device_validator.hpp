#pragma once

#include <string>

class DeviceValidator {
public:

    static bool is_real_keyboard(const std::string& devnode);
};
