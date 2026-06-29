#include "keyboard_device.hpp"
#include "device_manager.hpp"
#include "logger.hpp"

#include <csignal>
#include <iostream>
#include <vector>
#include <memory>
#include <unistd.h>
#include <sys/select.h>
#include <cerrno>

volatile sig_atomic_t keep_running = 1;

void signal_handler(int) {
    keep_running = 0;
}

int main() {
    Logger::info("Helper process started");

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    auto paths = DeviceManager::enumerate_keyboards();
    if (paths.empty()) {
        Logger::error("Helper: No keyboards found");
        std::cout << "FAIL\n" << std::flush;
        return 1;
    }

    std::vector<std::unique_ptr<KeyboardDevice>> grabbed_devices;
    bool permission_denied = false;

    for (const auto& path : paths) {
        auto device = std::make_unique<KeyboardDevice>(path);
        if (!device->open()) {
            if (errno == EACCES || errno == EPERM) {
                permission_denied = true;
            }
            Logger::warn("Helper: Skipping %s (failed to open)", path.c_str());
            continue;
        }
        if (!device->grab()) {
            Logger::warn("Helper: Skipping %s (failed to grab)", path.c_str());
            continue;
        }
        grabbed_devices.push_back(std::move(device));
    }

    if (grabbed_devices.empty()) {
        Logger::error("Helper: Failed to grab any keyboard");
        if (permission_denied) {
            std::cout << "FAIL\n" << std::flush;
        } else {
            std::cout << "FAIL\n" << std::flush;
        }
        return 1;
    }

    // Write READY to stdout and flush so the GUI process can proceed
    std::cout << "READY\n" << std::flush;
    Logger::info("Helper successfully grabbed %zu keyboards, waiting on stdin...", grabbed_devices.size());

    // Watch stdin for activity or EOF
    while (keep_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeval timeout{};
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ret = select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &timeout);
        if (ret > 0) {
            char buf;
            ssize_t bytes_read = read(STDIN_FILENO, &buf, 1);
            if (bytes_read <= 0) {
                Logger::info("GUI process disconnected (stdin closed), releasing keyboards");
                break;
            }
        } else if (ret < 0 && errno != EINTR) {
            Logger::error("select error on stdin: %d", errno);
            break;
        }
    }

    Logger::info("Helper process terminating cleanly");
    return 0;
}
