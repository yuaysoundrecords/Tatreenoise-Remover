# Tatree 2.1.0 — Windows x64 (free)

ใช้ฟรีทุกฟังก์ชัน ไม่จำกัดเวลา ไม่ต้องใส่คีย์ ปุ่ม TIP / KO-FI มุมขวาบนเปิด https://ko-fi.com/yuaylooper เมื่อกดเท่านั้น การให้ทิปเป็นทางเลือกและไม่มีผลต่อการใช้งาน

Free, with all features available and no time limit or activation key. The optional TIP / KO-FI button opens Yuaylooper’s donation page only when clicked.

## ติดตั้งและทดลอง

1. แตก ZIP ทั้งหมดก่อน เปิด `Standalone/Tatree Noise Remover 2.exe` เลือกอุปกรณ์เสียงและใช้หูฟัง เริ่มฟังเบา ๆ
2. รัน `Install-VST3.cmd` เพื่อติดตั้งใน `%LOCALAPPDATA%\Programs\Common\VST3` ไม่ต้อง administrator
3. เปิด DAW แล้ว rescan หรือเพิ่ม path ข้างต้น หาก DAW ไม่รองรับ per-user path ให้คัดลอก `.vst3` ไป `C:\Program Files\Common Files\VST3` ด้วยสิทธิ์ตามปกติ

v1 มี ID แยกต่างหาก ตัวติดตั้งเก็บ v2 ที่ชื่อเดียวกันไว้เป็น backup ก่อนแทนที่ ชุดทดสอบยังไม่มี Authenticode signature; ตรวจแหล่งที่มาและ checksums โดยไม่ปิดระบบรักษาความปลอดภัยทั้งเครื่อง

## ปุ่มและลำดับเสียง

Input Gain → Auto Gain → Filter → Noise → De-esser → Breath → Output Gain

Input/Output ±24 dB; double-click slider เพื่อคืน 0 dB, พิมพ์ค่าได้ ส่วนเปอร์เซ็นต์สามปุ่มหลัก double-click ที่ตัวเลขเพื่อแก้ค่า

Auto Gain เป็น vocal leveler ก่อน cleanup: เป้าหมาย -18 dBFS active RMS, ชดเชย -24/+12 dB, วิเคราะห์ 20 ms และเฉลี่ย 400 ms, ลด gain นุ่ม 350 ms / เพิ่ม 1.5 s พร้อมจำกัดความเร็ว -12/+3 dB/s มี HOLD ช่วงเงียบและลมหายใจเบา ปิดโดยปริยายและเปิด/ปิดได้ รอเสียงร้องสักระยะให้เรียนรู้ระดับก่อนตั้ง Output ตามต้องการ หากต้นทางเบามากต่ำกว่า activity gate ให้เพิ่ม Input trim ก่อน

มี peak-aware target เพื่อเหลือ headroom แต่ไม่ใช่ limiter/LUFS normalizer และกู้เสียงที่ clip มาแล้วไม่ได้ ลด Input/Output หากเตือน Hot

LISTEN ฟังเสียงที่ cleanup ตัดออก (หลัง Input/Auto Gain ก่อน Output trim) หากได้ยินคำพูดสำคัญมากไปให้ลดปุ่มที่เกี่ยวข้อง ปิดก่อน export ส่วน BYPASS กลับไปเสียงดิบต้นฉบับที่ชดเชย latency แล้ว ข้าม gain/cleanup ทั้งหมด

ไม่มีคำพูดตอนเปิดโปรแกรม ขยับ Input/Output, Noise, De-esser, Breath จะขึ้นประโยคของปุ่มนั้นประมาณ 3 วินาที Auto Gain ขึ้นทั้ง ON และ OFF ดูข้อความครบใน [UI.md](https://github.com/yuaysoundrecords/Tatreenoise-Remover/blob/codex/windows-first-tatree2/docs/UI.md)

Motion FX ปิดได้โดยไม่ปิดมิเตอร์/ข้อความ และการเปลี่ยนข้อความไม่สร้าง detector activity ขึ้นมาเอง

## หลักฐานและตัวอย่างเสียง

แพ็กเกจมี `validation/`, `checksums.json`, [รายงาน DSP](DSP-QUALITY.md), ตัวอย่าง [Auto Gain ก่อน](audio/vocal-gain-before.wav)/[หลัง](audio/vocal-gain-after.wav) และ [Noise ก่อน](audio/quality-speech-before.wav)/[หลัง](audio/quality-speech-after.wav) ตัวอย่างเป็นผู้พูดภาษาอังกฤษหนึ่งคน ไม่ใช่หลักฐานว่าเทียบเท่าทุกปลั๊กอินหรือรักษาพยัญชนะไทยได้ทุกกรณี

## บิลด์จาก source

ติดตั้ง Visual Studio 2022 Desktop C++, Windows SDK, Git และ CMake ≥3.22 จากนั้นเปิด PowerShell ใน source folder แล้วรัน `./scripts/build-windows.ps1`

สคริปต์รัน CTest, native UI integration และ pluginval strictness 5 แล้วตรวจ hash ก่อนแพ็ก ZIP ขั้นใดผิดพลาดจะไม่แพ็กเป็นชุดที่ผ่าน ผลอยู่ `build-windows-msvc/package/`

ก่อนใช้งานจริงให้ทดสอบเสียงของคุณ, automation, save/reopen, resize, เปิด/ปิด editor ระหว่างเล่น, 44.1/48/96 kHz และเปรียบเทียบ Bypass โดยคุมระดับความดัง
