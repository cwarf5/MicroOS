# MicroKernel OS Project

A minimalist, educational kernel implementation written in C and Assembly. This project demonstrates low-level system programming concepts including bootloader development, interrupt handling, and basic shell functionality.

## Features

- **Custom Bootloader**: Written in x86 Assembly, handles the transition from 16-bit real mode to 32-bit protected mode
- **Interrupt Handling**: Implements IDT (Interrupt Descriptor Table) and handles keyboard interrupts
- **VGA Text Mode Interface**: Direct manipulation of video memory for text display
- **Interactive Shell**: Supports basic commands and user input
- **Hardware Cursor Control**: Real-time cursor position updates using VGA registers

### Shell Commands
- `help` - Display available commands
- `hello` - Display a greeting message
- `clear` - Clear the screen
- `reboot` - Soft reboot the system
- `creed` - Displays my credentials
  
![creedcmd](https://github.com/user-attachments/assets/e8bc76a0-48c8-4bfe-b020-514e1f22f259)

## Technical Details

- **Architecture**: x86 (32-bit)
- **Language**: C and x86 Assembly
- **Build System**: Make
- **Bootloader Size**: 512 bytes (single sector)
- **Memory Model**: Protected Mode
- **Text Display**: 80x25 VGA text mode

## Building and Running

### Prerequisites
- NASM (Netwide Assembler)
- GCC (GNU Compiler Collection)
- QEMU (for emulation)
- Make

## Technical Implementation

- **Bootloader**: Custom implementation that sets up the system, enables A20 line, and loads the kernel
- **Interrupt System**: Configures PIC (Programmable Interrupt Controller) and handles keyboard interrupts
- **Memory Management**: Basic memory layout with stack initialization
- **I/O Handling**: Direct port I/O for hardware interaction
- **Text Display**: Direct VGA memory manipulation for text output

## Learning Outcomes

This project demonstrates understanding of:
- Operating System fundamentals
- x86 architecture and Assembly programming
- Interrupt handling and hardware interaction
- Memory management and segmentation
- Boot process and system initialization
- Real-time hardware control

## Future Enhancements

- Memory management system
- File system implementation
- Process scheduling
- More shell commands and utilities
- User space program support

## License

[MIT License](LICENSE)

## Author

[Creed Warf]

---
*Note: This is an educational project demonstrating OS development concepts. Not intended for production use.*
