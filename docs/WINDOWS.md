# Tatree Noise Remover 2 — Windows x64

Windows เป็นเป้าหมายหลักของรุ่นนี้ ส่วน Mac เป็นขั้นถัดไปหลัง Windows ผ่านการทดสอบ

## รุ่น 2.0.3 — คำพูดจากการขยับปุ่ม / Vocal Auto Gain

| การใช้งาน | ข้อความทนาย |
| --- | --- |
| ขยับ Input หรือ Output Gain | คุณมีสิทธิที่จะไม่พูด เพราะคำพูดของคุณอาจทำให้ Peak ได้! |
| ขยับ De-esser | กระบวนการยุติธรรม Sibilance นำเสมอ... ลดหน่อย! |
| ขยับ Noise Reduction | ศาลมีคำสั่ง! ให้ตัดเสียงรบกวนออกจากกระบวนการพิจารณา! |
| เปิด **หรือปิด** Auto Gain | ศาลมีคำสั่ง! ให้ปรับระดับเสียงของจำเลยให้เท่าเทียมกัน! |

ข้อความจัดสองบรรทัด ใช้ฟอนต์หนาขนาดเดียวกันที่วัดให้พอดีทุกประโยค กรอบยาวตามตัวอักษรจริง เว้นด้านข้าง 23 px และบน–ล่าง 12 px ขยับปุ่มซ้ำจะเริ่มเวลาค้างใหม่ ข้อความกลับปกติประมาณ 3 วินาทีหลังหยุดขยับ Automation ที่ทำให้ค่าปุ่มเปลี่ยนจะแสดงข้อความด้วย การเปิด editor เพียงอย่างเดียวไม่เรียกข้อความ

ข้อความแยกจาก detector ของ Motion และมิเตอร์ การเปลี่ยนปุ่มจึงไม่สร้างสถานะเสียงรบกวน/clip ขึ้นมาเอง และไม่มีเสียงพูดแทรกใน audio output

**Auto Gain สำหรับ gain staging เสียงร้อง:** อยู่หลัง Input trim และก่อน cleanup ทั้งหมด เป้าหมาย -18 dBFS RMS ของเสียงที่กำลังทำงาน ใช้ 20 ms analysis / 400 ms RMS smoothing, ปรับลงนุ่ม 350 ms และขึ้น 1.5 s พร้อมจำกัดความเร็วลง 12 dB/s และขึ้น 3 dB/s ค่าชดเชย -24 ถึง +12 dB มี deadband 0.15 dB, absolute/relative activity gate และ HOLD ระหว่างช่วงเงียบ/ลมหายใจเบา ๆ เพื่อลดการ pumping

หากเปลี่ยนไปเป็นวลีที่เบาลงมากต่อเนื่องประมาณ 3 วินาที ระบบจึงยอมเรียนรู้ระดับใหม่ การปิดจะกลับ unity อย่างนุ่มนวล ปุ่ม Output ใช้ปรับปลายทางตามต้องการ ส่วน Bypass ยังคงเป็นเสียงต้นฉบับที่ชดเชย latency แล้ว

Peak memory จำกัดเป้าหมาย boost เพื่อเหลือ headroom ประมาณ -3 dBFS แต่ Auto Gain ไม่ใช่ limiter และไม่รับประกัน peak เมื่อ input กระโดดฉับพลันหรือเพิ่ม Output Gain แรง หากต้นทางเบามากต่ำกว่า gate ให้ใช้ Input trim ก่อน เป้าหมาย RMS นี้ไม่ใช่ LUFS normalization หรือการบีบ dynamic แบบ compressor

ตัวอย่างฟังเฉพาะ Auto Gain จากเสียงพูดจริง (ปิด Noise/Ess/Breath/Filter): [ก่อน](audio/vocal-gain-before.wav) / [หลัง](audio/vocal-gain-after.wav)

## ฟีเจอร์และ UI ที่ยึดเป็นหลัก

