#include <msp430.h>

// MAX7219 Register addresses
#define MAX_NOOP    0x00
#define MAX_DIGIT0  0x01
#define MAX_DIGIT1  0x02
#define MAX_DIGIT2  0x03
#define MAX_DIGIT3  0x04
#define MAX_DIGIT4  0x05
#define MAX_DIGIT5  0x06
#define MAX_DIGIT6  0x07
#define MAX_DIGIT7  0x08
#define MAX_DECODEMODE  0x09
#define MAX_INTENSITY   0x0A
#define MAX_SCANLIMIT   0x0B
#define MAX_SHUTDOWN    0x0C
#define MAX_DISPLAYTEST 0x0F

unsigned char imageBuffer[16][16] = {

{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
{0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
{0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0},
{0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0},
{0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0}

};

unsigned char displayBuffer[] = {
0x00, 0x00, //0x01 0x01
0x00, 0x00, //0x02 0x02
0x00, 0x00, //0x03 0x03
0x00, 0x00, //0x04 0x04
0x00, 0x00, //0x05 0x05
0x00, 0x00, //0x06 0x06
0x00, 0x00, //0x07 0x07
0x00, 0x00, //0x08 0x08

0x00, 0x00,  //0x01 0x01
0x00, 0x00,  //0x02 0x02
0x00, 0x00,  //0x03 0x03
0x00, 0x00,  //0x04 0x04
0x00, 0x00,  //0x05 0x05
0x00, 0x00,  //0x06 0x06
0x00, 0x00,  //0x07 0x07
0x00, 0x00  //0x08 0x08
};

void inline movePad(unsigned char imageBuffer [][16], unsigned char direction){ //0: left 1: right

    unsigned int padPos = 0, i;
    for(i=0;i<14;i++){
        if(imageBuffer[15][i] == 1){
            padPos = i;
            break;
        }
    }

    if(direction){
        if(padPos<13){
        imageBuffer[15][padPos] = 0;
        imageBuffer[15][padPos+3] = 1;
        }
    }else{
        if(padPos>0){
            imageBuffer[15][padPos+2] = 0;
            imageBuffer[15][padPos-1] = 1;
        }
    }
}


void SPI_Send2MAX7219(unsigned char address, unsigned char data)
{
    _delay_cycles(50);
    while (UCA1STATW & UCBUSY);     // Wait until done
    UCA1TXBUF = (address & 0x0F);
    while (UCA1STATW & UCBUSY);     // Wait until done
    UCA1TXBUF = (data);
    while (UCA1STATW & UCBUSY);     // Wait until done
}

void SPI_MAX7219ToggleLoad(void)
{
    P1OUT |= BIT0;
    _delay_cycles(50);
    P1OUT &= ~BIT0;
}

void updateDisplay(unsigned char* buffer){

    unsigned char t = 0 ;
    for(t=0; t<8; t++){
        SPI_Send2MAX7219(7-t+1, buffer[2*t+17]);
        SPI_Send2MAX7219(7-t+1, buffer[2*t+16]);
        SPI_Send2MAX7219(7-t+1, buffer[2*t+1]);
        SPI_Send2MAX7219(7-t+1, buffer[2*t]);
        SPI_MAX7219ToggleLoad();
    }

    /*
    for(t=8; t<16; t++){
        SPI_Send2MAX7219(MAX_NOOP, 0x00);
        SPI_Send2MAX7219(MAX_NOOP, 0x00);

    }
    SPI_MAX7219ToggleLoad();
    */
}

void processImageBuffer(unsigned char imgbuffer[][16], unsigned char* dispBuf){

    unsigned char tmp=0;
    unsigned int k = 0, i=0;

    for (k = 0; k < 16; k++){

        for (i = 0; i < 8; i++){
            if (imgbuffer[k][i] == 1){
                tmp |= (1 << i);
            }
        }

        dispBuf[2*k] = tmp;
        tmp = 0;

        for (i = 8; i < 16; i++){
            if (imgbuffer[k][i] == 1){
                tmp |= (1 << (i-8));
            }
        }

        dispBuf[2*k+1] = tmp;
        tmp = 0;

    }
}



void delay_ms(unsigned int ms );

void SPI_init(void)
{
    //CSCTL3 |= SELREF__REFOCLK; //Select REF0CLK as reference clk

    // Configure SPI  pins3
    //P2SEL1 |= BIT4 | BIT5 | BIT6;
    //P2SEL0 |= BIT4 | BIT5 | BIT6;    // set pins 2.4, 2.5, 2.6 into SPI mode (primary)

    P2SEL0 |= BIT4 | BIT5 | BIT6;             // set 3-SPI pin as second function

    P1DIR |= BIT0; //P1.0 as output
    P2DIR |= BIT4 | BIT6; //P2.4 and P2.6 as Outputs
    P1OUT &= ~BIT0;

    //Configure UCA1 CONTROL register
    UCA1CTLW0 |= UCSWRST; // Software reset enable (To modify Settings)

    UCA1CTLW0 |= UCMST | UCSYNC | UCMSB | UCCKPL; //MSB first, select master mode,
    //select SPI mode, select SMCLK as clk source

    UCA1CTLW0 |= UCSSEL__SMCLK;               // SMCLK
    UCA1BR0 = 0x01;                           // /2,fBitClock = fBRCLK/(UCBRx+1).
    UCA1BR1 = 0;                              //
    UCA1MCTLW = 0;                            // No modulation

    UCA1CTLW0 &= ~UCSWRST; // Software reset disable (turn on SPI)
    __delay_cycles(100000);

    //UCA1IE |= UCTXIE; //Transmit interrupt enable, Receive interrupt enable
    _BIS_SR(GIE); //Enable general interrupts
}



void init_MAX7219(void)
{
    int i;
    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_NOOP, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_SCANLIMIT, 0x07);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_INTENSITY, 0x01);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DECODEMODE, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT0, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT1, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT2, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT3, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT4, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT5, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT6, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_DIGIT7, 0x00);
    SPI_MAX7219ToggleLoad();

    for(i = 0; i < 4; i++)
        SPI_Send2MAX7219(MAX_SHUTDOWN, 0x01);
    SPI_MAX7219ToggleLoad();
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // stop watchdog timer


    P2OUT |= BIT3 | BIT7;       // Configure P2.3 P2.7 pull-up
    P2DIR &= ~(BIT3 | BIT7);    // Configure P2.3 P2.7 as input
    P2REN |= BIT3 | BIT7;       // P2.3 P2.7 Enable pull-up resistors
    P2IES |= BIT3 | BIT7;       // P2.3 P2.7 Hi/Lo edge
    P2IE |= BIT3 | BIT7;        // P2.3 P2.7 interrupt enabled

    PM5CTL0 &= ~LOCKLPM5;           // Disable the GPIO power-on default high-impedance mode
                                        // to activate 1previously configured port settings3

    P2IFG &= ~(BIT3 | BIT7);    // Clear P2.3 P2.7 IFG

    __delay_cycles(100000);
    SPI_init();
    __delay_cycles(100000);
    init_MAX7219();
    __delay_cycles(100000);

    SPI_Send2MAX7219(0x0F, 0x00);
    SPI_Send2MAX7219(0x0F, 0x00);
    SPI_Send2MAX7219(0x0F, 0x00);
    SPI_Send2MAX7219(0x0F, 0x00);
    SPI_MAX7219ToggleLoad();
    __delay_cycles(1000000);

    unsigned char xofst = 1, yofst = 1, x = 14, y = 8;

    while(1){

        if(x==14){
            if(imageBuffer[x+1][y] == 1)
                xofst = -1;
            else
                break;//game failed
        }

        if(x==0)
            xofst=1;
        if(y==15)
            yofst=-1;
        if(y==0)
            yofst=1;

        imageBuffer[x][y] = 0;

        x+=xofst;
        y+=yofst;

        imageBuffer[x][y] = 1;

        processImageBuffer(imageBuffer, displayBuffer);
        updateDisplay(displayBuffer);
        __delay_cycles(90000);
    }

}

void delay_ms(unsigned int ms )
{
     unsigned int i;
     for (i = 0; i<= ms; i++)
     __delay_cycles(1000); //Built-in function that suspends the execution for 1000 cycles
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT2_VECTOR)))
#endif
void Port2_ISR(void)
{
    // Left button S1
    if (P2IFG & BIT3)
    {
        P2IFG &= ~BIT3;

        // Left button S1 released
        if (P2IN & BIT3)
        {
            movePad(imageBuffer, 0);
        }
        else
            P2IES &= ~BIT3;       // P2.3 Lo/Hi edge
    }
    // Right button S2
    if (P2IFG & BIT7)
    {
        P2IFG &= ~BIT7;

        // Right button S2 released
        if (P2IN & BIT7)
        {
            movePad(imageBuffer, 1);
        }
        else
            P2IES &= ~BIT7;       // P2.7 Lo/Hi edge
    }
}

