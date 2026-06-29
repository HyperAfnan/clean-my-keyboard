#pragma once

enum class AppState {
    Active,
    Freezing,
    Frozen,
    Unfreezing,
};

enum class FreezeResult {
    Success,
    PermissionDenied,
    NoKeyboardsFound,
    GrabFailed,
};
