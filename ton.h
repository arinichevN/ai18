#ifndef TON_H
#define TON_H

struct Ton{
    unsigned long end;
    unsigned long interval;
};

extern void ton_setInterval(Ton *item, unsigned long interval_ms);

extern void ton_reset(Ton *item);

extern unsigned long ton_timePassed(Ton *item);

extern int ton(Ton *item);

#endif 
