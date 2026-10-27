#!/bin/bash
set -euo pipefail
export PATH=/usr/bin:/bin:/usr/sbin:/sbin
close_window() { read -r -p 'กด Enter เพื่อปิด / Press Enter to close. ' ignored || true; }
fail() {
  printf '\nเกิดข้อผิดพลาด หยุดทำงาน ยังไม่ยืนยันว่าปลดบล็อกครบแล้ว\nAn error occurred. Stopped; approval is not confirmed complete.\n' >&2
  close_window
  exit 1
}
trap fail ERR
if [[ "$(uname -s)" != Darwin ]]; then
  printf 'ไฟล์นี้ใช้บน macOS เท่านั้น\nThis helper runs on macOS only.\n'
  close_window; exit 1
fi
printf '\nTatree Noise Remover 2 — Yuay Sound Records\n'
printf 'ติดตั้งปลั๊กอินก่อน แล้วปิด DAW และตัวสแกนปลั๊กอิน\nInstall the plug-in first. Close your DAW and plug-in scanners.\n\n'
printf 'ตัวช่วยนี้ถอด quarantine เฉพาะ Tatree AU/VST3 ที่แสดงด้านล่าง\nThis removes quarantine ONLY from the listed installed Tatree AU/VST3 bundles.\n'
printf 'ไม่ปิด Gatekeeper ทั้งเครื่อง และไม่ใช่การรับรองหรือ notarization จาก Apple\nIt does not disable Gatekeeper globally and is not Apple certification or notarization.\n'
printf 'ทำต่อเมื่อเชื่อถือแหล่งดาวน์โหลดนี้เท่านั้น\nContinue only if you trust this download source.\n\n'
bundles=()
verify_bundle() {
  local bundle="$1" ancestor="$1"
  while [[ "$ancestor" != / ]]; do
    if [[ -L "$ancestor" ]]; then
      printf 'หยุด: พบ symbolic link / Refusing symbolic link: %s\n' "$ancestor" >&2
      return 1
    fi
    ancestor="$(dirname "$ancestor")"
  done
  local link
  link="$(/usr/bin/find "$bundle" -type l -print -quit)"
  if [[ -n "$link" ]]; then
    printf 'หยุด: พบ symbolic link ภายใน / Refusing internal symbolic link: %s\n' "$link" >&2
    return 1
  fi
  if ! /usr/bin/codesign --verify --deep --strict "$bundle"; then
    printf 'ตรวจลายเซ็นไม่ผ่าน กรุณาติดตั้งใหม่จากแหล่งที่เชื่อถือ\nSignature verification failed. Reinstall from a trusted source.\n' >&2
    return 1
  fi
}
for root in /Library "$HOME/Library"; do
  for relative in 'Audio/Plug-Ins/Components/Tatree Noise Remover 2.component' 'Audio/Plug-Ins/VST3/Tatree Noise Remover 2.vst3'; do
    bundle="$root/$relative"
    if [[ -d "$bundle" || -L "$bundle" ]]; then
      verify_bundle "$bundle" || fail
      bundles+=("$bundle")
      printf 'ไฟล์ที่จะดำเนินการ / Target: %s\n' "$bundle"
    fi
  done
done
if [[ ${#bundles[@]} -eq 0 ]]; then
  printf 'ไม่พบ Tatree AU/VST3 กรุณาติดตั้งก่อน ยังไม่ได้เปลี่ยนไฟล์ใด\nTatree AU/VST3 was not found. Install it first. No files were changed.\n'
  close_window; exit 1
fi
printf '\nลายเซ็นที่ตรวจผ่านยืนยันความสมบูรณ์ ไม่ได้ยืนยันตัวตนผู้พัฒนา\nSignature integrity does not authenticate the publisher.\n'
answer=''
read -r -p 'พิมพ์ YES เพื่อยืนยัน หรือ Enter เพื่อยกเลิก / Type YES to confirm, or Enter to cancel: ' answer || true
if [[ "$answer" != YES ]]; then
  printf 'ยกเลิกแล้ว ไม่ได้เปลี่ยนไฟล์ใด\nCancelled. No files were changed.\n'
  close_window; exit 0
fi
for bundle in "${bundles[@]}"; do verify_bundle "$bundle" || fail; done
for bundle in "${bundles[@]}"; do
  printf '\nกำลังดำเนินการ / Processing: %s\n' "$bundle"
  if [[ "$bundle" == /Library/* ]]; then
    printf 'ใส่รหัสผ่าน Mac เมื่อระบบถาม ขณะพิมพ์จะไม่แสดงตัวอักษร\nEnter your Mac password when prompted; typing is hidden.\n'
    /usr/bin/sudo -p 'รหัสผ่าน Mac / Mac password: ' /usr/bin/xattr -dr com.apple.quarantine "$bundle"
  else
    /usr/bin/xattr -dr com.apple.quarantine "$bundle"
  fi
done
printf '\nเสร็จแล้ว เปิด DAW แล้วสแกน Tatree ใหม่ สคริปต์นี้ไม่ต้องรีสตาร์ตเครื่อง\nDone. Open your DAW and rescan Tatree. This script does not require a restart.\n'
close_window
