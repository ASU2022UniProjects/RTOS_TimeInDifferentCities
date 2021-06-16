#ifndef LCD_UTIL
#define LCD_UTIL

#define Ports
#include "stdint.h"

#define LCD_COMMAND_CLEAR 0x1               // Clear display screen
#define LCD_COMMAND_RETURN_CURSOR 0x2       // Return cursor home
#define LCD_COMMAND_INC_CURSOR 0x6          // Increment cursor (shift cursor to right)
#define LCD_COMMAND_DISPLAY_ON 0xF          // Display on, cursor blinking
#define LCD_COMMAND_CURSOR_FIRST_LINE 0x80  // Force cursor to beginning of 1st line
#define LCD_COMMAND_CURSOR_SECOND_LINE 0xC0 // Force cursor to beginning of 2nd line
#define LCD_COMMAND_8_BIT 0x38              // 2 lines and 5x7 character (8-bit data, D0 to D7)
#define LCD_COMMAND_4_BIT 0x28              // 2 lines and 5x7 character (4-bit data, D4 to D7)
#define LCD_COMMAND_WAKEUP 0x30             // wakeup

#define LCD_CTRL_ZERO 0
#define LCD_RS 0x1 // 0001
#define LCD_RW 0x2 // 0010
#define LCD_EN 0x4 // 0100

#define LCD_CTRL_CLK SYSCTL_RCGCGPIO_R4
#define LCD_DATA_CLK SYSCTL_RCGCGPIO_R1
#define LCD_DATA_BASE GPIO_PORTB_BASE
#define LCD_CTRL_BASE GPIO_PORTE_BASE

void lcdInit(void);
void LCD_data(unsigned char data);
void LCD_justPrint(char *word);
void LCD_firstLine(char *word);
void LCD_secondLine(char *word);

#endif //LCD_UTIL