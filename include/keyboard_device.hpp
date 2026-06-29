#pragma once

#include <string>

class KeyboardDevice {
public:
    explicit KeyboardDevice(std::string devnode);
    ~KeyboardDevice();

    KeyboardDevice(const KeyboardDevice&) = delete;
    KeyboardDevice& operator=(const KeyboardDevice&) = delete;
    KeyboardDevice(KeyboardDevice&&) = delete;
    KeyboardDevice& operator=(KeyboardDevice&&) = delete;

    bool open();

    bool grab();

    bool release();

    void close();

    const std::string& path() const;
    bool is_grabbed() const;
    int fd() const;

private:
    std::string path_;
    int fd_ = -1;
    bool grabbed_ = false;
};
