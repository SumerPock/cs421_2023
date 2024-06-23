/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "includes.h"
#include "bsp.h"

/*	д��ʱ����ò���̫�ѣ������������д���ˣ�MDK��Ӧ�����ִ��͹��̵��Ƿ���鷳	*/

/*	2023 5 12 
	����Ŀʹ���� DS18B20 ,�����Ҽ����˲���ϵͳ������ʹ����DWT��������ʹ������û����ܷ��� �� 
	�����Ե�ʱ������һ��������DEBUGģʽ�³�������������У������������ӷ������������
	�������µ��忨�����˿����������ո��ٵ�����ΪDS18B20�ĵ�����ɣ�����ʹ��DWT��ʱ�����
	����������̳����������Ҳ������������ո��ٵ�����Ϊrest and runѡ�ѡ������*/
	
/*	����ܷ�����ʵ�ܹǸа� */
/************************************** ���� *******************************************/

/*��������*/
void AppTaskStart(void *argument);
const osThreadAttr_t ThreadStart_Attr = 	/* ���������������� */
{
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh7,
	.stack_size = 2048,
};
osThreadId_t ThreadIdStart = NULL;				/*����������*/


/*�����������ȼ��Ӹߵ���*/


/*��ȡ DS18B20 ��LM75B ���� ���ȼ���� �� �������¿س���*/
void AppTaskDS18B20(void *argument);
const osThreadAttr_t ThreadDS18B20_Attr =
{
	.name 	    = "ThreadDS18B20",
	.attr_bits  =  osThreadDetached, 
	.priority   =  osPriorityHigh7,
	.stack_size =  4096,
};
osThreadId_t ThreadIdTaskDS18B20 = NULL;



