%ifdef CONFIG
{
  "RegData": {
    "RAX": "0x40"
  },
  "Env": { "FEX_X87REDUCEDPRECISION" : "1" }
}
%endif

; Test that fptan preserves RFLAGS when taking the fallback path.

mov rsp, 0xe0000010
mov rcx, 0xe0000000

lea rdx, [rel large_val]
fld qword [rdx]

; Set flags: ZF=1, SF=0, CF=0, OF=0.
mov rbx, 42
cmp rbx, 42

fptan

; fptan pushes 1.0 after the tangent result; pop both.
fstp qword [rcx]
fstp qword [rcx]

pushfq
pop rax
and rax, 0x8C1

hlt

align 8
large_val:
  dq 0x4170000000000000 ; 2^24 = 16777216.0
