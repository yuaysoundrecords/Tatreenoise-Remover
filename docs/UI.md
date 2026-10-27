# Current UI and control captions

Startup and quote expiry display no speech banner or default speech. Meters, labels and real detector-driven Motion remain active independently.

| Control change | Caption |
| --- | --- |
| Input / Output Gain | คุณมีสิทธิที่จะไม่พูด เพราะคำพูดของคุณอาจทำให้ Peak ได้! |
| De-esser | กระบวนการยุติธรรม Sibilance นำเสมอ... ลดหน่อย! |
| Noise | ศาลมีคำสั่ง! ให้ตัดเสียงรบกวนออกจากกระบวนการพิจารณา! |
| Breath | เก็บลมหายใจของท่าน ไปใช้บนชั้นศาล |
| Auto Gain ON or OFF | ศาลมีคำสั่ง! ให้ปรับระดับเสียงของจำเลยให้เท่าเทียมกัน! |

All five captions use one fitted bold Noto Sans Thai size, semantic two-line wrapping and dynamic frame width. Visible glyph-outline padding: 23 px left/right, 12 px top/bottom; centred lines, 8 px line gap, right anchor and matching tilt. A change resets the 3 s hold. Keyboard edits and host automation are also observed; simply opening the editor does not trigger speech.

Native UI tests exercise startup silence, all five mappings, both Auto Gain states, expiry, 80/150% resizing and independent DSP states. Current checked-in captures are Linux; native platform evidence is attached to CI runs.

[Current native UI captures](https://github.com/yuaysoundrecords/Tatreenoise-Remover/actions/runs/40507885353/artifacts/12736511036) are in the separate validation artifact. Older screenshots have been removed from the active source tree.
