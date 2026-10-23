# ทดสอบ Tatree Noise Remover 2

รุ่นนี้สร้างจากซอร์สใหม่ ไม่ได้แกะหรือแก้ไบนารี v1.02 เดิม ใช้รหัสปลั๊กอินใหม่ จึงเปิดคู่กับรุ่นเก่าได้และจะไม่แทนที่ในโปรเจกต์ DAW เก่าโดยอัตโนมัติ

## บิลด์บน Mac

ต้องมี Git, Apple Command Line Tools (`xcode-select --install`) และ CMake 3.22 ขึ้นไป (`brew install cmake` ถ้าใช้ Homebrew) เปิด Terminal ในโฟลเดอร์โปรเจกต์แล้วรัน:

```sh
bash scripts/build-macos.command
```

สคริปต์ดาวน์โหลด JUCE ตาม commit ที่ตรึงไว้ บิลด์สำหรับสถาปัตยกรรม Mac เครื่องนั้น และรันทดสอบอัตโนมัติ จากนั้นเปิดโฟลเดอร์ผลลัพธ์:

- `build-macos/Tatree_artefacts/Release/Standalone/Tatree Noise Remover 2.app`
- `build-macos/Tatree_artefacts/Release/VST3/Tatree Noise Remover 2.vst3`
- `build-macos/Tatree_artefacts/Release/AU/Tatree Noise Remover 2.component`

ลอง Standalone ก่อน เลือกอุปกรณ์ input/output จาก Options ของแอป ใช้หูฟังและตั้งระดับฟังต่ำก่อนเปิด input เพื่อหลีกเลี่ยง feedback ของลำโพง

ถ้าต้องการใช้ใน DAW ให้นำ VST3 ไปไว้ `~/Library/Audio/Plug-Ins/VST3/` หรือ AU ไป `~/Library/Audio/Plug-Ins/Components/` แล้วสแกนปลั๊กอินใหม่ อย่าทับไฟล์รุ่นเก่า

ไฟล์ที่บิลด์ในเครื่องเป็น development build ยังไม่มี Developer ID/notarization สำหรับแจกจ่ายสาธารณะ ไม่ต้องปิด Gatekeeper หรือปิดการตรวจสอบของระบบเพื่อบิลด์จากซอร์ส

## เช็กลิสต์ใน DAW / Standalone

1. **ทนายภาพสี + ธีมครีม/น้ำเงิน**: ตรวจความชัดที่ขนาด 80%, 100%, 150% ปุ่มและข้อความต้องไม่ถูกตัด
2. **Noise reduction**: ป้อนเสียงห้อง/พัดลมและเสียงพูด หมุนจาก 0 ไป 45–70% ฟังช่วงหยุดพูดและความชัดพยัญชนะ ทนายขยับตามค่า attenuation ไม่ใช่ตามตำแหน่งปุ่มอย่างเดียว
3. **De-esser**: ใช้เสียง S / SH เพิ่มทีละน้อย ทนายมีท่าขยับกับป้าย “ชู่ว์!” เมื่อย่านซ่าถูกกดจริง ลดปริมาณถ้าเสียงเริ่มทึบ
4. **Input Hot!**: เพิ่ม gain จากต้นทางจน peak ใกล้ 0 dBFS (เกณฑ์ .98 linear ≈ −0.18 dBFS) ป้ายแดงและอาการสั่นต้องขึ้นทันทีและค้างอย่างน้อยประมาณ 800 ms หลังยอดเสียงสุดท้าย การเตือนยังทำงานใน bypass
5. **BREATH**: ลอง 0 → 25 → 50% กับช่วงลมหายใจและคำพูดเบา ระบบนี้เป็นตัวลดเสียงเบาด้วย envelope ไม่ใช่ AI ตรวจจับลมหายใจ จึงอาจลดคำพูดเบาด้วย
6. **FILTER**: ON คือ high-pass 75 Hz ลองกับ hum/rumble ไม่ใช่ตัวกำจัด hum ทุกความถี่
7. **LISTEN**: ฟังเฉพาะสัญญาณที่ถูกนำออก (`dry - processed`) ไม่ใช่เพียงลดระดับ output
8. **BYPASS**: เสียง dry ตรงจังหวะ wet (latency 1024 samples) เปลี่ยนสถานะด้วย crossfade 15 ms ค่า latency ≈23.2 ms @44.1k, 21.3 ms @48k, 10.7 ms @96k
9. **MOTION FX OFF**: หยุดขยับทั้งหมด แต่ข้อความและมิเตอร์ยังอัปเดต สถานะนี้ถูกบันทึกกับ session
10. **Automation / session**: automate ทั้งสามปุ่ม, filter, listen, bypass บันทึก/ปิด/เปิด DAW ตรวจค่ากลับมาครบ และลอง mono/stereo ที่ 44.1/48/96 kHz

## ทดสอบไฟล์เสียงโดยไม่เปิด DAW

```sh
build-macos/TatreeOffline_artefacts/Release/TatreeOffline input.wav output.wav 45 30 25
```

ลำดับพารามิเตอร์คือ noise%, de-ess%, breath% รองรับไฟล์ mono/stereo ที่ JUCE อ่านได้ ส่งออก WAV 24 bit โดยชดเชย latency และรักษาความยาวเดิม ไม่เขียนทับไฟล์ที่มีอยู่

## ทดสอบอัตโนมัติ / จับภาพ UI

```sh
ctest --test-dir build-macos -C Release --output-on-failure
build-macos/TatreeRender_artefacts/Release/TatreeRender.app/Contents/MacOS/TatreeRender "$PWD/build-macos/ui-captures"
```

ตัว renderer ส่งสัญญาณทดสอบเข้า DSP จริง จับ idle / noise / de-esser / hot / bypass / reduced motion และทดสอบ binding ของ knob/filter จริง ไม่ใช่ภาพ mockup

## ขอบเขตความพร้อม

- การทดสอบสัญญาณสังเคราะห์ไม่ยืนยันว่าเสียงพูด/ดนตรีทุกชนิดดีขึ้น ต้อง A/B กับเสียงของผู้ใช้
- ยังไม่มีซอร์ส v1 จึงไม่อ้างว่าอัลกอริทึมใหม่เทียบเท่าหรือดีกว่ารุ่นเดิมทุกกรณี
- ผลที่ยืนยันในเครื่อง cloud และผล macOS/Windows เป็นคนละส่วน ดู `VALIDATION.md` สำหรับรายการที่รันจริง
- GitHub Actions ที่เพิ่มไว้บิลด์ macOS universal (Intel/Apple Silicon) และ Windows เมื่อ workflow ถูกรัน การมีไฟล์ workflow ไม่ได้แปลว่าบิลด์เหล่านั้นรันแล้ว