- กระดาษครีม / หมึกน้ำเงินตามแบบที่เลือก ฟีลอินดี้ไทย ปุ่มและมิเตอร์จัดเป็นชุดเดียวกัน
- ทนายภาพสี: ชูนิ้วตาม noise attenuation, นิ้วแตะปากเมื่อ de-esser ทำงาน, ตกใจเมื่อ input ใกล้ clip
- Breath มีจังหวะขยับแยก, Filter มี feedback ตอนสลับ, Listen มีสถานะฟังเสียงที่ถูกเอาออก, Bypass พัก motion และแสดงเสียง dry
- สีเตือน Input Hot มีลำดับสำคัญสูงสุด รวมถึงขณะ Listen/Bypass
- Motion FX ปิดได้โดยไม่ปิดข้อความและมิเตอร์
- IN/OUT เป็น peak meter หน่วย dBFS พร้อม peak hold; Gain Reduction แสดง attenuation จริง หน่วย dB
- พิมพ์เปอร์เซ็นต์ที่แม่นยำได้ด้วย double-click ที่ตัวเลข หมุน/ลาก/ใช้คีย์บอร์ดกับ knob ได้
- Mono/stereo, parameter smoothing, latency-compensated bypass, delta listen, automation และ session recall

## Gain และข้อความทนาย — รุ่น 2.0.1

ลำดับเสียง: Input Gain → Auto Gain → Filter → Noise Reduction → De-esser → Breath → Output Gain

- Input/Output ปรับได้ −24 ถึง +24 dB พิมพ์ค่าที่ช่อง dB ได้ และ double-click ที่ slider เพื่อคืน 0 dB
- Auto Gain อยู่ต้น chain เปิดด้วยปุ่ม AUTO GAIN เป้าหมาย −18 dBFS RMS แบบ stereo-linked ชดเชยได้ −24 ถึง +12 dB พร้อม gate ที่ −50 dBFS เพื่อไม่ไล่เร่งช่วงเงียบ และจำกัด boost ตาม peak headroom −3 dBFS
- Auto Gain ปิดไว้โดยปริยาย เปิดเมื่อต้องการ level เสียงเข้า; ลด gain เร็วกว่าการเพิ่ม gain และแสดง dB ชดเชยจริงใต้ปุ่ม
- IN แสดงระดับหลัง Input Gain/Auto Gain ส่วน OUT แสดงระดับหลัง Output Gain
- LISTEN ฟังผลต่างของเสียงหลัง Input Gain/Auto Gain กับเสียงที่ cleanup แล้ว ก่อนผ่าน Output Gain จึงไม่เข้าใจผิดว่า gain trim เป็น noise ที่ถูกตัด
- BYPASS ข้าม gain และ processing ทั้งหมด กลับไปเสียงต้นฉบับที่ชดเชย latency แล้ว
- เสียงดิบเข้า, เสียงหลัง gain เข้า หรือเสียงออกใกล้เกิน .98 linear จะเตือน: **อย่าทำ Meter แตก**
- De-esser ทำงานถึงเกณฑ์ motion: **อย่ากระเเทก Sibilance**
- Noise reducer ทำงานถึงเกณฑ์ motion: **จงระวัง Single Noise!**
- ข้อความแสดงบนป้ายทนาย ไม่มีเสียงพูดแทรกในสัญญาณเสียง; hot warning มีลำดับสูงสุด

Auto Gain เป็นตัวปรับระดับ ไม่ใช่ limiter และกู้เสียงที่ clip มาจากอุปกรณ์ต้นทางไม่ได้ หาก OUT เตือน ให้ลด Output Gain/ระดับต้นทาง

## รุ่น 2.0.2: กรอบคำพูดและ DSP

ทั้งสามประโยคใช้ Noto Sans Thai ตัวหนา ขนาดเดียวกัน กรอบปรับความยาวตามขอบตัวอักษรจริง รวมวรรณยุกต์ เว้นด้านข้าง 23 px และบน–ล่าง 12 px ก่อนหมุนกรอบทั้งชุด

Noise ใช้ minimum-statistics และชะลอเรียนรู้ช่วงที่น่าจะเป็นเสียงพูด; De-esser ใช้ TPT crossover กับ RMS detector ที่ทำงานตาม sample rate; De-breath รอหลักฐานเสียง unvoiced ต่อเนื่องก่อนลด และคืน gain เร็วเมื่อคำพูดกลับมา ดูผลเทียบกับ SpeexDSP และข้อจำกัดใน [DSP-QUALITY.md](DSP-QUALITY.md)

## สิ่งที่ปรับในระบบเสียงรอบก่อน (2.0.1)

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
