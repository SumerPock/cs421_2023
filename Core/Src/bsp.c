
#include "bsp.h"

unsigned char Bsp_initialize(void)
{
	unsigned char ucreturn = 0;
	
	MX_I2C1_Init(); //LM75B�豸����
	
	MX_DMA_Init();
	
	bsp_InitDWT();	//������һ������Ҫע���¡�
	
	/*����λ������*/
	MX_USART1_UART_Init();
	/**UART DMA config**/
	//HAL_UART_Transmit(&husart1, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&husart1, UART_IT_IDLE);	/* �������ڿ����ж� */
	#define UART_RX_MODE_DMA	/* �������ڽ��� ��С��������󳤶�rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart1, g_tBspUsart1.pRxBuf, g_tBspUsart1.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif
	
	/*��GPS����*/
	MX_USART3_UART_Init();		
	/**UART DMA config**/
	//HAL_UART_Transmit(&husart3, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&husart3, UART_IT_IDLE);	/* �������ڿ����ж� */
	#define UART_RX_MODE_DMA	/* �������ڽ��� ��С��������󳤶�rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart3, g_tBspUsart3.pRxBuf, g_tBspUsart3.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif

	/*��RK3588ͨѶ*/	
	MX_UART5_UART_Init();	
	/**UART DMA config**/
	//HAL_UART_Transmit(&huart5, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);	/* �������ڿ����ж� */
	#define UART_RX_MODE_DMA	/* �������ڽ��� ��С��������󳤶�rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&huart5, g_tBspUart5.pRxBuf, g_tBspUart5.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif
	

//	MX_USART6_UART_Init();
//	/**UART DMA config**/
//	//HAL_UART_Transmit(&husart6, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
//	__HAL_UART_ENABLE_IT(&husart6, UART_IT_IDLE);	/* �������ڿ����ж� */
//	#define UART_RX_MODE_DMA	/* �������ڽ��� ��С��������󳤶�rxSize */	
//	#ifdef UART_RX_MODE_DMA
//	HAL_UART_Receive_DMA(&husart6, g_tBspUsart6.pRxBuf, g_tBspUsart6.rxSize);
//	#else
//	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
//	#endif
	
	osDelay(200);

	bsp_InitLed();
	bsp_InitGpio();
	bsp_InitDS18B20();	/* ����GPIO ���ڷ���DS18B20 */
	bsp_InitHardTimer();
	
	Rheostat_Init(); 	/*ADC*/
		
	return ucreturn;
}

//uint32_t idAddr[]={ 0x1FFFF7AC,  /*STM32F0ΨһID��ʼ��ַ*/
//					0x1FFFF7E8,  /*STM32F1ΨһID��ʼ��ַ*/
//					0x1FFF7A10,  /*STM32F2ΨһID��ʼ��ַ*/
//					0x1FFFF7AC,  /*STM32F3ΨһID��ʼ��ַ*/
//					0x1FFF7A10,  /*STM32F4ΨһID��ʼ��ַ*/
//					0x1FF0F420,  /*STM32F7ΨһID��ʼ��ַ*/
//					0x1FF80050,  /*STM32L0ΨһID��ʼ��ַ*/
//					0x1FF80050,  /*STM32L1ΨһID��ʼ��ַ*/
//					0x1FFF7590,  /*STM32L4ΨһID��ʼ��ַ*/
//					0x1FF0F420}; /*STM32H7ΨһID��ʼ��ַ*/

///*��ȡMCU��ΨһID*/
//void GetSTM32MCUID(uint32_t *id,MCUTypedef type)
//{
//  if(id!=NULL)
//  {
//    id[0]=*(uint32_t*)(idAddr[type]);
//    id[1]=*(uint32_t*)(idAddr[type]+4);
//    id[2]=*(uint32_t*)(idAddr[type]+8);
//  }
//}

/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: 
*	��    ��: 	gettemp_1: �ڲ�18B20�¶�
				gettemp_2: �ⲿ18B20�¶�
				gettemp_3: �ڲ�LM75B�¶�
*	�� �� ֵ: 
*   �� �� ��:   
*********************************************************************************************************
*/
unsigned char auto_temperature_control(float gettemp_1 , float gettemp_2 , float gettemp_3)
{
	unsigned char ucreturn = 0;
	float temp18b20_1 = gettemp_1;
	float temp18b20_2 = gettemp_2;
	float templm75b 	= gettemp_3;
	
	//���DS18B20��LM75B������
	if(temp18b20_1 > 0 && templm75b > 0)
	{
		if(temp18b20_1 < 10.00)
		{
			Fan_Setup_Spped(1);//���ɢ�ȵ�λ
		}
		else if(temp18b20_1 < 28.00)
		{
			Fan_Setup_Spped(2);
		}
		else if(temp18b20_1 >= 28.00 && temp18b20_1 < 35.00)
		{
			Fan_Setup_Spped(3);
		}
		else if(temp18b20_1 >= 35.00 && temp18b20_1 < 38.000)
		{
			Fan_Setup_Spped(4);
		}
		else if(temp18b20_1 >= 38.00)
		{
			Fan_Setup_Spped(5);//���ɢ�ȵ�λ
		}
	}
	else//������һ����,ֱ���������ɢ�ȵ�λ
	{
		Fan_Setup_Spped(5);
	}
	return ucreturn;
}



