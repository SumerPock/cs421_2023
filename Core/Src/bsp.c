
#include "bsp.h"

unsigned char Bsp_initialize(void)
{
	unsigned char ucreturn = 0;
	
	MX_I2C1_Init(); //LM75B设备驱动
	
	MX_DMA_Init();
	
	bsp_InitDWT();	//这里有一个坑需要注意下。
	
	/*与上位机链接*/
	MX_USART1_UART_Init();
	/**UART DMA config**/
	//HAL_UART_Transmit(&husart1, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&husart1, UART_IT_IDLE);	/* 开启串口空闲中断 */
	#define UART_RX_MODE_DMA	/* 开启串口接收 大小缓冲区最大长度rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart1, g_tBspUsart1.pRxBuf, g_tBspUsart1.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif
	
	/*与GPS链接*/
	MX_USART3_UART_Init();		
	/**UART DMA config**/
	//HAL_UART_Transmit(&husart3, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&husart3, UART_IT_IDLE);	/* 开启串口空闲中断 */
	#define UART_RX_MODE_DMA	/* 开启串口接收 大小缓冲区最大长度rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&husart3, g_tBspUsart3.pRxBuf, g_tBspUsart3.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif

	/*与RK3588通讯*/	
	MX_UART5_UART_Init();	
	/**UART DMA config**/
	//HAL_UART_Transmit(&huart5, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);	/* 开启串口空闲中断 */
	#define UART_RX_MODE_DMA	/* 开启串口接收 大小缓冲区最大长度rxSize */	
	#ifdef UART_RX_MODE_DMA
	HAL_UART_Receive_DMA(&huart5, g_tBspUart5.pRxBuf, g_tBspUart5.rxSize);
	#else
	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
	#endif
	

//	MX_USART6_UART_Init();
//	/**UART DMA config**/
//	//HAL_UART_Transmit(&husart6, (uint8_t *)"this is uart1 send data\r\n", 24, 1000);	
//	__HAL_UART_ENABLE_IT(&husart6, UART_IT_IDLE);	/* 开启串口空闲中断 */
//	#define UART_RX_MODE_DMA	/* 开启串口接收 大小缓冲区最大长度rxSize */	
//	#ifdef UART_RX_MODE_DMA
//	HAL_UART_Receive_DMA(&husart6, g_tBspUsart6.pRxBuf, g_tBspUsart6.rxSize);
//	#else
//	HAL_UART_Receive_IT(&husart1, g_tBspUart1.pRxBuf, g_tBspUart1.rxSize);
//	#endif
	
	osDelay(200);

	bsp_InitLed();
	bsp_InitGpio();
	bsp_InitDS18B20();	/* 配置GPIO 用于访问DS18B20 */
	bsp_InitHardTimer();
	
	Rheostat_Init(); 	/*ADC*/
		
	return ucreturn;
}

//uint32_t idAddr[]={ 0x1FFFF7AC,  /*STM32F0唯一ID起始地址*/
//					0x1FFFF7E8,  /*STM32F1唯一ID起始地址*/
//					0x1FFF7A10,  /*STM32F2唯一ID起始地址*/
//					0x1FFFF7AC,  /*STM32F3唯一ID起始地址*/
//					0x1FFF7A10,  /*STM32F4唯一ID起始地址*/
//					0x1FF0F420,  /*STM32F7唯一ID起始地址*/
//					0x1FF80050,  /*STM32L0唯一ID起始地址*/
//					0x1FF80050,  /*STM32L1唯一ID起始地址*/
//					0x1FFF7590,  /*STM32L4唯一ID起始地址*/
//					0x1FF0F420}; /*STM32H7唯一ID起始地址*/

///*获取MCU的唯一ID*/
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
*	函 数 名: 
*	功能说明: 
*	形    参: 	gettemp_1: 内部18B20温度
				gettemp_2: 外部18B20温度
				gettemp_3: 内部LM75B温度
