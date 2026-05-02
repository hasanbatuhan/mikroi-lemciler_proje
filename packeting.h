// packeting.h
#ifndef PACKETING_H_
#define PACKETING_H_

// Buradaki bildirim ile .c dosyasýndaki tanýmlama birebir ayný olmalý
extern volatile unsigned char buffer[14];

void send_packet(int light, int speed, int id);

#endif