void Fan_Setup_Spped(unsigned char sp)
{
	unsigned char spped = sp;
	switch(sp)
	{
		case 1:
			bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_20);
			bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, 5000, smpercent_32);
			bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, 5000, smpercent_32);
			bsp_GpioOff(CRYOGEN);
			break;
		
		case 2:
			bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_40);
			bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_48);
			bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, INTERNAL_ARM_FAN_FREQ, smpercent_48);	
			bsp_GpioOff(CRYOGEN);	
			break;
		
		case 3:
			bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_60);
			bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_64);
			bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, INTERNAL_ARM_FAN_FREQ, smpercent_64);	
			bsp_GpioOff(CRYOGEN);
			break;
		
		case 4:
			bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_80);	
			bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_80);
			bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, INTERNAL_ARM_FAN_FREQ, smpercent_80);	
			bsp_GpioOff(CRYOGEN);
			break;
		
		case 5:
			bsp_SetTIMOutPWM(EXTERNAL_FAN_GPIOPORT, EXTERNAL_FAN_GPIOPIN,  EXTERNAL_FAN_TIMER,  EXTERNAL_FAN_CHANNEL, EXTERNAL_FAN_FREQ, bigpercent_100);
			bsp_SetTIMOutPWM(INTERNAL_RK_FAN_GPIOPORT, INTERNAL_RK_FAN_GPIOPIN,  INTERNAL_RK_FAN_TIMER,  INTERNAL_RK_FAN_CHANNEL, INTERNAL_RK_FAN_FREQ, smpercent_96);
			bsp_SetTIMOutPWM(INTERNAL_ARM_FAN_GPIOPORT, INTERNAL_ARM_FAN_GPIOPIN,  INTERNAL_ARM_FAN_TIMER,  INTERNAL_ARM_FAN_CHANNEL, INTERNAL_ARM_FAN_FREQ, smpercent_96);	
			bsp_GpioOn(CRYOGEN);	
			break;
		
		default:
			break;		
	}
	
}

/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: �ֶ��¿س���
*	��    ��:
*	�� �� ֵ: 
*   �� �� ��:   
*********************************************************************************************************
*/
unsigned char manual_temperature_control(unsigned char model)
{
	unsigned char ucreturn = 0;
	switch(model)
	{
		case 0: //Ұ��ģʽ
			Fan_Setup_Spped(5);
			break;
		
		case 1://��Ůģʽ
			Fan_Setup_Spped(1);
			break;
		
		case 2://����ģʽ
			Fan_Setup_Spped(3);
			break;
	}
	return ucreturn;
}

