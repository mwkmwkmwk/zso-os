#ifndef PIC_H
#define PIC_H

void init_pic(void);
void irq_enable(int idx);
void irq_disable(int idx);

#endif
