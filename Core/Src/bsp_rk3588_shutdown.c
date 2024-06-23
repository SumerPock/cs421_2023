#include "bsp.h"

/*2023.5.21�յ����Ҫ���յ��ػ�ָ��ҹػ�����ɱ*/
/*
*********************************************************************************************************
*	�� �� ��: AppTaskRK3588_Closure
*	����˵��: �ػ�ָ�ͬʱҲ�ǿ���ָ��
*	��    ��: ��
*	�� �� ֵ: ��
* 	�� �� ��:   
*********************************************************************************************************
*/
void AppTaskRK3588_Closure(void *argument)
{
	const uint16_t usFrequency = 2500;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
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
					case 0x01://�ػ�
						if(osEventFlagsClear(event_RK3588ShutDown_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						/*	2023.5.21 Ӧ��Ҫ��ػ��������ɱ���ܣ���ʵ���ǹرյ�ع���˿�	*/
						bsp_GpioOff(BATTERY);
						//bsp_GpioOff(BATTERY);
						break;
					
					case 0x02://���� - ��һ��ػ�
						if(osEventFlagsClear(event_RK3588ShutDown_ID , 0x02U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						osEventFlagsSet(event_RK3588ShutDown_ID , 0x03U);
						break;
							
					case 0x03://���� - �ڶ���ػ�
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
		tick += usFrequency;    /* ����ӳ� */                      
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
*	�� �� ��: Reboot
*	����˵��: ����ָ���ʵ���ǹ�2�λ�
*	��    ��: ��
*	�� �� ֵ: ��
* 	�� �� ��:  
*	���������֣�һ������ʱ������Ҫ����������Ϣ����ȡ������һ���Ǽ�ʱ����

*********************************************************************************************************
*/
void AppTaskRK3588_Reboot(void *argument)
{
	const uint16_t usFrequency = 3000;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
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
			os_Status = osMessageQueueGet(msgQueue_SetRk3588RebootTime_Data,/* ��Ϣ���е����� */
						  &rebottime,    				 	/* �����Ϣ���ڴ� */
						  NULL,              			 /* ȡ����Ϣ�����ȼ� */
						  osWaitForever  				 	/* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);	
		}			
		/*У�����*/
		if(rebottime.reboottime == (0xff - rebottime.reboottimecheck))
		{
			/* 1 ��һ������ʱ���� */
			if(FlagStartUp == 0x01)
			{
				if(loop_time == 0)//��һ����ѯ
				{
					loop_time++;	
					if(rebottime.reboottime <= 200)
					{
						unsigned int timer = rebottime.reboottime * 1000;
						osTimerStart(timerID_rk3588reboot , timer);	/* ����һ�������ʱ�����ڼ���ƻ�����ʱ�� */
						/*��һ��ػ�ָ��*/
						ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
					}
				}
				else if(loop_time == 1)//�ڶ�����ѯ
				{
					if(FlagRebootCountdown == 1)//�ж�����������ʱ�Ƿ���λ
					{
						if(FlagShutDown == 0x03)//�Ѿ�ִ�й�һ��ػ��������ǿ���ָ��
						{
							if(osEventFlagsClear(event_RK3588RebootCountdown_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
							if(osEventFlagsClear(event_RK3588StartUp_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
							/*�ڶ���ػ�ָ��*/
							loop = 0;
							ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
							osThreadExit();
						}
					}

				}
				
			}
			/* 2 ��һ���Ǽ�ʱ����*/	
			else if(FlagStartUp == 0x02)
			{
				if(loop == 0)//��һ����ѯ
				{
					/* 1.�ػ� */
					loop++;
					/* ��һ��RK3588�ػ����� */
					ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
				}
				else if(loop == 1)//�ڶ�����ѯ
				{
					loop = 0;
					if(FlagShutDown == 0x03)//�Ѿ�ִ�й�һ��ػ��������ǿ���ָ��
					{
						/* 2.���� */
						/* �ڶ���RK3588�ػ����� */
						ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
						if(osEventFlagsClear(event_RK3588StartUp_ID , 0x02U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
						osThreadExit();
					}
				}
			}
			
		}
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}	
}
