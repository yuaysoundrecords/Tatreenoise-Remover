#pragma once
#include <juce_core/juce_core.h>
#include "LicensePolicy.h"
#include <atomic>
#include <memory>

class TatreeLicense final : public ProcessingEntitlement, private juce::Thread {
public:
    enum class Verdict { accepted, rejected, unavailable };
    static std::shared_ptr<TatreeLicense> shared();
    ~TatreeLicense() override;
    bool isUnlocked() const noexcept { return unlocked.load(); }
    bool isBusy() const noexcept { return busy.load(); }
    double secondsRemaining() const noexcept;
    bool expired() const noexcept override { return !isUnlocked() && secondsRemaining() <= 0; }
    bool configured() const;
    juce::String statusText() const;
    void activate(juce::String key);
    static Verdict validateResponse(int httpStatus, const juce::String& body, const juce::String& productId);
private:
    TatreeLicense();
    void run() override;
    void setMessage(juce::String);
    juce::File receiptFile;
    const double startSeconds;
    std::atomic<bool> unlocked {false}, busy {false};
    mutable juce::CriticalSection mutex;
    juce::CriticalSection requestMutex;
    juce::WebInputStream* activeRequest = nullptr;
    juce::String pendingKey, message;
    juce::WaitableEvent pending;
};
