
#ifndef __BSP_H
#define __BSP_H

#include "EventRecorder.h"
#include "EventRecorderConf.h"
#include <stdio.h>
#include "includes.h"
#include "bsp_dwt.h"
#include "stm32f4xx_hal.h"
//#include "udp_echoclient.h"

#include "lwip/udp.h"
#include "bsp_can.h"
#include "bsp_gpio.h"

#include <string.h>
#include "bsp_uart.h"
#include "bsp_dma.h"
#include "bsp_i2c.h"

#include "bsp_timer.h"
#include "bsp_ds18b20.h"
#include "bsp_gps.h"

#include "bsp_tim_pwm.h"

#include "bsp_led.h"

#include "bsp_adc.h"

#include "bsp_task.h"
#include "bsp_task_rk3588commun.h"
#include "bsp_task_pccommun.h"
#include "bsp_rk3588_shutdown.h"


#include "udp_echoserver.h"

#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_i2c.h"

#define COLSETIME 600

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */


#define  USE_RTX    1

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define MQ_SIZE 16

/* 默认是关闭状态 */
#define  Enable_EventRecorder  1

/* 网络初始化 */
#define  Initialize_Network  0

#define DEBUG_MODE 0

#define TURNON_NO_Gps 		1

#define TURNON_NO_Normal	1


//外置大风扇
#define EXTERNAL_FAN_GPIOPIN	GPIO_PIN_14
#define EXTERNAL_FAN_GPIOPORT	GPIOE
#define EXTERNAL_FAN_TIMER		TIM1
#define EXTERNAL_FAN_CHANNEL 	4
#define EXTERNAL_FAN_FREQ		5000



//内置RK小风扇
#define INTERNAL_RK_FAN_GPIOPIN		GPIO_PIN_13
#define INTERNAL_RK_FAN_GPIOPORT	GPIOE
#define INTERNAL_RK_FAN_TIMER		TIM1
#define INTERNAL_RK_FAN_CHANNEL 	3
#define INTERNAL_RK_FAN_FREQ		5000


//内置ARM小风扇
#define INTERNAL_ARM_FAN_GPIOPIN	GPIO_PIN_11
#define INTERNAL_ARM_FAN_GPIOPORT	GPIOE
#define INTERNAL_ARM_FAN_TIMER		TIM1
#define INTERNAL_ARM_FAN_CHANNEL 	2
#define INTERNAL_ARM_FAN_FREQ		5000
void Error_Handler(void);
void SystemClock_Config(void);




/**************结构体定义****************/
typedef struct
{
	unsigned char dataid;					//ID
	unsigned char datalen;				//有效数据位								
	unsigned char dataloop;				//帧计数器
	unsigned char system_state;		//主控系统状态
	unsigned char system_error;		//系统异常代码
	unsigned char system_reboot_timer;	//主板断电重启 延时
	unsigned char system_reboot_check;	//断电重启校验
	unsigned short system_reboot_ask;		//主板断电请求	
	
	unsigned char system_reboot_askflag;//电源板请求应答标志位
	unsigned char askerror;			//校验失败，数据异常，请求重发
	unsigned char cputemp;			//CPU温度
	unsigned char ask;					//校验和
}SYSTEMDATA;

typedef struct
{
	unsigned char *pRxBuf;
	unsigned char rxFlag;
	unsigned char rxLen;
	const unsigned char rxSize;	/*const 必须在结构体变量定义时赋值 */
}BspUart_t;

typedef struct
{
	float get_temp;
}DS18B20_DATA;

typedef struct
{
	unsigned char reboottime;
	unsigned char reboottimecheck;
}REBOTTIME;
 
typedef struct
{
	float get_temp;
}LM75B_DATA;

/*LM1266采集的数据*/
struct LM1266{
	unsigned int voltage;
	unsigned int current;
	unsigned int power;
	unsigned int electrical_energy;
	unsigned int Power_Factor;
	unsigned int carbon_dioxide;
	unsigned int frequency;
	unsigned char number;
};


//#pragma pack(2)
struct UARTState_Machine{
	unsigned char buf[48];	/*数据存储数组*/
	unsigned char zero[128];
	unsigned char read;		/*读取的FIFO数据*/
	unsigned char ucStatus;  /* 状态机标志 */
	unsigned char ucCount;	/**/
	unsigned char UMflag;
	unsigned char Canflag;
	unsigned char datalen;
};

/*用于接受CAN数据*/
struct CAN_RESDATA{
	unsigned char can_resbuf[8];
	unsigned char can_resbuflen;
	unsigned int  resStdID;	//标准帧
	unsigned int  resExtID; //拓展帧
};


/*用于接收上位机发送来的ip地址设置*/
typedef struct 
{
	unsigned char  ip_1;
	unsigned char  ip_2;
	unsigned char  ip_3;
	unsigned char  ip_4;
	unsigned short local_port;  //本地端口
}PcSetNetwork;
//extern PcSetNetwork;


