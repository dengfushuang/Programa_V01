
#ifndef __NETDEV_H__
#define __NETDEV_H__

extern void netdev_init(uint8_t *mac);

extern unsigned int netdev_read(void);

extern void netdev_send(void);

#endif /* __NETDEV_H__ */
