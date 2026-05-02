#ifndef UART_H_
#define UART_H_

#include <msp430.h>

// --- Paylaþýlan Deðiþkenler ---
// "extern" anahtar kelimesi, bu deðiþkenin uart.c içinde tanýmlandýðýný 
// ve diðer dosyalar tarafýndan da kullanýlabileceðini belirtir.
extern volatile int uart_busy;

// --- Fonksiyon Prototipleri ---
// Fonksiyonlarýn sadece "imzalarýný" buraya ekliyoruz.
void uart_init(void);
void uart_send_string(char *str);

#endif /* UART_H_ */