/*用于接收上位机发送来的ip地址设置*/
typedef struct 
{
	unsigned char year;
	unsigned char moon;
	unsigned char day;
	unsigned char hour;
	unsigned char point;  //分
	unsigned char second; //秒
}PcSetTimer;


/*我思考了下这里为了传输方便创建一个大的消息队列传输风扇反馈数据*/
typedef struct
{
	unsigned short org_big_pwm_risingcount;
	
	unsigned short org_rk_pwm_risingcount;
	
	unsigned short org_arm_pwm_risingcount;
	
}GetFANOrg_Frequency;



typedef struct
{
	unsigned short org_big_pwm_fallingcount;
	
	unsigned short org_rk_pwm_fallingcount;
	
	unsigned short org_arm_pwm_fallingcount;
	
}GetFANOrg_duty_cycle;

/*计算出来的正确风扇反馈频率信号*/
typedef struct
{
	unsigned short big_fan_risingcount;
	unsigned short rk_fan_risingcount;
	unsigned short arm_fan_risingcount;
}CALFAN_Rising;


/****由于这次传输机制比较特殊****/
typedef struct
{
	unsigned char pow_close_loop; //电源板发送下电指令的帧序号	
}POW_CLOSE;

typedef struct
{
	unsigned char system_ans_loop; //主板接收到电源板下电的帧序号	
}SYS_ANS;

typedef struct
{
	unsigned char pow_renow_loop; //电源板下发立即重启	
}POW_RESTART_NOW;


typedef struct
{
	unsigned char pow_count_loop;//电源板下发倒计时重启
}POW_RESTART_COUNT;


/***************************************/


/** 引用部分 **/

/*GPS数据解析任务*/
extern void AppTaskGPS(void *argument); 		//GPS任务函数
extern const osThreadAttr_t ThreadGPS_Attr;     //GPS任务参数
extern osThreadId_t ThreadIdTaskGPS;			//GPS任务句柄


/*RK3588关机任务*/
extern void AppTaskRK3588_Closure(void *argument); 		//RK3588关机任务
extern const osThreadAttr_t ThreadRK3588_Closure_Attr;   //RK3588关机任务参数	
extern osThreadId_t ThreadIdTaskRK3588_Closure; 		//RK3588关机任务句柄

/*RK3588重启任务*/
extern void AppTaskRK3588_Reboot(void *argument);
extern const osThreadAttr_t ThreadRK3588_Reboot_Attr;
extern osThreadId_t ThreadIdTaskRK3588_Reboot;

/*读取俩个 DS18B20 任务*/
extern void AppTaskDS18B20(void *argument);		//DS18B20任务
extern const osThreadAttr_t ThreadDS18B20_Attr;	//DS18B20任务参数
extern osThreadId_t ThreadIdTaskDS18B20;		//DS18B20任务句柄


///*接收上位机通讯任务*/
//extern void AppTaskPCCommun(void *argument);	
//extern const osThreadAttr_t ThreadPCCommun_Attr;
//extern osThreadId_t ThreadIdTaskPCCommun;


/*用于记录RK3588重启时间*/
extern osMessageQueueId_t msgQueue_RK3588RebootTime_Data;
extern const osMessageQueueAttr_t RK3588RebootTime_Data_Attr;

extern osMessageQueueId_t msgQueue_SetRk3588RebootTime_Data;
extern const osMessageQueueAttr_t SetRk3588RebootTime_Data_Attr;



/*DS18B20 1消息队列*/
extern osMessageQueueId_t msgQueue_Ds18b20_1_Data;
extern const osMessageQueueAttr_t Ds18b20_1_Data_Attr;/*DS18B20 1消息队列句柄*/


/*DS18B20 2消息队列*/
extern osMessageQueueId_t msgQueue_Ds18b20_2_Data;
extern const osMessageQueueAttr_t Ds18b20_2_Data_Attr;/*DS18B20 2消息队列句柄*/


/*LM75B消息队列*/
extern osMessageQueueId_t msgQueue_LM75B_Data; 	
extern const osMessageQueueAttr_t LM75B_Data_Attr; /*LM75B 消息队列句柄*/


/*GPS数据消息队列*/
extern osMessageQueueId_t msgQueue_GPS_Data; 	//GPS消息队列名字
extern const osMessageQueueAttr_t GPS_Data_Attr;//GPS消息队列句柄

/*用于接收上位机设置的时间数据*/
//extern osMessageQueueId_t msgQueue_PcSetTime_Data; 
//extern const osMessageQueueAttr_t PcSetTime_Data_Attr;


/*用于接收上位机设置的网络数据*/
//extern osMessageQueueId_t msgQueue_PcSetNetwork_Data;
//extern const osMessageQueueAttr_t PcSetNetwork_Data_Attr;


