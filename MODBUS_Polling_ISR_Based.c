#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// Configuration bits
#pragma config OSC = IRCIO67, FCMEN = OFF, IESO = OFF, PWRT = ON, BOREN = BOACTIVE
#pragma config BORV = 3, WDT = ON, WDTPS = 512, PBADEN = OFF, LPT1OSC = OFF
#pragma config MCLRE = OFF, STVREN = ON, LVP = OFF, BBSIZ = 1024, XINST = OFF
#pragma config DEBUG = OFF, CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
#pragma config CPB = OFF, CPD = OFF, WRT0 = OFF, WRT1 = OFF, WRT2 = OFF
#pragma config WRT3 = OFF, WRTC = OFF, WRTB = OFF, WRTD = OFF, EBTR0 = OFF
#pragma config EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTRB = OFF

#define _XTAL_FREQ 4000000

// Pin definitions
#define LED0 PORTDbits.RD4
#define LED1 PORTDbits.RD5
#define DRIVER PORTDbits.RD1

// Variables
uint8_t slave_address;
uint8_t function_code;
uint16_t start_address;
uint16_t quantity;
uint16_t crc;
uint8_t rx_buffer[11];
uint8_t tx_buffer[37];
bool coil1, coil2;
uint16_t holding_reg[16];
uint8_t i_global, j;
uint8_t temp_byte;
uint8_t data_length;
uint8_t rx_count;
bool NewFrame;
uint8_t TMR1Ticks;
uint8_t Length;

// Function prototypes
void initialize(void);
void process_modbus_request(void);
void read_coils(void);
void read_holding_registers(void);
void write_single_coil(void);
void write_single_register(void);
void write_multiple_registers(void);
void calculate_crc(void);
void send_modbus_response(void);
void receive_modbus_frame(void);

void __interrupt() ISR(void) {
    if (PIR1bits.RCIF) {

        TRISDbits.TRISD1 = 0;
        receive_modbus_frame();
        if (rx_buffer[0] == slave_address) {
            process_modbus_request();
        }
        PIR1bits.RCIF = 0;
    }
}


void main(void) {
    initialize();
    
    while (1) {
        for (int counter2 = 0; counter2 <= 5; counter2++) {
            for (int counter1 = 0; counter1 <= 30000; counter1++) {
                // Your main program code here
            }
        }
        LED0 = !LED0;
    }
}

void initialize(void) {
    // Clear memory
    for (int i = 0; i < 16; i++) {
        holding_reg[i] = 0;
    }
    
    // Oscillator configuration
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    
    // UART configuration
    TXSTAbits.BRGH = 1;
    SPBRG = 25; // For 9600 baud at 4MHz
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 1;
    
    // Enable UART receive interrupt
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    // Initialize variables
    slave_address = 1;
    coil1 = false;
    coil2 = false;
    rx_count = 0;
    holding_reg[0] = 65535;
    LED0 = 0;
    
    NewFrame = false;
    TMR1Ticks = 0;
    Length = 0;
}

void process_modbus_request(void) {
    function_code = rx_buffer[1];
    start_address = (rx_buffer[2] << 8) | rx_buffer[3];
    quantity = rx_buffer[5];
    
    switch (function_code) {
        case 1:
            read_coils();
            break;
        case 3:
            read_holding_registers();
            break;
        case 5:
            write_single_coil();
            break;
        case 6:
            write_single_register();
            break;
        case 16:
            write_multiple_registers();
            break;
    }
}

void read_coils(void) {
    tx_buffer[0] = slave_address;
    tx_buffer[1] = function_code;
    tx_buffer[2] = 1;
    tx_buffer[3] = coil1 | (coil2 << 1);
    data_length = 4;
    calculate_crc();
    tx_buffer[4] = crc & 0xFF;
    tx_buffer[5] = (crc >> 8) & 0xFF;
    send_modbus_response();
}

void read_holding_registers(void) {
    tx_buffer[0] = slave_address;
    tx_buffer[1] = function_code;
    tx_buffer[2] = 32;
    for (i_global = 0; i_global < 16; i_global++) {
        tx_buffer[3 + i_global * 2] = holding_reg[i_global] >> 8;
        tx_buffer[4 + i_global * 2] = holding_reg[i_global] & 0xFF;
    }
    data_length = 35;
    calculate_crc();
    tx_buffer[35] = crc & 0xFF;
    tx_buffer[36] = (crc >> 8) & 0xFF;
    send_modbus_response();
}

void write_single_coil(void) {
    if (start_address == 1) {
        coil1 = (rx_buffer[4] == 0xFF);
    } else if (start_address == 2) {
        coil2 = (rx_buffer[4] == 0xFF);
    }
    send_modbus_response();
}

void write_single_register(void) {
    if (start_address == 1) {
        holding_reg[1] = (rx_buffer[4] << 8) | rx_buffer[5];
    } else if (start_address == 2) {
        holding_reg[2] = (rx_buffer[4] << 8) | rx_buffer[5];
    }
    send_modbus_response();
}

void write_multiple_registers(void) {
    quantity = (rx_buffer[7] << 8) | rx_buffer[8];
    for (i_global = 0; i_global < quantity; i_global++) {
        if (rx_buffer[3] + i_global < 16) {
            holding_reg[rx_buffer[3] + i_global] = (rx_buffer[7 + i_global * 2] << 8) | rx_buffer[8 + i_global * 2];
        }
    }
    
    tx_buffer[0] = slave_address;
    tx_buffer[1] = function_code;
    tx_buffer[2] = rx_buffer[2];
    tx_buffer[3] = rx_buffer[3];
    tx_buffer[4] = rx_buffer[4];
    tx_buffer[5] = rx_buffer[5];
    data_length = 6;
    calculate_crc();
    tx_buffer[6] = crc & 0xFF;
    tx_buffer[7] = (crc >> 8) & 0xFF;
    
    send_modbus_response();
}

void calculate_crc(void) {
    crc = 0xFFFF;
    for (i_global = 0; i_global < data_length; i_global++) {
        crc ^= tx_buffer[i_global];
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
}

void send_modbus_response(void) {
    for (i_global = 0; i_global < 37; i_global++) {
        DRIVER = 1;
        while (!TXSTAbits.TRMT);
        TXREG = tx_buffer[i_global];
    }
    __delay_ms(10);
    TRISDbits.TRISD1 = 1;
    DRIVER = 0;
}

void receive_modbus_frame(void) {
    TRISDbits.TRISD1 = 0;
    for (i_global = 0; i_global < 3; i_global++) {
        DRIVER = 0;
        while (!PIR1bits.RCIF);
        rx_buffer[i_global] = RCREG;
        DRIVER = 1;
    }
    
    start_address = (rx_buffer[2] << 8) | rx_buffer[3];
    if (start_address == 3) {
        for (; i_global < 5; i_global++) {
            DRIVER = 0;
            while (!PIR1bits.RCIF);
            rx_buffer[i_global] = RCREG;
            DRIVER = 1;
        }
    }
    __delay_us(50);
}