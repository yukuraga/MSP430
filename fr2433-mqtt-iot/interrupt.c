/*
 * interrupt.c
 *
 *  Created on: 28 Eyl 2019
 *      Author: ugur
 */
#include "driverlib.h"
#include "ssd1306Driver.h"

#define AT_MODE 0
#define WIFI_UART_PASSTHROUGH_MODE 1

extern unsigned char modeFlag;
extern esp8266_sendATCommandWithCRLF(const char* msg);
extern void esp8266_sendDataThroughUARTPassthrough(unsigned char*, unsigned char);
extern void MQTT_parseMqttByteStream(unsigned char* mqttByteStream, unsigned char length);
volatile uint8_t rxedChar0 = 0;

#pragma vector=USCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG:

        rxedChar0 = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
        while (EUSCI_A_UART_queryStatusFlags(EUSCI_A0_BASE, EUSCI_A_UART_BUSY));
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, rxedChar0);
        OLED_printTextToBuffer(&rxedChar0, 1);
        OLED_updateDisplay();

        break;
    case USCI_UART_UCTXIFG:
        break;
    case USCI_UART_UCSTTIFG:
        break;
    case USCI_UART_UCTXCPTIFG:
        break;
    }
}

#define MAX_STR_LEN 384
char rxString[MAX_STR_LEN] = {0};
void receiveStringWithTimer(char data);
unsigned char rxedChar1 = 0;

#pragma vector=USCI_A1_VECTOR
__interrupt void EUSCI_A1_ISR(void)
{
    switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG:
        rxedChar1 = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
        receiveStringWithTimer(rxedChar1);
        break;
    case USCI_UART_UCTXIFG:
        break;
    case USCI_UART_UCSTTIFG:
        break;
    case USCI_UART_UCTXCPTIFG:
        break;
    }
}


bool rxStringReady = false;

// Receives strings terminated with \n
void receiveStringLF(char data) {
    static bool rxInProgress = false;
    static unsigned int charCnt = 0;

    if(!rxInProgress){
        if ((data != '\n') ){
            rxInProgress = true;
            charCnt = 0;
            rxString[charCnt] = data;
        }
    }else{ // in progress
        charCnt++;
        if((data != '\n')){
            if (charCnt >= MAX_STR_LEN){
                rxInProgress = false;
            }else{
                rxString[charCnt] = data;
            }
        }else{
            rxInProgress = false;
            rxString[charCnt] = '\0';
            // String receive complete
            rxStringReady = true;
        }
    }
}

// Receives strings terminated with \r\n
void receiveStringCRLF(char data) {
    static unsigned int charCnt = 0;
    rxString[charCnt++] = data;
    if(data == '\n' && charCnt>3 && rxString[charCnt-4] == 'O' && rxString[charCnt-3] == 'K' && rxString[charCnt-2] == '\r'){

        rxString[charCnt] = '\0';
        charCnt = 0;
        rxStringReady = true;
    }
}

bool rxInProgress = false;
unsigned int charCnt = 0;

void receiveStringWithTimer(char data){
    rxString[charCnt++] = data;
    if(!rxInProgress){
        rxInProgress = true;
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    }else{

        Timer_A_stop(TIMER_A1_BASE);
        Timer_A_clear(TIMER_A1_BASE);
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    }
}

extern bool responseReceived;
//******************************************************************************
//
//This is the TIMER1_A3 interrupt vector service routine.
//
//******************************************************************************
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR (void)
{
    //Any access, read or write, of the TAIV register automatically resets the
    //highest "pending" interrupt flag
    switch ( __even_in_range(TA1IV,TA1IV_TAIFG) ){
        case  TA1IV_NONE: break;                       //No interrupt
        case  TA1IV_TACCR1: break;                     //CCR1 not used
        case  TA1IV_TACCR2: break;                     //CCR2 not used
        case  TA1IV_3: break;                          //CCR3 not used
        case  TA1IV_4: break;                          //CCR4 not used
        case  TA1IV_5: break;                          //CCR5 not used
        case  TA1IV_6: break;                          //CCR6 not used
        case  TA1IV_TAIFG:

            Timer_A_stop(TIMER_A1_BASE);
            Timer_A_clear(TIMER_A1_BASE);

            if (modeFlag == AT_MODE){

                rxString[charCnt] = '\0';

                OLED_printTextToBuffer(rxString, charCnt);
                OLED_updateDisplay();

            }else{

                MQTT_parseMqttByteStream((unsigned char*)rxString, charCnt);

            }

            rxInProgress = false;
            responseReceived = true;
            charCnt = 0;

            break;
        default: break;
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    switch (__even_in_range(P2IV, P1IV_P1IFG7))
    {
    case P1IV_NONE: /* No Interrupt pending */
        break;
    case P1IV_P1IFG0: /* P1IV P1IFG.0 */
        break;
    case P1IV_P1IFG1: /* P1IV P1IFG.1 */
        break;
    case P1IV_P1IFG2: /* P1IV P1IFG.2 */
        break;
    case P1IV_P1IFG3: /* P1IV P1IFG.3 */

        break;
    case P1IV_P1IFG4: /* P1IV P1IFG.4 */
        break;
    case P1IV_P1IFG5: /* P1IV P1IFG.5 */
        break;
    case P1IV_P1IFG6: /* P1IV P1IFG.6 */
        break;
    case P1IV_P1IFG7: /* P1IV P1IFG.7 */

        break;
    default:
        break;
    }

}
