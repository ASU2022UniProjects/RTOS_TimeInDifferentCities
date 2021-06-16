#ifndef UART
#define UART

#define BAUD 115200

void initUart(void);
void printToUART(const char *string);
char readFromUARTOneChar(void);

#endif //UART