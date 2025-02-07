// kernel_shell.c
#include "kernel.h"
#include "kernel_shell.h"
#include <string.h>
#include "ports.h"  // Add this new header for port I/O functions

// Video memory pointer
volatile unsigned short* video_memory = (unsigned short*)0xb8000;

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Current cursor position
int cursor_x = 0;  // Now exposed via kernel.h
int cursor_y = 0;  // Now exposed via kernel.h

// Buffer for command input
#define MAX_COMMAND_LENGTH 50
char command_buffer[MAX_COMMAND_LENGTH];
int buffer_index = 0;

// Update hardware cursor position
void update_cursor(void) {
    unsigned short position = cursor_y * SCREEN_WIDTH + cursor_x;

    // Tell VGA we are setting the low cursor byte
    outb(VGA_CTRL_REGISTER, 0x0F);
    outb(VGA_DATA_REGISTER, (unsigned char)(position & 0xFF));

    // Tell VGA we are setting the high cursor byte
    outb(VGA_CTRL_REGISTER, 0x0E);
    outb(VGA_DATA_REGISTER, (unsigned char)((position >> 8) & 0xFF));
}

// Clear the screen
void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video_memory[i] = 0x0720;  // Space character with white on black
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();  // Update cursor after clearing screen
}

// Print a single character
void print_char(char c) {
    // Handle newline
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        update_cursor();
        return;
    }

    // Calculate the offset in video memory
    int offset = cursor_y * SCREEN_WIDTH + cursor_x;
    
    // Write character with white text on black background
    video_memory[offset] = (0x0F << 8) | c;
    
    // Move cursor
    cursor_x++;
    
    // Wrap to next line if needed
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    update_cursor();
}

// Print a string
void print_string(const char* str) {
    while (*str != '\0') {
        print_char(*str);
        str++;
    }
    update_cursor();
}

// -------- COMMANDS --------
// Help command - list available commands
void help_command() {
    print_string("Available commands:\n");
    print_string("  help  - Show this help message\n");
    print_string("  hello - Display a greeting\n");
    print_string("  clear - Clear the screen\n");
    print_string("  reboot - Reboot the system\n");
    print_string("  creed - Display my credentials\n");
}

// Creed command function
void creed_command() {
    print_string("Name: Creed Warf\n");
    print_string("----- DEGREE -----\n");
    print_string("Bachelor of Science in Computer Science\n");
    print_string("University of Virginia's College at Wise\n");
    print_string("----- EXPERIENCE -----\n");
    print_string("IT Help Desk Analyst - Ballad Health\n");
    print_string("----- SKILLS -----\n");
    print_string("C, C++, Python, Assembly, SQL, HTML, CSS, JavaScript\n");
}


// Hello command
void hello_command() {
    print_string("Hello, Kernel World!\n");
}

// Reboot command (uses x86 keyboard controller)
void reboot_command() {
    print_string("Rebooting...\n");
    
    // Disable interrupts
    __asm__ volatile("cli");
    
    // Use keyboard controller to pulse reset line
    while ((inb(0x64) & 0x02) != 0){}  // Wait for input buffer to be clear
    outb(0x64, 0xFE);  // Send reset pulse
    
    // If that fails, halt
    while(1) {
        __asm__ volatile("hlt");
    }
}

// Process the entered command
void process_command(char* command) {
    // Trim leading/trailing whitespace (simple version)
    // Remove this if it gets too complex without standard library
    while (*command == ' ') command++;
    
    // Compare and execute
    if (command[0] == '\0') {
        // Empty command, just print a new prompt
        print_string("\n");
    } else if (strcmp(command, "help") == 0) {
        help_command();
    } else if (strcmp(command, "hello") == 0) {
        hello_command();
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();
    } else if (strcmp(command, "reboot") == 0) {
        reboot_command();
    } else if (strcmp(command, "creed") == 0) {
        creed_command();
    } else {
        print_string("Unknown command: ");
        print_string(command);
        print_string("\nType 'help' for a list of commands.\n");
    }
}

// Simple strcmp implementation (since we can't use standard library)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Shell prompt
void shell_prompt() {
    print_string("\nKernel> ");
    buffer_index = 0;
    update_cursor();  // Update cursor after showing prompt
}

// Initialize the shell
void initialize_shell() {
    // Clear screen first
    clear_screen();
    
    // Print welcome message
    print_string("Welcome to Kernel Shell!\n");
    print_string("Type 'help' for a list of commands.\n");
    
    // Show initial prompt
    shell_prompt();
    update_cursor();  // Update cursor after initialization
}

// Modify kernel_main to use shell
void kernel_main(void) {
    // Initialize interrupts before enabling them
    idt_init();

    // Initialize the shell
    initialize_shell();

    // Enable interrupts
    __asm__ volatile("sti");

    // Main loop (simplified - in a real kernel, this would be more complex)
    while(1) {
        // Wait for interrupts (keyboard input)
        __asm__ volatile("hlt");
    }
}