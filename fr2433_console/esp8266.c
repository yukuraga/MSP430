/*
 * esp8266.c
 *
 *  Created on: 10 Eyl 2019
 *      Author: ugur
 */


#include "esp8266.h"

void esp8266_sendATCommandWithCRLF(const char* str){

    while(*str){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, *(str++));
    }
    __delay_cycles(160000); //1ms delay before sending <CR> <LF>

    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, '\r');

    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, '\n');


}
