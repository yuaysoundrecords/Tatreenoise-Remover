# Tatree 2.1 — trial and Gumroad keys

## Customer behaviour

- Every plug-in instance in one host process shares a 30-minute wall-clock trial, starting when Tatree is first loaded. Closing an editor, changing sample rate, loading presets, or removing/reinserting the last instance does not restart the clock. Quit the host process completely and reopen it to start another trial. Bridged/sandboxed hosts may run separate plug-in processes; the clock is per process, not an OS-wide DAW identity.
- At expiry, processing fades over 15 ms to the original dry signal with the same 1024-sample latency. No silence bursts, added noise or abrupt mute. The UI remains usable. Activation restores processing smoothly without reopening the project.
- Click the trial/license button, paste the Gumroad key and choose ACTIVATE. Activation is shared by all Tatree instances in that process. The key is remembered in the user's application data; it is never included in project/preset state, logs or distribution archives.
- **Internet is required once per new DAW process to verify a remembered key.** After successful verification, that process stays unlocked without a continuous connection. With no connection at startup, the normal trial is available. This deliberately avoids trusting an editable local `unlocked=true` file; it is not an offline signed licence system or tamper-proof DRM.
- Only a matching, non-test, non-refunded, non-disputed purchase of **at least US$5** unlocks. Free/pay-what-you-want receipts under US$5 do not unlock, even if Gumroad issues a key. Discounted purchases below US$5 also do not unlock. This is a fixed USD policy, not currency conversion.
- Requests go by HTTPS POST directly to `api.gumroad.com/v2/licenses/verify`, with public product ID, entered key, and `increment_uses_count=false`. No audio, project data, machine ID or email is sent by the plug-in. Network and file operations run outside the audio callback. Gumroad receives normal connection metadata.

## Seller setup — required before selling 2.1

Tatree product: https://benjilennon.gumroad.com/l/rsecg . Do not use Prued FX (`rkpck`).

1. Enable Gumroad license keys on the paid Tatree product. Set the USD price/minimum to **5**. A free trial download must remain available separately, or a pay-what-you-want 0+ product must clearly tell buyers that only payments of US$5+ unlock. This repository cannot change account settings.
2. Copy Gumroad's **public product ID** into the repository Actions variable `TATREE_GUMROAD_PRODUCT_ID` (not a seller access token or a customer key). For a local build, set the environment variable with the same name, or pass `-DTATREE_GUMROAD_PRODUCT_ID=...` to CMake. Clear/reconfigure an existing build cache if the ID changes.
3. Build Windows and Mac from the same commit. Without a product ID, the code is a **trial-only staging build** with activation disabled; do not distribute it as unlockable. A supplied ID still needs a real matching paid-key integration test; mock JSON tests do not establish that checkout/key delivery works.
4. Verify real paid-key activation/restart, wrong-product/free/refunded cases, and an offline startup on both operating systems. Do not put customer keys or seller tokens in GitHub variables, logs or source control.
5. Upload both new platform ZIPs and the installation instructions to the original Tatree product. Only after uploads complete, remove older customer downloads. Preserve the seller's recovery archive outside the product. Do not claim the existing 2.0.4 downloads contain a trial or licensing system.

## Short customer copy (Thai, then English)

ทดลองใช้ครบทุกฟังก์ชันได้ 30 นาทีต่อการเปิด DAW เมื่อครบเวลา ปลั๊กอินจะค่อย ๆ กลับเป็นเสียงเดิม ปิด DAW ให้หมดแล้วเปิดใหม่เพื่อทดลองซ้ำได้

ซื้อคีย์ US$5 เพื่อใช้งานต่อเนื่อง กด UNLOCK ในปลั๊กอิน วางคีย์จากใบเสร็จ Gumroad แล้วกด ACTIVATE ต้องต่ออินเทอร์เน็ตเพื่อตรวจคีย์เมื่อเปิด DAW ใหม่ หลังตรวจผ่านแล้วใช้งานต่อในรอบนั้นได้แม้ตัดการเชื่อมต่อ คีย์จากการโหลดฟรีหรือยอดจ่ายต่ำกว่า US$5 จะไม่ปลดล็อก

Try every feature for 30 minutes per DAW session. When the trial ends, Tatree smoothly returns to your original audio. Quit your DAW completely and reopen it to try again.

Buy a US$5 key for uninterrupted use. Click UNLOCK, paste the key from your Gumroad receipt, then click ACTIVATE. An internet connection is required to verify the key each time you open a new DAW session. Once verified, that session can continue offline. Free-download keys and payments below US$5 do not unlock the plug-in.

## Release status

Implementation and automated tests are under development. Product ID and live paid-key verification are not yet available. Gumroad pricing, key delivery and uploads have not been changed by this environment. Existing Windows/Mac 2.0.4 validation remains historical evidence only; it does not validate 2.1.
