/*
 * esp8266.h
 *
 *  Created on: 10 Eyl 2019
 *      Author: ugur
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include "driverlib.h"

#define AT_COMMAND AT
#define AT_VERSION_COMMAND AT+GMR

void esp8266_sendATCommandWithCRLF(const char* str);

#endif /* ESP8266_H_ */
