#include  "config.h"
#include  "main.h"
#include  "uart2.h"
#include  "queue.h"


static uint8 UART2SendBuf[256];
static OS_EVENT *Uart2Sem;


uint8 Uart2WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
{
    uint16 temp;

    Mod = Mod;

    temp = QueueSize((void *)Buf);
    while (temp <= QueueNData((void *)Buf))         /* 等待数据队列不满 */
    {
        OSTimeDly(2*5);
    }
    return QueueWrite((void *)Buf, Data);           /* 数据重新入队 */
}


uint8_t UART2_Init(uint32 BPS)
{
    uint16 Fdiv;
	
    //使能以UART2控制器电源    
    LPC_SC->PCONP |= (1<<24);
    //Enable UART2 Pins.
    LPC_IOCON->P0_10 = 0x01;     //UART2_TXD
    LPC_IOCON->P0_11 = 0x01;     //UART2_RXD

    LPC_UART2->LCR = 0x80;                      // 允许访问分频因子寄存器     
    Fdiv = (PeripheralClock / 16) / BPS;        // 设置波特率 
    LPC_UART2->DLM = Fdiv / 256;
	  LPC_UART2->DLL = Fdiv % 256;
    LPC_UART2->LCR = 0x03;                      // 8N1模式，禁止访问分频因子寄存器 
                                                // 且设置为8,1,n 
	  LPC_UART2->IER = 0x05;                      // 允许接收和发送中断 
    LPC_UART2->FCR = 0x87;                      // 初始化FIFO 

    NVIC_EnableIRQ(UART2_IRQn);                 // Enable UART2 interrupt 
	  if (QueueCreate((void *)UART2SendBuf,
                     sizeof(UART2SendBuf),
                     NULL,
                     (uint8 (*)())Uart2WriteFull) 
                     == NOT_OK)
    {
        return FALSE;
    }
    Uart2Sem = OSSemCreate(0);
    if (Uart2Sem != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void UART2Putch(uint8 Data)
{
	  uint8 temp;
    OS_CPU_SR  cpu_sr;
	
    QueueWrite((void *)UART2SendBuf, Data);         /* 数据入队 */
    OS_ENTER_CRITICAL();
    if ((LPC_UART2->LSR & 0x00000020) != 0)
    {                                               /* UART2发送保持寄存器空 */
        QueueRead(&temp, UART2SendBuf);             /* 发送最初入队的数据 */
        LPC_UART2->THR = temp;
        LPC_UART2->IER = LPC_UART2->IER | 0x02;     /* 允许发送中断 */
    }
    OS_EXIT_CRITICAL();
}


void UART2Put_str(uint8 *Data, uint16 NByte)
{
    while (NByte-- > 0)
        UART2Putch(*Data++);
}


void UART2Write_Str(uint8_t *Data)
{
    while (*Data != '\0' )
        UART2Putch(*Data++);
}


uint8 UART2Getch(void)
{
    uint8 err;
	
    while ((LPC_UART2->LSR & 0x00000001) == 0)
    {                                           /* 没有收到数据 */
        LPC_UART2->IER = LPC_UART2->IER | 0x01; /* 允许接收中断 */
			  OSSemPend(Uart2Sem, 0, &err); 
    }
    err = LPC_UART2->RBR;                       /* 读取收到的数据 */
    return err;
}


uint8 UART2Get(void)
{
    uint8 err;
	
    if ((LPC_UART2->LSR & 0x00000001) == 1)
			  err = LPC_UART2->RBR;
		else err = 0;
    return err;
}


void UART2_IRQHandler (void) 
{
    uint8 IIR, temp, i;
	  OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();
    while(((IIR = LPC_UART2->IIR) & 0x01) == 0)         /* 有中断未处理完 */
    {
        switch (IIR & 0x0e)
        {
            case 0x02:                                  /* THRE中断    */
                for (i = 0; i < UART2_FIFO_LENGTH; i++) /* 向发送FIFO填充数据 */
                {
                    if (QueueRead(&temp, UART2SendBuf) == QUEUE_OK)
                    {
                        LPC_UART2->THR = temp;
                    }
                    else
                    {
                        LPC_UART2->IER = LPC_UART2->IER & (~0x02);        /* 队列空，则禁止发送中断 */
                    }
                }
                break;
            case 0x04:                                  /* 接收数据可用 */
                OSSemPost(Uart2Sem);                    /* 通知接收任务 */
                LPC_UART2->IER = LPC_UART2->IER & (~0x01);                /* 禁止接收及字符超时中断 */
                break;
            case 0x06:                                  /* 接收线状态   */
                temp = LPC_UART2->LSR;
                break;
            case 0x0c:                                  /* 字符超时指示 */
                OSSemPost(Uart2Sem);                    /* 通知接收任务 */
                LPC_UART2->IER = LPC_UART2->IER & (~0x01);                /* 禁止接收及字符超时中断 */
                break;
            default :
            	  temp = LPC_UART2->LSR;
                break;
        }
    }

    OS_EXIT_CRITICAL();
}







