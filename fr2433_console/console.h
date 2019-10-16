/*
 * console.h
 *
 *  Created on: 1 Eyl 2019
 *      Author: ugur
 */
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONSOLE_LINE_BUFFER_SIZE 16

void console_start();
void console_getChar(uint8_t _chr);
void console_processCommand(uint8_t* bfr);
void console_print(const char* _str);

#endif /* CONSOLE_H_ */
