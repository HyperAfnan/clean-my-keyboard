#include "evdev_backend.hpp"
#include "device_manager.hpp"
#include "device_validator.hpp"
#include "logger.hpp"

#include <cerrno>
#include <string>

EvdevBackend::EvdevBackend() {
    Logger::info("EvdevBackend created");
}

EvdevBackend::~EvdevBackend() {
    if (state_ == AppState::Frozen) {
        Logger::warn("EvdevBackend destroyed while frozen — forcing unfreeze");
        unfreeze();
    }
}

FreezeResult EvdevBackend::freeze() {
    if (state_ != AppState::Active) {
        Logger::warn("freeze() called in invalid state, ignoring");
        return FreezeResult::GrabFailed;
    }

    state_ = AppState::Freezing;

    start_monitor();

    auto paths = discover_devices();
    if (paths.empty()) {
        monitor_.stop();
        state_ = AppState::Active;
        return FreezeResult::NoKeyboardsFound;
    }

    FreezeResult result = grab_devices(paths);
    if (result != FreezeResult::Success) {

        release_devices();
        devices_.clear();
        monitor_.stop();
        state_ = AppState::Active;
        return result;
    }

    state_ = AppState::Frozen;
    Logger::info("All keyboards frozen (%zu device(s))", devices_.size());
    return FreezeResult::Success;
}

void EvdevBackend::unfreeze() {
    if (state_ != AppState::Frozen) {
        return;
    }

    state_ = AppState::Unfreezing;

    monitor_.stop();

    release_devices();
    devices_.clear();

    state_ = AppState::Active;
    Logger::info("All keyboards unfrozen");
}

AppState EvdevBackend::state() const {
    return state_;
}

std::vector<std::string> EvdevBackend::discover_devices() {
    return DeviceManager::enumerate_keyboards();
}

FreezeResult EvdevBackend::grab_devices(const std::vector<std::string>& paths) {
    bool permission_denied = false;
    int grabbed_count = 0;

    for (const auto& path : paths) {

        if (devices_.count(path)) {
            grabbed_count++;
            continue;
        }

        auto device = std::make_unique<KeyboardDevice>(path);

        if (!device->open()) {

            if (errno == EACCES || errno == EPERM) {
                permission_denied = true;
            }
            Logger::warn("Skipping %s: failed to open", path.c_str());
            continue;
        }

        if (!device->grab()) {
            Logger::warn("Skipping %s: failed to grab (another program may own it)",
                         path.c_str());
            continue;
        }

        devices_[path] = std::move(device);
        grabbed_count++;
    }

    if (grabbed_count == 0) {
        if (permission_denied) {
            return FreezeResult::PermissionDenied;
        }
        return FreezeResult::GrabFailed;
    }

    Logger::info("Grabbed %d/%zu keyboard(s)", grabbed_count, paths.size());
    return FreezeResult::Success;
}

void EvdevBackend::release_devices() {
    for (auto& [path, device] : devices_) {
        device->release();
    }
}

void EvdevBackend::start_monitor() {
    monitor_.start([this](const std::string& devnode, bool added) {
        on_device_event(devnode, added);
    });
}

void EvdevBackend::on_device_event(const std::string& devnode, bool added) {
    if (added && state_ == AppState::Frozen) {

        if (devices_.count(devnode)) {
            Logger::info("Hotplug: %s already tracked, skipping", devnode.c_str());
            return;
        }

        if (!DeviceValidator::is_real_keyboard(devnode)) {
            return;
        }

        auto device = std::make_unique<KeyboardDevice>(devnode);
        if (device->open() && device->grab()) {
            Logger::info("Hotplug: grabbed new keyboard %s", devnode.c_str());
            devices_[devnode] = std::move(device);
        } else {
            Logger::warn("Hotplug: failed to grab %s", devnode.c_str());
        }
    } else if (!added) {
        auto it = devices_.find(devnode);
        if (it != devices_.end()) {
            Logger::info("Hotplug: keyboard %s removed", devnode.c_str());
            devices_.erase(it);
        }
    }
}
