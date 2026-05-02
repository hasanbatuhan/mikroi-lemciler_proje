#include <msp430.h>
#include <stdio.h>
#include "uart.h"             // Yeni oluşturduğumuz UART başlığı
#include "LiquidCrystal_I2C.h"
#include "packeting.h"
#include  "datasaver.h"

// --- Global Değişkenler ---
volatile unsigned int ldr_degeri = 0;
volatile unsigned long adc_toplam = 0;
volatile unsigned int ornek_sayaci = 0;
volatile unsigned int filtreli_ldr = 0;
 char ldr_buffer[13];
 volatile unsigned int global_time = 0; // Her şeyin referans alacağı zaman değişkeni
 unsigned int second_counter = 0;        // 1 saniyeyi yakalamak için yardımcı sayaç

// Fonksiyon Prototipi
char* itoa(int value, char* str, int base);

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;

    // --- ADC10 Yapılandırması ---
    ADC10CTL1 = INCH_0; 
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10AE0 |= BIT0;

    // --- Timer_A1 (PWM) ---
    P2DIR |= BIT1; P2SEL |= BIT1;
    TA1CCR0 = 1023;
    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 + MC_1;

    // --- Donanım Başlatma ---
    lcd_init();
    uart_init(); // uart.c içindeki fonksiyonu çağırır
    datasaver_init_timer();



    lcd_write_string("I:");
    
    __enable_interrupt();




    while(1) {
        // ADC Örneklemesini Başlat
        ADC10CTL0 |= ENC + ADC10SC;


        // ADC ISR bizi uyandırana kadar bekle
        __bis_SR_register(LPM0_bits + GIE);
        datasaver_op() ;
        // --- LCD GÜNCELLEME ---
        lcd_set_cursor(0,2);
        itoa(filtreli_ldr,ldr_buffer, 10);
        lcd_write_string(ldr_buffer);
        lcd_write_string("    ");

        // --- UART PAKETLEME VE GÖNDERME ---
        // uart_busy kontrolü uart_send_string içinde yapılıyor


         send_packet((int)filtreli_ldr, 100, 34);


         __delay_cycles(160000); // 200ms bekleme

    }
}

// --- ADC10 Kesme Servis Rutini ---
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    adc_toplam += ADC10MEM;
    ornek_sayaci++;

    if (ornek_sayaci >= 128) {
        ldr_degeri = adc_toplam / 128;
        adc_toplam = 0;
        ornek_sayaci = 0;
        filtreli_ldr = ldr_degeri;
        
        TA1CCR1 = 900 - filtreli_ldr;
        
        __bic_SR_register_on_exit(LPM0_bits); // Ana döngüyü uyandır
    } else {
        ADC10CTL0 |= ENC + ADC10SC; // Bir sonraki örnek
    }
}

// --- Yardımcı Fonksiyon: itoa ---
char* itoa(int value, char* str, int base) {
    char *rc, *ptr, *low;
    if (base != 10) return str;
    rc = ptr = str;
    if (value < 0) {
        *ptr++ = '-';
        value = -value;
    }
    low = ptr;
    do {
        *ptr++ = "0123456789"[value % 10];
        value /= 10;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
    return rc;
}
