#include "log.h"

// Transmits string buffer through EUSCI UART
void LOG(const char* TAG, char *logStr)
{
    console_print("(");
    console_print(TAG);
    console_print(") ");
    console_print(logStr);
}
