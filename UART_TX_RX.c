/*
 * File:   newmain.c
 * Author: proos
 *
 * Created on November 7, 2024, 9:46 AM
 */


//#include <pic18f4680>
#include <xc.h>
#include <stdbool.h>
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


// Configuration bits and other settings remain unchanged

void delay(void);    
void UART_Init(void);

void main(void) {
    TRISCbits.TRISC1 = 1;
    TRISCbits.TRISC6 = 0;  // TX pin as output
    TRISCbits.TRISC7 = 1;  // RX pin as input
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD1 = 0;
    
    // Oscillator settings
    OSCCONbits.IRCF2 = 1;  // Bit 6
    OSCCONbits.IRCF1 = 1;  // Bit 5
    OSCCONbits.IRCF0 = 0;  // Bit 4

    UART_Init();

    // Enable interrupts
    INTCONbits.PEIE = 1;   // Enable peripheral interrupts
    INTCONbits.GIE = 1;    // Enable global interrupts
    
    while(1) {
        // LED blinking
        LATDbits.LATD4 = 1;
        LATDbits.LATD5 = 1;
        LATDbits.LATD1 = 1;
        delay();
        
        LATDbits.LATD4 = 0;
        LATDbits.LATD5 = 0;
        LATDbits.LATD1 = 0;
        delay();
    }
}

void UART_Init(void) {
    // UART configuration
    TXSTAbits.TXEN = 1;    // Enable transmitter
    TXSTAbits.BRGH = 0;    // Low speed baud rate
    TXSTAbits.SYNC = 0;    // Asynchronous mode

    RCSTAbits.SPEN = 1;    // Enable serial port
    RCSTAbits.CREN = 1;    // Enable continuous receive

    BAUDCONbits.BRG16 = 1;
    SPBRGH = 0x00;
    SPBRG = 0x19;          // For 9600 baud rate at 4MHz

    PIE1bits.RCIE = 1;     // Enable UART receive interrupt
    IPR1bits.RCIP = 1;     // Set as high priority interrupt
}

void delay(void) {
    int i,j;
    for(i=0;i<10000;i++)
        for(j=0;j<2;j++);
}

void __interrupt(high_priority) ISR(void) {
    if (PIR1bits.RCIF) {
        char received_char = RCREG;
        
        // Echo the received character back
        while (!PIR1bits.TXIF);  // Wait for transmit buffer to be empty
        TXREG = received_char;   // Send the data
        
        PIR1bits.RCIF = 0;  // Clear the receive interrupt flag
    }
}
