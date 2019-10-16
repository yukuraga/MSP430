/*
 * init.c
 *
 *  Created on: 4 Eyl 2019
 *      Author: ugur
 */

#include "driverlib.h"
#include "init.h"

void initCs(void)
{
    //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Set ACLK = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Create struct variable to store proper software trim values
    CS_initFLLParam param = { 0 };

    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(16000, 488, &param);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 488);
}

void initADC(){
    ADC_init (ADC_BASE, ADC_SAMPLEHOLDSOURCE_SC, ADC_CLOCKSOURCE_ADCOSC, ADC_CLOCKDIVIDER_1);
    ADC_enable(ADC_BASE);
    ADC_setupSamplingTimer (ADC_BASE, ADC_CYCLEHOLD_128_CYCLES, 0);
    ADC_configureMemory(ADC_BASE, ADC_INPUT_TEMPSENSOR,
                        ADC_VREFPOS_INT, // Vref+ = AVcc
                        ADC_VREFNEG_AVSS // Vref- = AVss
                        );
    ADC_enableInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT);
    PMM_enableTempSensor();
    PMM_enableInternalReference();
    /*
    // Configure ADC - Pulse sample mode; ADCSC trigger
    ADCCTL0 |= ADCSHT_15 | ADCON;               // ADC ON,temperature sample period>30us
    ADCCTL1 |= ADCSHP;                          // s/w trig, single ch/conv, MODOSC
    ADCCTL2 |= ADCRES;                          // 10-bit conversion results
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;         // ADC input ch A12 => temp sense
    ADCIE |= ADCIE0;                            // Enable the Interrupt request for a completed ADC_B conversion

    // Configure reference
    PMMCTL0_H = PMMPW_H;                        // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN;            // Enable internal reference and temperature sensor*/
    __delay_cycles(6400);                        // Delay for reference settling
}

void init_TimerA(){
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_initUpModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
    param.timerPeriod = 512;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.startTimer = false;
    Timer_A_initUpMode(TIMER_A1_BASE, &param);
}

void init_eUSCIA0()
{
    // Configure eUSCI_a0
    // http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    EUSCI_A_UART_initParam param = { 0 };
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 10;
    param.secondModReg = 247;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param))
    {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);      // Enable interrupt
}

void init_eUSCIA1()
{
    // Configure eUSCI_a1
    // http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    EUSCI_A_UART_initParam param = { 0 };
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 10;
    param.secondModReg = 247;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param))
    {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A1_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable eUSCI_A1 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);      // Enable interrupt
}
