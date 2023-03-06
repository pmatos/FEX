%ifdef CONFIG
{
  "HostFeatures": ["AVX"],
  "RegData": {
    "XMM0": ["0xFFFFFFFFEEEEEEEE", "0xDDDDDDDDCCCCCCCC", "0xBBBBBBBBAAAAAAAA", "0x9999999988888888"],
    "XMM1": ["0xCCCCCCCCDDDDDDDD", "0xEEEEEEEEFFFFFFFF", "0x8888888899999999", "0xAAAAAAAABBBBBBBB"],
    "XMM2": ["0xDDDDDDDDDDDDDDDD", "0xDDDDDDDDDDDDDDDD", "0x9999999999999999", "0x9999999999999999"],
    "XMM3": ["0xEEEEEEEEFFFFFFFF", "0xCCCCCCCCDDDDDDDD", "0xAAAAAAAABBBBBBBB", "0x8888888899999999"],
    "XMM4": ["0xCCCCCCCCDDDDDDDD", "0xEEEEEEEEFFFFFFFF", "0x0000000000000000", "0x0000000000000000"],
    "XMM5": ["0xDDDDDDDDDDDDDDDD", "0xDDDDDDDDDDDDDDDD", "0x0000000000000000", "0x0000000000000000"],
    "XMM6": ["0xEEEEEEEEFFFFFFFF", "0xCCCCCCCCDDDDDDDD", "0x0000000000000000", "0x0000000000000000"]
  }
}
%endif

lea rdx, [rel .data]

vmovapd ymm0, [rdx]

vpermilps ymm1, ymm0, [rel .invert]
vpermilps ymm2, ymm0, [rel .select_elem_3]
vpermilps ymm3, ymm0, [rel .reverse_quadwords]

vpermilps xmm4, xmm0, [rel .invert]
vpermilps xmm5, xmm0, [rel .select_elem_3]
vpermilps xmm6, xmm0, [rel .reverse_quadwords]

hlt

align 32
.data:
dq 0xFFFFFFFFEEEEEEEE
dq 0xDDDDDDDDCCCCCCCC
dq 0xBBBBBBBBAAAAAAAA
dq 0x9999999988888888

.invert:
dq 0x0000000200000003
dq 0x0000000000000001
dq 0x0000000200000003
dq 0x0000000000000001

.select_elem_3:
dq 0x0000000300000003
dq 0x0000000300000003
dq 0x0000000300000003
dq 0x0000000300000003

; Upper bits filled with junk. Should have no impact on operation
.reverse_quadwords:
dq 0xFFFFFFF0FFFFFFF1
dq 0xFFFFFFF2FFFFFFF3
dq 0xFFFFFFF0FFFFFFF1
dq 0xFFFFFFF2FFFFFFF3