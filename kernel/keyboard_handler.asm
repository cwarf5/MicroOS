[BITS 32]
[global keyboard_interrupt_handler]
[extern keyboard_handler]

keyboard_interrupt_handler:
    pusha                   ; Save registers
    cld                     ; Clear direction flag
    
    call keyboard_handler   ; Call C handler
    
    mov al, 0x20           ; Send EOI (End of Interrupt)
    out 0x20, al
    
    popa                    ; Restore registers
    iret                    ; Return from interrupt 