#include "driverlib.h"
#include "Board.h"
#include "console.h"
#include "command.h"
#include "init.h"
#include "queue.h"

extern volatile uint8_t rxedChar;
extern uint8_t newRcvFlag;
extern void OLED_printTextToBuffer(char* txt);
int main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
    initCs();

    PMM_unlockLPM5();

    //Configure eUSCI_a0 pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_UCA0TXD, GPIO_PIN_UCA0TXD, GPIO_FUNCTION_UCA0TXD);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCA0RXD, GPIO_PIN_UCA0RXD, GPIO_FUNCTION_UCA0RXD);
    //Configure eUSCI_a1 pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1); //On-Board green led
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);

    init_eUSCIA0();
    init_eUSCIA1();
    initCommands();
    initADC();
    init_TimerA();

    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

    // Enable global interrupts
    __enable_interrupt();
    OLED_printTextToBuffer("Deneme");
    console_start();



}
