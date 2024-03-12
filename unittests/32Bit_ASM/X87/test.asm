%ifdef CONFIG
{
  "RegData": {
    "MM6": ["0x8000000000000000", "0x4000"],
    "MM7": ["0x8000000000000000", "0x4001"]
  },
  "Mode": "32BIT"
}
%endif

fild dword [intVal]
fild dword [intVal]
fadd

hlt

section .data
    intVal dd 2
