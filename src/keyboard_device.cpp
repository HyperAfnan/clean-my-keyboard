#include "keyboard_device.hpp"
#include "logger.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

KeyboardDevice::KeyboardDevice(std::string devnode)
    : path_(std::move(devnode)) {}

KeyboardDevice::~KeyboardDevice() {
    release();
    close();
}

bool KeyboardDevice::open() {
    if (fd_ >= 0) {
        return true;
    }

    fd_ = ::open(path_.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd_ < 0) {
        Logger::error("Failed to open %s: %s", path_.c_str(), std::strerror(errno));
        return false;
    }

    Logger::info("Opened %s (fd=%d)", path_.c_str(), fd_);
    return true;
}

bool KeyboardDevice::grab() {
    if (fd_ < 0) {
        Logger::error("Cannot grab %s: not open", path_.c_str());
        return false;
    }

    if (grabbed_) {
        return true;
    }

    if (ioctl(fd_, EVIOCGRAB, 1) != 0) {
        Logger::warn("Failed to grab %s: %s", path_.c_str(), std::strerror(errno));
        return false;
    }

    grabbed_ = true;
    Logger::info("Grabbed %s", path_.c_str());
    return true;
}

bool KeyboardDevice::release() {
    if (!grabbed_ || fd_ < 0) {
        return true;
    }

    if (ioctl(fd_, EVIOCGRAB, 0) != 0) {
        Logger::warn("Failed to release %s: %s", path_.c_str(), std::strerror(errno));
        return false;
    }

    grabbed_ = false;
    Logger::info("Released %s", path_.c_str());
    return true;
}

void KeyboardDevice::close() {
    if (fd_ < 0) {
        return;
    }

    ::close(fd_);
    Logger::info("Closed %s (fd=%d)", path_.c_str(), fd_);
    fd_ = -1;
}

const std::string& KeyboardDevice::path() const {
    return path_;
}

bool KeyboardDevice::is_grabbed() const {
    return grabbed_;
}

int KeyboardDevice::fd() const {
    return fd_;
}
