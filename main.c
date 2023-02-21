/*
 * File:        main.c
 * Author:      Abdullah, Vaibhav
 * Instructors: Bosi Yang, Ted Higman
 * Course:      EE 3951W (004) - Junior Design Project
 */

#include <p24FJ64GA002.h>

#include "xc.h"
#include "buffer.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"

// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

volatile uint32_t sum = 0; 
volatile uint32_t avg = 0;
volatile int counter = 0;

void adc_init(){
    /* Setting the analog pins */
    TRISAbits.TRISA1 = 1;               // AN1 input
    AD1PCFGbits.PCFG1 = 0;              // AN1 analog
    AD1CHSbits.CH0SA = 0x0001;          // Change AD1CHS to work on AN1
    AD1CHSbits.CH0SB = 0x0001;
    
    /* Set the Vref to come from an external source */
    //TRISAbits.TRISA0 = 1;
    AD1CON2bits.VCFG = 0b001;           // Use AVdd(3.3V) and AVss(0V) for max/min
    
    /* Finish setting up the ADC */
    AD1CON3bits.ADCS = 0b00000001;      // TAD = 125ns
    AD1CON1bits.SSRC = 0b111;           // Autosample
    AD1CON3bits.SAMC = 0b0001;          // 1 auto sample time bit
    AD1CON1bits.FORM = 0b00;            // unsigned ints
    AD1CON1bits.ASAM = 1;               // SAMP bit is auto set
    AD1CON2bits.SMPI = 0b0000;          // Interrupt for each sample
    AD1CON1bits.ADON = 1;               // turn on ADC

    /* Enable ADC1 Interrupt */
    _AD1IF = 0;                         // clear interrupt flag
    _AD1IE = 1;                         // enable flag
}

void setup(void){
    _RCDIV = 0;                         // 16MHz clock
    AD1PCFG = 0xFFFF;       
    lcd_init();
    initBuffer();
    adc_init();
    
    /* Set up Timer 1, which determines how fast the LCD refreshes */
    TMR1 = 0;
    T1CON = 0;
    T1CONbits.TCKPS = 0b10;             // prescaler 1:64
    PR1 = 62500-1;                      // for 100ms
    T1CONbits.TON = 1;                  // enable timer 1
}

/* 
 * ADC1 Interrupt, creates the 14-bit reading
 */
void __attribute__((interrupt, auto_psv)) _ADC1Interrupt(){
    _AD1IF = 0;                         //reset interrupt flag
    
    if(counter == 256){
        avg = sum >> 4;
        sum = 0;
        counter = 0;
    }
    else {
        sum = sum + ADC1BUF0;           //accumulate samples
        counter++;
    }
}

 
int main(void) {
    setup();
    
    while(1){
        unsigned int avgTest = avg;
        char lcdIntStr[32];
        lcd_setCursor(0, 0);
        lcd_printStr("0.");
        int i;
        
        /* Top row of LCD */
        for (i = 0; i < 6; i++){
            lcdIntStr[i] = ((avgTest >> (13-i)) & 0b1) ? '1':'0';
        }
        lcdIntStr[6] = '\0';
        lcd_setCursor(0, 2);
        lcd_printStr(lcdIntStr);

        /* Bottom row of LCD */
        for(i = 0; i < 8; i++){
            lcdIntStr[i] = ((avgTest >> (7-i)) & 0b1) ? '1':'0';
        }
        lcdIntStr[8] = '\0';
        lcd_setCursor(1, 0);
        lcd_printStr(lcdIntStr);
        
        /* Wait 0.25 sec */
        while(TMR1 <= 24999);
        TMR1 = 0;
    }
        
    return 0;
}