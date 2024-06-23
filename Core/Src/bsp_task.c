
#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: AppTaskGPSSetup
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
void AppTaskGPSSetUp(void *argument)
{
	const uint16_t usFrequency = 1600;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	unsigned int Flag = 0;	
	while(1)
	{	
		//GNGGA
		unsigned char SucbufGNGGA[15] = {0x24 , 0x45 , 0x49 , 0x47 , 0x4E , 0x51 , 0x2C , 0x47 , 0x47 , 0x41 , 0x2A , 0x33 , 0x39 , 0x0D , 0x0A};
		unsigned char GucbufGNGGA[11] = {0xB5 , 0x62 , 0x06 , 0x01 , 0x03 , 0x00 , 0xF0 , 0x00 , 0x00 , 0xFA , 0x0F};
		//GNGLL
		unsigned char SucbufGNGLL[15] = {0x24 , 0x45 ,0x49 ,0x47 ,0x4E ,0x51 ,0x2C ,0x47 ,0x4C ,0x4C ,0x2A ,0x33 ,0x46 ,0x0D ,0x0A};
		unsigned char GucbufGNGLL[11] = {0xB5 , 0x62 ,0x06 ,0x01 ,0x03 ,0x00 ,0xF0 ,0x01 ,0x00 ,0xFB ,0x11};
		//GXGSA
		unsigned char SucbufGXGSA[15] = {0x24 ,0x45 ,0x49 ,0x47 ,0x4E ,0x51 ,0x2C ,0x47 ,0x53 ,0x41 ,0x2A ,0x32 ,0x44 ,0x0D ,0x0A};
		unsigned char GucbufGXGSA[11] = {0xB5 ,0x62 ,0x06 ,0x01 ,0x03 ,0x00 ,0xF0 ,0x02 ,0x00 ,0xFC ,0x13};
		//GPGSV
		unsigned char SucbufGPGSV[15] = {0x24 ,0x45 ,0x49 ,0x47 ,0x50 ,0x51 ,0x2C ,0x47 ,0x53 ,0x56 ,0x2A ,0x32 ,0x34 ,0x0D ,0x0A};
		unsigned char GucbufGPGSV[11] = {0xB5 ,0x62 ,0x06 ,0x01 ,0x03 ,0x00 ,0xF0 ,0x03 ,0x00 ,0xFD ,0x15};
		//GNVTG
		unsigned char SucbufGNVTG[15] = {0x24 ,0x45 ,0x49 ,0x47 ,0x4E ,0x51 ,0x2C ,0x56 ,0x54 ,0x47 ,0x2A ,0x33 ,0x44 ,0x0D ,0x0A};
		unsigned char GucbufGNVTG[11] = {0xB5 ,0x62 ,0x06 ,0x01 ,0x03 ,0x00 ,0xF0 ,0x05 ,0x00 ,0xFF ,0x19};
		//GNZDA
		unsigned char SucbufGNZDA[15] = {0x24 ,0x45 ,0x49 ,0x47 ,0x4E ,0x51 ,0x2C ,0x5A ,0x44 ,0x41 ,0x2A ,0x32 ,0x37 ,0x0D ,0x0A};
		unsigned char GucbufGNZDA[11] = {0xB5 ,0x62 ,0x06 ,0x01 ,0x03 ,0x00 ,0xF0 ,0x08 ,0x00 ,0x02 ,0x1F};	
		static unsigned char loop = 1;
		
		unsigned char i = 0;
		/*2023年 5月 20日 DONGZHE 修改将 BSP_UART_Transmit_IT 的最后一个参数 sizeof(SucbufGNGGA) - 1 修改为 sizeof(SucbufGNGGA) 因为 -1 是多余的*/
		for(i = 0 ; i < 3 ; i++)
		{
			switch(loop)
			{
				case 1:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGNGGA[0] , sizeof(SucbufGNGGA));
					loop = 2;
					break;
				case 2:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGNGGA[0] , sizeof(GucbufGNGGA));
					loop = 3;
					break;
				case 3:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGNGLL[0] , sizeof(SucbufGNGLL));
					loop = 4;
					break;
				case 4:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGNGLL[0] , sizeof(GucbufGNGLL));
					loop = 5;
					break;
				case 5:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGXGSA[0] , sizeof(SucbufGXGSA));
					loop = 6;
					break;
				case 6:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGXGSA[0] , sizeof(GucbufGXGSA));
					loop = 7;
					break;
				case 7:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGPGSV[0] , sizeof(SucbufGPGSV));
					loop = 8;
					break;
				case 8:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGPGSV[0] , sizeof(GucbufGPGSV));
					loop = 9;
					break;
				case 9:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGNVTG[0] , sizeof(SucbufGNVTG));
					loop = 10;
					break;
				case 10:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGNVTG[0] , sizeof(GucbufGNVTG));
					loop = 11;
					break;
				case 11:
					BSP_UART_Transmit_IT(&husart3 , &SucbufGNZDA[0] , sizeof(SucbufGNZDA));
					loop = 12;
					break;
				case 12:
					BSP_UART_Transmit_IT(&husart3 , &GucbufGNZDA[0] , sizeof(GucbufGNZDA));
					loop = 13;
					break;
				case 13:
					break;
				default:
					break;
			}	
		}
		/* GPS任务 */
		ThreadIdTaskGPS = osThreadNew(AppTaskGPS, NULL, &ThreadGPS_Attr);
		/*GSP消息队列*/
		msgQueue_GPS_Data = osMessageQueueNew(MQ_SIZE , sizeof(GPS_T) ,&GPS_Data_Attr);
		osThreadExit();
		
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGPS
*	功能说明: 自杀任务加入
*	形    参: 无
*	返 回 值: 无
* 优 先 级:   
*********************************************************************************************************
*/
void AppTaskGPS(void *argument)
{
	const uint16_t usFrequency = 500;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	uint32_t Flag = 0;	
	volatile unsigned int keil = 0;
	float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL] = {0}; 	
	while(1)
	{
		// ADC转换的电压值通过MDA方式传到SRAM 
		/* 24V电源电压 */
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1] / 4096 * (float)3.3;	
		if(keil >= (0x258 * 2))
		{
			bsp_GpioOff(BATTERY);
		}
		
		if(ADC_ConvertedValueLocal[1] > 2.0)/*电源模式 : 外置电源*/
		{
			keil = 0;
		}
		else/*电源模式 : 电池电源*/
		{
			keil++;
		}	
		
		Flag = osEventFlagsGet(event_usart3_ID);
		if(Flag)
		{
			unsigned char buffusart3[128] = "";
			memcpy(buffusart3 , g_usart3_rx_buf , 128);	
			gps_pro(&buffusart3[0] , sizeof(buffusart3));
			g_tBspUsart3.rxFlag = 0;
			memset(g_usart3_rx_buf, 0, sizeof(g_usart3_rx_buf));
			memset(buffusart3, 0, sizeof(buffusart3));
			if(osEventFlagsClear(event_usart3_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource))
			{		
			}				
		}			
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskDS18B20
*	功能说明: ds18b20和LM75B温度读取
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
void AppTaskDS18B20(void *argument)
{
	DS18B20_DATA ds18b20_in = {0.00};
	DS18B20_DATA ds18b20_out = {0.00};
	LM75B_DATA lm75b_in = {0.00};

	unsigned char g_rom_id[8] = {0};
	unsigned char g_rom_id1[8] = {0};	
	
	const uint16_t usFrequency = 1600;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
		
	/*测试温度转换函数 (根据 DS18B20.pdf page = 4 的表验证数值转换)*/
	unsigned short 	usReg[10] = {0x07D0 , 0x0550 , 0x0191 , 0x00A2 , 0x0008 , 0x0000 , 0xFFF8 , 0xFF5E , 0xFE6F , 0xFC90};
	unsigned char 	m = 0;
	unsigned short 	reg = 0;
	for (m = 0; m < 10; m++)
	{
		reg = usReg[m];
		#if DEBUG_MODE == 1  
				printf("reg = 0x%04X %6d --> %-4.04f C \n", (uint16_t)reg, reg, (float)reg / 16);
		#endif	
	}
	/* 读取第一个DS18B20 ROM ID */
	unsigned char DS18B20_Num1_Id[8] = {0};
	unsigned char DS18B20_Num2_Id[8] = {0};
	unsigned char ret = 0;
	unsigned char i = 0;
	ret = DS18B20_ReadID_Choose(DS18B20_Num1_Id , 0);
	if (ret == 0)
	{
		#if DEBUG_MODE == 1  
				printf("DS18B20 is no find \r\n");
		#endif		
		memset(g_rom_id, 0, 8);
	}
	else
	{	
		#if DEBUG_MODE == 1  
				printf("DS18B20 1 Ok, id = ");
		#endif		
		for (i = 0; i < 8; i++)
		{		
			#if DEBUG_MODE == 1  
					printf("%02X ", DS18B20_Num1_Id[i]);
			#endif
		}	
		#if DEBUG_MODE == 1  
				printf("\r\n");
		#endif			
		memcpy(g_rom_id, DS18B20_Num1_Id, 8);
	}
	/* 读取第二个DS18B20 ROM ID */
	ret = DS18B20_ReadID_Choose(DS18B20_Num2_Id , 1);
	if (ret == 0)
	{	
		#if DEBUG_MODE == 1  
				printf("DS18B20 is no find \r\n");
		#endif
		memset(g_rom_id1, 0, 8);
	}
	else
	{	
		#if DEBUG_MODE == 1  
				printf("DS18B20 2 Ok, id = ");
		#endif	
		for (i = 0; i < 8; i++)
		{	
			#if DEBUG_MODE == 1  
				printf("%02X ", DS18B20_Num2_Id[i]);
		#endif
		}
		#if DEBUG_MODE == 1  
				printf("\r\n");
		#endif
		memcpy(g_rom_id1, DS18B20_Num2_Id, 8);
	}	

	while(1)
	{
		unsigned short reg[2] = {0};
		reg[0] = DS18B20_ReadTempRegCRC_Choose(0);
		reg[1] = DS18B20_ReadTempRegCRC_Choose(1);
		
		/*DS18B20传感器一号*/
		if(reg[0] != 0x800)
		{
			#if DEBUG_MODE == 1  
				//printf("the ds18b20 one -> reg = 0x%04X %6d --> %-4.04f \n", (uint16_t)reg[0], reg[0], (float)reg[0] / 16);
			#endif
			ds18b20_in.get_temp = (float)(*(reg + 0)) / 16;
			ds18b20_in.get_temp=( (float)( (int)( (ds18b20_in.get_temp + 0.005) * 100 ) ) )/100;
			
			*(reg + 0) = 0;
			if(ds18b20_in.get_temp != 0)
			{
				/*启动消息队列*/
				os_Status = osMessageQueuePut(msgQueue_Ds18b20_1_Data, 	/* 消息队列的名字 */
											  &ds18b20_in, 				/* 放入消息队列的变量（消息） */
											  NULL,      				/* 放入消息的优先级 */
											  NULL      	 			/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
											  );	
			}
		}
		else
		{
			#if DEBUG_MODE == 1  
				printf("crc is error\r\n");
			#endif
		}

		/*DS18B20温度传感器二号*/
		if(reg[1] != 0x800)
		{
			#if DEBUG_MODE == 1  
				//printf("the ds18b20 two -> reg = 0x%04X %6d --> %-4.04f \n", (uint16_t)reg[1], reg[1], (float)reg[1] / 16);
			#endif
			ds18b20_out.get_temp = (float)(*(reg + 1)) / 16;
			ds18b20_out.get_temp=( (float)( (int)( (ds18b20_out.get_temp + 0.005) * 100 ) ) )/100;
			*(reg + 1) = 0;
			if(ds18b20_out.get_temp != 0)
			{
				/*启动消息队列*/
				os_Status = osMessageQueuePut(msgQueue_Ds18b20_2_Data,	/* 消息队列的名字 */
											  &ds18b20_out, 						/* 放入消息队列的变量（消息） */
											  NULL,      							/* 放入消息的优先级 */
											  NULL      	 						/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
											  );
			}
		}
		else
		{
			#if DEBUG_MODE == 1  
				printf("crc is error\r\n");
			#endif
		}		
		
		/*I2C温度传感器*/
		unsigned char sensortemp[2] = {0};
		unsigned char gettemp[2] = {0};	
		if(HAL_I2C_Mem_Read(&hi2c1 , 0X90 , 0x00, I2C_MEMADD_SIZE_8BIT, sensortemp, 2, 1000) != HAL_OK)
		{
			#if DEBUG_MODE == 1  
				printf("the I2C find error\r\n");
			#endif
		}
		else
		{
			if(LM75BTemp(sensortemp , sizeof(sensortemp) , gettemp , sizeof(gettemp)))
			{
				lm75b_in.get_temp = (gettemp[1] << 8 | gettemp[0]) * 0.01;
				lm75b_in.get_temp =( (float)( (int)( (lm75b_in.get_temp + 0.005) * 100 ) ) )/100;
				/*启动消息队列*/
				os_Status = osMessageQueuePut(msgQueue_LM75B_Data, 	/* 消息队列的名字 */
								  &lm75b_in.get_temp, 				/* 放入消息队列的变量（消息） */
								  NULL,      						/* 放入消息的优先级 */
								  NULL      	 					/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
								 );	
			#if DEBUG_MODE == 1  
				printf("lm75b data  = %f\r\n" , lm75b_in.get_temp);
			#endif
			}
			else
			{
				#if DEBUG_MODE == 1
					printf(" temp = -data ");
				#endif
			}
		}
		auto_temperature_control(ds18b20_in.get_temp , ds18b20_out.get_temp , lm75b_in.get_temp);
		
		tick += usFrequency;                          
		osDelayUntil(tick);
	}
}


