// interrupt.c
#include "io.h"
#include "kernel_shell.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define ICW1_INIT 0x11
#define ICW4_8086 0x01

struct IDT_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

struct IDT_entry IDT[256];

void idt_init(void) {
    // Remap PIC
    outb(PIC1_COMMAND, ICW1_INIT);
    outb(PIC2_COMMAND, ICW1_INIT);
    outb(PIC1_DATA, 0x20);    // ICW2: Master PIC vector offset
    outb(PIC2_DATA, 0x28);    // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 0x04);    // ICW3: tell Master PIC that there is a slave PIC at IRQ2
    outb(PIC2_DATA, 0x02);    // ICW3: tell Slave PIC its cascade identity
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Clear masks
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}

void keyboard_interrupt_handler(void) {
    keyboard_handler();  // Call the shell's keyboard handler
    outb(PIC1_COMMAND, 0x20); // Send EOI
}

void setup_keyboard_interrupt() {
    // Configure interrupt vector for keyboard (usually interrupt 33)
    // This involves low-level programming of the Programmable Interrupt Controller (PIC)
    // and setting up the appropriate interrupt handler
}