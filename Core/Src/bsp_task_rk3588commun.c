#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: AppTaskRK3588_Send
*	功能说明: RK3588数据发送任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
void AppTaskRK3588_Send(void *argument)
{
	const uint16_t usFrequency = 100;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	uint32_t Flag = 0;	
	
	/*用于标记重启指令*/
	unsigned int rk3588readyFlag = 0;
	
	/*应答指令*/
	unsigned char answdata[]   = {0x23 , 0x06 , 0x01 , 0x01 , 0x00 , 0x00};
	/*紧急断电指令*/
	unsigned char closurepow[] = {0x22 , 0x06 , 0x01 , 0x43 , 0x4C , 0x00};
	/*普通上报的数据*/
	unsigned char normal[] = {0x25 , 0x0D , 0x01 , 0x00 , 0x60 , 0x12 , 0x34 , 0x56 , 0x11 , 0x22 , 0x33 , 0x44 ,0x66};
	/*GPS数据*/
	unsigned char gpsdata[] = {0x21 , 0x11 , 0x01 , //枕头 ， 总长 ， 帧序号
							   0x00 , //维度类别
							   0x60 , 0x00 , 0x12 , 0x34 , 0x56 , 0x00,//经度
							   0x11 , 0x00 , 0x22 , 0x33 , 0x44 , 0x00,//维度
							   0x66};//校验
	
	unsigned char loop = 0;	

	/*重启指令消息队列数量*/
	unsigned char msg_rk3588rebottimr_Number = 0;
	/*三个温度传感器消息队列数量*/
	unsigned char msg_DS18B20_1_Number = 0;
	unsigned char msg_DS18B20_2_Number = 0;
	unsigned char msg_LM75B_Number = 0;
	/*GPS消息队列数量*/
	unsigned char msg_GPS_Number = 0;
	/*风扇机散热器消息队列*/
	unsigned char msg_calfan_Number = 0;
	
	//PcSetNetwork pcsetnet = {0};
	//PcSetTimer pcsettim = {0};
	//static unsigned char msg_PcSetNetwork_Number = 0;
	//static unsigned char msg_PcSetTime_Number = 0;	
	
	/*用于存储供电模式*/
	unsigned int Flag_PowMode = 0;
	/*三个温度传感器数据*/
	DS18B20_DATA ds18b20_in = {0.00};
	DS18B20_DATA ds18b20_out = {0.00};
	LM75B_DATA lm75b_in = {0.00};
	/*分包计数器*/
	static unsigned char NumLoop = 0;
	/*GPS数据结构体*/
	GPS_T my_tGPS = {0};
	/*风扇数据结构体*/
	CALFAN_Rising calfan_risi = {0};
	/*重启结构体*/
	REBOTTIME rebottime = {0};
	
	while(1)
	{
		Flag 												= osEventFlagsGet(event_RK3588Sinal_ID);		
		Flag_PowMode 								= osEventFlagsGet(event_PowMode_ID);	
		rk3588readyFlag 						= osEventFlagsGet(event_RK3588Ready_ID);	
		
		msg_rk3588rebottimr_Number 	= osMessageQueueGetCount(msgQueue_RK3588RebootTime_Data);	
		msg_DS18B20_1_Number 		 		= osMessageQueueGetCount(msgQueue_Ds18b20_1_Data);	
		msg_DS18B20_2_Number 		 		= osMessageQueueGetCount(msgQueue_Ds18b20_2_Data);	
		msg_LM75B_Number 			 			= osMessageQueueGetCount(msgQueue_LM75B_Data);	
		msg_GPS_Number			  	 		= osMessageQueueGetCount(msgQueue_GPS_Data);	
		msg_calfan_Number 					= osMessageQueueGetCount(msgQueue_CALFAN_Rising_Data);
		
		if(NumLoop <= 10)	
		{
			NumLoop++;
		}else
		{
			NumLoop = 0;
		}		
		if(Flag != 0)
		{
			switch(Flag)
			{
				//应答指令：断电
				case 0x01:
					*(answdata + 2) = loop++;
					*(answdata + 3) = 0x02;
					*(answdata + 4) = 0x00;
					*(answdata + 5) = checksum(&answdata[0], sizeof(answdata) - 1);
					BSP_UART_Transmit_IT(&huart5 , &answdata[0] , sizeof(answdata));
				
//					/*稍等片刻*/
//					unsigned int place = 5000;
//					place += usFrequency;    /* 相对延迟 */                      
//					osDelayUntil(place);	
	
//					/* 置位关机指令ID 关机开始 为了区分断电和重启，他们的关机标志位必须不一样*/
//					osEventFlagsSet(event_RK3588ShutDown_ID , 0x01U);
//					/* 置位开机指令ID 保险期间开机结束*/
//					osEventFlagsSet(event_RK3588StartUp_ID  , 0x00U);
//					/*2023.5.21应栾的要求，由于动作太快，所以将这里面的应答关机模式关闭	理想很丰满现实很骨感啊*/
//					/* RK3588关机任务 */
//					ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
				
					if(osEventFlagsClear(event_RK3588Sinal_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}	
				break;
						
				//应答指令：重启		
				case 0x03:
					if(msg_rk3588rebottimr_Number > 0)
					{
						os_Status = osMessageQueueGet(msgQueue_RK3588RebootTime_Data,/* 消息队列的名字 */
													  &rebottime,    				 /* 存放消息的内存 */
													  NULL,              			 /* 取出消息的优先级 */
													  osWaitForever  				 /* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
						
						*(answdata + 2) = loop++;
						*(answdata + 3) = 0x01;
						*(answdata + 4) = rebottime.reboottime;
						*(answdata + 5) = checksum(&answdata[0], sizeof(answdata) - 1);
						BSP_UART_Transmit_IT(&huart5 , &answdata[0] , sizeof(answdata));
						
						//入队
						os_Status = osMessageQueuePut(msgQueue_SetRk3588RebootTime_Data,/* 消息队列的名字 */
													  &rebottime,    				 /* 存放消息的内存 */
													  NULL,              			 /* 取出消息的优先级 */
													  osWaitForever  				 /* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
						
						if(rebottime.reboottime == 0xff)//立即重启
						{
							#if DEBUG_MODE == 1  
								printf("rebottime.reboottime = %d \r\n" , rebottime.reboottime);
							#endif	
							/* 置位关机指令ID 关机开始 为了区分断电和重启，他们的关机标志位必须不一样*/
							osEventFlagsSet(event_RK3588ShutDown_ID , 0x02U);				
							/* 置位开机指令ID 保险期间开机结束*/
							osEventFlagsSet(event_RK3588StartUp_ID  , 0x02U);					
							/* RK3588重启任务 */
							ThreadIdTaskRK3588_Reboot = osThreadNew(AppTaskRK3588_Reboot, NULL, &ThreadRK3588_Reboot_Attr);
						}
						else
						{
							//延时重启
							/* 置位关机指令ID 关机开始 为了区分断电和重启，他们的关机标志位必须不一样*/
							osEventFlagsSet(event_RK3588ShutDown_ID , 0x02U);
							/* 置位开机指令ID 保险期间开机结束 区别与*/
							osEventFlagsSet(event_RK3588StartUp_ID  , 0x01U);
							/* RK3588重启任务 */
							ThreadIdTaskRK3588_Reboot = osThreadNew(AppTaskRK3588_Reboot, NULL, &ThreadRK3588_Reboot_Attr);
						}
					}
					if(osEventFlagsClear(event_RK3588Sinal_ID , 0x03U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}	
				break;
						
				//紧急断电指令,电源断开后自动进入
				case 0x05:
					if(rk3588readyFlag == 0)//证明还没收到过主板准备完成的指令
					{
						*(closurepow + 2) = loop++;
						*(closurepow + 5) = checksum(&closurepow[0], sizeof(closurepow) - 1);
						BSP_UART_Transmit_IT(&huart5 , &closurepow[0] , sizeof(closurepow));
					}
					else
					{			
					}
					if(osEventFlagsClear(event_RK3588Sinal_ID , 0x05U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource))
					{}
					break;
						
				default:
					break;
			}
		}
		else
		{
			//正常指令，2023.5.20日分析认为正常指令即便丢给RK3588也没用,所以将正常数据通过RS422串口爆出
			//但GPS信息还需要给RK3588
			if(NumLoop == 5)/*普通数据*/
			{
				*(normal + 2) = loop++;
				/* 1.电源状态 */
				if(Flag_PowMode == 0x01){
					*(normal + 3) = 0x01;//电池模式
					if(osEventFlagsClear(event_PowMode_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource))
					{
					}
				}
				else
				{
					*(normal + 3) = 0x00;//电源模式
				}
				/* 2.电池电量 */
				float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL]={0}; 
				/* 电池电压 */
				ADC_ConvertedValueLocal[0] =((float) ADC_ConvertedValue[0] / 4096 * (float)3.3) *5;	
				ADC_ConvertedValueLocal[0] = ADC_ConvertedValueLocal[0] * 10;
				*(normal + 4) = (unsigned char)ADC_ConvertedValueLocal[0];
				
				/* 3.内部散热温度1 */
				if(msg_DS18B20_1_Number > 0)
				{
					unsigned char temp_ds18b20_1 = 0;
					os_Status = osMessageQueueGet(msgQueue_Ds18b20_1_Data,/* 消息队列的名字 */
												  &ds18b20_in,    		  /* 存放消息的内存 */
												  NULL,              	  /* 取出消息的优先级 */
												  osWaitForever  	      /* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
					temp_ds18b20_1 = (unsigned char)ds18b20_in.get_temp;
					printf("ds18b20_in.get_temp = %f \n" , ds18b20_in.get_temp);
					*(normal + 5) = temp_ds18b20_1 + 0x28;
				}
//				else
//				{
//					*(normal + 5) = 0x00;
//				}

				/* 4.内部散热温度2 */
				if(msg_DS18B20_2_Number > 0)
				{
					unsigned char temp_ds18b20_2 = 0;
					os_Status = osMessageQueueGet(msgQueue_Ds18b20_2_Data,/* 消息队列的名字 */
												  &ds18b20_out,    		  /* 存放消息的内存 */
												  NULL,              	  /* 取出消息的优先级 */
												  osWaitForever  	      /* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
					temp_ds18b20_2 = (unsigned char)ds18b20_out.get_temp;
					printf("ds18b20_in.get_temp = %f \n" , ds18b20_out.get_temp);
					*(normal + 6) =temp_ds18b20_2 + 0x28;
				}
//				else
//				{
//					*(normal + 6) = 0x00;
//				}
			
				/* 5.外部散热温度1 */
				if(msg_LM75B_Number > 0)
				{
					unsigned char temp_lm75b = 0;
					os_Status = osMessageQueueGet(msgQueue_LM75B_Data,    /* 消息队列的名字 */
												  &lm75b_in,    		  /* 存放消息的内存 */
												  NULL,              	  /* 取出消息的优先级 */
												  osWaitForever  	      /* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);
					temp_lm75b = (unsigned char)lm75b_in.get_temp;
					*(normal + 7) = temp_lm75b + 0x28;
				}
//				else
//				{
//					*(normal + 7) = 0x00;
//				}
		
				if(msg_calfan_Number > 0)
				{
					//入队					
					os_Status = osMessageQueueGet(msgQueue_CALFAN_Rising_Data,/* 消息队列的名字 */
											      &calfan_risi,    		  		/* 存放消息的内存 */
											      NULL,              	  				/* 取出消息的优先级 */
											      osWaitForever  	      				/* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);	
					
					if(calfan_risi.big_fan_risingcount >= 255)/* 6.内部风扇负载1 ， 这些风扇的转速反馈数据1Byte装不下，所以展示这样表示*/
					{
						*(normal + 8) = 0xff;
					}
					else
					{
						*(normal + 8) = calfan_risi.big_fan_risingcount;
					}
					
					if(calfan_risi.arm_fan_risingcount >= 255)	/* 7.外部风扇负载2 */
					{
						*(normal + 9) = 0xff;
					}
					else
					{
						*(normal + 9) = calfan_risi.arm_fan_risingcount;
					}
					
					if(calfan_risi.rk_fan_risingcount >= 255)/* 8.TEC功率 --由于没有TEC功率之说，所以修改为 -外部风扇负载2*/
					{
						*(normal + 10) = 0xff;
					}
					else
					{
						*(normal + 10) = calfan_risi.rk_fan_risingcount;			
					}
				}				
				/* 9.加热片功率*/
				*(normal + 11) = 0x00;
				*(normal + 12) = checksum(&normal[0], sizeof(normal) - 1);
				#if TURNON_NO_Normal == 1
					//2023年 7月 7日 DONGZHE 修改为	husart5
					BSP_UART_Transmit_IT(&huart5 , &normal[0] , sizeof(normal));
				#endif					
			}
			else if(NumLoop == 10)
			{
				/*GPS数据*/
				if(msg_GPS_Number > 0) //消息队列中存在数据
				{
					os_Status = osMessageQueueGet(msgQueue_GPS_Data,/* 消息队列的名字 */
												&my_tGPS,    		  						/* 存放消息的内存 */
												NULL,              	  						/* 取出消息的优先级 */
												osWaitForever  	      						/* 超时时间，在中断函数里调用osMessageQueueGet时，此参数必须是NULL */);				
					
					float LongtitudeNumber = 0;
					float LatitudeNumber = 0;
							
					int LongtitudeHour = 0;
					int LongtitudeMinute = 0;
					int LongtitudeSecond = 0;
					//float intLongtitudeSecond = 0; 
					
					int LatitudeMinute = 0;
					int LatitudeHour = 0;
					int LatitudeSecond = 0;
					
					LongtitudeNumber = my_tGPS.WeiDu_Du;  	//经度度
					double testdemo = 0.00 , int_part = 0.00 , frac_part = 0.00;
					frac_part = modf(my_tGPS.WeiDu_Fen * 0.0001 , &int_part);		
					LongtitudeMinute = int_part;						//经度分
					float temp = (frac_part * 60);					//经度秒
					//LongtitudeSecond = (int)(100.0 * temp + 0.5) / 100.0;
					LongtitudeSecond = (int)(temp * 100);
					
					#if DEBUG_MODE == 1  
						//printf("Lde = %0.2f , LMinute = %d , LSecond = %d\r\n" , LongtitudeNumber , LongtitudeMinute , LongtitudeSecond);
					#endif	
				
					LatitudeNumber = my_tGPS.JingDu_Du;		//维度度
					testdemo = 0.00 , int_part = 0.00 , frac_part = 0.00;
					frac_part = modf(my_tGPS.JingDu_Fen * 0.0001 , &int_part);	
					LatitudeMinute = int_part;							//维度分
					temp = (frac_part * 60);									//维度秒
					//LatitudeSecond = (int)(100.0 * temp + 0.5) / 100.0;
					LatitudeSecond = (int)(temp * 100);

					#if DEBUG_MODE == 1  
						//printf("Ade = %0.2f , AMinute = %d , ASecond = %d\r\n" , LongtitudeNumber , LongtitudeMinute , LongtitudeSecond);
					#endif

					*(gpsdata + 2) = loop++;
					/*坐标类型判断*/
					if(my_tGPS.NS == 'S')//南纬
					{
						*(gpsdata + 3) = gpsdata[3] | 0X02;
					}
					else if(my_tGPS.NS == 'N')//北纬
					{
						*(gpsdata + 3) = gpsdata[3] | 0X01;
					}	
					if(g_tGPS.EW == 'E')//东经
					{
						*(gpsdata + 3) = gpsdata[3] | 0X10;
					}else if(g_tGPS.EW == 'W')//西经
					{
						*(gpsdata + 3) = gpsdata[3] | 0X20;
					}
					
					*(gpsdata + 4) = my_tGPS.JingDu_Du >> 8;		//高位经度
					*(gpsdata + 5) = my_tGPS.JingDu_Du;					//低位经度
					*(gpsdata + 6) = LatitudeMinute; 						//经度分					
					*(gpsdata + 7) = LatitudeSecond >> 8; 			//高位经度秒
					*(gpsdata + 8) = LatitudeSecond;						//低位经度秒				
					*(gpsdata + 9) = 0;
					
					*(gpsdata + 10) = my_tGPS.WeiDu_Du >> 8;		//高位维度
					*(gpsdata + 11) = my_tGPS.WeiDu_Du;				//低位维度
					*(gpsdata + 12) = LongtitudeMinute;				//维度分	
					*(gpsdata + 13) = LongtitudeSecond >> 8;	//高位维度秒
					*(gpsdata + 14) = LongtitudeSecond;				//低位维度秒
					*(gpsdata + 15) = 0;
					
					*(gpsdata + 16) = checksum(&gpsdata[0], sizeof(gpsdata) - 1);
					#if TURNON_NO_Gps == 1
						/*2023 7 7 董哲关闭这个*/
						BSP_UART_Transmit_IT(&huart5 , &gpsdata[0] , sizeof(gpsdata));
					#endif	
				}
				else//不存在数据
				{
					*(gpsdata + 2) = loop++;
					
					*(gpsdata + 3) = zero;				//维度类型
					
					*(gpsdata + 4) = zero;				//高位经度
					*(gpsdata + 5) = zero;				//低位经度
					*(gpsdata + 6) = zero; 				//经度分					
					*(gpsdata + 7) = zero; 				//高位经度秒
					*(gpsdata + 8) = zero;				//高位经度毫秒				
					*(gpsdata + 9) = zero;				//低位经度毫秒
					
					*(gpsdata + 10) = zero;				//低位维度
					*(gpsdata + 11) = zero;				//维度分	
					*(gpsdata + 12) = zero;				//高位维度秒
					*(gpsdata + 13) = zero;				//高位经度毫秒		
					*(gpsdata + 15) = zero;				//低位经度毫秒	
					
					*(gpsdata + 16) = checksum(&gpsdata[0], sizeof(gpsdata) - 1);
					#if TURNON_NO_Gps == 1
						BSP_UART_Transmit_IT(&huart5 , &gpsdata[0] , sizeof(gpsdata));
					#endif	
				}				
			}			
		}
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskRK3588_Receive
*	功能说明: 接收RK3588任务
*	形    参: 无
*	返 回 值: 无
* 	优 先 级:   
*********************************************************************************************************
*/
void AppTaskRK3588_Receive(void *argument)
{
	const uint16_t usFrequency = 50;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	unsigned int Flag = 0;		
	while(1)
	{
		unsigned char datatype = 0;
		Flag = osEventFlagsGet(event_uart5_ID);
		if(Flag != 0 )
		{
			if(osEventFlagsClear(event_uart5_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
			
			/*对接收的指令分包*/
			datatype = Get_RK3588_Data(g_uart5_rx_buf , g_tBspUart5.rxLen);		
			switch(datatype)
			{
				case 0x01://系统正常指令
					#if DEBUG_MODE == 1  
						printf("generally data \r\n");
					#endif
					break;
			
				case 0x10://主板温度高
					break;
				
				case 0x11://主板温度低
					break;
				
				case 0x12://摄像头初始化异常
					break;
				
				case 0x13://摄像头通讯异常
					break;
				
				case 0x14://图像读取失败
					break;
				
				case 0x15://文件写入失败
					break;
				
				case 0x16://推流输出异常
					break;
				
				case 0x17://音频设备异常
					break;
				
				case 0x18://音频文件存储失败
					break;	
				
				case 0x19://网络校准失败
					break;
				
				case 0x03://重启指令
						#if DEBUG_MODE == 1  
							printf("reboot data \r\n");
						#endif
					osEventFlagsSet(event_RK3588Sinal_ID , 0x03U);
					break;
				
				case 0x04://关机指令
						#if DEBUG_MODE == 1  
							printf("closure data \r\n");	
						#endif
					osEventFlagsSet(event_RK3588Ready_ID , 0x01U);
					osEventFlagsSet(event_RK3588Sinal_ID , 0x01U);
					break;
				
				default:
					#if DEBUG_MODE == 1  
						printf("outher data \r\n");	
					#endif	
					break;
			}
		}
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskRK3588_Send
*	功能说明: 
*	形    参: 无
*	返 回 值: ucreturn ->
				正常数据
					0x01 -- 正常数据,系统正常
					0x10 -- 主板温度高
					0x11 -- 主板温度低
					0x12 -- 摄像头初始化异常
					0x13 -- 摄像头通讯异常
					0x14 -- 图像读取失败
					0x15 -- 文件写入失败
					0x16 -- 推流输出异常
					0x17 -- 音频设备异常
					0x18 -- 音频文件存储失败
					0x19 -- 网络校准失败
					
					0x1F -- 数据是正常数据，但校验和不对
					0x1E -- 数据是正常数据，但帧长度不对
				
//				应答指令
//				    0x02 -- 数据是应答指令，对时应答
//				    0x20 -- 数据是应答指令，返回设置的IP数据
//				    0x21 -- 数据是应答指令，放回查询的IP数据
//				
//				    0x2F -- 数据是应答指令，但帧长度不对
//				    0X2E -- 数据是应答指令，但校验和不对
//				    0x2D -- 未知协议
				 
				重启指令
				   0x03 -- 重启指令
				   
				   0x3F -- 重启指令但帧产长不对
				   0x3E -- 重启指令但校验不对
				   
				关机指令
				   0x04 -- 关机指令
				   
				   0x4F -- 校验错误
				   0x4E -- 数据错误
				   0x4D -- 帧长错误
				位置
					0xF0
*   优 先 级:
*********************************************************************************************************
*/
unsigned char Get_RK3588_Data(unsigned char *GetBuff , unsigned char GetBLen)
{
	unsigned char ucreturn = 0;		
	unsigned char getbuff[25] = {0};
	memcpy(getbuff , GetBuff , GetBLen);
	REBOTTIME rebottime = {0};
	osStatus_t os_Status;
	
	//PcSetNetwork pcgetnet = {0};
	PcSetTimer pcgettim   = {0};
	
	switch(*(getbuff + 0))
	{
		case 0xAA://正常指令 ,返回0x10-0x1f
			if(*(getbuff + 1) == GetBLen)
			{
				unsigned char getcheck = 0;
				getcheck = checksum(getbuff, GetBLen - 1);
				if(getcheck == *(getbuff + 7))
				{
					switch( *(getbuff + 4) )
					{
						case 0x00:
              ucreturn = 0x01;
							break;
						
						case 0x01:
              ucreturn = 0x10;
							break;
						
						case 0x02:
              ucreturn = 0x11;
							break;
						
						case 0x03:
              ucreturn = 0x12;
							break;
						
						case 0x04:
              ucreturn = 0x13;
							break;
						
						case 0x05:
              ucreturn = 0x14;
							break;
						
						case 0x06:
              ucreturn = 0x15;
							break;
						
						case 0x07:
              ucreturn = 0x16;
							break;
						
						case 0x08:
              ucreturn = 0x17;
							break;

						case 0x09:
							ucreturn = 0x18;
							break;

						case 0x0A:
              ucreturn = 0x19;
							break;
						
						default:
							break;
					}	
				}
				else
				{
					ucreturn = 0x1F;
				}
			}
			else
			{
				ucreturn = 0x1E;
			}
			break;

		case 0xA2://重启指令,返回
			if(*(getbuff + 1) == GetBLen)
			{
				unsigned char getcheck = 0;
				getcheck = checksum(getbuff, GetBLen - 1);
				if(getcheck == *(getbuff + 5))
				{
					rebottime.reboottime = *(getbuff + 3);
					rebottime.reboottimecheck = *(getbuff + 4);
					//队列入队
					os_Status = osMessageQueuePut(msgQueue_RK3588RebootTime_Data, 	/* 消息队列的名字 */
												  &rebottime, 						/* 放入消息队列的变量（消息） */
												  NULL,      						/* 放入消息的优先级 */
												  NULL      	 					/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
												  );	
					
					ucreturn = 0x03;
				}
				else
				{
						ucreturn = 0x3F;
				}
			}
			else
			{
				ucreturn = 0x3E;
			}
			break;

		case 0xA3://关机指令,返回
			if(*(getbuff + 1) == GetBLen)
			{	
				if((*(getbuff + 3) ==  0x6F) && (*(getbuff + 4) == 0X66) && ( *(getbuff + 5) == 0x66))
				{
					unsigned char getcheck = 0;
					getcheck = checksum(getbuff, GetBLen - 1);
					if(getcheck == *(getbuff + 6))
					{
						ucreturn = 0x04;
					}	
					else
					{
						ucreturn = 0x4F;
					}

				}
				else{
					ucreturn = 0x4E;
				}
			}
			else
			{
				ucreturn = 0x4D;
			}
			break;

		default:
			ucreturn = 0xF0;
			break;
	}	
	return ucreturn;
}

