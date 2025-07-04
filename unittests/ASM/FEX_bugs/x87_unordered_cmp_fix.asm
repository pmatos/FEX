%ifdef CONFIG
{
  "RegData": {
    "RAX": "0x0000000cafecafe"
  }
}
%endif

; This test verifies the fix for x87 FUCOMIP unordered comparison detection
; Before the fix (commit 8e877789f), all floating-point comparisons were incorrectly 
; flagged as unordered due to a faulty NaN detection in SoftFloat.h
; After the fix, only comparisons involving NaN should set PF=1 (unordered)
;
; The bug caused SPEC GCC-C-execute-ieee-fp-cmp-8l test failures because
; __builtin_isunordered() was returning true for all comparisons, not just NaN

mov rsp, 0xe000_1000

; Test 1: Compare two normal numbers (1.0 vs 2.0)
; This should NOT be unordered (PF should be 0)
lea rdx, [rel one]
fld tword [rdx]

lea rdx, [rel two]
fld tword [rdx]

fucomip st1
jp test_failed  ; Jump if PF=1 (unordered) - this should NOT happen

; Test 2: Compare two equal normal numbers (2.0 vs 2.0)
; This should NOT be unordered (PF should be 0)
lea rdx, [rel two]
fld tword [rdx]

lea rdx, [rel two]
fld tword [rdx]

fucomip st1
jp test_failed  ; Jump if PF=1 (unordered) - this should NOT happen

; Test 3: Compare normal number with NaN
; This SHOULD be unordered (PF should be 1)
lea rdx, [rel one]
fld tword [rdx]

lea rdx, [rel qnan]
fld tword [rdx]

fucomip st1
jnp test_failed  ; Jump if PF=0 (ordered) - this should NOT happen

; Test 4: Compare NaN with normal number
; This SHOULD be unordered (PF should be 1)
lea rdx, [rel qnan]
fld tword [rdx]

lea rdx, [rel one]
fld tword [rdx]

fucomip st1
jnp test_failed  ; Jump if PF=0 (ordered) - this should NOT happen

; All tests passed
mov rax, 0xcafecafe
hlt

test_failed:
; Test failed - return different value
mov rax, 0xdeadbeef
hlt

align 8
one:
  dt 1.0

align 8
two:
  dt 2.0

align 8
qnan:
  dq 0xC000000000000001
  dw 0x7FFF