/*
*********************************************************************************************************
*	�� �� ��: checksum
*	����˵��: 
*	��    ��: 
*	�� �� ֵ: 
*   �� �� ��:   
*********************************************************************************************************
*/
unsigned char checksum(unsigned char *data, int len)
{
    unsigned char sum = 0;
		unsigned int i = 0;
	
    for (i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

/*
*********************************************************************************************************
*	�� �� ��: getSystemData
*	����˵��: 
*	��    ��: 
*	�� �� ֵ: 
*   �� �� ��:   
*********************************************************************************************************
*/
unsigned char getSystemData(unsigned char *GetBuff , unsigned char GetBLen , unsigned char loopdata)
{
	unsigned char ucreturn = 0;
	unsigned char buffuart1[25] = {0};
	SYSTEMDATA sysdata = {0};
	memcpy(buffuart1 , GetBuff , GetBLen);
	osStatus_t os_Status;	
	SYS_ANS sysans = {0};
	//�ȼ����Э��ָ���Ƿ���ȷ
	if(*(buffuart1 + 0) == 0xAA && buffuart1[1] == 0x0D)
	{
		//id
		sysdata.dataid = *(buffuart1 + 0);
		//����
		sysdata.datalen = *(buffuart1 + 1);
		//�����
		sysdata.dataloop = *(buffuart1 + 2);
		//ϵͳ״̬
		sysdata.system_state = *(buffuart1 + 3);
		//ϵͳ����
		sysdata.system_error = *(buffuart1 + 4);
		//��������ϵ� ��ʱ
		sysdata.system_reboot_timer = *(buffuart1 + 5);
		//�ϵ�����У��
		sysdata.system_reboot_check =  *(buffuart1 + 6);
		//����ϵ�����
		sysdata.system_reboot_ask = *(buffuart1 + 7) << 8 | *(buffuart1 + 8);

		//��Դ������Ӧ���־
		sysdata.system_reboot_askflag = *(buffuart1 + 9);				
		sysdata.askerror = *(buffuart1 + 10);				
		sysdata.ask  = *(buffuart1 + 11);
		if(sysdata.system_reboot_ask == 0x5566)
		{
			//�ж����������Ƕϵ�
			if(sysdata.system_reboot_timer == 0x00  && sysdata.system_reboot_check == 0xFF)//�ϵ�
			{		
				//�յ����巵�ص�֡ID 
				if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
//					ucreturn = 0x04;
//					//��¼������ε�֡���
//					//������Ϣ����    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//					sysans.system_ans_loop = sysdata.dataloop;
//					os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* ��Ϣ���е����� */
//								&sysans.system_ans_loop, 				/* ������Ϣ���еı�������Ϣ�� */
//								NULL,      								/* ������Ϣ�����ȼ� */
//								NULL      	 							/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
//								);
//					
//					/*֪ͨ �����ϵ����� ֹͣ���Ͷϵ�����*/
//					osEventFlagsSet(event_initiativePowOff_ID , 0x01U);
				}
				//��һ�Σ�û���յ����ݣ����ҷ��͵�Ҳ��0
				else if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
				{
					ucreturn = 0x01;
				}
				//��N�Σ�û�յ����巵�ص�֡ID
				else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
					//֤������ڶ��η�����������ݣ�����Ϊ���������ˣ�������Ϊ���һ
					ucreturn = 0x01;
				}
			}
			else if(sysdata.system_reboot_timer == 0xFF && sysdata.system_reboot_check == 0x00)//�������� , ��Ҫ��������У��
			{
				//��һ�Σ�û���յ����ݣ����ҷ��͵�Ҳ��0
				if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
				{
					ucreturn = 0x02;
				}
				//��N�Σ�û�յ����巵�ص�֡ID
				else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
					//֤������ڶ��η�����������ݣ�����Ϊ���������ˣ�������Ϊ���һ
					ucreturn = 0x02;
				}
				//�յ����巵�ص�֡ID 
				else if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
//					ucreturn = 0x05;
//					//��¼������ε�֡���
//					//������Ϣ����    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//					sysans.system_ans_loop = sysdata.dataloop;
//					os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* ��Ϣ���е����� */
//								&sysans.system_ans_loop, 				/* ������Ϣ���еı�������Ϣ�� */
//								NULL,      								/* ������Ϣ�����ȼ� */
//								NULL      	 							/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
//								);
				}				
			}
			else//������������Ҫ��������У��
			{
				if(sysdata.system_reboot_timer != 0x00 && sysdata.system_reboot_timer != 0xff)
				{
					//��һ�β���û�յ����ݣ����͵�Ҳ��0
					if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
					{
						ucreturn = 0x03;
					}
					//��N�Σ�û�յ����巵�ص�֡ID
					else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
					{
						//֤������ڶ��η�����������ݣ�����Ϊ���������ˣ�������Ϊ���һ
						ucreturn = 0x03;
					}
					//�յ����巵�ص�֡ID 
					else if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
					{
//						ucreturn = 0x06;
//						//��¼������ε�֡���
//						//������Ϣ����    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//						sysans.system_ans_loop = sysdata.dataloop;
//						os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* ��Ϣ���е����� */
//									&sysans.system_ans_loop, 				/* ������Ϣ���еı�������Ϣ�� */
//									NULL,      								/* ������Ϣ�����ȼ� */
//									NULL      	 							/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
//									);
					}
					
				}
				
			}
		}	
		else
		{
			ucreturn = 0x00;
		}
	}
	return ucreturn;
}







/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/*
*********************************************************************************************************
*	�� �� ��: ChangetoSexagesimal
*	����˵��: GPS����ת������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��:   
*********************************************************************************************************
*/
int ChangetoSexagesimal(float Num, int *a, int *b, int *c)
{

    float e;
    int Hour,Minute,Second;
        
    Hour = (int)(Num);                              //��                        
    Minute = (int)((Num-Hour)*60);                  //��
    Second = (int)(((Num-Hour)*60 - Minute) *60);   //��

    e = ((Num-Hour)*60 - Minute) *60 - Second;      //��������

    if(5 <=(int)(e *10) )
    {
        Second = Second + 1;
    }
    
    if(60 == Second)
    {
        Second = 0;
        Minute = Minute + 1;
    }    
    
    if(60 == Minute)
    {
        Minute = 0;
        Hour = Hour + 1;
    }    

    *a = Hour;
    *b = Minute;
    *c = Second;

    return 1;
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */