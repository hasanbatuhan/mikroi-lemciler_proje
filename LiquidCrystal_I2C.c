#include "LiquidCrystal_I2C.h"

// I2C Donanýmýný Baþlatma
void i2c_init(void) {
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;
    UCB0CTL1 = UCSSEL_2 + UCSWRST;
    UCB0BR0 = 10;
    UCB0BR1 = 0;
    UCB0I2CSA = LCD_SLAVE_ADDRESS;
    P1SEL |= BIT6 + BIT7;
    P1SEL2|= BIT6 + BIT7;
    UCB0CTL1 &= ~UCSWRST;
}

// I2C Veri Gönderme
void i2c_write(unsigned char data) {
    while (UCB0STAT & UCBBUSY);
    UCB0CTL1 |= UCTR + UCTXSTT;
    while (!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = data;
    while (!(IFG2 & UCB0TXIFG));
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);
}

// LCD'ye Komut Gönderme
void lcd_send_cmd(unsigned char cmd) {
    unsigned char upper, lower;
    upper = (cmd & 0xF0) | 0x0C;
    lower = ((cmd << 4) & 0xF0) | 0x0C;
    i2c_write(upper);
    __delay_cycles(1000);
    i2c_write(upper & ~0x04);
    i2c_write(lower);
    __delay_cycles(1000);
    i2c_write(lower & ~0x04);
}

// LCD Baþlatma
void lcd_init(void) {
    i2c_init();
    __delay_cycles(50000);
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x01);
    __delay_cycles(20000);
}

// Tek Karakter Yazma
void lcd_write_char(unsigned char data) {
    unsigned char upper, lower;
    upper = (data & 0xF0) | 0x0D;
    lower = ((data << 4) & 0xF0) | 0x0D;
    i2c_write(upper);
    i2c_write(upper & ~0x04);
    i2c_write(lower);
    i2c_write(lower & ~0x04);
}

// Metin Yazma
void lcd_write_string(char *str) {
    while (*str) lcd_write_char(*str++);
}

// Ekraný tamamen temizler
void lcd_clear(void) {
    lcd_send_cmd(LCD_CLEARDISPLAY);
    __delay_cycles(20000); // Temizleme komutu zaman alýr
}

// Belirtilen satýr (0-1) ve sütuna (0-15) gider
void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char address;
    if (row == 0) address = 0x80 + col;      // 1. Satýr baþlangýcý 0x80
    else address = 0xC0 + col;               // 2. Satýr baþlangýcý 0xC0
    lcd_send_cmd(address);
}
