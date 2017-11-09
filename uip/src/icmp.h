#ifndef __ICMP_H
#define __ICMP_H


extern u8_t    ICMP_buf[64];
extern u16_t   icmp_seqno[4];
extern u16_t   icmp_recseqno[4];


extern  u8_t ICMP_set(u8_t num);

#endif /* end __TARGET_H */
