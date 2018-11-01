//******************************************************************************************************************************************
//                 MSP430FR2433
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |                 |
//            |                 |
//            |         SDA/P1.2|------->
//            |         SCK/P1.3|------->
//******************************************************************************************************************************************
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306_i2c_lib.h"
#include "driverlib.h"

volatile uint32_t _i;
uint8_t SlaveFlag = 0;
unsigned char data [2];
// Pointer to TX data
uint8_t *TXData;
uint8_t TXByteCtr;

#define OLED_I2C_ADDRESS 0x3C

//Init function
void i2c_init () {
    //************************PORT SETTING*************************
    // Old Configuration for PORT settings
    /*
    P1OUT &= ~BIT3;
    P1DIR|=BIT2 | BIT3;
    // Configure Pins for I2C
    P1SEL0 |= BIT2 | BIT3;                            // I2C pins
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    */

    //New Configuration for PORT settings from driverlib
    // Configure Pins for I2C
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN2,
            GPIO_PRIMARY_MODULE_FUNCTION
            );
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN3,
            GPIO_PRIMARY_MODULE_FUNCTION
            );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();
    //************************END PORT SETTING*********************

    //************************USCI SETTING*************************
    //Old configuration for USCI setting
    /*
    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                             // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST;                    // I2C master mode, SMCLK
    UCB0BRW = 0x8;                                    // baudrate = SMCLK / 8
    UCB0CTLW0 &=~ UCSWRST;                            // clear reset register
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable
    */

    //New configuration for USCI setting from driverlib
    EUSCI_B_I2C_initMasterParam param = { 0 };
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, OLED_I2C_ADDRESS);

    //Set Master in transmit mode
    EUSCI_B_I2C_setMode(
            EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_MODE
            );

    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(
            EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT
            );
    //Enable master Receive interrupt
    EUSCI_B_I2C_enableInterrupt(
            EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT
            );

    //************************END USCI SETTING*********************

    __enable_interrupt();
}
//Transmit function
void i2c_transmit (unsigned char *params, unsigned char length) {
    //Old function body
    /*
    TI_transmit_field = params;

    __delay_cycles(5000);
    UCB0I2CSA = SlaveAddress;       // configure slave address
    TXByteCtr = flag;               // Load TX byte counter
    while (UCB0CTLW0 & UCTXSTP);    // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;    // I2C TX, start condition

    __delay_cycles(2500);
    */

    //driverlib based new function body
    __delay_cycles(500);
    TXData = params;
    TXByteCtr = length;
    while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));
    EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);

    __delay_cycles(500);

}
/*
// I2C interrupt service routine
#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void) {
  switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG)) {
  case USCI_I2C_UCNACKIFG:
      UCB0CTL1 |= UCTXSTT;                      //resend start if NACK
      break;                                      // Vector 4: NACKIFG break;
  case USCI_I2C_UCTXIFG0:
      if (TXByteCtr)  {                              // Check TX byte counter
          UCB0TXBUF = *TI_transmit_field;
          TI_transmit_field++;
          TXByteCtr--;                              // Decrement TX byte counter
      } else {
          UCB0CTLW0 |= UCTXSTP;                     // I2C stop condition
          UCB0IFG &= ~UCTXIFG;                      // Clear USCI_B0 TX int flag
      }
      break;                                      // Vector 26: TXIFG0 break;
  }
}*/

//------------------------------------------------------------------------------
// The USCIAB0TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    switch (__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
    {
    case USCI_NONE:             // No interrupts break;
        break;
    case USCI_I2C_UCALIFG:      // Arbitration lost
        break;
    case USCI_I2C_UCNACKIFG:    // NAK received (master only)
        //resend start if NACK
        EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
        break;
    case USCI_I2C_UCTXIFG0:     // TXIFG0
        // Check TX byte counter
        if (TXByteCtr)
        {
            EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, *(TXData++));
            // Decrement TX byte counter
            TXByteCtr--;
        }
        else
        {
            EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
            EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
        }
        break;
    default:
        break;
    }
}
