#include "Licensing.h"
#ifndef TATREE_GUMROAD_PRODUCT_ID
#define TATREE_GUMROAD_PRODUCT_ID ""
#endif
#if ! JUCE_WINDOWS
#include <sys/stat.h>
#endif

namespace {
double monotonicSeconds() { return juce::Time::getMillisecondCounterHiRes() * .001; }
juce::String productId() { return TATREE_GUMROAD_PRODUCT_ID; }
bool trueBool(const juce::var& v) { return v.isBool() && static_cast<bool>(v); }
}
std::shared_ptr<TatreeLicense> TatreeLicense::shared() {
    // Deliberately retain the session even after the last instance is removed.
    static auto license = std::shared_ptr<TatreeLicense>(new TatreeLicense());
    return license;
}
TatreeLicense::TatreeLicense() : Thread("Tatree license verification"),
    receiptFile(juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Yuay Sound Records/Tatree2/activation.json")), startSeconds(monotonicSeconds()) {
    startThread();
    // Remember the key, never trust a locally editable "unlocked" flag.
    // Gumroad must verify it once per host session, outside the audio thread.
    // Never read a licence from DAW state/presets or ship a seller's receipt.
    const auto receipt = juce::JSON::parse(receiptFile.loadFileAsString());
    if (configured() && receipt["product_id"].toString() == productId()
        && receipt["key"].isString() && receipt["key"].toString().isNotEmpty()) {
        activate(receipt["key"].toString());
    }
}
TatreeLicense::~TatreeLicense() {
    signalThreadShouldExit(); pending.signal();
    { const juce::ScopedLock lock(requestMutex); if(activeRequest) activeRequest->cancel(); }
    stopThread(-1);
}
double TatreeLicense::secondsRemaining() const noexcept { return SessionTrial::remaining(startSeconds, monotonicSeconds()); }
bool TatreeLicense::configured() const { return productId().isNotEmpty(); }
juce::String TatreeLicense::statusText() const { const juce::ScopedLock lock(mutex); return message; }
void TatreeLicense::setMessage(juce::String s) { const juce::ScopedLock lock(mutex); message=std::move(s); }
void TatreeLicense::activate(juce::String key) {
    key=key.trim();
    if (!configured()) { setMessage("Activation is not configured in this build. Please contact support."); return; }
    if (key.isEmpty() || key.length()>256) { setMessage("Please enter the license key from your Gumroad receipt."); return; }
    bool expected=false;
    if (!busy.compare_exchange_strong(expected,true)) return;
    { const juce::ScopedLock lock(mutex); pendingKey=key; message="Checking your key with Gumroad..."; }
    pending.signal();
}
TatreeLicense::Verdict TatreeLicense::validateResponse(int status, const juce::String& body, const juce::String& id) {
    if (id.isEmpty() || body.length()>65536) return Verdict::unavailable;
    const auto root=juce::JSON::parse(body);
    if (!root.isObject() || !root["success"].isBool()) return Verdict::unavailable;
    if (status!=200) {
        // Rate limits, server faults and network errors must not revoke a verified session.
        return status==404 && !trueBool(root["success"]) ? Verdict::rejected : Verdict::unavailable;
    }
    if (!trueBool(root["success"])) return Verdict::rejected;
    const auto p=root["purchase"];
    if (!p.isObject() || !p["product_id"].isString() || p["product_id"].toString()!=id) return Verdict::rejected;
    // Require these fields; a malformed positive response must never unlock.
    if (!p["refunded"].isBool() || !p["disputed"].isBool()) return Verdict::unavailable;
    if (trueBool(p["refunded"]) || trueBool(p["disputed"]) || trueBool(p["chargebacked"]) || trueBool(p["test"])) return Verdict::rejected;
    // Free/pay-what-you-want downloads must not unlock the paid edition.
    if (!(p["price"].isInt() || p["price"].isInt64()) || int(p["price"])<500
        || p["currency"].toString().toLowerCase()!="usd") return Verdict::rejected;
    if (p["subscription_cancelled_at"].toString().isNotEmpty()
        || p["subscription_failed_at"].toString().isNotEmpty()
        || p["subscription_ended_at"].toString().isNotEmpty()) return Verdict::rejected;
    return Verdict::accepted;
}
void TatreeLicense::run() {
    while (!threadShouldExit()) {
        pending.wait();
        if (threadShouldExit()) break;
        juce::String key;
        { const juce::ScopedLock lock(mutex); key=std::move(pendingKey); pendingKey.clear(); }
        if (key.isEmpty()) continue;
        int status=0;
        const auto url=juce::URL("https://api.gumroad.com/v2/licenses/verify")
            .withParameter("product_id",productId()).withParameter("license_key",key)
            .withParameter("increment_uses_count","false");
        juce::WebInputStream stream(url,true);
        stream.withConnectionTimeout(8000).withNumRedirectsToFollow(0).withCustomRequestCommand("POST");
        { const juce::ScopedLock lock(requestMutex);
            if(threadShouldExit()) break;
            activeRequest=&stream;
        }
        juce::String body;
        if (stream.connect(nullptr)) {
            status=stream.getStatusCode();
            juce::MemoryBlock data;
            stream.readIntoMemoryBlock(data,65537);
            if (data.getSize()<=65536) body=juce::String::fromUTF8(static_cast<const char*>(data.getData()),int(data.getSize()));
        }
        { const juce::ScopedLock lock(requestMutex); activeRequest=nullptr; }
        if (threadShouldExit()) break;
        const auto verdict=validateResponse(status,body,productId());
        if (verdict==Verdict::accepted) {
            auto object=std::make_unique<juce::DynamicObject>();
            object->setProperty("product_id",productId()); object->setProperty("key",key);
            const bool saved=receiptFile.getParentDirectory().createDirectory().wasOk()
                && receiptFile.replaceWithText(juce::JSON::toString(juce::var(object.release())));
#if ! JUCE_WINDOWS
            if(saved) ::chmod(receiptFile.getFullPathName().toRawUTF8(),0600);
#endif
            unlocked.store(true);
            setMessage(saved?"Unlocked. Thank you!":"Unlocked for this session. Could not save activation; check folder permissions.");
        } else if (verdict==Verdict::rejected) {
            // An invalid newly typed key must not overwrite an existing good activation.
            const auto receipt=juce::JSON::parse(receiptFile.loadFileAsString());
            if (receipt["key"].toString()==key) { unlocked.store(false); receiptFile.deleteFile(); }
            setMessage("This key is invalid, refunded, disputed, or belongs to another product.");
        } else setMessage(isUnlocked()?"Could not reach Gumroad. This session remains unlocked.":"Could not reach Gumroad. Check your connection and try again; trial remains available.");
        busy.store(false);
    }
}
