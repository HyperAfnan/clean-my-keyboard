#include "device_validator.hpp"
#include "logger.hpp"

#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BITS_TO_BYTES(max) (((max) + 7) / 8)

#define TEST_BIT(arr, bit) ((arr)[(bit) / 8] & (1 << ((bit) % 8)))

bool DeviceValidator::is_real_keyboard(const std::string& devnode) {
    int fd = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {

        return false;
    }

    unsigned char evbits[BITS_TO_BYTES(EV_MAX + 1)] = {};
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0) {
        ::close(fd);
        return false;
    }

    bool has_key = TEST_BIT(evbits, EV_KEY);
    bool has_rep = TEST_BIT(evbits, EV_REP);

    ::close(fd);

    if (!has_key || !has_rep) {
        Logger::info("Skipping %s: EV_KEY=%d EV_REP=%d (not a real keyboard)",
                     devnode.c_str(), has_key, has_rep);
        return false;
    }

    Logger::info("Validated %s as real keyboard", devnode.c_str());
    return true;
}
