#include "udev_monitor.hpp"
#include "logger.hpp"

#include <cstring>
#include <glib-unix.h>
#include <libudev.h>

UdevMonitor::UdevMonitor() {
    init_udev();
}

UdevMonitor::~UdevMonitor() {
    stop();
    cleanup_udev();
}

bool UdevMonitor::init_udev() {
    udev_ = udev_new();
    if (!udev_) {
        Logger::error("UdevMonitor: failed to create udev context");
        return false;
    }

    monitor_ = udev_monitor_new_from_netlink(udev_, "udev");
    if (!monitor_) {
        Logger::error("UdevMonitor: failed to create monitor");
        udev_unref(udev_);
        udev_ = nullptr;
        return false;
    }

    udev_monitor_filter_add_match_subsystem_devtype(monitor_, "input", nullptr);

    Logger::info("UdevMonitor: initialized");
    return true;
}

void UdevMonitor::cleanup_udev() {
    if (monitor_) {
        udev_monitor_unref(monitor_);
        monitor_ = nullptr;
    }
    if (udev_) {
        udev_unref(udev_);
        udev_ = nullptr;
    }
}

bool UdevMonitor::start(DeviceCallback callback) {
    if (running_) {
        return true;
    }

    if (!monitor_) {
        Logger::error("UdevMonitor: not initialized, cannot start");
        return false;
    }

    callback_ = std::move(callback);

    udev_monitor_enable_receiving(monitor_);
    int fd = udev_monitor_get_fd(monitor_);
    if (fd < 0) {
        Logger::error("UdevMonitor: failed to get monitor fd");
        return false;
    }

    source_id_ = g_unix_fd_add(fd, G_IO_IN, on_udev_event, this);
    running_ = true;

    Logger::info("UdevMonitor: started (fd=%d)", fd);
    return true;
}

void UdevMonitor::stop() {
    if (!running_) {
        return;
    }

    if (source_id_ > 0) {
        g_source_remove(source_id_);
        source_id_ = 0;
    }

    running_ = false;
    callback_ = nullptr;

    Logger::info("UdevMonitor: stopped");
}

bool UdevMonitor::is_running() const {
    return running_;
}

gboolean UdevMonitor::on_udev_event(gint , GIOCondition , gpointer data) {
    auto* self = static_cast<UdevMonitor*>(data);

    struct udev_device* dev = udev_monitor_receive_device(self->monitor_);
    if (!dev) {
        return G_SOURCE_CONTINUE;
    }

    const char* action = udev_device_get_action(dev);
    const char* devnode = udev_device_get_devnode(dev);

    if (!action || !devnode) {
        udev_device_unref(dev);
        return G_SOURCE_CONTINUE;
    }

    if (std::strncmp(devnode, "/dev/input/event", 16) != 0) {
        udev_device_unref(dev);
        return G_SOURCE_CONTINUE;
    }

    const char* is_keyboard = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");
    if (!is_keyboard || std::strcmp(is_keyboard, "1") != 0) {
        udev_device_unref(dev);
        return G_SOURCE_CONTINUE;
    }

    const char* is_mouse = udev_device_get_property_value(dev, "ID_INPUT_MOUSE");
    const char* is_touchpad = udev_device_get_property_value(dev, "ID_INPUT_TOUCHPAD");
    const char* is_joystick = udev_device_get_property_value(dev, "ID_INPUT_JOYSTICK");

    if ((is_mouse && std::strcmp(is_mouse, "1") == 0) ||
        (is_touchpad && std::strcmp(is_touchpad, "1") == 0) ||
        (is_joystick && std::strcmp(is_joystick, "1") == 0)) {
        udev_device_unref(dev);
        return G_SOURCE_CONTINUE;
    }

    std::string path(devnode);
    bool added = (std::strcmp(action, "add") == 0);
    bool removed = (std::strcmp(action, "remove") == 0);

    udev_device_unref(dev);

    if (self->callback_ && (added || removed)) {
        Logger::info("UdevMonitor: device %s %s", path.c_str(),
                     added ? "added" : "removed");
        self->callback_(path, added);
    }

    return G_SOURCE_CONTINUE;
}
