#include  "config.h"
#include  "main.h"
#include  "uart1.h"
#include  "queue.h"


static uint8 UART1SendBuf[256];
static OS_EVENT *Uart1Sem;


extern uint32 SystemCoreClock;


uint8 Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
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


uint8_t UART1_Init(uint32 BPS)
{
    uint16 Fdiv;
	
    //使能以UART1控制器电源    
    LPC_SC->PCONP |= (1<<4);
    //Enable UART1 Pins.
    LPC_IOCON->P0_15 = 0x01;     //UART1_TXD
    LPC_IOCON->P0_16 = 0x01;     //UART1_RXD

    LPC_UART1->LCR = 0x80;                      // 允许访问分频因子寄存器     
    Fdiv = (PeripheralClock / 16) / BPS;        // 设置波特率 
    LPC_UART1->DLM = Fdiv / 256;
	  LPC_UART1->DLL = Fdiv % 256;
    LPC_UART1->LCR = 0x03;                      // 8N1模式，禁止访问分频因子寄存器 
                                                // 且设置为8,1,n 
	  LPC_UART1->IER = 0x05;                      // 允许接收和发送中断 
    LPC_UART1->FCR = 0x87;                      // 初始化FIFO 

    NVIC_EnableIRQ(UART1_IRQn);                 // Enable UART1 interrupt 
	  if (QueueCreate((void *)UART1SendBuf,
                     sizeof(UART1SendBuf),
                     NULL,
                     (uint8 (*)())Uart1WriteFull) 
                     == NOT_OK)
    {
        return FALSE;
    }
    Uart1Sem = OSSemCreate(0);
    if (Uart1Sem != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void UART1Putch(uint8 Data)
{
	  uint8 temp;
    OS_CPU_SR  cpu_sr;
	
    QueueWrite((void *)UART1SendBuf, Data);         /* 数据入队 */
    OS_ENTER_CRITICAL();
    if ((LPC_UART1->LSR & 0x00000020) != 0)
    {                                               /* UART0发送保持寄存器空 */
        QueueRead(&temp, UART1SendBuf);             /* 发送最初入队的数据 */
        LPC_UART1->THR = temp;
        LPC_UART1->IER = LPC_UART1->IER | 0x02;     /* 允许发送中断 */
    }
    OS_EXIT_CRITICAL();
}


void UART1Put_str(uint8 *Data, uint16 NByte)
{
    while (NByte-- > 0)
    {
        UART1Putch(*Data++);
    }
}


void UART1Write_Str(uint8_t *Data)
{
    while (*Data != '\0' )
    {
        UART1Putch(*Data++);
    }
}


uint8 UART1Getch(void)
{
    uint8 err;
	
    while ((LPC_UART1->LSR & 0x00000001) == 0)
    {                                           /* 没有收到数据 */
        LPC_UART1->IER = LPC_UART1->IER | 0x01; /* 允许接收中断 */
			  OSSemPend(Uart1Sem, 0, &err); 
    }
    err = LPC_UART1->RBR;                       /* 读取收到的数据 */
    return err;
}


uint8 UART1Get(void)
{
    uint8 err;
	
    if ((LPC_UART1->LSR & 0x00000001) == 0)
    {                                           /* 没有收到数据 */
        LPC_UART1->IER = LPC_UART1->IER | 0x01; /* 允许接收中断 */
			  OSSemPend(Uart1Sem, 1, &err); 
    }
    err = LPC_UART1->RBR;                       /* 读取收到的数据 */
    return err;
}


void UART1_IRQHandler (void) 
{
    uint8 IIR, temp, i;
	  OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();

    while(((IIR = LPC_UART1->IIR) & 0x01) == 0)
    {                                                   /* 有中断未处理完 */
        switch (IIR & 0x0e)
        {
            case 0x02:                                  /* THRE中断    */
                for (i = 0; i < UART1_FIFO_LENGTH; i++) /* 向发送FIFO填充数据 */
                {
                    if (QueueRead(&temp, UART1SendBuf) == QUEUE_OK)
                    {
                        LPC_UART1->THR = temp;
                    }
                    else
                    {
                        LPC_UART1->IER = LPC_UART1->IER & (~0x02);        /* 队列空，则禁止发送中断 */
                    }
                }
                break;
            case 0x04:                                  /* 接收数据可用 */
                OSSemPost(Uart1Sem);                    /* 通知接收任务 */
                LPC_UART1->IER = LPC_UART1->IER & (~0x01);                /* 禁止接收及字符超时中断 */
                break;
            case 0x06:                                  /* 接收线状态   */
                temp = LPC_UART1->LSR;
                break;
            case 0x0c:                                  /* 字符超时指示 */
                OSSemPost(Uart1Sem);                    /* 通知接收任务 */
                LPC_UART1->IER = LPC_UART1->IER & (~0x01);                /* 禁止接收及字符超时中断 */
                break;
            default :
            	  temp = LPC_UART1->LSR;
                break;
        }
    }
    //VICVectAddr = 0;            // 通知中断控制器中断结束
    OS_EXIT_CRITICAL();
}







