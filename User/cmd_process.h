#ifndef __CMD_PROCESS_H_
#define __CMD_PROCESS_H_

#include "main.h"

uint16 change_ascii_date(char *x, uint8 len ,uint8 *err);
void eprom_set(void);
extern uint16 cmd_process( char *sprintf_buf );

#endif //__CMD_PROCESS_H_

