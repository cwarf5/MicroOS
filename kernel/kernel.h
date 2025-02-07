#ifndef KERNEL_H
#define KERNEL_H

// Video functions
void print_char(char c);
void print_string(const char* str);
void update_cursor(void);

// Command processing
void process_command(char* command);

// Keyboard handling
void keyboard_handler(void);

// Global variables
extern int cursor_x;
extern int cursor_y;

#endif 