#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306_i2c_lib.h"
#include "ssd1306_lib.h"
#include "driverlib.h"
#include "uart_log.h"

int main(void)
{

    WDT_A_hold(WDT_A_BASE);

    //Set DCO FLL reference = REFO
    CS_initClockSignal(
            CS_FLLREF,
            CS_REFOCLK_SELECT,
            CS_CLOCK_DIVIDER_1
    );

    //Set Ratio and Desired MCLK Frequency and initialize DCO
    CS_initFLLSettle(1000, 30);

    //Set ACLK = VLO with frequency divider of 1
    CS_initClockSignal(
            CS_ACLK,
            CS_VLOCLK_SELECT,
            CS_CLOCK_DIVIDER_1
    );

    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(
            CS_SMCLK,
            CS_DCOCLKDIV_SELECT,
            CS_CLOCK_DIVIDER_1
    );

    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(
            CS_MCLK,
            CS_DCOCLKDIV_SELECT,
            CS_CLOCK_DIVIDER_1
    );

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    ssd1306Init();
    i2c_init();
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

    fillDisplay (0x00);

    draw12x16Str(0,  0, "HelloWorld", 1);
    draw6x8Str(0, 3, "/* Ugur Kaya */", 1, 0);
    draw6x8Str(0, 6, "/* MSP430FR2433 */", 1, 0);

    return (0);
}
