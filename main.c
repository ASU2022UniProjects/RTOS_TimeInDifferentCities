#include "FreeRTOS.h"
#include "TM4C123GH6PM.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdint.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"

#include "util.h"
#include "lcd.h"
#include "uart.h"

xQueueHandle xQueue;
xSemaphoreHandle xBinarySemaphore;
xSemaphoreHandle xMutex;

char cities[][10] = {"London", "Paris", "Madrid", "Rome", "Athens", "Ankara", "Istanbul", "Cairo", "Moscow", "Tehran"};

char timediff[] = {0, 1, 1, 1, 2, 2, 2, 2, 3, 4};

unsigned int selection;

typedef struct Message
{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
} AMessage;

xQueueHandle controllerToLCDQueue;
xQueueHandle xTimeQueue;

void incrementTime(AMessage *currentTime)
{
	currentTime->seconds++;
	if (currentTime->seconds > 59)
	{
		currentTime->seconds = 0;
		currentTime->minutes++;
		if (currentTime->minutes > 59)
		{
			currentTime->minutes = 0;
			currentTime->hours++;
			if (currentTime->hours > 23)
			{
				currentTime->hours = 0;
			}
		}
	}
}

void TimeController(void *pvParameters)
{
	AMessage currentTime;

	xQueueReceive(xTimeQueue, &currentTime, portMAX_DELAY);

	TickType_t xLastWakeTime = xTaskGetTickCount();
	const int oneSecond = msToTicks(1000);

	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, oneSecond);

		incrementTime(&currentTime);

		xQueueSend(controllerToLCDQueue, &currentTime, 0);
	}
}

// format hh:mm:ss-
void readCurrentTimeFromKeyboard(char buf[])
{
	unsigned char char_counter = 0;
	char c[2];
	c[1] = '\0';
	while (1)
	{
		c[0] = readFromUARTOneChar();
		if (c[0] == '\0')
		{
			continue;
		}
		printToUART(&c);
		if (c[0] == '\r')
			break;
		buf[char_counter] = c[0];
		char_counter++;
	}
	buf[char_counter] = '\0';
}

unsigned int readCityNumberFromKeyboard()
{
	unsigned int totalCityNumber = 0;
	unsigned char cityNumberDigit[2];
	cityNumberDigit[1] = '\0';

	while (1)
	{
		cityNumberDigit[0] = readFromUARTOneChar();
		if (cityNumberDigit[0] == '\0')
		{
			continue;
		}
		printToUART(cityNumberDigit);
		if (cityNumberDigit[0] == '\r')
			break;
		cityNumberDigit[0] = cityNumberDigit[0] - '0';
		totalCityNumber = 10 * totalCityNumber + cityNumberDigit[0];
	}
	return totalCityNumber;
}

void intToString(unsigned char value, char *Txt)
{
	Txt[0] = (value / 10) % 10 + '0';
	Txt[1] = value % 10 + '0';
	Txt[2] = '\0';
}

void LCDController(void *pvParameters)
{
	const char Txt[9];
	AMessage currentTime;
	lcdInit();
	selection = 0;

	while (1)
	{
		xQueueReceive(controllerToLCDQueue, &currentTime, portMAX_DELAY);

		LCD_firstLine(cities[selection]);

		currentTime.hours = currentTime.hours + timediff[selection];
		if (currentTime.hours > 23)
			currentTime.hours = currentTime.hours - 24;

		intToString(currentTime.hours, Txt);
		LCD_secondLine(Txt);
		LCD_justPrint(":");
		intToString(currentTime.minutes, Txt);
		LCD_justPrint(Txt);
		LCD_justPrint(":");
		intToString(currentTime.seconds, Txt);
		LCD_justPrint(Txt);
	}
}

void UartController(void *pvParameters)
{
	AMessage currentTime;

	initUart();
	printToUART("\n\rTime in Different Countries");
	printToUART("\n\r===========================");
	printToUART("\n\rEnter the time in london (hh:mm:ss): ");

	uint8_t buffer[9];
	readCurrentTimeFromKeyboard(buffer);
	currentTime.hours = 10 * (buffer[0] - '0') + buffer[1] - '0';
	currentTime.minutes = 10 * (buffer[3] - '0') + buffer[4] - '0';
	currentTime.seconds = 10 * (buffer[6] - '0') + buffer[7] - '0';

	xQueueSend(xTimeQueue, &currentTime, 0);

	uint8_t char_counter;
	char c[2];
	c[1] = '\0';
	while (1)
	{
		printToUART("\n\r\n\rSelect a City:");

		for (char_counter = 0; char_counter < 10; char_counter++)
		{
			printToUART("\n\r");
			c[0] = char_counter + '0';
			printToUART(c);
			printToUART(". ");
			printToUART(cities[char_counter]);
		}
		printToUART("\n\rSelection: ");
		selection = readCityNumberFromKeyboard();
	}
}

int main()
{
	controllerToLCDQueue = xQueueCreate(1, //size
										8  // sizeof
	);
	xTimeQueue = xQueueCreate(1, //size
							  8	 // sizeof
	);

	xTaskCreate(
		(TaskFunction_t)TimeController,
		"Time Controller",
		100,
		NULL,
		10,
		NULL);
	xTaskCreate(
		(TaskFunction_t)LCDController,
		"LCD Controller",
		100,
		NULL,
		10,
		NULL);
	xTaskCreate(
		(TaskFunction_t)UartController,
		"UART Controller",
		100,
		NULL,
		10,
		NULL);

	vTaskStartScheduler();
}
