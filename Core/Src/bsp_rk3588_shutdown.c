#include "bsp.h"

/*2023.5.21收到栾的要求收到关机指令并且关机后自杀*/
/*
*********************************************************************************************************
*	函 数 名: AppTaskRK3588_Closure
*	功能说明: 关机指令，同时也是开机指令
*	形    参: 无
*	返 回 值: 无
* 	优 先 级:   
*********************************************************************************************************
*/
void AppTaskRK3588_Closure(void *argument)
{
	const uint16_t usFrequency = 2500;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	unsigned char loop = 0;
		
	unsigned int FlagShutDown = 0;
	unsigned int FlagStartUp = 0;
	
	while(1)
	{
		FlagShutDown =  osEventFlagsGet(event_RK3588ShutDown_ID);
		FlagStartUp  =  osEventFlagsGet(event_RK3588StartUp_ID);
		
		switch(loop)
		{
			case 0:
				bsp_GpioOff(RK3588POWER);
				loop = 1;
				break;
			
			case 1:
				bsp_GpioOff(RK3588POWER);
				loop = 2;
				break;
			
			case 2:
				bsp_GpioOff(RK3588POWER);
				loop = 3;
				break;
			
			case 3:
				bsp_GpioOn(RK3588POWER);
				loop = 1;
				switch(FlagShutDown)
				{
					case 0x01://关机
						if(osEventFlagsClear(event_RK3588ShutDown_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						/*	2023.5.21 应栾要求关机后加入自杀功能，其实就是关闭电池供电端口	*/
						bsp_GpioOff(BATTERY);
						//bsp_GpioOff(BATTERY);
						break;
					
					case 0x02://重启 - 第一遍关机
						if(osEventFlagsClear(event_RK3588ShutDown_ID , 0x02U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						osEventFlagsSet(event_RK3588ShutDown_ID , 0x03U);
						break;
							
					case 0x03://重启 - 第二遍关机
						if(osEventFlagsClear(event_RK3588ShutDown_ID , 0x03U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						break;
					
					default:
						break;
				}
				osThreadExit();
				
				break;
			
			default:
				break;
		}
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}



void timer_rk3588reboot_App(void *argument)
{
	#if DEBUG_MODE == 1  
		printf("rebottime.reboottime\r\n");	
	#endif	
	osEventFlagsSet(event_RK3588RebootCountdown_ID , 0x01U);	
}


/*
*********************************************************************************************************
*	函 数 名: Reboot
*	功能说明: 重启指令，其实就是关2次机
*	形    参: 无
*	返 回 值: 无
* 	优 先 级:  
*	重启分俩种，一种是延时重启需要将重启的消息队列取出，另一种是即时重启

*********************************************************************************************************
*/
void AppTaskRK3588_Reboot(void *argument)
{
	const uint16_t usFrequency = 3000;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	unsigned char loop = 0;
	unsigned char loop_time = 0;
	
	REBOTTIME rebottime = {0};
	unsigned char msg_rebottime_Number = 0; 
	
	unsigned int FlagShutDown = 0;
	unsigned int FlagStartUp = 0;
	unsigned int FlagRebootCountdown = 0;
	while(1)
	{
		FlagShutDown 		 =  osEventFlagsGet(event_RK3588ShutDown_ID);		
		FlagStartUp 		 =  osEventFlagsGet(event_RK3588StartUp_ID);			
		FlagRebootCountdown  =  osEventFlagsGet(event_RK3588RebootCountdown_ID);	
		msg_rebottime_Number = osMessageQueueGetCount(msgQueue_SetRk3588RebootTime_Data);
		
		if(msg_rebottime_Number > 0 )
		{
			os_Status = osMessageQueueGet(msgQueue_SetRk3588RebootTime_Data,/* 消息队列的名字 */
						  &rebottime,    				 	/* 存放消息的内存 */
						  NULL,              			 /* 取出消息的优先级 */
						  osWaitForever  				 	/* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);	
		}			
		/*校验计算*/
		if(rebottime.reboottime == (0xff - rebottime.reboottimecheck))
		{
			/* 1 第一次是延时重启 */
			if(FlagStartUp == 0x01)
			{
				if(loop_time == 0)//第一遍轮询
				{
					loop_time++;	
					if(rebottime.reboottime <= 200)
					{
						unsigned int timer = rebottime.reboottime * 1000;
						osTimerStart(timerID_rk3588reboot , timer);	/* 启动一个软件定时器用于计算计划重启时间 */
						/*第一遍关机指令*/
						ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
					}
				}
				else if(loop_time == 1)//第二遍轮询
				{
					if(FlagRebootCountdown == 1)//判断下重启倒计时是否置位
					{
						if(FlagShutDown == 0x03)//已经执行过一遍关机，现在是开机指令
						{
							if(osEventFlagsClear(event_RK3588RebootCountdown_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
							if(osEventFlagsClear(event_RK3588StartUp_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
							/*第二遍关机指令*/
							loop = 0;
							ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
							osThreadExit();
						}
					}

				}
				
			}
			/* 2 第一次是即时重启*/	
			else if(FlagStartUp == 0x02)
			{
				if(loop == 0)//第一遍轮询
				{
					/* 1.关机 */
					loop++;
					/* 第一遍RK3588关机任务 */
					ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
				}
				else if(loop == 1)//第二遍轮询
				{
					loop = 0;
					if(FlagShutDown == 0x03)//已经执行过一遍关机，现在是开机指令
					{
						/* 2.开机 */
						/* 第二遍RK3588关机任务 */
						ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
						if(osEventFlagsClear(event_RK3588StartUp_ID , 0x02U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						osThreadExit();
					}
				}
			}
			
		}
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}	
}
