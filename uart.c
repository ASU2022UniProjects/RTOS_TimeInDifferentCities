// #include "FreeRTOS.h"
// #include "TM4C123GH6PM.h"
// #include "task.h"
// #include "queue.h"
// #include "semphr.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"

#include "stdint.h"

#include "uart.h"

void initUart()
{
    HWREG(SYSCTL_RCGCUART) |= 0x01;                // enable UART0
    HWREG(SYSCTL_RCGCGPIO) |= 0x01;                // enable portA
    HWREG(GPIO_PORTA_BASE + GPIO_O_AFSEL) |= 0x03; // enable alternate functions
    HWREG(GPIO_PORTA_BASE + GPIO_O_PCTL) |= 0x11;
    HWREG(GPIO_PORTA_BASE + GPIO_O_DEN) |= 0x03;
    HWREG(UART0_BASE + UART_O_CTL) &= ~0X01;
    HWREG(UART0_BASE + UART_O_IBRD) = 50000000 / 16 / BAUD;
    HWREG(UART0_BASE + UART_O_FBRD) = ((64 * ((50000000 / 16) % BAUD)) + BAUD / 2) / BAUD;
    HWREG(UART0_BASE + UART_O_LCRH) = (0x3 << 5);
    HWREG(UART0_BASE + UART_O_CC) = (0x00);
    HWREG(UART0_BASE + UART_O_CTL) = 0x301;
}

void printToUART(const char *string)
{
    while (*string)
    {
        while ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF) != 0)
        {
        };
        HWREG(UART0_BASE + UART_O_DR) = *string;
        string++;
    }
}

char readFromUARTOneChar(void)
{
    if ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE) == 0)
    {
        return (char)HWREG(UART0_BASE + UART_O_DR);
    }
}