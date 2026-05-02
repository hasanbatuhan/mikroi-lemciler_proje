#include "packeting.h"
#include "uart.h"
volatile unsigned char buffer[14];
// ULP 14.1 ve 5.1 uyarılarını azaltmak için parametreleri unsigned yaptık
static char* local_itoa(unsigned int value, char* str) {
    char *rc, *ptr, *low;
    rc = ptr = str;

    // Değer 0 ise direkt yaz
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return rc;
    }

    low = ptr;
    do {
        *ptr++ = "0123456789"[value % 10];
        value /= 10;
    } while (value);

    *ptr-- = '\0';

    // Karakterleri ters çevir
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
    return rc;
}

void send_packet(int intensity, int node, int speed) {
    static unsigned int arama_sayaci = 0;

    unsigned int p = 0; // ULP 14.1: İndisler için unsigned int kullanımı önerilir

    if (arama_sayaci >= 10) {
        // 1. Işık (I)
        buffer[p++] = 'I';
        local_itoa((unsigned int)intensity, &buffer[p]);
        while (buffer[p] != '\0') p++;

        // 2. Node (N)
        buffer[p++] = 'N';
        local_itoa((unsigned int)node, &buffer[p]);
        while (buffer[p] != '\0') p++;

        // 3. Hız (S)
        buffer[p++] = 'S';
        local_itoa((unsigned int)speed, &buffer[p]);
        while (buffer[p] != '\0') p++;

        // 4. Sonlandırma
        buffer[p++] = '\n';
        buffer[p] = '\0';

        // UART ile gönder
        uart_send_string(buffer);

        arama_sayaci = 0;
    }

    arama_sayaci++;
}
