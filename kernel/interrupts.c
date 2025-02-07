#include "ports.h"
#include "kernel.h"

// Define IDT entry structure
struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

// Define IDT pointer structure
struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

// Define IDT
#define IDT_ENTRIES 256
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

// Keyboard interrupt handler
void keyboard_interrupt_handler(void);

// Set an IDT gate
void set_idt_gate(int n, unsigned int handler) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].selector = 0x08;  // Kernel code segment
    idt[n].zero = 0;
    idt[n].flags = 0x8E;     // Present, Ring 0, 32-bit Interrupt Gate
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

// Function to load IDT
void load_idt(void) {
    __asm__ volatile("lidt %0" : : "m"(idtp));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Initialize PIC
void init_pic(void) {
    // ICW1
    outb(0x20, 0x11);    // Initialize master PIC
    outb(0xA0, 0x11);    // Initialize slave PIC
    
    // ICW2
    outb(0x21, 0x20);    // Master PIC vector offset (32)
    outb(0xA1, 0x28);    // Slave PIC vector offset (40)
    
    // ICW3
    outb(0x21, 0x04);    // Tell Master PIC about Slave PIC at IRQ2
    outb(0xA1, 0x02);    // Tell Slave PIC its cascade identity
    
    // ICW4
    outb(0x21, 0x01);    // 8086/88 mode
    outb(0xA1, 0x01);    // 8086/88 mode
    
    // Set masks - only enable keyboard interrupt (IRQ1)
    outb(0x21, ~0x02);  // Enable only IRQ1 (keyboard) on master PIC
    outb(0xA1, 0xFF);   // Disable all interrupts on slave PIC
}

void idt_init(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (unsigned int)&idt;

    // Clear IDT
    for(int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].base_high = 0;
    }

    // Set up keyboard interrupt (IRQ1 -> INT 33)
    set_idt_gate(33, (unsigned int)keyboard_interrupt_handler);

    // Initialize PIC
    init_pic();

    // Load IDT
    load_idt();
}

// Keyboard handler function (moved from kernel_shell.c)
void keyboard_handler(void) {
    static int buffer_index = 0;
    static char command_buffer[50];

    unsigned char scancode = inb(0x60);  // Read keyboard input
    
    // Only handle key press events (ignore key release)
    if (scancode & 0x80) {
        return;
    }
    
    // Very basic input handling
    if (scancode >= 0x02 && scancode <= 0x0D) {  // Numbers
        char key = "1234567890-="[scancode - 0x02];
        if (buffer_index < 49) {
            command_buffer[buffer_index++] = key;
            print_char(key);
            update_cursor();
        }
    }
    else if (scancode >= 0x10 && scancode <= 0x1C) {  // First row (Q to Enter)
        if (scancode == 0x1C) {  // Enter
            command_buffer[buffer_index] = '\0';
            print_char('\n');
            process_command(command_buffer);
            buffer_index = 0;
            print_string("\nKernel> ");
            update_cursor();
        }
        else {
            char key = "qwertyuiop[]"[scancode - 0x10];
            if (buffer_index < 49) {
                command_buffer[buffer_index++] = key;
                print_char(key);
                update_cursor();
            }
        }
    }
    else if (scancode >= 0x1E && scancode <= 0x28) {  // Second row (A to ')
        char key = "asdfghjkl;'"[scancode - 0x1E];
        if (buffer_index < 49) {
            command_buffer[buffer_index++] = key;
            print_char(key);
            update_cursor();
        }
    }
    else if (scancode >= 0x2C && scancode <= 0x35) {  // Third row (Z to /)
        char key = "zxcvbnm,./"[scancode - 0x2C];
        if (buffer_index < 49) {
            command_buffer[buffer_index++] = key;
            print_char(key);
            update_cursor();
        }
    }
    else if (scancode == 0x39) {  // Space
        if (buffer_index < 49) {
            command_buffer[buffer_index++] = ' ';
            print_char(' ');
            update_cursor();
        }
    }
    else if (scancode == 0x0E) {  // Backspace
        if (buffer_index > 0) {
            buffer_index--;
            // Move cursor back and clear character
            cursor_x--;
            print_char(' ');
            cursor_x--;
            update_cursor();
        }
    }
} 