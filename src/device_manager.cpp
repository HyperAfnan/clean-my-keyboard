#include "device_manager.hpp"
#include "device_validator.hpp"
#include "logger.hpp"

#include <cstring>
#include <libudev.h>

std::vector<std::string> DeviceManager::enumerate_keyboards() {
    std::vector<std::string> result;

    struct udev* udev = udev_new();
    if (!udev) {
        Logger::error("Failed to create udev context");
        return result;
    }

    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        Logger::error("Failed to create udev enumerate");
        udev_unref(udev);
        return result;
    }

    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEYBOARD", "1");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* entry;

    udev_list_entry_foreach(entry, devices) {
        const char* syspath = udev_list_entry_get_name(entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, syspath);
        if (!dev) continue;

        const char* devnode = udev_device_get_devnode(dev);
        if (!devnode) {
            udev_device_unref(dev);
            continue;
        }

        if (std::strncmp(devnode, "/dev/input/event", 16) != 0) {
            udev_device_unref(dev);
            continue;
        }

        const char* is_mouse = udev_device_get_property_value(dev, "ID_INPUT_MOUSE");
        const char* is_touchpad = udev_device_get_property_value(dev, "ID_INPUT_TOUCHPAD");
        const char* is_joystick = udev_device_get_property_value(dev, "ID_INPUT_JOYSTICK");

        if ((is_mouse && std::strcmp(is_mouse, "1") == 0) ||
            (is_touchpad && std::strcmp(is_touchpad, "1") == 0) ||
            (is_joystick && std::strcmp(is_joystick, "1") == 0)) {
            Logger::info("Skipping %s: matched negative filter (mouse/touchpad/joystick)",
                         devnode);
            udev_device_unref(dev);
            continue;
        }

        std::string path(devnode);
        udev_device_unref(dev);

        if (DeviceValidator::is_real_keyboard(path)) {
            result.push_back(std::move(path));
        }
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    Logger::info("Enumerated %zu keyboard device(s)", result.size());
    return result;
}
