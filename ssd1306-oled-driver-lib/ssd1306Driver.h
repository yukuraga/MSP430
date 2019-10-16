/*
 * typedef.h
 *
 *  Created on: 25 Eyl 2019
 *      Author: ugur
 */

#ifndef SSD1306DRIVER_H_
#define SSD1306DRIVER_H_

#define DISPLAY_BUFFER_SIZE 1024
#define ROW_SIZE 128

#define DATA 0
#define COMMAND 1

#define OLED_RESET_PIN GPIO_PIN3
#define OLED_RESET_PORT GPIO_PORT_P1

#define SSD1306_SET_CONTRAST           0x81
#define SSD1306_ENTIRE_DISPLAY_RESUME  0xA4
#define SSD1306_ENTIRE_DISPLAY_ON      0xA5
#define SSD1306_NORMAL_DISPLAY         0xA6
#define SSD1306_INVERSE_DISPLAY        0xA7
#define SSD1306_DISPLAY_OFF            0xAE
#define SSD1306_DISPLAY_ON             0xAF
#define SSD1306_SET_LCOL_START_ADDRESS 0x00
#define SSD1306_SET_HCOL_START_ADDRESS 0x10
#define SSD1306_MEMORY_ADDRESS_MODE    0x20
#define SSD1306_SET_COLUMN_ADDRESS     0x21
#define SSD1306_SET_PAGE_ADDRESS       0x22
#define SSD1306_SET_PAGE_START_ADDRESS 0xB0
#define SSD1306_SET_START_LINE         0x40
#define SSD1306_SEGMENT_REMAP          0xA0
#define SSD1306_SET_MULTIPLEX_RATIO    0xA8
#define SSD1306_COM_SCAN_NORMAL        0xC0
#define SSD1306_COM_SCAN_INVERSE       0xC8
#define SSD1306_SET_DISPLAY_OFFSET     0xD3
#define SSD1306_SET_COM_PINS_CONFIG    0xDA
#define SSD1306_SET_DISPLAY_CLOCK_DIV  0xD5
#define SSD1306_SET_PRECHARGE_PERIOD   0xD9
#define SSD1306_SET_VCOM_DESELECT_LVL  0xDB
#define SSD1306_NOP                    0xE3
#define SSD1306_SET_CHARGE_PUMP        0x8D

//Function pointer for sending the buffer to OLED display module
typedef void (*sendFunction)(unsigned char buffer, unsigned char dataOrCommand);

void OLED_setSendFunction(sendFunction _send);
void OLED_ssd1306Init();
void OLED_clearBuffer();
void OLED_printTextToBuffer(const char* txt, int size);
void OLED_updateDisplay();


#endif /* SSD1306DRIVER_H_ */
