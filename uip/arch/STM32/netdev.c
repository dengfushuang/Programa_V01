#include <stdint.h>
#include "uip-conf.h"
#include "uip.h"
#include "lpc17xx_emac.h"

extern void _delay_ms(uint16_t ms);

void netdev_init(uint8_t *mac)
{  
   // dm9000x_inital(mac);
}

unsigned int netdev_read(void)
{	

	//return dm9000x_receivepacket(uip_buf, UIP_BUFSIZE);
}


void netdev_send(void)
{
   // dm9000x_sendpacket(uip_buf, uip_len);

}

