#include <msp430.h>

#include "LiquidCrystal_I2C.h"
// Global Deðiþkenler (Diðer fonksiyonlarýn eriþebilmesi için)
unsigned int ldr_degeri = 0;
char* itoa(int value, char* str, int base) ;
unsigned long adc_toplam = 0;
unsigned int ornek_sayaci = 0;
unsigned int filtreli_ldr = 0;

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    if (CALBC1_1MHZ == 0xFF) {
            while(1); // Eðer kalibrasyon verisi silinmiþse burada dur (Güvenlik önlemi)
        }
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = CAL_DCO_1MHZ;

char sayýsaldeger[6] ;


    // --- ADC10 Yapýlandýrmasý ---
    ADC10CTL1 = INCH_0; // Þu an sadece A0 (LDR) seçili
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10AE0 |= BIT0;

    // --- Timer_A1 Yapýlandýrmasý (PWM - P2.1) ---
    P2DIR |= BIT1; P2SEL |= BIT1;
    TA1CCR0 = 1023;
    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 + MC_1;

    // lcd


    lcd_init();

    lcd_write_string("I:");


    __enable_interrupt();

    while(1) {
        ADC10CTL0 |= ENC + ADC10SC;
        __bis_SR_register(LPM0_bits + GIE);
        lcd_set_cursor(0,2);
         itoa(filtreli_ldr, sayýsaldeger, 10);
         lcd_write_string(sayýsaldeger);
         lcd_write_string("    ");

        __delay_cycles(25000);
    }
}

// --- ADC10 Kesme Servis Rutini ---

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    adc_toplam += ADC10MEM; // Her okumayý biriktir
    ornek_sayaci++;

    if (ornek_sayaci >= 128) { // 64 örnek toplandýðýnda
        ldr_degeri = adc_toplam /128 ; // 64'e böl (bit kaydýrma)


        adc_toplam = 0;                 // Toplamý sýfýrla
        ornek_sayaci = 0;               // Sayacý sýfýrla
        filtreli_ldr = ldr_degeri;
        // Sadece ortalama hazýr olduðunda iþlemciyi uyandýr
        __bic_SR_register_on_exit(LPM0_bits);
        TA1CCR1 = 900 - filtreli_ldr;
      }else {
          ADC10CTL0 |= ENC +ADC10SC ;
      }

}
// Basit ve hafýza dostu itoa implementasyonu
char* itoa(int value, char* str, int base) {
    char *rc;
    char *ptr;
    char *low;
    // Sadece 10'luk taban için hýzlý çözüm (LDR için yeterli)
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
    // Stringi ters çevirme
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
    return rc;
}
