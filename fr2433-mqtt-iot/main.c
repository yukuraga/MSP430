#include <ssd1306Driver.h>
#include "driverlib.h"
#include "initialization.h"
#include "Board.h"
#include <string.h>

#define AT_MODE 0
#define WIFI_UART_PASSTHROUGH_MODE 1

void sendByteToOLEDviaSPI (unsigned char data, unsigned char dataOrCommand){
    
    if(dataOrCommand == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        __delay_cycles(40);
    }else{
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        __delay_cycles(40);
    }
        //---------------Send procedure-------------
        //USCI_B0 TX buffer ready?
        while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)) ;

        //Transmit Data to slave
        EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, data);
        //---------------Send procedure-------------
}

extern void esp8266_sendATCommandWithCRLF(const char* str);
extern void esp8266_sendDataThroughUARTPassthrough(const unsigned char* data, unsigned char length);
extern bool responseReceived;

const unsigned char conn_package[] = {
    0x10,0x3a,0x00,0x04,0x4d,0x51,0x54,0x54,
    0x04,0xC2,0x00,0x3c,0x00,0x16,0x6d,0x71,
    0x74,0x74,0x2d,0x65,0x78,0x70,0x6c,0x6f,
    0x72,0x65,0x72,0x2d,0x63,0x38,0x39,0x65,
    0x62,0x31,0x64,0x62,0x00,0x08,0x75,0x78,
    0x67,0x6e,0x72,0x7a,0x64,0x72,0x00,0x0c,
    0x6b,0x75,0x30,0x34,0x5a,0x39,0x58,0x41,
    0x70,0x36,0x52,0x71};

unsigned char publish_package[] = {
    0x30,0x0B,0x00,0x08,0x4d,0x53,0x50,0x2f,
    0x74,0x65,0x73,0x74,0x31};


extern char rxString[256];

unsigned char modeFlag = AT_MODE;

void main (void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    init_ClockSystem();
    init_GPIO();
    init_SPI();
    init_eUSCIA0();
    init_eUSCIA1();
    init_TimerA();

    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN2);

    __enable_interrupt();
    OLED_setSendFunction(sendByteToOLEDviaSPI);
    OLED_ssd1306Init();

    OLED_printTextToBuffer("MSP433FR2433 IoT Node\n\n", 23);
    OLED_updateDisplay();

    __delay_cycles(60000000);

    esp8266_sendATCommandWithCRLF("AT");
    while(!responseReceived);

    esp8266_sendATCommandWithCRLF("AT+CIPMODE=1");
    while(!responseReceived);

    esp8266_sendATCommandWithCRLF("AT+CIPSTART=\"TCP\",\"108.129.17.116\",16786");
    while(!responseReceived);

    esp8266_sendATCommandWithCRLF("AT+CIPSEND");
    while(!responseReceived);
    modeFlag = WIFI_UART_PASSTHROUGH_MODE;

    esp8266_sendDataThroughUARTPassthrough(conn_package, 60);
    while(!responseReceived);

    unsigned char t = 0;
    while(1){

        esp8266_sendDataThroughUARTPassthrough(publish_package, 13);

        t ^= 0x01;
        publish_package[12] = 0x30 + t;
        OLED_printTextToBuffer("Sent: ", 6);
        OLED_printTextToBuffer((const char*)&publish_package[12], 1);
        OLED_printTextToBuffer("\n", 1);
        OLED_updateDisplay();

        __delay_cycles(64000000);

    }
}

