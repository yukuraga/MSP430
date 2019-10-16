/*
 * console.c
 *
 *  Created on: 1 Eyl 2019
 *      Author: ugur
 */

#include "console.h"
#include "command.h"
#include "log.h"

uint8_t buffer[CONSOLE_LINE_BUFFER_SIZE] = {0};
uint8_t index = 0;

void console_start(){
    console_print("************************************\r\n");
    console_print("*    Welcome to MSP430 console!    *\r\n");
    console_print("*                                  *\r\n");
    console_print("*    For commands write HELP and   *\r\n");
    console_print("*    press Enter.                  *\r\n");
    console_print("*                                  *\r\n");
    console_print("*    Please set up your terminal   *\r\n");
    console_print("*    to send CR as new line.       *\r\n");
    console_print("************************************\r\n");
    console_print(">");
}

void console_processCommand(uint8_t* bfr){

    runCommand((const char *)bfr);
    console_print(">");

}

void console_print(const char* _str){
    while (*_str != 0)
    {
        // Transmit Character
        while (EUSCI_A_UART_queryStatusFlags(EUSCI_A0_BASE, EUSCI_A_UART_BUSY));
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, *(_str++));

    }
}

void console_getChar(uint8_t _chr){

    switch (_chr){

    case 0x08:  //backspace
    case 0x7F:  //backspace

        if(index > 0){
            buffer[--index] = 0;
        }else if(index==0){
            console_print(">");
        }
        break;

    case '\r':
        console_print("\n");
        console_processCommand(buffer);
        memset((uint8_t*)buffer, 0, CONSOLE_LINE_BUFFER_SIZE);
        index=0;
        break;

    default:

        if(index < CONSOLE_LINE_BUFFER_SIZE){
            buffer[index++] = _chr;
        }else{
            LOG("MAIN LOG", "Buffer overflow!\r\n");
        }
    }
}
