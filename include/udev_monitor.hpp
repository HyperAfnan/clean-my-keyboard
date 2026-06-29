#pragma once

#include <glib.h>
#include <functional>
#include <string>

struct udev;
struct udev_monitor;

class UdevMonitor {
public:
    using DeviceCallback = std::function<void(const std::string& devnode, bool added)>;

    UdevMonitor();
    ~UdevMonitor();

    UdevMonitor(const UdevMonitor&) = delete;
    UdevMonitor& operator=(const UdevMonitor&) = delete;

    bool start(DeviceCallback callback);

    void stop();

    bool is_running() const;

private:
    struct udev* udev_ = nullptr;
    struct udev_monitor* monitor_ = nullptr;
    unsigned int source_id_ = 0;
    DeviceCallback callback_;
    bool running_ = false;

    bool init_udev();
    void cleanup_udev();

    static gboolean on_udev_event(gint fd, GIOCondition condition, gpointer data);
};
