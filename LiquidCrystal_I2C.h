#ifndef LIQUIDCRYSTAL_I2C_H_
#define LIQUIDCRYSTAL_I2C_H_

#include <msp430.h>

// I2C Adresi - Eðer ekran gelmezse 0x3F olarak deðiþtirip dene
#define LCD_SLAVE_ADDRESS 0x27

// LCD Komutlarý
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Display Ayarlarý
#define LCD_DISPLAYON 0x04
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF 0x00

// Fonksiyon Prototipleri
void i2c_init(void);
void i2c_write(unsigned char data);
void lcd_init(void);
void lcd_send_cmd(unsigned char cmd);
void lcd_write_char(unsigned char data);
void lcd_write_string(char *str);
void lcd_clear(void);
void lcd_set_cursor(unsigned char row, unsigned char col);
#endif /* LIQUIDCRYSTAL_I2C_H_ */
