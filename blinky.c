#include "Configuration_Header_File.h"  /*Header file for Configuration Bits*/
#include <pic18f4550.h>                  /*Header file PIC18f4550 definitions*/

#pragma config PLLDIV = 5 // this does not matter since we are using the internal clock
#pragma config CPUDIV = OSC1_PLL2// this does not matter since we are using the internal clock
#pragma config USBDIV = 2 // this does not matter since we are using the internal clock
                         
                                  // internal clock, pin #14 (RA6) as 1/0 pin, pin #13 unused
#pragma config FOSC = INTOSCIO_EC // if OSCCON is left as default, clock speed will be 1 MHz

// now for the other less confusing options ...
#pragma config FCMEN = OFF // fail-safe monitor disabled
#pragma config IESO = OFF // internal / external osc switchover bit disabled
#pragma config PWRT = OFF // power-up timer disabled
#pragma config BOR = OFF // brown-out reset disabled in hardware & software
#pragma config BORV = 3 // brown-out reset voltage bits, does not matter since brown-out voltage is disabled
#pragma config VREGEN = OFF // USB voltage regulator, would need to turn on if using internal USB voltage regulator for USB comm.
#pragma config WDT = OFF // watchdog timer disabled
#pragma config WDTPS = 32768 // watchdog timer postscale, does not matter since watchdog timer is disabled
#pragma config CCP2MX = ON // use RC1 (pin #16) as CCP2 MUX (this is the default pin for CCP2 MUX)
#pragma config PBADEN = OFF // RBO, RB1, RB2, RB3, & RB4 are configured as digital 1/0 on reset
#pragma config LPT1OSC = OFF// disable low-power option for timer 1 (timer 1 in regular mode)
#pragma config MCLRE = OFF // master clear disabled, pin #1 is for VPP and / or RE3 use
#pragma config STVREN = ON // stack full / underflow will cause reset
#pragma config LVP = OFF// single-supply ICSP disabled
#pragma config ICPRT = OFF // in-circuit debug / programming port (ICPORT) disabled, this feature is not available on 40 pin DIP package
#pragma config XINST = OFF // instruction set extension and indexex addressing mode disabled
#pragma config DEBUG = OFF // background debugger disabled, RA6 & RB7 configured as general purpose 1/0 pins
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF // code protection bits off
#pragma config CPB = OFF // boot block code protection off
#pragma config CPD = OFF // data EEPROM code protection off
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF // write protection bits off
#pragma config WRTC = OFF // config registers write protection off
#pragma config WRTB = OFF // boot block is not write protected
#pragma config WRTD = OFF // data EEPROM is not write protected
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF // table read protection bits off
#pragma config EBTRB = OFF // boot block table read protection off


void delay(void);

void main()
{
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    while(1)
    {
        PORTBbits.RB0 = 1;
        PORTBbits.RB1 = 1;
        PORTBbits.RB2 = 1;
        PORTBbits.RB3 = 1;
        delay();
        PORTBbits.RB0 = 0;
        PORTBbits.RB1 = 0;
        PORTBbits.RB2 = 0;
        PORTBbits.RB3 = 0;
        delay();
    }
}
void delay(void) {
 int i,j;
 
 for(i=0;i<1000;i++)
     for(j=0;j<2;j++);         /*This count Provide delay of 1 ms for 8MHz Frequency */
 } 
