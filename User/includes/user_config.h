#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
	
/*********************************************************************************************************
  �忨����
*********************************************************************************************************/
#define TYPE_OPM
#define FSW    0
#define OTDR   1
#define WDM    2
#define OPM    3
#define OS     4

#define CHANNEL_NUM 16   //���ͨ����Ϊ16·
	
	

#ifdef  TYPE_OPM
#define AMP_CHANNEL  8   //����Ŵ�������ͨ���� 8ͨ��
#define ADC_USE_NUM  2   //ADCʹ��ͨ����
#endif

	
	
	
	
	
	
	
	
	
	
	
	

#ifdef __cplusplus
}
#endif
#endif
