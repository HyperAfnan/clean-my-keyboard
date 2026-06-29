#include "logger.hpp"

#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace {

void log_message(const char* level, const char* fmt, va_list args) {

    std::time_t now = std::time(nullptr);
    std::tm tm_buf{};
    localtime_r(&now, &tm_buf);
    char time_str[20];
    std::strftime(time_str, sizeof(time_str), "%H:%M:%S", &tm_buf);

    std::fprintf(stderr, "[%s] %s: ", level, time_str);
    std::vfprintf(stderr, fmt, args);
    std::fprintf(stderr, "\n");
}

}

namespace Logger {

void info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("INFO", fmt, args);
    va_end(args);
}

void warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("WARN", fmt, args);
    va_end(args);
}

void error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("ERROR", fmt, args);
    va_end(args);
}

}
