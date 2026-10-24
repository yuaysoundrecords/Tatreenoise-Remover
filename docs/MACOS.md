# บิลด์ Tatree 2 บน Mac

รองรับ Apple Silicon และ Intel (macOS 11 ขึ้นไป) ค่าเริ่มต้นบิลด์ Universal ทั้งสองสถาปัตยกรรม ได้ VST3, AU และ Standalone

## เริ่มครั้งแรก

1. เปิด Terminal รัน `xcode-select --install` และติดตั้ง Apple Command Line Tools ให้เสร็จ
2. ติดตั้ง CMake จาก https://cmake.org/download/ หรือ `brew install cmake` และ Git หากยังไม่มี
3. แตกชุด source แล้วดับเบิลคลิก **BUILD-MAC.command** ที่โฟลเดอร์หลัก หรือรัน `bash BUILD-MAC.command` ใน Terminal

ครั้งแรกต้องต่ออินเทอร์เน็ตเพื่อดึง JUCE เวอร์ชันที่ตรึงไว้ สคริปต์จะ configure, build, รัน CTest, เปิด native UI เพื่อทดสอบข้อความ/สถานะ และสร้าง ZIP เมื่อผ่าน ไม่มีการติดตั้งปลั๊กอินทับเครื่องอัตโนมัติ

ผลลัพธ์อยู่ที่ `build-macos/package/Tatree-2-macOS-universal.zip` ภายในมี AU / VST3 / Standalone, notices และ SHA256SUMS.txt ส่วนหลักฐานอยู่ใน `build-macos/validation/`

สำหรับรอบพัฒนาที่ต้องการเฉพาะสถาปัตยกรรมเครื่อง:

```sh
TATREE_MAC_ARCHS="$(uname -m)" bash BUILD-MAC.command
```

ชุดนี้ตั้งชื่อ archive ตาม architecture ที่เลือก (เช่น `Tatree-2-macOS-arm64.zip`) ไม่ใช่ Universal ใช้ค่าเริ่มต้นสำหรับ Universal release

## ทดลองใช้งาน

เปิด Standalone ที่บิลด์ในเครื่องก่อน จากนั้นนำ `.vst3` ไป `~/Library/Audio/Plug-Ins/VST3` หรือ `.component` ไป `~/Library/Audio/Plug-Ins/Components` แล้วเปิด DAW และ rescan รุ่น v1 มี ID คนละชุด ส่วน v2 ที่ชื่อเดียวกันควรสำรองไว้ก่อนแทนที่

สคริปต์ใช้ ad-hoc signature เพื่อทดสอบในเครื่อง ไม่ใช่ Developer ID และยังไม่ใช่ notarized public Mac release อย่าปิด Gatekeeper ทั้งระบบ การเซ็นและ notarization สำหรับเผยแพร่เชิงพาณิชย์เป็นขั้น release แยกจากการบิลด์ source

Windows รุ่นหลักผ่านแล้ว และ CI แยกตรวจ Windows กับ Mac Universal, core/speech tests, native UI, Mach-O architectures, signature verification และ AU validation (`auval`) ดูผลรอบจริงใน VALIDATION.md ก่อนอ้างว่า Mac-tested
