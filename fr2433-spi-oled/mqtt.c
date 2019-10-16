/*
 * mqtt.c
 *
 *  Created on: 6 Eki 2019
 *      Author: ugur
 */

#include "ssd1306Driver.h"

#define CONNECT 0x10
#define CONNACK 0x20
#define PUBLISH 0x30
#define PUBACK 0x40
#define PUBREC 0x50
#define PUBREL 0x62
#define PUBCOMP 0x70
#define SUBSCRIBE 0x82
#define SUBACK 0x90
#define UNSUBSCRIBE 0xA2
#define UNSUBACK 0xB0
#define PINGREQ 0xC0
#define PINGRESP 0xD0
#define DISCONNECT 0xE0
#define AUTH 0xF0

void MQTT_parseMqttByteStream(unsigned char* mqttByteStream, unsigned char length){

    switch(mqttByteStream[0]){

    case CONNACK:
        if(length==4 && mqttByteStream[3] == 0){
            OLED_printTextToBuffer("Successfully connected to CloudMQTT!\n", 37);
            OLED_updateDisplay();
        }
        break;
    case SUBACK:
        break;
    case PUBLISH:
        break;
    default:
        OLED_printTextToBuffer("MQTT response error!\n", 21);
        OLED_updateDisplay();
        break;
    }

}
