%ifdef CONFIG
{
  "RegData": {
    "XMM0":  ["0x414243447f800000", "0x5152535455565758"],
    "XMM1":  ["0x41424344ff800000", "0x5152535455565758"],
    "XMM2":  ["0x41424344ffc00000", "0x5152535455565758"]
  }
}
%endif

mov rdx, 0xe0000000

mov rax, 0x4142434400000000 ; 0.0, result is inf
mov [rdx + 8 * 0], rax
mov rax, 0x5152535455565758
mov [rdx + 8 * 1], rax

mov rax, 0x4142434480000000 ; -0.0, result is -inf
mov [rdx + 8 * 2], rax
mov rax, 0x5152535455565758
mov [rdx + 8 * 3], rax

mov rax, 0x41424344c0800000 ; -4.0, result is -nan
mov [rdx + 8 * 4], rax
mov rax, 0x5152535455565758
mov [rdx + 8 * 5], rax


movapd xmm0, [rdx + 8 * 0]
movapd xmm1, [rdx + 8 * 2]
movapd xmm2, [rdx + 8 * 4]

rsqrtss xmm0, xmm0
rsqrtss xmm1, xmm1
rsqrtss xmm2, xmm2
hlt
