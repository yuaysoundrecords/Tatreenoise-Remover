# Gumroad update handoff

Product: https://benjilennon.gumroad.com/l/rsecg

Windows download: https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556048528

Mac Universal test build: https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556491264 (ad-hoc signed, not a notarized public installer).

Upload the validated Windows ZIP and the separate Mac source kit. GitHub artifact downloads wrap the actual package ZIP: extract the outer artifact once and upload the inner Windows ZIP to Gumroad, so customers do not receive unnecessary nested archives. Label the Mac kit as **source to build**, not a notarized installer. Keep v1 available separately for existing sessions if desired; v2 has different IDs. No pricing/licence change or subscriber email is included in this update.

Suggested Thai release text:

> Tatree Noise Remover 2.0.4 — หน้าตาครีม/น้ำเงิน ทนายสีและ Motion FX พร้อม Noise Reduction, De-esser, Breath Reduction, Filter และ Listen มี Input/Output gain กับ Vocal Auto Gain สำหรับตั้งระดับเฉลี่ยเสียงร้อง ขยับปุ่มแล้วทนายขึ้นประโยคประจำปุ่ม ไม่มีคำพูดตอนเริ่มเปิด ใช้งาน offline ได้
>
> Windows: VST3 + Standalone, ผ่าน automated audio/UI tests และ pluginval
>
> Mac: มี source kit พร้อม BUILD-MAC.command สำหรับบิลด์ Universal AU/VST3/Standalone (ต้องมี Apple Command Line Tools, Git และ CMake)
>
> มีตัวอย่างเสียงก่อน–หลังและผลตรวจ DSP โปรดทดลองกับ DAW/เสียงของคุณก่อนใช้ในงานจริง ชุดทดสอบยังไม่ได้เซ็น Authenticode/Developer ID notarization

Upload status: not uploaded. This environment has no authenticated Gumroad browser/connector capable of updating product files. Do not enter passwords or access tokens in chat. The source kit, platform download links and copy are prepared for the authorised product update.
