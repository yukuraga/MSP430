#include "driverlib.h"

bool responseReceived = false;

void esp8266_sendATCommandWithCRLF(const char* str){
    responseReceived = false;
    while(*str){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, *(str++));
    }
    //__delay_cycles(160000); //1ms delay before sending <CR> <LF>

    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, '\r');

    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, '\n');

}

void esp8266_sendDataThroughUARTPassthrough(const unsigned char* data, unsigned char length){

    unsigned char i = 0;
    for(; i<length; i++){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY));
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, data[i]);
    }

}

