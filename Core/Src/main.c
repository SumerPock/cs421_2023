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

/*	写的时候觉得不是太难，现在他快把我写吐了，MDK在应付这种大型工程的是否很麻烦	*/

/*	2023 5 12 
	本项目使用了 DS18B20 ,并且我加入了操作系统，所以使用了DWT计数器，使用起来没问题很方便 ， 
	但调试的时候发现了一个现象，在DEBUG模式下程序可以正常运行，但是正常不接仿真器的情况下
	重新上下电后板卡出现了卡死现象，最终跟踪到是因为DS18B20的调用造成，其中使用DWT计时器这个
	东西，查论坛发现其他人也有这个现象，最终跟踪到是因为rest and run选项勾选的问题*/
	
/*	理想很丰满现实很骨感啊 */
/************************************** 任务 *******************************************/

/*启动任务*/
void AppTaskStart(void *argument);
const osThreadAttr_t ThreadStart_Attr = 	/* 启动任务属性设置 */
{
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh7,
	.stack_size = 2048,
};
osThreadId_t ThreadIdStart = NULL;				/*启动任务句柄*/


/*以下任务优先级从高到低*/


/*读取 DS18B20 ，LM75B 任务 优先级最高 ， 并运行温控程序*/
void AppTaskDS18B20(void *argument);
const osThreadAttr_t ThreadDS18B20_Attr =
{
	.name 	    = "ThreadDS18B20",
	.attr_bits  =  osThreadDetached, 
	.priority   =  osPriorityHigh7,
	.stack_size =  4096,
};
osThreadId_t ThreadIdTaskDS18B20 = NULL;



