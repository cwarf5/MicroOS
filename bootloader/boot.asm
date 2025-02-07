[BITS 16]           ; 16-bit real mode
[ORG 0x7C00]        ; Bootloader loads at 0x7C00

; Constants
KERNEL_OFFSET equ 0x1000
VIDEO_MEMORY equ 0xB8000

start:
    ; Initialize segments and stack
    cli             ; Disable interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00  ; Set stack pointer
    sti             ; Re-enable interrupts for BIOS calls

    ; Save boot drive number
    mov [BOOT_DRIVE], dl

    ; Print boot message
    mov si, BOOT_MSG
    call print_string

    ; Load kernel from disk
    mov si, LOAD_KERNEL_MSG
    call print_string
    
    push dx         ; Save boot drive number
    mov dl, [BOOT_DRIVE]
    call load_kernel
    pop dx
    
    jc disk_error   ; Check if kernel load failed

    ; Print success message
    mov si, KERNEL_SUCCESS_MSG
    call print_string

    ; Enable A20 line and switch to protected mode
    call enable_a20
    cli             ; Disable interrupts
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1     ; Set protected mode bit
    mov cr0, eax
    jmp CODE_SEG:protected_mode_init

; Function to print a string
print_string:
    pusha
    mov ah, 0x0E        ; BIOS teletype output
.loop:
    lodsb               ; Load next character from SI into AL
    test al, al         ; Check if character is null (string end)
    jz .done           ; If null, we're done
    int 0x10            ; Print character
    jmp .loop          ; Continue with next character
.done:
    popa
    ret

; Function to load kernel from disk
load_kernel:
    pusha
    mov ah, 0x02        ; BIOS read sectors function
    mov al, 20          ; Number of sectors to read
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2
    mov dh, 0           ; Head 0
    mov bx, KERNEL_OFFSET ; Load to KERNEL_OFFSET
    int 0x13            ; BIOS disk interrupt
    jc disk_error       ; If carry flag set, there was an error
    popa
    ret

; Disk error handler
disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $               ; Infinite loop

; Function to enable A20 line
enable_a20:
    in al, 0x92        ; Read System Control Port A
    or al, 2           ; Set A20 bit
    out 0x92, al       ; Write back
    ret

[BITS 32]
protected_mode_init:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Jump to kernel
    jmp CODE_SEG:KERNEL_OFFSET

; GDT
gdt_start:
    dd 0x0          ; Null descriptor
    dd 0x0          

gdt_code:           ; Code segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_data:           ; Data segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10010010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Start address of GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Data
BOOT_DRIVE db 0
BOOT_MSG db 'Starting bootloader...', 13, 10, 0
LOAD_KERNEL_MSG db 'Loading kernel...', 13, 10, 0
KERNEL_SUCCESS_MSG db 'Kernel loaded successfully!', 13, 10, 0
DISK_ERROR_MSG db 'Disk read error!', 13, 10, 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xAA55