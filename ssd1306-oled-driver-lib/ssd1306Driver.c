#include <ssd1306Driver.h>
#include "font5x7.h"
#include <string.h>
#include <stdint.h>
#include "driverlib.h"

char displayBuffer[1024] = { 0 };
int cursor = 0;
int numOfColumnsInRow = 0;
int line = 0;

//number of char bytes
int OLED_getNumberOfCharBytes(char c) {
	unsigned int i = 6 * (c - 0x20);

	uint8_t count = 1; // to include the last empty column of the char
	if (c == ' ') {
		return 4;
	}
	else if (c == '"') {
		return 5;
	}else if (c == '\n' || c == '\r'){
	    return 0;
	}
	else {

		while (font5x7[i++] > 0) {

			count++;

		}
	}
	return count;
}

//Returns the start offset of given char from font file
int OLED_getStartOffsetOfChar(char c) {
    if(c<0x20)
        return 0x23A; //Start offset of filler char for undefined chars
	return 6 * (c - 0x20);

}

void OLED_clearBuffer() {

	memset((char*)displayBuffer, 0, DISPLAY_BUFFER_SIZE);
	cursor = 0;

}

void OLED_scrollUp() {
    int i = 1;
	for (; i<8; i++) {
		memcpy(displayBuffer + (ROW_SIZE * (i - 1)), displayBuffer + (ROW_SIZE * i), ROW_SIZE);
	}
	memset( displayBuffer + 896, 0, ROW_SIZE );
	//OLED_updateDisplay();
	//__delay_cycles(15000000);

}

void OLED_printTextToBuffer(const char* txt, int size) {
    unsigned int i = 0;
	for(; i<size; i++)
	{
	    if(txt[i] == '\r'){
	        continue;
	    }
	    if( numOfColumnsInRow + OLED_getNumberOfCharBytes(txt[i]) > ROW_SIZE || txt[i] == '\n' )
        {
            numOfColumnsInRow = 0;
            if( line < 7 )
            {
                line++;
            }
            else
            {
                OLED_scrollUp();

            }
            cursor = line * ROW_SIZE;
            if(txt[i] == '\n'){
                continue;
            }
        }

        memcpy((char*) (displayBuffer + cursor), (const char*) (font5x7 + OLED_getStartOffsetOfChar(txt[i])), OLED_getNumberOfCharBytes(txt[i]));
        cursor += OLED_getNumberOfCharBytes(txt[i]);
        numOfColumnsInRow += OLED_getNumberOfCharBytes(txt[i]);
	}
}

sendFunction OLED_sendBufferToOLED;

void OLED_updateDisplay() {
    int i = 0;
    for(;i<DISPLAY_BUFFER_SIZE;i++)
        OLED_sendBufferToOLED(displayBuffer[i], DATA);
}

void OLED_setSendFunction(sendFunction _send) {

    OLED_sendBufferToOLED = _send;
}

void OLED_sendCommand(unsigned char command){
    OLED_sendBufferToOLED(command, COMMAND);
}

void OLED_ssd1306Init() {
    GPIO_setOutputLowOnPin(OLED_RESET_PORT, OLED_RESET_PIN);
    __delay_cycles(100);
    GPIO_setOutputHighOnPin(OLED_RESET_PORT, OLED_RESET_PIN);

    OLED_sendCommand(SSD1306_DISPLAY_OFF);                                   /* Entire Display OFF */
    OLED_sendCommand(SSD1306_SET_DISPLAY_CLOCK_DIV);                         /* Set Display Clock Divide Ratio and Oscillator Frequency */
    OLED_sendCommand(0x80);                                                  /* Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended */
    OLED_sendCommand(SSD1306_SET_MULTIPLEX_RATIO);                           /* Set Multiplex Ratio */
    OLED_sendCommand(0x3F);                                                  /* 64 COM lines */
    OLED_sendCommand(SSD1306_SET_DISPLAY_OFFSET);                            /* Set display offset */
    OLED_sendCommand(0x00);                                                  /* 0 offset */
    OLED_sendCommand(SSD1306_SET_START_LINE | 0x00);                         /* Set first line as the start line of the display */
    OLED_sendCommand(SSD1306_SET_CHARGE_PUMP);                               /* Charge pump */
    OLED_sendCommand(0x14);                                                  /* Enable charge dump during display on */
    OLED_sendCommand(SSD1306_MEMORY_ADDRESS_MODE);                           /* Set memory addressing mode */
    OLED_sendCommand(SSD1306_SET_LCOL_START_ADDRESS);                        /* Horizontal addressing mode */
    OLED_sendCommand(SSD1306_SEGMENT_REMAP | 0x01);                          /* Set segment remap with column address 127 mapped to segment 0 */
    OLED_sendCommand(SSD1306_COM_SCAN_INVERSE);                              /* Set com output scan direction, scan from com63 to com 0 */
    OLED_sendCommand(SSD1306_SET_COM_PINS_CONFIG);                           /* Set com pins hardware configuration */
    OLED_sendCommand(0x12);                                                  /* Alternative com pin configuration, disable com left/right remap */
    OLED_sendCommand(SSD1306_SET_CONTRAST);                                  /* Set contrast control */
    OLED_sendCommand(0x80);                                                  /* Set Contrast to 128 */
    OLED_sendCommand(SSD1306_SET_PRECHARGE_PERIOD);                          /* Set pre-charge period */
    OLED_sendCommand(0xF1);                                                  /* Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK */
    OLED_sendCommand(SSD1306_SET_VCOM_DESELECT_LVL);                         /* Set Vcomh deselect level */
    OLED_sendCommand(0x40);                                                  /* Vcomh deselect level */
    OLED_sendCommand(SSD1306_ENTIRE_DISPLAY_RESUME);                         /* Entire display ON, resume to RAM content display */
    OLED_sendCommand(SSD1306_NORMAL_DISPLAY);                                /* Set Display in Normal Mode, 1 = ON, 0 = OFF */
    OLED_sendCommand(SSD1306_DISPLAY_ON);                                    /* Display on in normal mode */
}

