#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: AppTaskRK3588_Send
*	����˵��: RK3588���ݷ�������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
void AppTaskRK3588_Send(void *argument)
{
	const uint16_t usFrequency = 100;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	uint32_t Flag = 0;	
	
	/*���ڱ������ָ��*/
	unsigned int rk3588readyFlag = 0;
	
	/*Ӧ��ָ��*/
	unsigned char answdata[]   = {0x23 , 0x06 , 0x01 , 0x01 , 0x00 , 0x00};
	/*�����ϵ�ָ��*/
	unsigned char closurepow[] = {0x22 , 0x06 , 0x01 , 0x43 , 0x4C , 0x00};
	/*��ͨ�ϱ�������*/
	unsigned char normal[] = {0x25 , 0x0D , 0x01 , 0x00 , 0x60 , 0x12 , 0x34 , 0x56 , 0x11 , 0x22 , 0x33 , 0x44 ,0x66};
	/*GPS����*/
	unsigned char gpsdata[] = {0x21 , 0x11 , 0x01 , //��ͷ �� �ܳ� �� ֡���
							   0x00 , //ά�����
							   0x60 , 0x00 , 0x12 , 0x34 , 0x56 , 0x00,//����
							   0x11 , 0x00 , 0x22 , 0x33 , 0x44 , 0x00,//ά��
							   0x66};//У��
	
	unsigned char loop = 0;	

	/*����ָ����Ϣ��������*/
	unsigned char msg_rk3588rebottimr_Number = 0;
	/*�����¶ȴ�������Ϣ��������*/
	unsigned char msg_DS18B20_1_Number = 0;
	unsigned char msg_DS18B20_2_Number = 0;
	unsigned char msg_LM75B_Number = 0;
	/*GPS��Ϣ��������*/
	unsigned char msg_GPS_Number = 0;
	/*���Ȼ�ɢ������Ϣ����*/
	unsigned char msg_calfan_Number = 0;
	
	//PcSetNetwork pcsetnet = {0};
	//PcSetTimer pcsettim = {0};
	//static unsigned char msg_PcSetNetwork_Number = 0;
	//static unsigned char msg_PcSetTime_Number = 0;	
	
	/*���ڴ洢����ģʽ*/
	unsigned int Flag_PowMode = 0;
	/*�����¶ȴ���������*/
	DS18B20_DATA ds18b20_in = {0.00};
	DS18B20_DATA ds18b20_out = {0.00};
	LM75B_DATA lm75b_in = {0.00};
	/*�ְ�������*/
	static unsigned char NumLoop = 0;
	/*GPS���ݽṹ��*/
	GPS_T my_tGPS = {0};
	/*�������ݽṹ��*/
	CALFAN_Rising calfan_risi = {0};
	/*�����ṹ��*/
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
				//Ӧ��ָ��ϵ�
				case 0x01:
					*(answdata + 2) = loop++;
					*(answdata + 3) = 0x02;
					*(answdata + 4) = 0x00;
					*(answdata + 5) = checksum(&answdata[0], sizeof(answdata) - 1);
					BSP_UART_Transmit_IT(&huart5 , &answdata[0] , sizeof(answdata));
				
//					/*�Ե�Ƭ��*/
//					unsigned int place = 5000;
//					place += usFrequency;    /* ����ӳ� */                      
//					osDelayUntil(place);	
	
//					/* ��λ�ػ�ָ��ID �ػ���ʼ Ϊ�����ֶϵ�����������ǵĹػ���־λ���벻һ��*/
//					osEventFlagsSet(event_RK3588ShutDown_ID , 0x01U);
//					/* ��λ����ָ��ID �����ڼ俪������*/
//					osEventFlagsSet(event_RK3588StartUp_ID  , 0x00U);
//					/*2023.5.21Ӧ���Ҫ�����ڶ���̫�죬���Խ��������Ӧ��ػ�ģʽ�ر�	����ܷ�����ʵ�ܹǸа�*/
//					/* RK3588�ػ����� */
//					ThreadIdTaskRK3588_Closure = osThreadNew(AppTaskRK3588_Closure, NULL, &ThreadRK3588_Closure_Attr);
				
					if(osEventFlagsClear(event_RK3588Sinal_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}	
				break;
						
				//Ӧ��ָ�����		
				case 0x03:
					if(msg_rk3588rebottimr_Number > 0)
					{
						os_Status = osMessageQueueGet(msgQueue_RK3588RebootTime_Data,/* ��Ϣ���е����� */
													  &rebottime,    				 /* �����Ϣ���ڴ� */
													  NULL,              			 /* ȡ����Ϣ�����ȼ� */
													  osWaitForever  				 /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
						
						*(answdata + 2) = loop++;
						*(answdata + 3) = 0x01;
						*(answdata + 4) = rebottime.reboottime;
						*(answdata + 5) = checksum(&answdata[0], sizeof(answdata) - 1);
						BSP_UART_Transmit_IT(&huart5 , &answdata[0] , sizeof(answdata));
						
						//���
						os_Status = osMessageQueuePut(msgQueue_SetRk3588RebootTime_Data,/* ��Ϣ���е����� */
													  &rebottime,    				 /* �����Ϣ���ڴ� */
													  NULL,              			 /* ȡ����Ϣ�����ȼ� */
													  osWaitForever  				 /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
						
						if(rebottime.reboottime == 0xff)//��������
						{
							#if DEBUG_MODE == 1  
								printf("rebottime.reboottime = %d \r\n" , rebottime.reboottime);
							#endif	
							/* ��λ�ػ�ָ��ID �ػ���ʼ Ϊ�����ֶϵ�����������ǵĹػ���־λ���벻һ��*/
							osEventFlagsSet(event_RK3588ShutDown_ID , 0x02U);				
							/* ��λ����ָ��ID �����ڼ俪������*/
							osEventFlagsSet(event_RK3588StartUp_ID  , 0x02U);					
							/* RK3588�������� */
							ThreadIdTaskRK3588_Reboot = osThreadNew(AppTaskRK3588_Reboot, NULL, &ThreadRK3588_Reboot_Attr);
						}
						else
						{
							//��ʱ����
							/* ��λ�ػ�ָ��ID �ػ���ʼ Ϊ�����ֶϵ�����������ǵĹػ���־λ���벻һ��*/
							osEventFlagsSet(event_RK3588ShutDown_ID , 0x02U);
							/* ��λ����ָ��ID �����ڼ俪������ ������*/
							osEventFlagsSet(event_RK3588StartUp_ID  , 0x01U);
							/* RK3588�������� */
							ThreadIdTaskRK3588_Reboot = osThreadNew(AppTaskRK3588_Reboot, NULL, &ThreadRK3588_Reboot_Attr);
						}
					}
					if(osEventFlagsClear(event_RK3588Sinal_ID , 0x03U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}	
				break;
						
				//�����ϵ�ָ��,��Դ�Ͽ����Զ�����
				case 0x05:
					if(rk3588readyFlag == 0)//֤����û�յ�������׼����ɵ�ָ��
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
			//����ָ�2023.5.20�շ�����Ϊ����ָ��㶪��RK3588Ҳû��,���Խ���������ͨ��RS422���ڱ���
			//��GPS��Ϣ����Ҫ��RK3588
			if(NumLoop == 5)/*��ͨ����*/
			{
				*(normal + 2) = loop++;
				/* 1.��Դ״̬ */
				if(Flag_PowMode == 0x01){
					*(normal + 3) = 0x01;//���ģʽ
					if(osEventFlagsClear(event_PowMode_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource))
					{
					}
				}
				else
				{
					*(normal + 3) = 0x00;//��Դģʽ
				}
				/* 2.��ص��� */
				float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL]={0}; 
				/* ��ص�ѹ */
				ADC_ConvertedValueLocal[0] =((float) ADC_ConvertedValue[0] / 4096 * (float)3.3) *5;	
				ADC_ConvertedValueLocal[0] = ADC_ConvertedValueLocal[0] * 10;
				*(normal + 4) = (unsigned char)ADC_ConvertedValueLocal[0];
				
				/* 3.�ڲ�ɢ���¶�1 */
				if(msg_DS18B20_1_Number > 0)
				{
					unsigned char temp_ds18b20_1 = 0;
					os_Status = osMessageQueueGet(msgQueue_Ds18b20_1_Data,/* ��Ϣ���е����� */
												  &ds18b20_in,    		  /* �����Ϣ���ڴ� */
												  NULL,              	  /* ȡ����Ϣ�����ȼ� */
												  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
					temp_ds18b20_1 = (unsigned char)ds18b20_in.get_temp;
					printf("ds18b20_in.get_temp = %f \n" , ds18b20_in.get_temp);
					*(normal + 5) = temp_ds18b20_1 + 0x28;
				}
//				else
//				{
//					*(normal + 5) = 0x00;
//				}

				/* 4.�ڲ�ɢ���¶�2 */
				if(msg_DS18B20_2_Number > 0)
				{
					unsigned char temp_ds18b20_2 = 0;
					os_Status = osMessageQueueGet(msgQueue_Ds18b20_2_Data,/* ��Ϣ���е����� */
												  &ds18b20_out,    		  /* �����Ϣ���ڴ� */
												  NULL,              	  /* ȡ����Ϣ�����ȼ� */
												  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
					temp_ds18b20_2 = (unsigned char)ds18b20_out.get_temp;
					printf("ds18b20_in.get_temp = %f \n" , ds18b20_out.get_temp);
					*(normal + 6) =temp_ds18b20_2 + 0x28;
				}
//				else
//				{
//					*(normal + 6) = 0x00;
//				}
			
				/* 5.�ⲿɢ���¶�1 */
				if(msg_LM75B_Number > 0)
				{
					unsigned char temp_lm75b = 0;
					os_Status = osMessageQueueGet(msgQueue_LM75B_Data,    /* ��Ϣ���е����� */
												  &lm75b_in,    		  /* �����Ϣ���ڴ� */
												  NULL,              	  /* ȡ����Ϣ�����ȼ� */
												  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
					temp_lm75b = (unsigned char)lm75b_in.get_temp;
					*(normal + 7) = temp_lm75b + 0x28;
				}
//				else
//				{
//					*(normal + 7) = 0x00;
//				}
		
				if(msg_calfan_Number > 0)
				{
					//���					
					os_Status = osMessageQueueGet(msgQueue_CALFAN_Rising_Data,/* ��Ϣ���е����� */
											      &calfan_risi,    		  		/* �����Ϣ���ڴ� */
											      NULL,              	  				/* ȡ����Ϣ�����ȼ� */
											      osWaitForever  	      				/* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);	
					
					if(calfan_risi.big_fan_risingcount >= 255)/* 6.�ڲ����ȸ���1 �� ��Щ���ȵ�ת�ٷ�������1Byteװ���£�����չʾ������ʾ*/
					{
						*(normal + 8) = 0xff;
					}
					else
					{
						*(normal + 8) = calfan_risi.big_fan_risingcount;
					}
					
					if(calfan_risi.arm_fan_risingcount >= 255)	/* 7.�ⲿ���ȸ���2 */
					{
						*(normal + 9) = 0xff;
					}
					else
					{
						*(normal + 9) = calfan_risi.arm_fan_risingcount;
					}
					
					if(calfan_risi.rk_fan_risingcount >= 255)/* 8.TEC���� --����û��TEC����֮˵�������޸�Ϊ -�ⲿ���ȸ���2*/
					{
						*(normal + 10) = 0xff;
					}
					else
					{
						*(normal + 10) = calfan_risi.rk_fan_risingcount;			
					}
				}				
				/* 9.����Ƭ����*/
				*(normal + 11) = 0x00;
				*(normal + 12) = checksum(&normal[0], sizeof(normal) - 1);
				#if TURNON_NO_Normal == 1
					//2023�� 7�� 7�� DONGZHE �޸�Ϊ	husart5
					BSP_UART_Transmit_IT(&huart5 , &normal[0] , sizeof(normal));
				#endif					
			}
			else if(NumLoop == 10)
			{
				/*GPS����*/
				if(msg_GPS_Number > 0) //��Ϣ�����д�������
				{
					os_Status = osMessageQueueGet(msgQueue_GPS_Data,/* ��Ϣ���е����� */
												&my_tGPS,    		  						/* �����Ϣ���ڴ� */
												NULL,              	  						/* ȡ����Ϣ�����ȼ� */
												osWaitForever  	      						/* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);				
					
					float LongtitudeNumber = 0;
					float LatitudeNumber = 0;
							
					int LongtitudeHour = 0;
					int LongtitudeMinute = 0;
					int LongtitudeSecond = 0;
					//float intLongtitudeSecond = 0; 
					
					int LatitudeMinute = 0;
					int LatitudeHour = 0;
					int LatitudeSecond = 0;
					
					LongtitudeNumber = my_tGPS.WeiDu_Du;  	//���ȶ�
					double testdemo = 0.00 , int_part = 0.00 , frac_part = 0.00;
					frac_part = modf(my_tGPS.WeiDu_Fen * 0.0001 , &int_part);		
					LongtitudeMinute = int_part;						//���ȷ�
					float temp = (frac_part * 60);					//������
					//LongtitudeSecond = (int)(100.0 * temp + 0.5) / 100.0;
					LongtitudeSecond = (int)(temp * 100);
					
					#if DEBUG_MODE == 1  
						//printf("Lde = %0.2f , LMinute = %d , LSecond = %d\r\n" , LongtitudeNumber , LongtitudeMinute , LongtitudeSecond);
					#endif	
				
					LatitudeNumber = my_tGPS.JingDu_Du;		//ά�ȶ�
					testdemo = 0.00 , int_part = 0.00 , frac_part = 0.00;
					frac_part = modf(my_tGPS.JingDu_Fen * 0.0001 , &int_part);	
					LatitudeMinute = int_part;							//ά�ȷ�
					temp = (frac_part * 60);									//ά����
					//LatitudeSecond = (int)(100.0 * temp + 0.5) / 100.0;
					LatitudeSecond = (int)(temp * 100);

					#if DEBUG_MODE == 1  
						//printf("Ade = %0.2f , AMinute = %d , ASecond = %d\r\n" , LongtitudeNumber , LongtitudeMinute , LongtitudeSecond);
					#endif

					*(gpsdata + 2) = loop++;
					/*���������ж�*/
					if(my_tGPS.NS == 'S')//��γ
					{
						*(gpsdata + 3) = gpsdata[3] | 0X02;
					}
					else if(my_tGPS.NS == 'N')//��γ
					{
						*(gpsdata + 3) = gpsdata[3] | 0X01;
					}	
					if(g_tGPS.EW == 'E')//����
					{
						*(gpsdata + 3) = gpsdata[3] | 0X10;
					}else if(g_tGPS.EW == 'W')//����
					{
						*(gpsdata + 3) = gpsdata[3] | 0X20;
					}
					
					*(gpsdata + 4) = my_tGPS.JingDu_Du >> 8;		//��λ����
					*(gpsdata + 5) = my_tGPS.JingDu_Du;					//��λ����
					*(gpsdata + 6) = LatitudeMinute; 						//���ȷ�					
					*(gpsdata + 7) = LatitudeSecond >> 8; 			//��λ������
					*(gpsdata + 8) = LatitudeSecond;						//��λ������				
					*(gpsdata + 9) = 0;
					
					*(gpsdata + 10) = my_tGPS.WeiDu_Du >> 8;		//��λά��
					*(gpsdata + 11) = my_tGPS.WeiDu_Du;				//��λά��
					*(gpsdata + 12) = LongtitudeMinute;				//ά�ȷ�	
					*(gpsdata + 13) = LongtitudeSecond >> 8;	//��λά����
					*(gpsdata + 14) = LongtitudeSecond;				//��λά����
					*(gpsdata + 15) = 0;
					
					*(gpsdata + 16) = checksum(&gpsdata[0], sizeof(gpsdata) - 1);
					#if TURNON_NO_Gps == 1
						/*2023 7 7 ���ܹر����*/
						BSP_UART_Transmit_IT(&huart5 , &gpsdata[0] , sizeof(gpsdata));
					#endif	
				}
				else//����������
				{
					*(gpsdata + 2) = loop++;
					
					*(gpsdata + 3) = zero;				//ά������
					
					*(gpsdata + 4) = zero;				//��λ����
					*(gpsdata + 5) = zero;				//��λ����
					*(gpsdata + 6) = zero; 				//���ȷ�					
					*(gpsdata + 7) = zero; 				//��λ������
					*(gpsdata + 8) = zero;				//��λ���Ⱥ���				
					*(gpsdata + 9) = zero;				//��λ���Ⱥ���
					
					*(gpsdata + 10) = zero;				//��λά��
					*(gpsdata + 11) = zero;				//ά�ȷ�	
					*(gpsdata + 12) = zero;				//��λά����
					*(gpsdata + 13) = zero;				//��λ���Ⱥ���		
					*(gpsdata + 15) = zero;				//��λ���Ⱥ���	
					
					*(gpsdata + 16) = checksum(&gpsdata[0], sizeof(gpsdata) - 1);
					#if TURNON_NO_Gps == 1
						BSP_UART_Transmit_IT(&huart5 , &gpsdata[0] , sizeof(gpsdata));
					#endif	
				}				
			}			
		}
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskRK3588_Receive
*	����˵��: ����RK3588����
*	��    ��: ��
*	�� �� ֵ: ��
* 	�� �� ��:   
*********************************************************************************************************
*/
void AppTaskRK3588_Receive(void *argument)
{
	const uint16_t usFrequency = 50;	/* �ӳ����� */
	unsigned int tick = 0;
	tick = osKernelGetTickCount(); 	/* ��ȡ��ǰʱ�� */
	osStatus_t os_Status;
	unsigned int Flag = 0;		
	while(1)
	{
		unsigned char datatype = 0;
		Flag = osEventFlagsGet(event_uart5_ID);
		if(Flag != 0 )
		{
			if(osEventFlagsClear(event_uart5_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
			
			/*�Խ��յ�ָ��ְ�*/
			datatype = Get_RK3588_Data(g_uart5_rx_buf , g_tBspUart5.rxLen);		
			switch(datatype)
			{
				case 0x01://ϵͳ����ָ��
					#if DEBUG_MODE == 1  
						printf("generally data \r\n");
					#endif
					break;
			
				case 0x10://�����¶ȸ�
					break;
				
				case 0x11://�����¶ȵ�
					break;
				
				case 0x12://����ͷ��ʼ���쳣
					break;
				
				case 0x13://����ͷͨѶ�쳣
					break;
				
				case 0x14://ͼ���ȡʧ��
					break;
				
				case 0x15://�ļ�д��ʧ��
					break;
				
				case 0x16://��������쳣
					break;
				
				case 0x17://��Ƶ�豸�쳣
					break;
				
				case 0x18://��Ƶ�ļ��洢ʧ��
					break;	
				
				case 0x19://����У׼ʧ��
					break;
				
				case 0x03://����ָ��
						#if DEBUG_MODE == 1  
							printf("reboot data \r\n");
						#endif
					osEventFlagsSet(event_RK3588Sinal_ID , 0x03U);
					break;
				
				case 0x04://�ػ�ָ��
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
		tick += usFrequency;    /* ����ӳ� */                      
		osDelayUntil(tick);	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskRK3588_Send
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ucreturn ->
				��������
					0x01 -- ��������,ϵͳ����
					0x10 -- �����¶ȸ�
					0x11 -- �����¶ȵ�
					0x12 -- ����ͷ��ʼ���쳣
					0x13 -- ����ͷͨѶ�쳣
					0x14 -- ͼ���ȡʧ��
					0x15 -- �ļ�д��ʧ��
					0x16 -- ��������쳣
					0x17 -- ��Ƶ�豸�쳣
					0x18 -- ��Ƶ�ļ��洢ʧ��
					0x19 -- ����У׼ʧ��
					
					0x1F -- �������������ݣ���У��Ͳ���
					0x1E -- �������������ݣ���֡���Ȳ���
				
//				Ӧ��ָ��
//				    0x02 -- ������Ӧ��ָ���ʱӦ��
//				    0x20 -- ������Ӧ��ָ��������õ�IP����
//				    0x21 -- ������Ӧ��ָ��Żز�ѯ��IP����
//				
//				    0x2F -- ������Ӧ��ָ���֡���Ȳ���
//				    0X2E -- ������Ӧ��ָ���У��Ͳ���
//				    0x2D -- δ֪Э��
				 
				����ָ��
				   0x03 -- ����ָ��
				   
				   0x3F -- ����ָ�֡��������
				   0x3E -- ����ָ�У�鲻��
				   
				�ػ�ָ��
				   0x04 -- �ػ�ָ��
				   
				   0x4F -- У�����
				   0x4E -- ���ݴ���
				   0x4D -- ֡������
				λ��
					0xF0
*   �� �� ��:
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
		case 0xAA://����ָ�� ,����0x10-0x1f
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

		case 0xA2://����ָ��,����
			if(*(getbuff + 1) == GetBLen)
			{
				unsigned char getcheck = 0;
				getcheck = checksum(getbuff, GetBLen - 1);
				if(getcheck == *(getbuff + 5))
				{
					rebottime.reboottime = *(getbuff + 3);
					rebottime.reboottimecheck = *(getbuff + 4);
					//�������
					os_Status = osMessageQueuePut(msgQueue_RK3588RebootTime_Data, 	/* ��Ϣ���е����� */
												  &rebottime, 						/* ������Ϣ���еı�������Ϣ�� */
												  NULL,      						/* ������Ϣ�����ȼ� */
												  NULL      	 					/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
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

		case 0xA3://�ػ�ָ��,����
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

