#include "uart.h"

// uart.h dosyasında "extern" olarak söz verdiğimiz değişkenin gerçek tanımı burada yapılır
volatile int uart_busy = 0; 

// Sadece bu dosya içinde kullanılacak olan işaretçi
static char *uart_ptr; 

void uart_init(void) {
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;

    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2;            // SMCLK (8 MHz)

    // --- 8 MHz @ 9600 için Oversampling Ayarları ---
    // N = 8,000,000 / 9600 = 833.333
    // Oversampling (UCOS16=1) için: N/16 = 52.083
    UCA0BR0 = 52;                    // 52 (Tam kısım)
    UCA0BR1 = 0;
    // UCBRF = 1 (0.083 * 16 = 1.33 -> 1)
    // UCOS16 = 1 (Açık)
    UCA0MCTL = UCBRF_1 | UCOS16;

    UCA0CTL1 &= ~UCSWRST;
}

void uart_send_string(char *str) {
    while(uart_busy) ;
    uart_ptr = str;                 //
    
    if (*uart_ptr) {                // Eğer metin boş değilse
        uart_busy = 1;              //
        UCA0TXBUF = *uart_ptr++;    // İLK KARAKTERİ MANUEL GÖNDER (Bu ISR'ı tetikler)
        IE2 |= UCA0TXIE;            //
    }
}
// UART Transmit
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
    if (*uart_ptr) {                 // İşaretçinin gösterdiği yerde karakter var mı?
        UCA0TXBUF = *uart_ptr++;     // K
    } else {
        IE2 &= ~UCA0TXIE;            // Karakterler bitti, kesmeyi kapat
        uart_busy = 0;               // İşlemciyi "boşa" çıkar
    }
}
