#include "serial.h"
#include "io.h"
#include "stdio.h"

void init_serial() {
    outb(COM1 + 1, 0x00);
    //DLAB
    outb(COM1 + 3, 0x80);
    //115200
    outb(COM1 + 0, 0x01);
    outb(COM1 + 1, 0x00);
    //DLAB off, 8, N, 1
    outb(COM1 + 3, 0x03);
    // fifo
    outb(COM1 + 2, 0xC7); 
    // todo enable irq for recv
    /*outb(COM1 + 4, 0x0B);*/
}

void serial_send(char a) {
    while(inb(COM1 + 5) & 0x20 == 0) ;
    outb(COM1, a);
}

struct FILE serout_s = {.putc = serial_send};
struct FILE *serout = &serout_s;
