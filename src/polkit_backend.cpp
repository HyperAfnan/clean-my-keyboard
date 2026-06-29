#include "polkit_backend.hpp"
#include "logger.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cerrno>
#include <string>

PolkitBackend::PolkitBackend() {
    Logger::info("PolkitBackend created");
}

PolkitBackend::~PolkitBackend() {
    if (state_ == AppState::Frozen) {
        unfreeze();
    }
}

FreezeResult PolkitBackend::freeze() {
    if (state_ != AppState::Active) {
        return FreezeResult::GrabFailed;
    }

    state_ = AppState::Freezing;

    char* argv[] = {
        (char*)"/usr/bin/pkexec",
        (char*)HELPER_PATH,
        nullptr
    };

    GError* error = nullptr;
    gboolean ok = g_spawn_async_with_pipes(
        nullptr,
        argv,
        nullptr,
        GSpawnFlags(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
        nullptr,
        nullptr,
        &child_pid_,
        &stdin_fd_,
        &stdout_fd_,
        nullptr,
        &error
    );

    if (!ok) {
        std::string err_msg = error ? error->message : "unknown error";
        Logger::error("Failed to spawn pkexec helper: %s", err_msg.c_str());
        if (error) g_error_free(error);
        state_ = AppState::Active;
        return FreezeResult::PermissionDenied;
    }

    Logger::info("Spawned pkexec helper with PID %d, waiting for READY signal...", child_pid_);

    // Read status from stdout. The helper writes READY\n upon successful grab.
    char buffer[32];
    ssize_t total_read = 0;
    while (total_read < 6) { // "READY\n" is 6 characters
        ssize_t bytes_read = read(stdout_fd_, buffer + total_read, 6 - total_read);
        if (bytes_read <= 0) {
            break;
        }
        total_read += bytes_read;
    }

    if (total_read >= 5 && std::string(buffer, 5) == "READY") {
        state_ = AppState::Frozen;
        Logger::info("Keyboards frozen via Polkit helper");
        return FreezeResult::Success;
    }

    // If we didn't receive READY, it means the helper failed to grab (or user canceled authorization)
    Logger::warn("Helper failed to return READY signal, cleaning up");
    unfreeze();
    return FreezeResult::PermissionDenied;
}

void PolkitBackend::unfreeze() {
    if (state_ == AppState::Active) {
        return;
    }

    state_ = AppState::Unfreezing;

    // 1. Close stdin/stdout to the helper to tell it to terminate cleanly
    if (stdin_fd_ != -1) {
        close(stdin_fd_);
        stdin_fd_ = -1;
    }
    if (stdout_fd_ != -1) {
        close(stdout_fd_);
        stdout_fd_ = -1;
    }

    // 2. Terminate pkexec process to ensure it shuts down
    if (child_pid_ != 0) {
        kill(child_pid_, SIGTERM);

        // Wait a short time for the child to exit
        int status;
        int wait_count = 0;
        while (waitpid(child_pid_, &status, WNOHANG) == 0 && wait_count < 10) {
            usleep(50000); // 50ms
            wait_count++;
        }

        // If it still hasn't exited, kill it forcefully
        if (waitpid(child_pid_, &status, WNOHANG) == 0) {
            kill(child_pid_, SIGKILL);
            waitpid(child_pid_, &status, 0);
        }

        g_spawn_close_pid(child_pid_);
        child_pid_ = 0;
    }

    state_ = AppState::Active;
    Logger::info("Keyboards unfrozen (Polkit helper terminated)");
}

AppState PolkitBackend::state() const {
    return state_;
}
