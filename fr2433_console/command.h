/*
 * command.h
 *
 *  Created on: 2 Eyl 2019
 *      Author: ugur
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdlib.h>
#include <stdio.h>
#include "console.h"
#include <string.h>

typedef void (*commandFunc)(void);

typedef struct commandStruct {
    const char* commandName;
    commandFunc invoke;
    struct commandStruct* next;
}commandStruct;

void initCommands();
void registerCommand(const char* name, commandFunc f);
void printCommands();
void runCommand(const char* name);

void helpCommand();
void ledCommand();
void tempCommand();
void clockCommand();
void timerCommand();
void esp8266ATCommand();

#endif /* COMMAND_H_ */
