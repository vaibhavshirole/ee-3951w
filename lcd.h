/*
 * File:        lcd.h
 * Author:      Abdullah, Vaibhav
 * Instructors: Bosi Yang, Ted Higman
 * Course:      EE 3951W (004) - Junior Design Project
 */

#ifndef ALHAI006_LAB6_LCD_V001_H
#define	ALHAI006_LAB6_LCD_V001_H

#ifdef	__cplusplus
extern "C" {
#endif
    void delay_ms(int ms);
    void lcd_init(void);
    void lcd_cmd(char package);
    void lcd_setCursor(char x, char y);
    void lcd_printChar(char myChar);
    void lcd_printStr(const char s[]);



#ifdef	__cplusplus
}
#endif

#endif	/* ALHAI006_LAB6_LCD_V001_H */