/*用于返回上位机设置的时间数据*/
//extern osMessageQueueId_t msgQueue_PcGetTime_Data;
//extern const osMessageQueueAttr_t PcGetTime_Data_Attr;


/*用于返回上位机设置的网络数据*/
//extern osMessageQueueId_t msgQueue_PcGetNetwork_Data;
//extern const osMessageQueueAttr_t PcGetNetwork_Data_Attr;



extern osMessageQueueId_t msgQueue_GetFanFrequency_Data;
extern const osMessageQueueAttr_t GetFanFrequency_Data_Attr;


extern osMessageQueueId_t msgQueue_GetFanDuty_cycle_Data; 
extern const osMessageQueueAttr_t GetFanDuty_cycle_Data_Attr;

extern osMessageQueueId_t msgQueue_CALFAN_Rising_Data; 
extern const osMessageQueueAttr_t CALFAN_Rising_Data_Attr;



extern osTimerId_t  timerID_rk3588reboot;
extern void timer_rk3588reboot_App(void *argument);
extern const osTimerAttr_t timer_rk3588reboot_Attr;



/*RK3588事件标志组*/
extern osEventFlagsId_t event_RK3588Sinal_ID;

/*串口1事件标志位*/
extern osEventFlagsId_t event_usart1_ID;

/*串口3事件标志位*/
extern osEventFlagsId_t event_usart3_ID;

/*串口5事件标志位*/
extern osEventFlagsId_t event_uart5_ID;

/*网线插拔事件标志组*/
extern osEventFlagsId_t event_cable_ID;

/*RK3588事件*/
extern osEventFlagsId_t event_RK3588Sinal_ID;

/*RK3588工作完成事件*/
extern osEventFlagsId_t event_RK3588Ready_ID;

/*用于上位机接收状态*/
//extern osEventFlagsId_t event_SetPcCommon_ID;

/*电源模式状态*/
extern osEventFlagsId_t event_PowMode_ID;

/*关机事件*/
extern osEventFlagsId_t event_RK3588ShutDown_ID;

/*开机事件*/
extern osEventFlagsId_t event_RK3588StartUp_ID;

/*rk3588重启标记事件*/
extern osEventFlagsId_t event_RK3588RebootCountdown_ID;


/**************引用其他文件****************/
extern BspUart_t g_tBspUsart1;
extern BspUart_t g_tBspUart4;
extern BspUart_t g_tBspUart5;
extern BspUart_t g_tBspUsart2;
extern BspUart_t g_tBspUsart3;
extern BspUart_t g_tBspUsart6;

extern struct CAN_RESDATA can1resdata;
extern struct CAN_RESDATA can2resdata;

extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

//extern ADC_HandleTypeDef ADC_Handle;
extern unsigned char auto_temperature_control(float gettemp_1 , float gettemp_2 , float gettemp_3);

extern volatile unsigned short ADC_ConvertedValue[RHEOSTAT_NOFCHANEL];

extern unsigned char checksum(unsigned char *data, int len);

extern unsigned char Bsp_initialize(void);

extern void Reboot(void);

//extern unsigned char Get_PC_Data(unsigned char *GetBuff , unsigned char GetBLen);

extern unsigned char Get_RK3588_Data(unsigned char *GetBuff , unsigned char GetBLen);

extern void Fan_Setup_Spped(unsigned char sp);

extern void MX_TIM4_Init(void);


enum CAN_Prescaler{
	CAN_PRE_1=1,
	CAN_PRE_2,
	CAN_PRE_3,
	CAN_PRE_4,
	CAN_PRE_5,
	CAN_PRE_6,
	CAN_PRE_7,
	CAN_PRE_8,
	CAN_PRE_9,
	CAN_PRE_10
};
enum num{
	zero = 0
};
enum LEDDEVICE{
	ONBOARD_LED_1 = 1,
	ONBOARD_LED_2 = 2,
	EXTERNAL_LED_1 = 3,
	EXTERNAL_LED_2 = 4,
	EXTERNAL_LED_3 = 5
	
};

enum GPIODEVICE{
	INTERNALFAN = 1,
	HEATING = 2,
	EXTERNALFAN = 3,
	CRYOGEN = 4,
	BATTERY = 6, //
	RK3588POWER = 7
};

enum BIG_FANGesar
{
	bigpercent_20 = 5500,
	bigpercent_40 = 6600,
	bigpercent_60 = 7700,
	bigpercent_80 = 8800,
	bigpercent_100 = 9900
};

enum SM_FANGesar
{
	smpercent_16 = 2000,
	smpercent_32 = 3200,
	smpercent_48 = 4400,
	smpercent_64 = 5600,
	smpercent_80 = 6800,
	smpercent_96 = 8000
};


#endif /* */