/*接收RK3588数据任务*/
void AppTaskRK3588_Receive(void *argument);
const osThreadAttr_t ThreadRK3588_Receive_Attr =
{
	.name = "ThreadRK3588_Receive",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskRK3588_Receive = NULL;



/*RK3588发送数据任务*/
void AppTaskRK3588_Send(void *argument);
const osThreadAttr_t ThreadRK3588_Send_Attr =
{
	.name = "ThreadRK3588_Send",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh5,
	.stack_size = 4096,
};
osThreadId_t ThreadIdTaskRK3588_Send = NULL;



/*RK3588关机任务*/
void AppTaskRK3588_Closure(void *argument);
const osThreadAttr_t ThreadRK3588_Closure_Attr =
{
	.name = "ThreadRK3588_Closure",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskRK3588_Closure = NULL;


/*RK3588重启任务*/
void AppTaskRK3588_Reboot(void *argument);
const osThreadAttr_t ThreadRK3588_Reboot_Attr =
{
	.name = "ThreadRK3588_Reboot",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 1024,
};
osThreadId_t ThreadIdTaskRK3588_Reboot = NULL;



/*上位机通讯任务*/
//void AppTaskPCCommun(void *argument);
//const osThreadAttr_t ThreadPCCommun_Attr =
//{
//	.name = "ThreadPCCommun",
//	.attr_bits = osThreadDetached, 
//	.priority = osPriorityHigh6,
//	.stack_size = 4096,
//};
//osThreadId_t ThreadIdTaskPCCommun = NULL;


/*GPS数据解析任务*/
void AppTaskGPS(void *argument);
const osThreadAttr_t ThreadGPS_Attr =
{
	.name = "ThreadGPS",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh3,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskGPS = NULL;


/*2023.5.8补充，因为GPS上的电池太小，上电后断电闲置1天左右就会断电 ， 断电后其内配置信息丢失，造成解析GPS数据量过大，所以每次上电后
  都要给GPS下发一遍关闭指令，所以需要创建一个上电后发送GPS协议的线程，其在配置完毕后就关闭此线程*/
/*GPS设置任务*/
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
/*LWIP网络轮询处理任务*/
void AppTaskLwIPPro(void *argument);	
const osThreadAttr_t ThreadLwIPPro_Attr = 		
{
	.name = "osRtxLwIPProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityHigh4,
	.stack_size = 2048,
};
osThreadId_t ThreadIdTaskLwIPPro = NULL;	


/*网线插拔状态任务*/
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



/************************************** 消息队列 *******************************************/

/*用于接收上位机设置的时间数据*/
//osMessageQueueId_t msgQueue_PcSetTime_Data; 	
//const osMessageQueueAttr_t PcSetTime_Data_Attr =
//{
//	.name = "PcSetTime_Data",   					
//};


/*用于接收上位机设置的网络数据*/
//osMessageQueueId_t msgQueue_PcSetNetwork_Data; 	
//const osMessageQueueAttr_t PcSetNetwork_Data_Attr =
//{
//	.name = "PcSetNetwork_Data",   					
//};

/*用于返回上位机设置的时间数据*/
//osMessageQueueId_t msgQueue_PcGetTime_Data; 	
//const osMessageQueueAttr_t PcGetTime_Data_Attr =
//{
//	.name = "PcGetTime_Data",   					
//};


/*用于返回上位机设置的网络数据*/
//osMessageQueueId_t msgQueue_PcGetNetwork_Data; 	
//const osMessageQueueAttr_t PcGetNetwork_Data_Attr =
//{
//	.name = "PcGetNetwork_Data",   					
//};


/*用于接收第一个DS18B29温度数据*/
osMessageQueueId_t msgQueue_Ds18b20_1_Data; 	
const osMessageQueueAttr_t Ds18b20_1_Data_Attr =
{
	.name = "Ds18b20_1_Data",   					
};

/*用于接收第二个DS18B29温度数据*/
osMessageQueueId_t msgQueue_Ds18b20_2_Data; 	
const osMessageQueueAttr_t Ds18b20_2_Data_Attr =
{
	.name = "Ds18b20_2_Data",   					
};

/*用于接收板载LM75B温度数据*/
osMessageQueueId_t msgQueue_LM75B_Data; 	
const osMessageQueueAttr_t LM75B_Data_Attr =
{
	.name = "LM75B_Data",   					
};


/*用于接收GPS数据*/
osMessageQueueId_t msgQueue_GPS_Data; 	
const osMessageQueueAttr_t GPS_Data_Attr =
{
	.name = "GPS_Data",   					
};


/*用于记录RK3588重启时间*/
osMessageQueueId_t msgQueue_RK3588RebootTime_Data; 	
const osMessageQueueAttr_t RK3588RebootTime_Data_Attr =
{
	.name = "RK3588RebootTime_Data",   					
};


/*设置RK3588重启时间*/
osMessageQueueId_t msgQueue_SetRk3588RebootTime_Data; 	
const osMessageQueueAttr_t SetRk3588RebootTime_Data_Attr =
{
	.name = "SetRk3588RebootTime_Data",   					
};


/*用于传输频率*/
osMessageQueueId_t msgQueue_GetFanFrequency_Data; 	
const osMessageQueueAttr_t GetFanFrequency_Data_Attr =
{
	.name = "GetFanFrequency_Data",   					
};

/*用于传输占空比*/
//duty_cycle
osMessageQueueId_t msgQueue_GetFanDuty_cycle_Data; 	
const osMessageQueueAttr_t GetFanDuty_cycle_Data_Attr =
{
	.name = "GetFanDuty_cycle_Data",   					
};


/*计算出来的实际风扇转速*/
osMessageQueueId_t msgQueue_CALFAN_Rising_Data; 	
const osMessageQueueAttr_t CALFAN_Rising_Data_Attr =
{
	.name = "CALFAN_Rising_Data",   					
};


/************************************** 事件标志位 **********************************************/

/* 网线插拔事件标志组属性 */
const osEventFlagsAttr_t event_cable_Attr = 
{
	.name = "event_cable",   	/* 事件标志组的名字 */
								/* 其他默认 */
};
/*网线插拔事件标志组ID */
osEventFlagsId_t event_cable_ID = NULL;


/*USART1 串口接收数据标志位 用于接收上位机数据*/
const osEventFlagsAttr_t event_usart1_Attr = 
{
	.name = "event_usart1",   	/* 事件标志组的名字 */
								/* 其他默认 */
};
osEventFlagsId_t event_usart1_ID = NULL;


/*UART3 串口接收数据标志位 用于接收GPS数据*/
const osEventFlagsAttr_t event_usart3_Attr = 
{
	.name = "event_usart3",   	/* 事件标志组的名字 */
								/* 其他默认 */
};
osEventFlagsId_t event_usart3_ID = NULL;



/*UART5 串口接收数据标志位 用于接收RK3588数据*/
const osEventFlagsAttr_t event_uart5_Attr = 
{
	.name = "event_uart5",   	/* 事件标志组的名字 */
								/* 其他默认 */
};
osEventFlagsId_t event_uart5_ID = NULL;


/*主动断电请求ID*/
const osEventFlagsAttr_t event_initiativePowOff_Attr = 
{
	.name = "event_initiativePowOff",   /* 事件标志组的名字 */
										/* 其他默认 */
};
osEventFlagsId_t event_initiativePowOff_ID = NULL;


/*电源模式*/
const osEventFlagsAttr_t event_PowMode_Attr = 
{
	.name = "event_PowMode",   			/* 事件标志组的名字 */
										/* 其他默认 */
};
osEventFlagsId_t event_PowMode_ID = NULL;

/*RK3588事件*/
const osEventFlagsAttr_t event_RK3588Sinal_Attr = 
{
	.name = "event_RK3588Sinal",   			/* 事件标志组的名字 */
											/* 其他默认 */
};
osEventFlagsId_t event_RK3588Sinal_ID = NULL;


/*RK3588工作完成事件*/
const osEventFlagsAttr_t event_RK3588Ready_Attr = 
{
	.name = "event_RK3588Ready",   			/* 事件标志组的名字 */
											/* 其他默认 */
};
osEventFlagsId_t event_RK3588Ready_ID = NULL;


/*RK3588 关机事件*/
const osEventFlagsAttr_t event_RK3588ShutDown_Attr = 
{
	.name = "event_RK3588ShutDown",   			/* 事件标志组的名字 */
											/* 其他默认 */
};
osEventFlagsId_t event_RK3588ShutDown_ID = NULL;


/*RK3588 开机事件*/
const osEventFlagsAttr_t event_RK3588StartUp_Attr = 
{
	.name = "event_RK3588StartUp",   			/* 事件标志组的名字 */
												/* 其他默认 */
};
osEventFlagsId_t event_RK3588StartUp_ID = NULL;


/* 用于标记重启倒计时 */
const osEventFlagsAttr_t event_RK3588RebootCountdown_Attr = 
{
	.name = "event_RK3588RebootCountdown",   	/* 事件标志组的名字 */
												/* 其他默认 */
};
osEventFlagsId_t event_RK3588RebootCountdown_ID = NULL;



/************************************** 软件定时器 *******************************************/

/* 保存定时器ID */
osTimerId_t  timerID_rk3588reboot = NULL; 

 /* 软件定时器 */
void timer_rk3588reboot_App(void *argument); 

/*软件定时器用于标记重启时间*/
const osTimerAttr_t timer_rk3588reboot_Attr = 
{
	.name = "rk3588reboottimer",				/*定时器名字*/
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
	HAL_Init(); //设定中断优先组4
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* 
	   Event Recorder：
	   - 可用于代码执行时间测量，MDK5.25及其以上版本才支持，IAR不支持。
	   - 默认不开启，如果要使能此选项，务必看V5开发板用户手册第8章
	*/
#if Enable_EventRecorder == 1  
	/* 初始化EventRecorder并开启 */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif	

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	/* 内核初始化 */
	osKernelInitialize();                                  
	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  
	/* 开启多任务 */
	osKernelStart();
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，也就是最高优先级任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 100; /* 延迟周期 */
	uint32_t tick;	
	/* 初始化外设 */
	HAL_ResumeTick();
	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
	Bsp_initialize(); /*初始化大部分设备*/
	
	bsp_GpioOn(INTERNALFAN);	//内置小风扇
	bsp_GpioOn(EXTERNALFAN);	//外置风扇
	bsp_GpioOn(BATTERY);		//电池供电
	bsp_GpioOn(RK3588POWER);	//RK3588开机按键
	
	bsp_GpioOff(CRYOGEN);		//半导体制冷
	bsp_GpioOff(HEATING);		//加热片
	bsp_GpioOff(5);				//GPIO5未使用 PA8
	
	bsp_LedOff(ONBOARD_LED_1);	//内部LED
	bsp_LedOff(ONBOARD_LED_2);
	
//	bsp_LedOff(EXTERNAL_LED_1); //外部LED
//	bsp_LedOff(EXTERNAL_LED_2);
//	bsp_LedOff(EXTERNAL_LED_3);
	
	/* 5个挡位 大风扇
	反馈信号	[0] = 58hz , [1] = 68 ， [2]= 77 ， [3]= 111 ， [4]=138	*/
	unsigned int pwmButy_Cycle[5] = {3700  , 4400 , 5100 , 5800 , 6500}; //150000000
	//9900--136 5500--83
	bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_20); /*500KHz，占空比50% */
	//bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, 5000000, 50000000); /*500KHz，占空比50% */
	/* 6个挡位 RK端小风扇
		反馈信号 	[0] = 53 , [1] = 89 , [2] = 122 , [3] = 148 , [4] = 183 , [5] =  215			*/	
//	unsigned int pwmButy_Cycle1_Sm[6] = {2000  , 3200 , 4400 , 5600  , 6800 , 8000};
	bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_16);
//	
//	/* 6个挡位 ARM端小风扇
//		反馈信号 	[0] = 53 , [1] = 89 , [2] = 122 , [3] = 148 , [4] = 183 , [5] =  215			*/		
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
	/* 初始化DP83848并开启 */
	g_ucEthLinkStatus = 0;
	/* 初始化网络 */
	if(net_init() != 0)
	{
		printf("the lwip config is flad \r\n");//LWIP协议初始化失败
	}
#endif	
	
	/********************************** 系统标志位创建 *****************************************/
	
	/*创建事件标志组*/
	event_cable_ID = osEventFlagsNew(&event_cable_Attr);
	
	/*USART1 串口接收事件标志组*/
	event_usart1_ID = osEventFlagsNew(&event_usart1_Attr);
   
	/*UART3 串口接收事件标志组*/
	event_usart3_ID = osEventFlagsNew(&event_usart3_Attr);	
	
	/*USART5 串口接收事件标志组*/
	event_uart5_ID = osEventFlagsNew(&event_uart5_Attr);		

	/*主动断电请求*/
	event_initiativePowOff_ID = osEventFlagsNew(&event_initiativePowOff_Attr);	
	
	/*电池模式选择*/
	event_PowMode_ID = osEventFlagsNew(&event_PowMode_Attr);	

	/*RK3588事件*/
	event_RK3588Sinal_ID = osEventFlagsNew(&event_RK3588Sinal_Attr);

	/*RK3588准备完成*/
	event_RK3588Ready_ID = osEventFlagsNew(&event_RK3588Ready_Attr);
	
	/*RK3588 关机事件*/
	event_RK3588ShutDown_ID = osEventFlagsNew(&event_RK3588ShutDown_Attr);
	
	/*RK3588 开机事件*/
	event_RK3588StartUp_ID = osEventFlagsNew(&event_RK3588StartUp_Attr);
	
	/* 标记RK3588重启倒计时 */
	event_RK3588RebootCountdown_ID = osEventFlagsNew(&event_RK3588RebootCountdown_Attr);
		
	/********************************** 任务创建 *****************************************/
	
#if Initialize_Network == 1  	
	/* 创建LWIP网络轮询任务 */
	ThreadIdTaskLwIPPro = osThreadNew(AppTaskLwIPPro, NULL, &ThreadLwIPPro_Attr);
	osDelay(200);
	
	/*	网线插拔状态检测	*/								
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr); 
#endif


	/*	干一些杂乱对时序不是太关注的活	
			目前这个任务 读取了FAN的反馈转速信息，并将数据放入消息队列中去，
			闪烁板载2个LED , 其中这里面的自杀程序是完整的
	*/
	ThreadIdTaskOutherThred 	=	osThreadNew(AppTaskOutherThred, NULL, &ThreadOutherThred_Attr);


	/*	内部DS18B20温度读取，并且阉割掉外部DS18B20 
			LM75B温度读取
			ADC电池，ADC24v采集任务	
			自动温控
	*/
	ThreadIdTaskDS18B20 		=	osThreadNew(AppTaskDS18B20, NULL, &ThreadDS18B20_Attr);


	/* RK3588接收数据任务
	*/
	ThreadIdTaskRK3588_Receive 	=	osThreadNew(AppTaskRK3588_Receive, NULL, &ThreadRK3588_Receive_Attr);
	
	
	/* 电源板向RK3588发送数据任务，其中电源板正常信息上报修改成了RS422 
	*/
	ThreadIdTaskRK3588_Send		=	osThreadNew(AppTaskRK3588_Send, NULL, &ThreadRK3588_Send_Attr);	


	/* 上位机任务 
		 主要是报一些修改ip的，时间的数据
	*/
	//ThreadIdTaskPCCommun 		=	osThreadNew(AppTaskPCCommun, NULL, &ThreadPCCommun_Attr);
	
	
	/*GPS设置任务
		上电后发送GPS配置参数设置，发送完毕后，启动GPS接收任务，其中GPS接收任务里还有一个用于冗余的自杀程序
	*/
	ThreadIdTaskGPSSetUp 		=	osThreadNew(AppTaskGPSSetUp, NULL, &ThreadGPSSetUp_Attr);	
	
	/********************************** 消息队列创建 *****************************************/

	msgQueue_Ds18b20_1_Data 	   = osMessageQueueNew(MQ_SIZE , sizeof(DS18B20_DATA) ,&Ds18b20_1_Data_Attr);
	
	msgQueue_Ds18b20_2_Data 	   = osMessageQueueNew(MQ_SIZE , sizeof(DS18B20_DATA) ,&Ds18b20_2_Data_Attr);

	msgQueue_LM75B_Data     	   = osMessageQueueNew(MQ_SIZE , sizeof(LM75B_DATA) ,&LM75B_Data_Attr);

	/*传输频率信号*/
	msgQueue_GetFanFrequency_Data  = osMessageQueueNew(MQ_SIZE , sizeof(GetFANOrg_Frequency) ,&GetFanFrequency_Data_Attr);
	
	/*传输占空比信号*/
	msgQueue_GetFanDuty_cycle_Data = osMessageQueueNew(MQ_SIZE , sizeof(GetFANOrg_duty_cycle) ,&GetFanDuty_cycle_Data_Attr);
	
	/*传输计算出的风扇转速信号*/
	msgQueue_CALFAN_Rising_Data    = osMessageQueueNew(MQ_SIZE , sizeof(CALFAN_Rising) ,&CALFAN_Rising_Data_Attr);
	
	msgQueue_RK3588RebootTime_Data = osMessageQueueNew(MQ_SIZE , sizeof(REBOTTIME) ,&RK3588RebootTime_Data_Attr);
	
	msgQueue_SetRk3588RebootTime_Data  = osMessageQueueNew(MQ_SIZE , sizeof(REBOTTIME) ,&SetRk3588RebootTime_Data_Attr);	

	/********************************** 定时器创建 *****************************************/

	/* 创建定时器 */
	timerID_rk3588reboot = osTimerNew(timer_rk3588reboot_App,   /* 定时器回调函数 */
									  osTimerOnce,      		/* 连续运行 */
									  NULL,              		/* 不传递参数 */
									  &timer_rk3588reboot_Attr  /* 定时器属性 */ );
									  
									  
									  
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
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
		/* Start线程退出 */
		osThreadExit();  /* Start线程属于osThreadDetached,退出后无法通过osThreadJoin加入调度 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskOutherThred
*	功能说明: 自杀任务加入
*	形    参: 
*	返 回 值: 无
*   优 先 级: 
*   
*********************************************************************************************************
*/
void AppTaskOutherThred(void *argument)
{
	unsigned int Flag = 0;
	const uint16_t usFrequency = 1000; /* 延迟周期 */
	uint32_t tick;	
	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 	
	osStatus_t os_Status;
	GetFANOrg_Frequency getorg_fanfreq = {0};
	GetFANOrg_duty_cycle getorg_dutycycle = {0};
	CALFAN_Rising calfan_risi = {0};
	
	unsigned char msg_FANFrequency_Number = 0;
	unsigned char msg_FANDuty_cycle_Number = 0;	
	
	__IO float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL] = {0}; 
	__IO static unsigned int keill = 0; //自杀倒计时

	while(1)
	{			
		unsigned int pwmfreq[3] = {0};
		float pwmduty_cycle[3]  = {0.00};

		msg_FANFrequency_Number = osMessageQueueGetCount(msgQueue_GetFanFrequency_Data);
		msg_FANDuty_cycle_Number = osMessageQueueGetCount(msgQueue_GetFanDuty_cycle_Data);
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1] / 4096 * (float)3.3;	
		// ADC转换的电压值通过MDA方式传到SRAM 
		/* 24V电源电压 */
		if(keill >= 0x258)
		//if(keill >= 0x20)
		{
			bsp_GpioOff(BATTERY);
		}
		
		if(ADC_ConvertedValueLocal[1] > 2.0)/*电源模式 : 外置电源*/
		{
			keill = 0;
		}
		else	/*电源模式 : 电池电源*/
		{
			osEventFlagsSet(event_PowMode_ID , 0x01U);
			osEventFlagsSet(event_RK3588Sinal_ID , 0x05U);
			keill++;
		}

		
		if(msg_FANFrequency_Number > 0 && msg_FANDuty_cycle_Number > 0)
		{
			os_Status = osMessageQueueGet(msgQueue_GetFanFrequency_Data,/* 消息队列的名字 */
										  &getorg_fanfreq,    		  	/* 存放消息的内存 */
										  NULL,              	  		/* 取出消息的优先级 */
										  osWaitForever  	      		/* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
			os_Status = osMessageQueueGet(msgQueue_GetFanDuty_cycle_Data,/* 消息队列的名字 */
										  &getorg_dutycycle,    		  		/* 存放消息的内存 */
										  NULL,              	  				/* 取出消息的优先级 */
										  osWaitForever  	      				/* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);	

			pwmfreq[0] = (uint16_t)10000/getorg_fanfreq.org_big_pwm_risingcount;	//外部
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
			
			/*启动另外一个消息队列将 pwmfreq 数据传入即可*/
			
			//入队
			os_Status = osMessageQueuePut(msgQueue_CALFAN_Rising_Data,  	/* 消息队列的名字 */
										  &calfan_risi, 				/* 放入消息队列的变量（消息） */
										  NULL,      						/* 放入消息的优先级 */
										  NULL      	 					/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
										  );

		}
		bsp_LedToggle(1);
		bsp_LedToggle(2);


		/* 相对延迟 */
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
