#!/bin/bash
set -euo pipefail
export PATH=/usr/bin:/bin:/usr/sbin:/sbin
printf '\nTatree Noise Remover — อนุญาตปลั๊กอินบน Mac / Mac approval\n'
printf 'ใช้หลังติดตั้งปลั๊กอิน และปิด DAW ก่อน\nRun AFTER installation. Close your DAW first.\n'
printf 'ไฟล์นี้ถอดสถานะ quarantine เฉพาะ Tatree VST3 ที่ติดตั้งแล้ว\nThis removes download quarantine ONLY from the installed Tatree VST3.\n'
printf 'ไม่ได้ปิด Gatekeeper ทั้งเครื่อง และไม่ใช่การรับรองจาก Apple\nIt does not disable Gatekeeper or certify this unsigned developer build.\n'
printf 'ทำต่อเมื่อคุณเชื่อถือแหล่งดาวน์โหลดปลั๊กอินนี้เท่านั้น\nContinue only if you trust the source of this plug-in.\n\n'
bundles=()
for bundle in '/Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3' "$HOME/Library/Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3"; do
  if [[ -d "$bundle" ]]; then
    if [[ -L "$bundle" ]] || [[ -n "$(/usr/bin/find "$bundle" -type l -print -quit)" ]]; then
      printf 'หยุดทำงาน: พบลิงก์ไฟล์ / Refusing symbolic links: %s\n' "$bundle"
      read -r -p 'กด Enter เพื่อปิด / Press Enter to close. ' ignored
      exit 1
    fi
    /usr/bin/codesign --verify --deep --strict "$bundle" || { printf 'ตรวจลายเซ็นไม่ผ่าน กรุณาติดตั้งใหม่จากแหล่งที่เชื่อถือได้\nSignature verification failed. Reinstall from a trusted source.\n'; read -r -p 'กด Enter เพื่อปิด / Press Enter to close. ' ignored; exit 1; }
    bundles+=("$bundle")
    printf 'ไฟล์ที่จะดำเนินการ / Target: %s\n' "$bundle"
  fi
done
if [[ ${#bundles[@]} -eq 0 ]]; then
  printf 'ไม่พบ Tatree VST3 กรุณาติดตั้งปลั๊กอินก่อน\nTatree VST3 was not found. Install it first.\n'
  read -r -p 'กด Enter เพื่อปิด / Press Enter to close. ' ignored
  exit 1
fi
printf '\nลายเซ็นที่ตรวจผ่านไม่ได้ยืนยันตัวตนผู้พัฒนา\nSignature integrity does not establish publisher trust.\n'
read -r -p 'พิมพ์ YES เพื่อยืนยัน หรือกด Enter เพื่อยกเลิก / Type YES to approve, or Enter to cancel: ' answer
if [[ "$answer" != YES ]]; then exit 0; fi
for bundle in "${bundles[@]}"; do
  if [[ "$bundle" == /Library/* ]]; then
    printf 'ใส่รหัสผ่าน Mac เมื่อขึ้น Password (ขณะพิมพ์จะไม่แสดงตัวอักษร)\nEnter your Mac password when prompted (typing is hidden).\n'
    /usr/bin/sudo /usr/bin/xattr -dr com.apple.quarantine "$bundle"
  else
    /usr/bin/xattr -dr com.apple.quarantine "$bundle"
  fi
done
printf '\nเสร็จแล้ว เปิด DAW แล้วสแกน Tatree ใหม่ สคริปต์นี้ไม่ต้องรีสตาร์ตเครื่อง\nDone. Open your DAW and rescan Tatree. No restart is required by this script.\n'
read -r -p 'กด Enter เพื่อปิด / Press Enter to close. ' ignored