#if Initialize_Network == 1  

/*
*********************************************************************************************************
*	函 数 名: AppTaskEthCheck
*	功能说明: 检查网线插拔状态。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 
*********************************************************************************************************
*/
void AppTaskEthCheck(void *argument)
{
	const uint16_t usFrequency = 250;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	unsigned char strct = 0;
	unsigned int Flag = 0;
	while(1)
	{
		/*DUBUG模式下可以正常识别，但断电后不进入DUBUG模式，无法识别*/
		ethernetif_check_link(&netif);
		/*只有当网线插上后才建立UDP链接*/
		
		//Flag = osEventFlagsGet(event_initiativePowOff_ID);
		/*网线链接*/
		if(netif_is_link_up(&netif) == 1)
		{
			udp_echoserver_init();
			osEventFlagsSet(event_cable_ID , 0x01U);
		}
		/*网线未链接 */
		else if(netif_is_link_up(&netif) == 0)
		{
			osEventFlagsSet(event_cable_ID , 0x02U);
		}
				
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLwIPPro
*	功能说明: LwIP数据接收处理
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
void AppTaskLwIPPro(void *argument)
{
	const uint16_t usFrequency = 5;	/* 延迟周期 */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* 获取当前时间 */
	osStatus_t os_Status;
	while(1)
	{		
		ethernetif_poll(&netif);
		tick += usFrequency;    /* 相对延迟 */                      
		osDelayUntil(tick);	
	}
}

#endif