/*����RK3588��������*/
void AppTaskRK3588_Receive(void *argument);
const osThreadAttr_t ThreadRK3588_Receive_Attr =
{
	.name = "ThreadRK3588_Receive",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskRK3588_Receive = NULL;



/*RK3588������������*/
void AppTaskRK3588_Send(void *argument);
const osThreadAttr_t ThreadRK3588_Send_Attr =
{
	.name = "ThreadRK3588_Send",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskRK3588_Send = NULL;



/*RK3588�ػ�����*/
void AppTaskRK3588_Closure(void *argument);
const osThreadAttr_t ThreadRK3588_Closure_Attr =
{
	.name = "ThreadRK3588_Closure",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskRK3588_Closure = NULL;


/*RK3588��������*/
void AppTaskRK3588_Reboot(void *argument);
const osThreadAttr_t ThreadRK3588_Reboot_Attr =
{
	.name = "ThreadRK3588_Reboot",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskRK3588_Reboot = NULL;



/*��λ��ͨѶ����*/
//void AppTaskPCCommun(void *argument);
//const osThreadAttr_t ThreadPCCommun_Attr =
//{
//	.name = "ThreadPCCommun",
//	.attr_bits = osThreadDetached, 
//	.priority = osPriorityHigh6,
//	.stack_size = 4096,
//};
//osThreadId_t ThreadIdTaskPCCommun = NULL;


/*GPS���ݽ�������*/
void AppTaskGPS(void *argument);
const osThreadAttr_t ThreadGPS_Attr =
{
	.name = "ThreadGPS",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskGPS = NULL;


/*2023.5.8���䣬��ΪGPS�ϵĵ��̫С���ϵ��ϵ�����1�����Ҿͻ�ϵ� �� �ϵ������������Ϣ��ʧ����ɽ���GPS��������������ÿ���ϵ��
  ��Ҫ��GPS�·�һ��ر�ָ�������Ҫ����һ���ϵ����GPSЭ����̣߳�����������Ϻ�͹رմ��߳�*/
/*GPS��������*/
void AppTaskGPSSetUp(void *argument);
const osThreadAttr_t ThreadGPSSetUp_Attr =
{
	.name = "ThreadGPSSetUp",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskGPSSetUp = NULL;


void AppTaskOutherThred(void *argument);
const osThreadAttr_t ThreadOutherThred_Attr =
{
	.name = "ThreadOutherThred",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskOutherThred = NULL;


#if Initialize_Network == 1  
/*LWIP������ѯ��������*/
void AppTaskLwIPPro(void *argument);	
const osThreadAttr_t ThreadLwIPPro_Attr = 		
{
	.name = "osRtxLwIPProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskLwIPPro = NULL;	


/*���߲��״̬����*/
void AppTaskEthCheck(void *argument);
const osThreadAttr_t ThreadEthCheck_Attr =
{
	.name = "osRtxEthCheckThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskEthCheck = NULL;	
#endif	



/************************************** ��Ϣ���� *******************************************/

/*���ڽ�����λ�����õ�ʱ������*/
//osMessageQueueId_t msgQueue_PcSetTime_Data; 	
//const osMessageQueueAttr_t PcSetTime_Data_Attr =
//{
//	.name = "PcSetTime_Data",   					
//};


/*���ڽ�����λ�����õ���������*/
//osMessageQueueId_t msgQueue_PcSetNetwork_Data; 	
//const osMessageQueueAttr_t PcSetNetwork_Data_Attr =
//{
//	.name = "PcSetNetwork_Data",   					
//};

/*���ڷ�����λ�����õ�ʱ������*/
//osMessageQueueId_t msgQueue_PcGetTime_Data; 	
//const osMessageQueueAttr_t PcGetTime_Data_Attr =
//{
//	.name = "PcGetTime_Data",   					
//};


/*���ڷ�����λ�����õ���������*/
//osMessageQueueId_t msgQueue_PcGetNetwork_Data; 	
//const osMessageQueueAttr_t PcGetNetwork_Data_Attr =
//{
//	.name = "PcGetNetwork_Data",   					
//};


/*���ڽ��յ�һ��DS18B29�¶�����*/
osMessageQueueId_t msgQueue_Ds18b20_1_Data; 	
const osMessageQueueAttr_t Ds18b20_1_Data_Attr =
{
	.name = "Ds18b20_1_Data",   					
};

/*���ڽ��յڶ���DS18B29�¶�����*/
osMessageQueueId_t msgQueue_Ds18b20_2_Data; 	
const osMessageQueueAttr_t Ds18b20_2_Data_Attr =
{
	.name = "Ds18b20_2_Data",   					
};

/*���ڽ��հ���LM75B�¶�����*/
osMessageQueueId_t msgQueue_LM75B_Data; 	
const osMessageQueueAttr_t LM75B_Data_Attr =
{
	.name = "LM75B_Data",   					
};


/*���ڽ���GPS����*/
osMessageQueueId_t msgQueue_GPS_Data; 	
const osMessageQueueAttr_t GPS_Data_Attr =
{
	.name = "GPS_Data",   					
};


/*���ڼ�¼RK3588����ʱ��*/
osMessageQueueId_t msgQueue_RK3588RebootTime_Data; 	
const osMessageQueueAttr_t RK3588RebootTime_Data_Attr =
{
	.name = "RK3588RebootTime_Data",   					
};


/*����RK3588����ʱ��*/
osMessageQueueId_t msgQueue_SetRk3588RebootTime_Data; 	
const osMessageQueueAttr_t SetRk3588RebootTime_Data_Attr =
{
	.name = "SetRk3588RebootTime_Data",   					
};


/*���ڴ���Ƶ��*/
osMessageQueueId_t msgQueue_GetFanFrequency_Data; 	
const osMessageQueueAttr_t GetFanFrequency_Data_Attr =
{
	.name = "GetFanFrequency_Data",   					
};

/*���ڴ���ռ�ձ�*/
//duty_cycle
osMessageQueueId_t msgQueue_GetFanDuty_cycle_Data; 	
const osMessageQueueAttr_t GetFanDuty_cycle_Data_Attr =
{
	.name = "GetFanDuty_cycle_Data",   					
};


/*���������ʵ�ʷ���ת��*/
osMessageQueueId_t msgQueue_CALFAN_Rising_Data; 	
const osMessageQueueAttr_t CALFAN_Rising_Data_Attr =
{
	.name = "CALFAN_Rising_Data",   					
};


/************************************** �¼���־λ **********************************************/

/* ���߲���¼���־������ */
const osEventFlagsAttr_t event_cable_Attr = 
{
	.name = "event_cable",   	/* �¼���־������� */
								/* ����Ĭ�� */
};
/*���߲���¼���־��ID */
osEventFlagsId_t event_cable_ID = NULL;


/*USART1 ���ڽ������ݱ�־λ ���ڽ�����λ������*/
const osEventFlagsAttr_t event_usart1_Attr = 
{
	.name = "event_usart1",   	/* �¼���־������� */
								/* ����Ĭ�� */
};
osEventFlagsId_t event_usart1_ID = NULL;


/*UART3 ���ڽ������ݱ�־λ ���ڽ���GPS����*/
const osEventFlagsAttr_t event_usart3_Attr = 
{
	.name = "event_usart3",   	/* �¼���־������� */
								/* ����Ĭ�� */
};
osEventFlagsId_t event_usart3_ID = NULL;



/*UART5 ���ڽ������ݱ�־λ ���ڽ���RK3588����*/
const osEventFlagsAttr_t event_uart5_Attr = 
{
	.name = "event_uart5",   	/* �¼���־������� */
								/* ����Ĭ�� */
};
osEventFlagsId_t event_uart5_ID = NULL;


/*�����ϵ�����ID*/
const osEventFlagsAttr_t event_initiativePowOff_Attr = 
{
	.name = "event_initiativePowOff",   /* �¼���־������� */
										/* ����Ĭ�� */
};
osEventFlagsId_t event_initiativePowOff_ID = NULL;


/*��Դģʽ*/
const osEventFlagsAttr_t event_PowMode_Attr = 
{
	.name = "event_PowMode",   			/* �¼���־������� */
										/* ����Ĭ�� */
};
osEventFlagsId_t event_PowMode_ID = NULL;

/*RK3588�¼�*/
const osEventFlagsAttr_t event_RK3588Sinal_Attr = 
{
	.name = "event_RK3588Sinal",   			/* �¼���־������� */
											/* ����Ĭ�� */
};
osEventFlagsId_t event_RK3588Sinal_ID = NULL;


/*RK3588��������¼�*/
const osEventFlagsAttr_t event_RK3588Ready_Attr = 
{
	.name = "event_RK3588Ready",   			/* �¼���־������� */
											/* ����Ĭ�� */
};
osEventFlagsId_t event_RK3588Ready_ID = NULL;


/*RK3588 �ػ��¼�*/
const osEventFlagsAttr_t event_RK3588ShutDown_Attr = 
{
	.name = "event_RK3588ShutDown",   			/* �¼���־������� */
											/* ����Ĭ�� */
};
osEventFlagsId_t event_RK3588ShutDown_ID = NULL;


/*RK3588 �����¼�*/
const osEventFlagsAttr_t event_RK3588StartUp_Attr = 
{
	.name = "event_RK3588StartUp",   			/* �¼���־������� */
												/* ����Ĭ�� */
};
osEventFlagsId_t event_RK3588StartUp_ID = NULL;


/* ���ڱ����������ʱ */
const osEventFlagsAttr_t event_RK3588RebootCountdown_Attr = 
{
	.name = "event_RK3588RebootCountdown",   	/* �¼���־������� */
												/* ����Ĭ�� */
};
osEventFlagsId_t event_RK3588RebootCountdown_ID = NULL;



/************************************** �����ʱ�� *******************************************/

/* ���涨ʱ��ID */
osTimerId_t  timerID_rk3588reboot = NULL; 

 /* �����ʱ�� */
void timer_rk3588reboot_App(void *argument); 

/*�����ʱ�����ڱ������ʱ��*/
const osTimerAttr_t timer_rk3588reboot_Attr = 
{
	.name = "rk3588reboottimer",				/*��ʱ������*/
};


TIM_HandleTypeDef htim4;

TIM_HandleTypeDef htim2;

TIM_HandleTypeDef htim3;

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init(); //�趨�ж�������4
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* 
	   Event Recorder��
	   - �����ڴ���ִ��ʱ�������MDK5.25�������ϰ汾��֧�֣�IAR��֧�֡�
	   - Ĭ�ϲ����������Ҫʹ�ܴ�ѡ���ؿ�V5�������û��ֲ��8��
	*/
#if Enable_EventRecorder == 1  
	/* ��ʼ��EventRecorder������ */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif	

	/* �ں˿���ǰ�ر�HAL��ʱ���׼ */
	HAL_SuspendTick();
	/* �ں˳�ʼ�� */
	osKernelInitialize();                                  
	/* ������������ */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  
	/* ���������� */
	osKernelStart();
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ��������Ҳ����������ȼ�����
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 100; /* �ӳ����� */
	uint32_t tick;	
	/* ��ʼ������ */
	HAL_ResumeTick();
	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
	Bsp_initialize(); /*��ʼ���󲿷��豸*/
	
	bsp_GpioOn(INTERNALFAN);	//����С����
	bsp_GpioOn(EXTERNALFAN);	//���÷���
	bsp_GpioOn(BATTERY);		//��ع���
	bsp_GpioOn(RK3588POWER);	//RK3588��������
	
	bsp_GpioOff(CRYOGEN);		//�뵼������
	bsp_GpioOff(HEATING);		//����Ƭ
	bsp_GpioOff(5);				//GPIO5δʹ�� PA8
	
	bsp_LedOff(ONBOARD_LED_1);	//�ڲ�LED
	bsp_LedOff(ONBOARD_LED_2);
	
//	bsp_LedOff(EXTERNAL_LED_1); //�ⲿLED
//	bsp_LedOff(EXTERNAL_LED_2);
//	bsp_LedOff(EXTERNAL_LED_3);
	
	/* 5����λ �����
	�����ź�	[0] = 58hz , [1] = 68 �� [2]= 77 �� [3]= 111 �� [4]=138	*/
	unsigned int pwmButy_Cycle[5] = {3700  , 4400 , 5100 , 5800 , 6500}; //150000000
	//9900--136 5500--83
	bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_20); /*500KHz��ռ�ձ�50% */
	//bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, 5000000, 50000000); /*500KHz��ռ�ձ�50% */
	/* 6����λ RK��С����
		�����ź� 	[0] = 53 , [1] = 89 , [2] = 122 , [3] = 148 , [4] = 183 , [5] =  215			*/	
//	unsigned int pwmButy_Cycle1_Sm[6] = {2000  , 3200 , 4400 , 5600  , 6800 , 8000};
	bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_16);
//	
//	/* 6����λ ARM��С����
//		�����ź� 	[0] = 53 , [1] = 89 , [2] = 122 , [3] = 148 , [4] = 183 , [5] =  215			*/		
	bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, INTERNAL_ARM_FAN_FREQ, smpercent_16);

	MX_TIM4_Init();
	HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
	
	MX_TIM2_Init();
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
	
	MX_TIM3_Init();
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	
#if Initialize_Network == 1  
	/* ��ʼ��DP83848������ */
	g_ucEthLinkStatus = 0;
	/* ��ʼ������ */
	if(net_init() != 0)
	{
		printf("the lwip config is flad \r\n");//LWIPЭ���ʼ��ʧ��
	}
#endif	
	
	/********************************** ϵͳ��־λ���� *****************************************/
	
	/*�����¼���־��*/
	event_cable_ID = osEventFlagsNew(&event_cable_Attr);
	
	/*USART1 ���ڽ����¼���־��*/
	event_usart1_ID = osEventFlagsNew(&event_usart1_Attr);
   
	/*UART3 ���ڽ����¼���־��*/
	event_usart3_ID = osEventFlagsNew(&event_usart3_Attr);	
	
	/*USART5 ���ڽ����¼���־��*/
	event_uart5_ID = osEventFlagsNew(&event_uart5_Attr);		

	/*�����ϵ�����*/
	event_initiativePowOff_ID = osEventFlagsNew(&event_initiativePowOff_Attr);	
	
	/*���ģʽѡ��*/
	event_PowMode_ID = osEventFlagsNew(&event_PowMode_Attr);	

	/*RK3588�¼�*/
	event_RK3588Sinal_ID = osEventFlagsNew(&event_RK3588Sinal_Attr);

	/*RK3588׼�����*/
	event_RK3588Ready_ID = osEventFlagsNew(&event_RK3588Ready_Attr);
	
	/*RK3588 �ػ��¼�*/
	event_RK3588ShutDown_ID = osEventFlagsNew(&event_RK3588ShutDown_Attr);
	
	/*RK3588 �����¼�*/
	event_RK3588StartUp_ID = osEventFlagsNew(&event_RK3588StartUp_Attr);
	
	/* ���RK3588��������ʱ */
	event_RK3588RebootCountdown_ID = osEventFlagsNew(&event_RK3588RebootCountdown_Attr);
		
	/********************************** ���񴴽� *****************************************/
	
#if Initialize_Network == 1  	
	/* ����LWIP������ѯ���� */
	ThreadIdTaskLwIPPro = osThreadNew(AppTaskLwIPPro, NULL, &ThreadLwIPPro_Attr);
	osDelay(200);
	
	/*	���߲��״̬���	*/								
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr); 
#endif


	/*	��һЩ���Ҷ�ʱ����̫��ע�Ļ�	
			Ŀǰ������� ��ȡ��FAN�ķ���ת����Ϣ���������ݷ�����Ϣ������ȥ��
			��˸����2��LED , �������������ɱ������������
	*/
	ThreadIdTaskOutherThred 	=	osThreadNew(AppTaskOutherThred, NULL, &ThreadOutherThred_Attr);


	/*	�ڲ�DS18B20�¶ȶ�ȡ�������˸���ⲿDS18B20 
			LM75B�¶ȶ�ȡ
			ADC��أ�ADC24v�ɼ�����	
			�Զ��¿�
	*/
	ThreadIdTaskDS18B20 		=	osThreadNew(AppTaskDS18B20, NULL, &ThreadDS18B20_Attr);


	/* RK3588������������
	*/
	ThreadIdTaskRK3588_Receive 	=	osThreadNew(AppTaskRK3588_Receive, NULL, &ThreadRK3588_Receive_Attr);
	
	
	/* ��Դ����RK3588���������������е�Դ��������Ϣ�ϱ��޸ĳ���RS422 
	*/
	ThreadIdTaskRK3588_Send		=	osThreadNew(AppTaskRK3588_Send, NULL, &ThreadRK3588_Send_Attr);	


	/* ��λ������ 
		 ��Ҫ�Ǳ�һЩ�޸�ip�ģ�ʱ�������
	*/
	//ThreadIdTaskPCCommun 		=	osThreadNew(AppTaskPCCommun, NULL, &ThreadPCCommun_Attr);
	
	
	/*GPS��������
		�ϵ����GPS���ò������ã�������Ϻ�����GPS������������GPS���������ﻹ��һ�������������ɱ����
	*/
	ThreadIdTaskGPSSetUp 		=	osThreadNew(AppTaskGPSSetUp, NULL, &ThreadGPSSetUp_Attr);	
	
	/********************************** ��Ϣ���д��� *****************************************/

	msgQueue_Ds18b20_1_Data 	   = osMessageQueueNew(MQ_SIZE , sizeof(DS18B20_DATA) ,&Ds18b20_1_Data_Attr);
	
	msgQueue_Ds18b20_2_Data 	   = osMessageQueueNew(MQ_SIZE , sizeof(DS18B20_DATA) ,&Ds18b20_2_Data_Attr);

	msgQueue_LM75B_Data     	   = osMessageQueueNew(MQ_SIZE , sizeof(LM75B_DATA) ,&LM75B_Data_Attr);

	/*����Ƶ���ź�*/
	msgQueue_GetFanFrequency_Data  = osMessageQueueNew(MQ_SIZE , sizeof(GetFANOrg_Frequency) ,&GetFanFrequency_Data_Attr);
	
	/*����ռ�ձ��ź�*/
	msgQueue_GetFanDuty_cycle_Data = osMessageQueueNew(MQ_SIZE , sizeof(GetFANOrg_duty_cycle) ,&GetFanDuty_cycle_Data_Attr);
	
	/*���������ķ���ת���ź�*/
	msgQueue_CALFAN_Rising_Data    = osMessageQueueNew(MQ_SIZE , sizeof(CALFAN_Rising) ,&CALFAN_Rising_Data_Attr);
	
	msgQueue_RK3588RebootTime_Data = osMessageQueueNew(MQ_SIZE , sizeof(REBOTTIME) ,&RK3588RebootTime_Data_Attr);
	
	msgQueue_SetRk3588RebootTime_Data  = osMessageQueueNew(MQ_SIZE , sizeof(REBOTTIME) ,&SetRk3588RebootTime_Data_Attr);	

	/********************************** ��ʱ������ *****************************************/

	/* ������ʱ�� */
	timerID_rk3588reboot = osTimerNew(timer_rk3588reboot_App,   /* ��ʱ���ص����� */
									  osTimerOnce,      		/* �������� */
									  NULL,              		/* �����ݲ��� */
									  &timer_rk3588reboot_Attr  /* ��ʱ������ */ );
									  
									  
									  
	unsigned int CPU_Sn0, CPU_Sn1, CPU_Sn2;

	CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
	CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
	CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

	#if DEBUG_MODE == 1  
		printf("CPU : STM32F407IGT6, LQFP176, Frequency: %dMHz\r\n", SystemCoreClock / 1000000);
		printf("UID = %08X %08X %08X\r\n", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	#endif

	while(1)
	{
		/* ��Ҫ�����Դ���ĳ��򣬶�Ӧ������̵��õ�SysTick_ISR */
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
		/* Start�߳��˳� */
		osThreadExit();  /* Start�߳�����osThreadDetached,�˳����޷�ͨ��osThreadJoin������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskOutherThred
*	����˵��: ��ɱ�������
*	��    ��: 
*	�� �� ֵ: ��
*   �� �� ��: 
*   
*********************************************************************************************************
*/
void AppTaskOutherThred(void *argument)
{
	unsigned int Flag = 0;
	const uint16_t usFrequency = 1000; /* �ӳ����� */
	uint32_t tick;	
	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 	
	osStatus_t os_Status;
	GetFANOrg_Frequency getorg_fanfreq = {0};
	GetFANOrg_duty_cycle getorg_dutycycle = {0};
	CALFAN_Rising calfan_risi = {0};
	
	unsigned char msg_FANFrequency_Number = 0;
	unsigned char msg_FANDuty_cycle_Number = 0;	
	
	__IO float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL] = {0}; 
	__IO static unsigned int keill = 0; //��ɱ����ʱ

	while(1)
	{			
		unsigned int pwmfreq[3] = {0};
		float pwmduty_cycle[3]  = {0.00};

		msg_FANFrequency_Number = osMessageQueueGetCount(msgQueue_GetFanFrequency_Data);
		msg_FANDuty_cycle_Number = osMessageQueueGetCount(msgQueue_GetFanDuty_cycle_Data);
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1] / 4096 * (float)3.3;	
		// ADCת���ĵ�ѹֵͨ��MDA��ʽ����SRAM 
		/* 24V��Դ��ѹ */
		if(keill >= 0x258)
		//if(keill >= 0x20)
		{
			bsp_GpioOff(BATTERY);
		}
		
		if(ADC_ConvertedValueLocal[1] > 2.0)/*��Դģʽ : ���õ�Դ*/
		{
			keill = 0;
		}
		else	/*��Դģʽ : ��ص�Դ*/
		{
			osEventFlagsSet(event_PowMode_ID , 0x01U);
			osEventFlagsSet(event_RK3588Sinal_ID , 0x05U);
			keill++;
		}

		
		if(msg_FANFrequency_Number > 0 && msg_FANDuty_cycle_Number > 0)
		{
			os_Status = osMessageQueueGet(msgQueue_GetFanFrequency_Data,/* ��Ϣ���е����� */
										  &getorg_fanfreq,    		  	/* �����Ϣ���ڴ� */
										  NULL,              	  		/* ȡ����Ϣ�����ȼ� */
										  osWaitForever  	      		/* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
			os_Status = osMessageQueueGet(msgQueue_GetFanDuty_cycle_Data,/* ��Ϣ���е����� */
										  &getorg_dutycycle,    		  		/* �����Ϣ���ڴ� */
										  NULL,              	  				/* ȡ����Ϣ�����ȼ� */
										  osWaitForever  	      				/* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);	

			pwmfreq[0] = (uint16_t)10000/getorg_fanfreq.org_big_pwm_risingcount;	//�ⲿ
			pwmfreq[1] = (uint16_t)10000/getorg_fanfreq.org_arm_pwm_risingcount;	//RK
			pwmfreq[2] = (uint16_t)10000/getorg_fanfreq.org_rk_pwm_risingcount;		//ARM

			pwmduty_cycle[0] = (float)getorg_dutycycle.org_big_pwm_fallingcount / getorg_fanfreq.org_big_pwm_risingcount;
			pwmduty_cycle[1] = (float)getorg_dutycycle.org_arm_pwm_fallingcount / getorg_fanfreq.org_arm_pwm_risingcount;
			pwmduty_cycle[2] = (float)getorg_dutycycle.org_rk_pwm_fallingcount  / getorg_fanfreq.org_rk_pwm_risingcount;
			
			#if DEBUG_MODE == 1  
				//printf("FAN1 = %d , FAN2 = %d , FAN3 = %d \r\n" ,pwmfreq[0] , pwmfreq[1] , pwmfreq[2]);
			#endif
	
			calfan_risi.arm_fan_risingcount = pwmfreq[2];
			calfan_risi.big_fan_risingcount = pwmfreq[0];
			calfan_risi.rk_fan_risingcount = pwmfreq[1];
			
			/*��������һ����Ϣ���н� pwmfreq ���ݴ��뼴��*/
			
			//���
			os_Status = osMessageQueuePut(msgQueue_CALFAN_Rising_Data,  	/* ��Ϣ���е����� */
										  &calfan_risi, 				/* ������Ϣ���еı�������Ϣ�� */
										  NULL,      						/* ������Ϣ�����ȼ� */
										  NULL      	 					/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
										  );

		}
		bsp_LedToggle(1);
		bsp_LedToggle(2);


		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
	}
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}
