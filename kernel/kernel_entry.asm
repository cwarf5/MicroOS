[BITS 32]
[global _start]
[extern kernel_main]
[extern idt_init]

section .text
_start:
    ; Align stack to 16 bytes
    and esp, 0xFFFFFFF0
    mov ebp, esp
    
    ; Clear registers
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    
    ; Call kernel_main first, it will handle IDT setup
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top: