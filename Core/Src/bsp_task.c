
#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: AppTaskGPSSetup
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
void AppTaskGPSSetUp(void *argument)
{
	const uint16_t usFrequency = 1600;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
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
		/*2023�� 5�� 20�� DONGZHE �޸Ľ� BSP_UART_Transmit_IT �����һ������ sizeof(SucbufGNGGA) - 1 �޸�Ϊ sizeof(SucbufGNGGA) ��Ϊ -1 �Ƕ����*/
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
		/* GPS���� */
		ThreadIdTaskGPS = osThreadNew(AppTaskGPS, NULL, &ThreadGPS_Attr);
		/*GSP��Ϣ����*/
		msgQueue_GPS_Data = osMessageQueueNew(MQ_SIZE , sizeof(GPS_T) ,&GPS_Data_Attr);
		osThreadExit();
		
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskGPS
*	����˵��: ��ɱ�������
*	��    ��: ��
*	�� �� ֵ: ��
* �� �� ��:   
*********************************************************************************************************
*/
void AppTaskGPS(void *argument)
{
	const uint16_t usFrequency = 500;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	uint32_t Flag = 0;	
	volatile unsigned int keil = 0;
	float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL] = {0}; 	
	while(1)
	{
		// ADCת���ĵ�ѹֵͨ��MDA��ʽ����SRAM 
		/* 24V��Դ��ѹ */
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1] / 4096 * (float)3.3;	
		if(keil >= (0x258 * 2))
		{
			bsp_GpioOff(BATTERY);
		}
		
		if(ADC_ConvertedValueLocal[1] > 2.0)/*��Դģʽ : ���õ�Դ*/
		{
			keil = 0;
		}
		else/*��Դģʽ : ��ص�Դ*/
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
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskDS18B20
*	����˵��: ds18b20��LM75B�¶ȶ�ȡ
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
void AppTaskDS18B20(void *argument)
{
	DS18B20_DATA ds18b20_in = {0.00};
	DS18B20_DATA ds18b20_out = {0.00};
	LM75B_DATA lm75b_in = {0.00};

	unsigned char g_rom_id[8] = {0};
	unsigned char g_rom_id1[8] = {0};	
	
	const uint16_t usFrequency = 1600;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
		
	/*�����¶�ת������ (���� DS18B20.pdf page = 4 �ı���֤��ֵת��)*/
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
	/* ��ȡ��һ��DS18B20 ROM ID */
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
	/* ��ȡ�ڶ���DS18B20 ROM ID */
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
		
		/*DS18B20������һ��*/
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
				/*������Ϣ����*/
				os_Status = osMessageQueuePut(msgQueue_Ds18b20_1_Data, 	/* ��Ϣ���е����� */
											  &ds18b20_in, 				/* ������Ϣ���еı�������Ϣ�� */
											  NULL,      				/* ������Ϣ�����ȼ� */
											  NULL      	 			/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
											  );	
			}
		}
		else
		{
			#if DEBUG_MODE == 1  
				printf("crc is error\r\n");
			#endif
		}

		/*DS18B20�¶ȴ���������*/
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
				/*������Ϣ����*/
				os_Status = osMessageQueuePut(msgQueue_Ds18b20_2_Data,	/* ��Ϣ���е����� */
											  &ds18b20_out, 						/* ������Ϣ���еı�������Ϣ�� */
											  NULL,      							/* ������Ϣ�����ȼ� */
											  NULL      	 						/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
											  );
			}
		}
		else
		{
			#if DEBUG_MODE == 1  
				printf("crc is error\r\n");
			#endif
		}		
		
		/*I2C�¶ȴ�����*/
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
				/*������Ϣ����*/
				os_Status = osMessageQueuePut(msgQueue_LM75B_Data, 	/* ��Ϣ���е����� */
								  &lm75b_in.get_temp, 				/* ������Ϣ���еı�������Ϣ�� */
								  NULL,      						/* ������Ϣ�����ȼ� */
								  NULL      	 					/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
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
*	�� �� ��: AppTaskEthCheck
*	����˵��: ������߲��״̬��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 
*********************************************************************************************************
*/
void AppTaskEthCheck(void *argument)
{
	const uint16_t usFrequency = 250;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	unsigned char strct = 0;
	unsigned int Flag = 0;
	while(1)
	{
		/*DUBUGģʽ�¿�������ʶ�𣬵��ϵ�󲻽���DUBUGģʽ���޷�ʶ��*/
		ethernetif_check_link(&netif);
		/*ֻ�е����߲��Ϻ�Ž���UDP����*/
		
		//Flag = osEventFlagsGet(event_initiativePowOff_ID);
		/*��������*/
		if(netif_is_link_up(&netif) == 1)
		{
			udp_echoserver_init();
			osEventFlagsSet(event_cable_ID , 0x01U);
		}
		/*����δ���� */
		else if(netif_is_link_up(&netif) == 0)
		{
			osEventFlagsSet(event_cable_ID , 0x02U);
		}
				
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLwIPPro
*	����˵��: LwIP���ݽ��մ���
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
void AppTaskLwIPPro(void *argument)
{
	const uint16_t usFrequency = 5;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	while(1)
	{		
		ethernetif_poll(&netif);
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}

#endif

