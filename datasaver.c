#include <msp430.h>
#include "datasaver.h"
#include "packeting.h"
#include "LiquidCrystal_I2C.h" // LCD fonksiyonlarý için gerekli

// Dýþarýdan fonksiyon ve deðiþken tanýmlarý
extern char* itoa(int value, char* str, int base);
extern volatile unsigned char buffer[14];

// Global Deðiþkenler
volatile unsigned char hist[7][15];
volatile unsigned int hist_i = 0;
static volatile unsigned int counter1 = 0;
volatile unsigned int saniye = 0;
const unsigned int time_dk = 1;
volatile unsigned int dk_counter = 0;
volatile unsigned char saveflag = 0;
char char_hist_i[6]; // itoa için yeterli alan

void datasaver_init_timer(void) {
    TA0CTL = TASSEL_2 + ID_3 + MC_1; 
    TA0CCR0 = 50000; 
    TA0CCTL0 = CCIE; 
}

void datasaver_op(void) {
    if (saveflag == 1) {
        saveflag = 0;
        dk_counter = 0;

        unsigned int j;
        for (j = 0; j < 14; j++) {
            hist[hist_i][j] = buffer[j];
        }
        hist[hist_i][14] = '\0';

        lcd_set_cursor(1, 0);
        itoa((int)hist_i, char_hist_i, 10);
        lcd_write_string(char_hist_i);

        lcd_set_cursor(1, 3);
        lcd_write_string((char*)hist[hist_i]);

        hist_i++;
        if (hist_i >= 7) hist_i = 0;

        __delay_cycles(160000);
    }
}

// Kesme servis rutini - Yazým hatasý ve eksik parantez düzeltildi
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void)
{
    counter1++;
    if (counter1 >= 20) {
        saniye++;
        counter1 = 0;

        if (saniye >= 60) {
            dk_counter++;
            saniye = 0;
        }
    }

    if (dk_counter >= time_dk) {
        saveflag = 1;
    }
}
