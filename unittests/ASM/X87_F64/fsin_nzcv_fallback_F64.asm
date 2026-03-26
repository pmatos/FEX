%ifdef CONFIG
{
  "RegData": {
    "RAX": "0x40"
  },
  "Env": { "FEX_X87REDUCEDPRECISION" : "1" }
}
%endif

; Test that fsin preserves RFLAGS when taking the fallback path.
;
; The fallback path is triggered by |x| >= 2^23. In the optimized JIT handler,
; an fcmp for the range check clobbers the hardware NZCV. If the subsequent ABI
; thunk's SpillStaticRegs re-reads the (now clobbered) NZCV and overwrites the
; saved value, the flags returned to the caller are wrong.

mov rsp, 0xe0000010
mov rcx, 0xe0000000

; Load a value that triggers the fallback path (|x| >= 2^23).
lea rdx, [rel large_val]
fld qword [rdx]

; Set flags to a known state: ZF=1, SF=0, CF=0, OF=0.
mov rbx, 42
cmp rbx, 42

; fsin must NOT modify RFLAGS.
fsin

; Store the sin result out of the way (so the FPU stack is clean).
fstp qword [rcx]

; Read flags.
pushfq
pop rax
; Mask to the NZCV-mapped flags: OF(bit 11), SF(bit 7), ZF(bit 6), CF(bit 0).
and rax, 0x8C1

hlt

align 8
large_val:
  dq 0x4170000000000000 ; 2^24 = 16777216.0
