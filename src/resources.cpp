#include "resources.hpp"
#include "logger.hpp"

#include <sys/stat.h>

namespace Resources {

static bool file_exists(const std::string& path) {
    struct stat st{};
    return stat(path.c_str(), &st) == 0;
}

std::string path(const char* filename) {
    // Try installed location first
    std::string installed = std::string(INSTALLED_RESOURCE_DIR) + "/" + filename;
    if (file_exists(installed)) {
        return installed;
    }

    // Fall back to source tree (development)
    std::string dev = std::string(RESOURCE_DIR) + "/" + filename;
    if (file_exists(dev)) {
        return dev;
    }

    Logger::warn("Resource not found: %s (tried %s and %s)",
                 filename, installed.c_str(), dev.c_str());
    return dev; // Return dev path anyway, let caller handle the error
}

} // namespace Resources