*	返 回 值: 
*   优 先 级:   
*********************************************************************************************************
*/
unsigned char auto_temperature_control(float gettemp_1 , float gettemp_2 , float gettemp_3)
{
	unsigned char ucreturn = 0;
	float temp18b20_1 = gettemp_1;
	float temp18b20_2 = gettemp_2;
	float templm75b 	= gettemp_3;
	
	//如果DS18B20和LM75B都正常
	if(temp18b20_1 > 0 && templm75b > 0)
	{
		if(temp18b20_1 < 10.00)
		{
			Fan_Setup_Spped(1);//最低散热挡位
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
			Fan_Setup_Spped(5);//最高散热挡位
		}
	}
	else//俩者有一个损坏,直接启动最高散热挡位
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
*	函 数 名: 
*	功能说明: 手动温控程序
*	形    参:
*	返 回 值: 
*   优 先 级:   
*********************************************************************************************************
*/
unsigned char manual_temperature_control(unsigned char model)
{
	unsigned char ucreturn = 0;
	switch(model)
	{
		case 0: //野兽模式
			Fan_Setup_Spped(5);
			break;
		
		case 1://淑女模式
			Fan_Setup_Spped(1);
			break;
		
		case 2://均衡模式
			Fan_Setup_Spped(3);
			break;
	}
	return ucreturn;
}

/*
*********************************************************************************************************
*	函 数 名: checksum
*	功能说明: 
*	形    参: 
*	返 回 值: 
*   优 先 级:   
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
*	函 数 名: getSystemData
*	功能说明: 
*	形    参: 
*	返 回 值: 
*   优 先 级:   
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
	//先检查下协议指令是否正确
	if(*(buffuart1 + 0) == 0xAA && buffuart1[1] == 0x0D)
	{
		//id
		sysdata.dataid = *(buffuart1 + 0);
		//长度
		sysdata.datalen = *(buffuart1 + 1);
		//包序号
		sysdata.dataloop = *(buffuart1 + 2);
		//系统状态
		sysdata.system_state = *(buffuart1 + 3);
		//系统错误
		sysdata.system_error = *(buffuart1 + 4);
		//主板请求断电 延时
		sysdata.system_reboot_timer = *(buffuart1 + 5);
		//断电重启校验
		sysdata.system_reboot_check =  *(buffuart1 + 6);
		//主板断电请求
		sysdata.system_reboot_ask = *(buffuart1 + 7) << 8 | *(buffuart1 + 8);

		//电源板请求应答标志
		sysdata.system_reboot_askflag = *(buffuart1 + 9);				
		sysdata.askerror = *(buffuart1 + 10);				
		sysdata.ask  = *(buffuart1 + 11);
		if(sysdata.system_reboot_ask == 0x5566)
		{
			//判断是重启还是断电
			if(sysdata.system_reboot_timer == 0x00  && sysdata.system_reboot_check == 0xFF)//断电
			{		
				//收到主板返回的帧ID 
				if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
//					ucreturn = 0x04;
//					//记录以下这次的帧序号
//					//启动消息队列    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//					sysans.system_ans_loop = sysdata.dataloop;
//					os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* 消息队列的名字 */
//								&sysans.system_ans_loop, 				/* 放入消息队列的变量（消息） */
//								NULL,      								/* 放入消息的优先级 */
//								NULL      	 							/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
//								);
//					
//					/*通知 主动断电请求 停止发送断电请求*/
//					osEventFlagsSet(event_initiativePowOff_ID , 0x01U);
				}
				//第一次，没有收到数据，并且发送的也是0
				else if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
				{
					ucreturn = 0x01;
				}
				//第N次，没收到主板返回的帧ID
				else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
					//证明主板第二次发送了这个数据，我认为他出问题了，还是视为情况一
					ucreturn = 0x01;
				}
			}
			else if(sysdata.system_reboot_timer == 0xFF && sysdata.system_reboot_check == 0x00)//立即重启 , 需要计算重启校验
			{
				//第一次，没有收到数据，并且发送的也是0
				if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
				{
					ucreturn = 0x02;
				}
				//第N次，没收到主板返回的帧ID
				else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
					//证明主板第二次发送了这个数据，我认为他出问题了，还是视为情况一
					ucreturn = 0x02;
				}
				//收到主板返回的帧ID 
				else if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
				{
//					ucreturn = 0x05;
//					//记录以下这次的帧序号
//					//启动消息队列    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//					sysans.system_ans_loop = sysdata.dataloop;
//					os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* 消息队列的名字 */
//								&sysans.system_ans_loop, 				/* 放入消息队列的变量（消息） */
//								NULL,      								/* 放入消息的优先级 */
//								NULL      	 							/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
//								);
				}				
			}
			else//计数重启，需要计算重启校验
			{
				if(sysdata.system_reboot_timer != 0x00 && sysdata.system_reboot_timer != 0xff)
				{
					//第一次并且没收到数据，发送的也是0
					if( ((sysdata.system_reboot_askflag + loopdata) == 0) )
					{
						ucreturn = 0x03;
					}
					//第N次，没收到主板返回的帧ID
					else if(sysdata.system_reboot_askflag != loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
					{
						//证明主板第二次发送了这个数据，我认为他出问题了，还是视为情况一
						ucreturn = 0x03;
					}
					//收到主板返回的帧ID 
					else if(sysdata.system_reboot_askflag == loopdata && ((sysdata.system_reboot_askflag + loopdata) != 0) )
					{
//						ucreturn = 0x06;
//						//记录以下这次的帧序号
//						//启动消息队列    osMessageQueueGetCount(msgQueue_SYS_ANSData);
//						sysans.system_ans_loop = sysdata.dataloop;
//						os_Status = osMessageQueuePut(msgQueue_SYS_ANSData, /* 消息队列的名字 */
//									&sysans.system_ans_loop, 				/* 放入消息队列的变量（消息） */
//									NULL,      								/* 放入消息的优先级 */
//									NULL      	 							/* 超时时间为0，如果osMessageQueuePut被中断回调函数调用时，此参数必须是NULL */
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
*	函 数 名: ChangetoSexagesimal
*	功能说明: GPS数据转换函数
*	形    参: 无
*	返 回 值: 无
*   优 先 级:   
*********************************************************************************************************
*/
int ChangetoSexagesimal(float Num, int *a, int *b, int *c)
{

    float e;
    int Hour,Minute,Second;
        
    Hour = (int)(Num);                              //度                        
    Minute = (int)((Num-Hour)*60);                  //分
    Second = (int)(((Num-Hour)*60 - Minute) *60);   //秒

    e = ((Num-Hour)*60 - Minute) *60 - Second;      //四舍五入

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