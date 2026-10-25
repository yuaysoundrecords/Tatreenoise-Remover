#pragma once
#include <algorithm>
#include <cmath>

// Wall-clock session time, shared by every instance in this host process.
// Reopening an editor, restoring state or preparing audio must not reset it.
struct SessionTrial {
    static constexpr double durationSeconds = 30.0 * 60.0;
    static double remaining(double startSeconds, double nowSeconds) noexcept {
        if (!std::isfinite(startSeconds) || !std::isfinite(nowSeconds)) return 0;
        return std::clamp(durationSeconds - std::max(0.0, nowSeconds-startSeconds), 0.0, durationSeconds);
    }
};
