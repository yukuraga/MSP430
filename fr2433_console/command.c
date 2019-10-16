/*
 * commands.c
 *
 *  Created on: 2 Eyl 2019
 *      Author: ugur
 */

#include "command.h"
#include "driverlib.h"
#include "esp8266.h"
#include <stdlib.h>

commandStruct* p_head = NULL;
commandStruct* p_curr = NULL;

void initCommands(){
    registerCommand("help", helpCommand);
    registerCommand("led", ledCommand);
    registerCommand("temp", tempCommand);
    registerCommand("clock", clockCommand);
    registerCommand("timer", timerCommand);
    registerCommand("esp", esp8266ATCommand);
}

void registerCommand(const char* name, commandFunc f){
    if(p_head == NULL){
        p_head = (commandStruct*)malloc(sizeof(commandStruct));
        p_curr = p_head;
    }else{
        p_curr -> next = (commandStruct*)malloc(sizeof(commandStruct));
        p_curr = p_curr -> next;
    }
    p_curr -> commandName = name;
    p_curr -> invoke = f;
    p_curr -> next = NULL;
}

void printCommands(){
    console_print("Here are the list of commands:\r\n");
    commandStruct* cp = p_head;
    for(; cp != NULL; cp = cp -> next){
        console_print(cp -> commandName);
        console_print("\r\n");
    }
}

void runCommand(const char* name){
    commandStruct* cp = p_head;
    for (; cp != NULL; cp = cp->next)
    {
        if(0==strcmp(cp -> commandName, name)){
            cp -> invoke();
            return;
        }
    }
    console_print("Command not found!\r\n");
}

void helpCommand(){
    printCommands();
}

void clockCommand(){
    char buffer[20];
    uint32_t clk = 0;
    clk = CS_getMCLK();
    ltoa(clk, buffer);
    console_print("MCLK: ");console_print(buffer);console_print("\r\n");
    clk = CS_getACLK();
    ltoa(clk, buffer);
    console_print("ACLK: ");console_print(buffer);console_print("\r\n");
    clk = CS_getSMCLK();
    ltoa(clk, buffer);
    console_print("SMCLK: ");console_print(buffer);console_print("\r\n");
}

extern volatile long IntDegC;
void tempCommand()
{

    long temp = 0;
    char* res = (char*)malloc(5 * sizeof(char));

    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);

    while (ADC_BUSY == ADC_isBusy(ADC_BASE));

    temp = IntDegC;
    ltoa(temp, res);
    console_print("Internal sensor temperature: ");
    console_print(res);
    console_print(" degrees.\r\n");

    free(res);
}

void ledCommand(){
    if (GPIO_INPUT_PIN_HIGH == GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1))
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);
        console_print("Green LED is off.\r\n");
    }
    else
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
        console_print("Green LED is on.\r\n");
    }
}

uint8_t started = 0;
void timerCommand(){
    if(!started){
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
        started = 1;
        console_print("Timer started.\r\n");
    }else{
        Timer_A_stop(TIMER_A1_BASE);
        started = 0;
        console_print("Timer stopped.\r\n");
    }
}

void esp8266ATCommand(){

    esp8266_sendATCommandWithCRLF("AT");

}
