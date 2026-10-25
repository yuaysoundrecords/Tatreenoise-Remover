# Gumroad update handoff

Product: https://benjilennon.gumroad.com/l/rsecg

Windows download: https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556048528

Mac Universal test build: https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40083443048/artifacts/12556491264 (ad-hoc signed, not a notarized public installer).

Upload the validated Windows ZIP and Mac Universal binary ZIP as the two customer downloads. The source kit is optional developer material. GitHub artifact downloads wrap the actual package ZIP: extract the outer artifact once and upload the inner platform ZIP to Gumroad, so customers do not receive unnecessary nested archives. After both new uploads complete, remove all older downloadable versions from this product, as explicitly requested by the owner. Keep recovery copies outside the customer download list. Version 2 has different plugin IDs from v1; it does not replace v1 instances in existing sessions. Label Mac binaries as ad-hoc signed and not Apple-notarized. No pricing/licence change or subscriber email is included in this update.

Suggested Thai release text:

> Tatree Noise Remover 2.0.4 — หน้าตาครีม/น้ำเงิน ทนายสีและ Motion FX พร้อม Noise Reduction, De-esser, Breath Reduction, Filter และ Listen มี Input/Output gain กับ Vocal Auto Gain สำหรับตั้งระดับเฉลี่ยเสียงร้อง ขยับปุ่มแล้วทนายขึ้นประโยคประจำปุ่ม ไม่มีคำพูดตอนเริ่มเปิด ใช้งาน offline ได้
>
> Windows: VST3 + Standalone, ผ่าน automated audio/UI tests และ pluginval
>
> Mac: Universal รองรับ Intel และ Apple Silicon มี AU / VST3 / Standalone พร้อมใช้งาน ตัว Mac ยังไม่ได้ notarize กับ Apple จึงอาจมีคำเตือนตอนติดตั้ง มีซอร์สสำหรับบิลด์เองแยกต่างหาก
>
> มีตัวอย่างเสียงก่อน–หลังและผลตรวจ DSP โปรดทดลองกับ DAW/เสียงของคุณก่อนใช้ในงานจริง ชุดทดสอบยังไม่ได้เซ็น Authenticode/Developer ID notarization

Upload status: not uploaded. This environment has no authenticated Gumroad browser/connector capable of updating product files. Do not enter passwords or access tokens in chat. The source kit, platform download links and copy are prepared for the authorised product update.

Owner acceptance, 2026-10-25: owner reports successful use in their DAW and requests completion of the release. DAW name, OS, architecture and package version were not specified; this is user-reported acceptance, not evidence of an additional cross-platform test matrix.
