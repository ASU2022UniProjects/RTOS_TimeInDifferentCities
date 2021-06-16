#include "util.h"

#include "FreeRTOS.h"

int msToTicks(int ms)
{
    return ms / portTICK_RATE_MS;
}