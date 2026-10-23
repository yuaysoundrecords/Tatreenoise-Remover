# Tatree Noise Remover 2 — Windows x64

Windows เป็นเป้าหมายหลักของรุ่นนี้ ส่วน Mac เป็นขั้นถัดไปหลัง Windows ผ่านการทดสอบ

## ฟีเจอร์และ UI ที่ยึดเป็นหลัก

- กระดาษครีม / หมึกน้ำเงินตามแบบที่เลือก ฟีลอินดี้ไทย ปุ่มและมิเตอร์จัดเป็นชุดเดียวกัน
- ทนายภาพสี: ชูนิ้วตาม noise attenuation, นิ้วแตะปากเมื่อ de-esser ทำงาน, ตกใจเมื่อ input ใกล้ clip
- Breath มีจังหวะขยับแยก, Filter มี feedback ตอนสลับ, Listen มีสถานะฟังเสียงที่ถูกเอาออก, Bypass พัก motion และแสดงเสียง dry
- สีเตือน Input Hot มีลำดับสำคัญสูงสุด รวมถึงขณะ Listen/Bypass
- Motion FX ปิดได้โดยไม่ปิดข้อความและมิเตอร์
- IN/OUT เป็น peak meter หน่วย dBFS พร้อม peak hold; Gain Reduction แสดง attenuation จริง หน่วย dB
- พิมพ์เปอร์เซ็นต์ที่แม่นยำได้ด้วย double-click ที่ตัวเลข หมุน/ลาก/ใช้คีย์บอร์ดกับ knob ได้
- Mono/stereo, parameter smoothing, latency-compensated bypass, delta listen, automation และ session recall

## สิ่งที่ปรับในระบบเสียงรอบนี้

Noise control ใช้ progressive taper ให้ช่วงใช้งานปกติรักษาเสียงพูดมากขึ้น และยังได้ช่วงลด noise เต็มที่เมื่อหมุนสูงสุด ส่วน Breath ใช้ high-band/unvoiced confidence ร่วมกับระดับเสียงและ envelope ที่นุ่มขึ้น เพื่อลดโอกาสตัดเสียงพูดเบา ๆ

ตัวเลข benchmark เป็นเพียง fixture เดียว ไม่ใช่คำรับรองว่าดีกว่า v1 กับทุกไฟล์ ต้อง A/B ด้วยเสียงของผู้ใช้ก่อนสรุปคุณภาพ

## ทดลองจากแพ็กเกจ Windows

1. แตก ZIP ทั้งโฟลเดอร์ก่อน ไม่เปิด EXE จากใน ZIP
2. เปิด `Standalone/Tatree Noise Remover 2.exe` เลือกอุปกรณ์เสียง ใช้หูฟังและเริ่มฟังเบา ๆ
3. เปิด `Install-VST3.cmd` เพื่อติดตั้ง VST3 ให้บัญชี Windows ปัจจุบัน ไม่ต้องสิทธิ์ administrator
4. ตำแหน่งติดตั้งคือ `%LOCALAPPDATA%\Programs\Common\VST3` เปิด DAW แล้ว rescan ถ้า DAW ไม่สแกนตำแหน่งนี้ ให้นำ path ดังกล่าวไปเพิ่มในการตั้งค่า VST3
   ถ้า DAW ไม่รองรับ path ต่อผู้ใช้ ให้คัดลอกโฟลเดอร์ `.vst3` ไป `C:\Program Files\Common Files\VST3` ด้วยตนเอง และให้ Windows ขอสิทธิ์ตามปกติ
5. ปลั๊กอิน v1 ชื่อเดิมจะไม่ถูกแทนที่ ส่วน Tatree 2 รุ่นก่อนหน้าในตำแหน่งเดียวกันจะถูกเก็บเป็นโฟลเดอร์ backup

แพ็กเกจทดสอบยังไม่มี Authenticode signature อย่าปิด SmartScreen/การตรวจสอบระบบเพื่อให้รันผ่าน หาก Windows เตือน ให้ตรวจแหล่งที่มาและ SHA-256 ใน `checksums.json` ก่อนตัดสินใจเปิด การเซ็นไฟล์เป็นขั้น release แยกต่างหาก

## บิลด์บน Windows โดยตรง

ต้องมี Visual Studio 2022: workload **Desktop development with C++**, Windows SDK, Git และ CMake 3.22 ขึ้นไป เปิด PowerShell ในโฟลเดอร์โปรเจกต์:

```powershell
./scripts/build-windows.ps1
```

ถ้านโยบายเครื่องไม่ให้รัน `.ps1` ให้ตรวจ script ตามนโยบายของเครื่อง หรือรันคำสั่ง CMake/CTest ตามด้านล่างเอง ไม่ต้องปิด policy ทั้งระบบ:

```powershell
cmake -S . -B build-windows-msvc -G "Visual Studio 17 2022" -A x64
cmake --build build-windows-msvc --config Release --parallel 3
ctest --test-dir build-windows-msvc -C Release --output-on-failure
```

สคริปต์เต็มรัน audio/motion tests, เปิด native UI และจับภาพสถานะ, ดาวน์โหลด pluginval รุ่นที่ตรึง SHA-256 ไว้, ตรวจ VST3 จริงระดับ 5 แล้วจึงแพ็ก ZIP หากขั้นใดล้มเหลวจะไม่สร้างแพ็กเกจที่ระบุว่าผ่าน

ไฟล์บิลด์อยู่ใน `build-windows-msvc/Tatree_artefacts/Release/` หลักฐานอยู่ใน `validation/` และ ZIP หลังตรวจผ่านอยู่ใน `package/`

## GitHub Actions

workflow `Windows x64 build and validation` ทำ Windows ก่อนเสมอ ส่วน Mac ปิดไว้โดยปริยาย เปิด `build_macos` เฉพาะเมื่อต้องการต่อ Mac หลัง Windows job ผ่านแล้ว

ไฟล์ workflow ที่ยังไม่ถูกรันไม่ใช่หลักฐานว่า Windows ผ่าน ดู `VALIDATION.md` และ logs ของ job จริงก่อนเรียกรุ่นนั้นว่า Windows-tested

## เช็กลิสต์ฟังจริงใน DAW

- เริ่ม Noise 25–45%, Breath 0–25%, De-esser 0–30% แล้วค่อยเพิ่ม ฟังการออกเสียงเบา/พยัญชนะ/ช่วงเว้นวรรค
- ใช้ Listen ตรวจว่าสิ่งที่เอาออกมีคำพูดหรือรายละเอียดสำคัญมากไปหรือไม่
- เปรียบเทียบ Bypass โดยคุมระดับความดังเท่ากัน ไม่ตัดสินจากไฟล์ที่ดังขึ้นเพียงอย่างเดียว
- ทดสอบ stereo image, 44.1/48/96 kHz, automation, save/reopen session, resize 80–150%, เปิด/ปิด editor ระหว่างเล่นเสียง
- Input Hot เตือนเมื่อ input peak ≥ .98 linear (ประมาณ −0.18 dBFS) ให้ลด gain ต้นทาง ปลั๊กอินนี้ไม่ใช่ brickwall limiter
