#ifndef PORTS_H
#define PORTS_H

// Function declarations
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
void idt_init(void);

// VGA ports
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

#endif