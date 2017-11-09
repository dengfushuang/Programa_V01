#ifndef __CAT1025_H
#define __CAT1025_H

void I2C_ramwrite(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t n);
void I2C_ramread(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t n);

#endif
