/*
 * File:        lcd.c
 * Author:      Abdullah, Vaibhav
 * Instructors: Bosi Yang, Ted Higman
 * Course:      EE 3951W (004) - Junior Design Project
 */


#include "xc.h"
#include "lcd.h"
#include "string.h"

void delay_ms(int ms){
    while(ms --> 0){
        asm("repeat #15998");
        asm("nop");
    }
}

void lcd_init(void){
    I2C2BRG = 157;              // 100kHz Baud rate 
    IFS3bits.MI2C2IF = 0;       // clear interrupt flag
    I2C2CONbits.I2CEN = 1;      // enable I2C2
    
    delay_ms(40);
    
    lcd_cmd(0b00111000);        // function set
    lcd_cmd(0b00111001);        // function set, advanced instruction mode
    lcd_cmd(0b00010100);        // internal OSC freq
    lcd_cmd(0b01110000);        // contrast C3-C0
    lcd_cmd(0b01010110);        // pwr/ICON/contrast control
    lcd_cmd(0b01101100);        // follower control
    
    delay_ms(200);
    
    lcd_cmd(0b00111000);        // function set
    lcd_cmd(0b00001100);        // display ON
    lcd_cmd(0b00000001);        // clear display
    
    delay_ms(2);
}

void lcd_cmd(char package){
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1;                // send START
    while(I2C2CONbits.SEN == 1);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;               // send slave address
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b00000000;               // send ctrl byte
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = package;                  // send data
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1;                // send stop
    while(I2C2CONbits.PEN == 1);         // wait for stop to complete
}

void lcd_setCursor(char x, char y){
   char location = ((0x40 * x) + y) | 0b10000000;
   lcd_cmd(location);
}

void lcd_printChar(char myChar){
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1;                // send START
    while(I2C2CONbits.SEN == 1);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;               // send slave address
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01000000;               // send ctrl byte with RS = 1
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = myChar;                  // send data
    while(IFS3bits.MI2C2IF == 0);        // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1;                // send stop
    while(I2C2CONbits.PEN == 1);         // wait for stop to complete
}

void lcd_printStr(const char s[]) {
    int strLength = strlen(s);
    int i = 0;
    
    I2C2CONbits.SEN = 1;                    // send START
    while(I2C2CONbits.SEN == 1);            // wait for ack from slave  
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;                   // send slave address
    while (IFS3bits.MI2C2IF == 0);          // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    
    for (i = 0; i < strLength - 1; i++){
        
        I2C2TRN = 0b11000000;               // send ctrl byte with RS = 1
        while (IFS3bits.MI2C2IF == 0);      // wait for ack from slave
        IFS3bits.MI2C2IF = 0;

        I2C2TRN = s[i];                     // send data
        while (IFS3bits.MI2C2IF == 0);      // wait for ack from slave
        IFS3bits.MI2C2IF = 0;
    }
    
    I2C2TRN = 0b01000000;               // send ctrl byte with RS = 1
    while (IFS3bits.MI2C2IF == 0);      // wait for ack from slave
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = s[strLength - 1]; 
    while (IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2CONbits.PEN = 1;                    // send STOP
    while(I2C2CONbits.PEN == 1);            // wait for STOP to complete
}
