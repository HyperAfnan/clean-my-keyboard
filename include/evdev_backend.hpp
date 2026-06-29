#pragma once

#include "keyboard_backend.hpp"
#include "keyboard_device.hpp"
#include "udev_monitor.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class EvdevBackend : public IKeyboardBackend {
public:
    EvdevBackend();
    ~EvdevBackend() override;

    FreezeResult freeze() override;
    void unfreeze() override;
    AppState state() const override;

private:
    AppState state_ = AppState::Active;
    std::unordered_map<std::string, std::unique_ptr<KeyboardDevice>> devices_;
    UdevMonitor monitor_;

    std::vector<std::string> discover_devices();

    FreezeResult grab_devices(const std::vector<std::string>& paths);

    void release_devices();

    void start_monitor();

    void on_device_event(const std::string& devnode, bool added);
};
