/*
 * File:   Basic_UART_TX.c
 * Author: Phil Roos
 *
 * Created on November 7, 2024, 9:46 AM
 */

//#include <pic18f4680>
#include <xc.h>
                /*Header file PIC18f4550 definitions*/

                        
                                  // internal clock, pin #14 (RA6) as 1/0 pin, pin #13 unused
#pragma config OSC = IRCIO67 // if OSCCON is left as default, clock speed will be 1 MHz

#pragma config FCMEN = OFF // fail-safe monitor disabled
#pragma config IESO = OFF // internal / external osc switchover bit disabled
#pragma config PWRT = ON // power-up timer disabled
#pragma config BOREN = BOACTIVE      // Brown-out Reset disabled in hardware and software
#pragma config BORV = 3 // brown-out reset voltage bits, does not matter since brown-out voltage is disabled

#pragma config WDT = ON // watchdog timer disabled
#pragma config WDTPS = 512 // watchdog timer postscale, does not matter since watchdog timer is disabled
#pragma config PBADEN = OFF // RBO, RB1, RB2, RB3, & RB4 are configured as digital 1/0 on reset
#pragma config LPT1OSC = OFF// disable low-power option for timer 1 (timer 1 in regular mode)
#pragma config MCLRE = OFF // master clear disabled, pin #1 is for VPP and / or RE3 use
#pragma config STVREN = ON // stack full / underflow will cause reset
#pragma config LVP = OFF// single-supply ICSP disabled
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
void UART_TransmitString(char* str);
void UART_Transmit(char data);

void main(void) {
    TRISCbits.TRISC1 = 1;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD1 = 0;
    OSCCONbits.IRCF2 = 1;  // Bit 6
    OSCCONbits.IRCF1 = 1;  // Bit 5
    OSCCONbits.IRCF0 = 0;  // Bit 4
    
    // UART configuration
    // Baud rate calculation for 9600 baud with 4 MHz oscillator
    // SPBRG = (FOSC / (64 * Desired Baud Rate)) - 1
    // SPBRG = (4000000 / (64 * 9600)) - 1 ≈ 6.51
    SPBRG = 6;

    // Configure TXSTA register
    TXSTAbits.TXEN = 1;    // Enable transmitter
    TXSTAbits.BRGH = 0;    // Low speed baud rate
    TXSTAbits.SYNC = 0;    // Asynchronous mode

    // Configure RCSTA register
    RCSTAbits.SPEN = 1;    // Enable serial port
    RCSTAbits.CREN = 1;    // Enable continuous receive

    // Configure BAUDCON register (if needed)
    BAUDCONbits.BRG16 = 0;
    
    while(1) {
        // Turn on the LED
        LATDbits.LATD4 = 1;
        LATDbits.LATD5 = 1;
        LATDbits.LATD1 = 1;
        delay();
        UART_TransmitString("Hello World!\r\n");
        delay();
        
        // Turn off the LED
        LATDbits.LATD4 = 0;
        LATDbits.LATD5 = 0;
        LATDbits.LATD1 = 0;
        delay();
    }
    
    return;
}

void delay(void) {
    int i,j;

    for(i=0;i<10000;i++)
        for(j=0;j<2;j++);         /*This count Provide delay of 1 ms for 8MHz Frequency */
 } 

void UART_TransmitString(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        UART_Transmit(str[i]);
    }
}

void UART_Transmit(char data) {
    while (!PIR1bits.TXIF);  // Wait for transmit buffer to be empty
    TXREG = data;            // Send the data
}
