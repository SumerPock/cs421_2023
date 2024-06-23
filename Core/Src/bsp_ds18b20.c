/*
*********************************************************************************************************
*
*	模块名称 : DS18B20 驱动模块(1-wire 数字温度传感器）
*	文件名称 : bsp_ds18b20.c
*	版    本 : V1.0
*	说    明 : DS18B20和CPU之间采用1个GPIO接口。
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2014-01-24  armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	DS18B20 可以直接查到STM32-V5开发板的U16 (3P) 插座.

	DS18B20     STM32F407开发板
	VCC   ------  3.3V
	DQ    ------  PB1   (开发板上有 4.7K 上拉电阻)
	GND   ------  GND
*/

/* 定义GPIO端口 */
#define RCC_DQ		__HAL_RCC_GPIOD_CLK_ENABLE()
#define PORT_DQ		GPIOD
#define PIN_DQ		GPIO_PIN_1


#if 1 /* 库函数方式 */
	#define DQ_0()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ , GPIO_PIN_RESET)
	#define DQ_1()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ , GPIO_PIN_SET)

	/* 判断DQ输入是否为低 GPIO_PIN_RESET*/
	#define DQ_IS_LOW()	(HAL_GPIO_ReadPin(PORT_DQ, PIN_DQ) == GPIO_PIN_RESET)	
	
#else	/* 直接操作寄存器，提高速度 */
//	#define DQ_0()		PORT_DQ->BSRRH = PIN_DQ
//	#define DQ_1()		PORT_DQ->BSRR = PIN_DQ
	#define DQ_0()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ, GPIO_PIN_SET)
	#define DQ_1()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ, GPIO_PIN_RESET)
	/* 判断DQ输入是否为低 */
	#define DQ_IS_LOW()	((PORT_DQ->IDR & PIN_DQ) == 0)
#endif


/* 定义GPIO端口 */
#define PIN_DQ1		GPIO_PIN_0


#if 1 /* 库函数方式 */
	#define DQ1_0()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1 , GPIO_PIN_RESET)
	#define DQ1_1()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1 , GPIO_PIN_SET)

	/* 判断DQ输入是否为低 GPIO_PIN_RESET*/
	#define DQ1_IS_LOW()	(HAL_GPIO_ReadPin(PORT_DQ, PIN_DQ1) == GPIO_PIN_RESET)	
	
#else	/* 直接操作寄存器，提高速度 */
//	#define DQ_0()		PORT_DQ->BSRRH = PIN_DQ
//	#define DQ_1()		PORT_DQ->BSRR = PIN_DQ
	#define DQ1_0()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1, GPIO_PIN_SET)
	#define DQ1_1()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1, GPIO_PIN_RESET)
	/* 判断DQ输入是否为低 */
	#define DQ1_IS_LOW()	((PORT_DQ->IDR & PIN_DQ1) == 0)
#endif


/*
*********************************************************************************************************
*	函 数 名: bsp_InitDS18B20
*	功能说明: 配置STM32的GPIO和SPI接口，用于连接 DS18B20
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDS18B20(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	RCC_DQ;

	DQ_1();
	DQ1_1();
	
	GPIO_InitStruct.Pin       = PIN_DQ | PIN_DQ1;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;	/* 设为开漏模式 */
	GPIO_InitStruct.Pull      = GPIO_NOPULL;			/* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_DQ, &GPIO_InitStruct);	
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_Reset
*	功能说明: 复位DS18B20。 拉低DQ为低，持续最少480us，然后等待
*	形    参: 选择18B20编号
*	返 回 值: 0 失败； 1 表示成功
*********************************************************************************************************
*/
unsigned char DS18B20_Reset_Choose(unsigned char num)
{
	/*
		复位时序, 见DS18B20 page 15

		首先主机拉低DQ，持续最少 480us
		然后释放DQ，等待DQ被上拉电阻拉高，约 15-60us
		DS18B20 将驱动DQ为低 60-240us， 这个信号叫 presence pulse  (在位脉冲,表示DS18B20准备就绪 可以接受命令)
		如果主机检测到这个低应答信号，表示DS18B20复位成功
	*/
	uint8_t i = 0;
	uint16_t k = 0;

	DISABLE_INT();/* 禁止全局中断 */
	
	if(num == 0)
	{
		/* 复位，如果失败则返回0 */
		for (i = 0; i < 1; i++)
		{
			DQ_0();				/* 拉低DQ */
			bsp_DelayUS(520);	/* 延迟 520uS， 要求这个延迟大于 480us */
			DQ_1();				/* 释放DQ */

			bsp_DelayUS(15);	/* 等待15us */

			/* 检测DQ电平是否为低 */
			for (k = 0; k < 10; k++)
			{
				if (DQ_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* 等待65us */
			}
			if (k >= 10)
			{
				continue;		/* 失败 */
			}

			/* 等待DS18B20释放DQ */
			for (k = 0; k < 30; k++)
			{
				if (!DQ_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* 等待65us */
			}
			if (k >= 30)
			{
				continue;		/* 失败 */
			}

			break;
		}
	}
	else if(num == 1)
	{
				/* 复位，如果失败则返回0 */
		for (i = 0; i < 1; i++)
		{
			DQ1_0();				/* 拉低DQ */
			bsp_DelayUS(520);	/* 延迟 520uS， 要求这个延迟大于 480us */
			DQ1_1();				/* 释放DQ */

			bsp_DelayUS(15);	/* 等待15us */

			/* 检测DQ电平是否为低 */
			for (k = 0; k < 10; k++)
			{
				if (DQ1_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* 等待65us */
			}
			if (k >= 10)
			{
				continue;		/* 失败 */
			}

			/* 等待DS18B20释放DQ */
			for (k = 0; k < 30; k++)
			{
				if (!DQ1_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* 等待65us */
			}
			if (k >= 30)
			{
				continue;		/* 失败 */
			}

			break;
		}	
	}
	
	ENABLE_INT();	/* 使能全局中断 */
	bsp_DelayUS(5);
	if (i >= 1)
	{
		return 0;
	}

	return 1;
}


/*
*********************************************************************************************************
*	函 数 名: DS18B20_WriteByte
*	功能说明: 向DS18B20写入1字节数据
*	形    参: 选择18B20编号
*	返 回 值: 无
*********************************************************************************************************
*/
static void DS18B20_WriteByte_Choose(unsigned char _val , unsigned char num)
{
	/*
		写数据时序, 见DS18B20 page 16
	*/
	unsigned char i;
	
	if(num == 0)
	{
		for (i = 0; i < 8; i++)
		{
			DQ_0();
			bsp_DelayUS(2);

			if (_val & 0x01)
			{
				DQ_1();
			}
			else
			{
				DQ_0();
			}
			bsp_DelayUS(60);
			DQ_1();
			bsp_DelayUS(2);
			_val >>= 1;
		}
	}
	else if(num == 1)
	{
		for (i = 0; i < 8; i++)
		{
			DQ1_0();
			bsp_DelayUS(2);

			if (_val & 0x01)
			{
				DQ1_1();
			}
			else
			{
				DQ1_0();
			}
			bsp_DelayUS(60);
			DQ1_1();
			bsp_DelayUS(2);
			_val >>= 1;
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadByte
*	功能说明: 向DS18B20读取1字节数据
*	形    参: 选择DS18B20编号
*	返 回 值: 无
*********************************************************************************************************
*/
static unsigned char DS18B20_ReadByte_Choose(unsigned char num)
{
	unsigned char ucreturn = 0;
	/*
		写数据时序, 见DS18B20 page 16
	*/
	if(num == 0)
	{
		unsigned char read = 0;
		unsigned char i;
		for (i = 0; i < 8; i++)
		{
			read >>= 1;

			DQ_0();
			bsp_DelayUS(3);
			DQ_1();
			bsp_DelayUS(3);

			if (DQ_IS_LOW())
			{
				;
			}
			else
			{
				read |= 0x80;
			}
			bsp_DelayUS(60);
		}
		ucreturn = read;
	}
	else if(num == 1)
	{
		unsigned char read = 0;
		unsigned char i;
		for (i = 0; i < 8; i++)
		{
			read >>= 1;

			DQ1_0();
			bsp_DelayUS(3);
			DQ1_1();
			bsp_DelayUS(3);

			if (DQ1_IS_LOW())
			{
				;
			}
			else
			{
				read |= 0x80;
			}
			bsp_DelayUS(60);
		}
		ucreturn = read;
	}
	return ucreturn;
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadTempReg
*	功能说明: 读温度寄存器的值（原始数据）
*	形    参: 选择DS18B20编号
*	返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
unsigned short DS18B20_ReadTempReg_Choose(unsigned char num)
{
	uint8_t temp1, temp2;

	/* 总线复位 */
	if (DS18B20_Reset_Choose(num) == 0)
	{
		return 0;
	}	
	DS18B20_WriteByte_Choose(0xcc , num);
	DS18B20_WriteByte_Choose(0x44 , num);

	DS18B20_Reset_Choose(num);

	DS18B20_WriteByte_Choose(0xcc , num);
	DS18B20_WriteByte_Choose(0xbe , num);

	temp1 = DS18B20_ReadByte_Choose(num);	/* 读温度值低字节 */
	temp2 = DS18B20_ReadByte_Choose(num);	/* 读温度值高字节 */

	return ((temp2 << 8) | temp1);	/* 返回16位寄存器值 */
}



/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadTempRegCRC
*	功能说明: 读温度寄存器的值（原始数据），带CRC校验。
*	形    参: 无
*	返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
unsigned char dallas_crc8(unsigned char *data, unsigned char size)
{
  unsigned char crc = 0, inbyte, j, mix;
	unsigned int i;
	for (i = 0; i < size; ++i )
	{
			inbyte = data[i];
			for (j = 0; j < 8; ++j )
			{
					mix = (crc ^ inbyte) & 0x01;
					crc >>= 1;
					if ( mix ) crc ^= 0x8C;
					inbyte >>= 1;
			}
	}
  return crc;
}




unsigned short DS18B20_ReadTempRegCRC_Choose(unsigned char num)
{
	uint8_t temp[10] = {0} , crc = 0;
	
	/* 总线复位 */
	if (DS18B20_Reset_Choose(num) == 0)
	{
		return 0;
	}		

	DS18B20_WriteByte_Choose(0xcc , num);	/* 发命令 */
	DS18B20_WriteByte_Choose(0x44 , num);	/* 发转换命令 */

	DS18B20_Reset_Choose(num);		/* 总线复位 */

	DS18B20_WriteByte_Choose(0xcc , num);	/* 发命令 */
	DS18B20_WriteByte_Choose(0xbe , num);

	temp[0] = DS18B20_ReadByte_Choose(num);	/* 读温度值低字节 */
	temp[1] = DS18B20_ReadByte_Choose(num);	/* 读温度值高字节 */

	temp[2] = DS18B20_ReadByte_Choose(num);	/* Alarm High Byte    */
	temp[3] = DS18B20_ReadByte_Choose(num);	/* Alarm Low Byte     */
	temp[4] = DS18B20_ReadByte_Choose(num);	/* Reserved Byte 1    */
	temp[5] = DS18B20_ReadByte_Choose(num);	/* Reserved Byte 2    */
	temp[6] = DS18B20_ReadByte_Choose(num);	/* Count Remain Byte  */
	temp[7] = DS18B20_ReadByte_Choose(num);	/* Count Per degree C */
    crc = DS18B20_ReadByte_Choose(num);

	/* 最高的125°对应0x07D0，这里返回0x0800表示错误*/
	if(crc != dallas_crc8(temp, 8))
	{
		return 0x800;
	}
	
	return ((temp[1] << 8) | temp[0]);	/* 返回16位寄存器值 */
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadID
*	功能说明: 读DS18B20的ROM ID， 总线上必须只有1个芯片
*	形    参: _id 存储ID
*	返 回 值: 0 表示失败， 1表示检测到正确ID
*********************************************************************************************************
*/
unsigned char DS18B20_ReadID_Choose(unsigned char *_id , unsigned char num)
{
	unsigned char i = 0;
	unsigned char ucreturn = 0;
	if(num == 0)
	{
		/* 总线复位 */
		if (DS18B20_Reset_Choose(num) == 0)
		{
			ucreturn = 0; 
		}
		DS18B20_WriteByte_Choose(0x33 , num);/* 发命令 */
		for (i = 0; i < 8; i++)
		{
			_id[i] = DS18B20_ReadByte_Choose(num);
		}

		DS18B20_Reset_Choose(num);		/* 总线复位 */
		
		ucreturn = 1;
	}
	else if(num == 1)
	{
		/* 总线复位 */
		if (DS18B20_Reset_Choose(num) == 0)
		{
			ucreturn = 0; 
		}
		DS18B20_WriteByte_Choose(0x33 , num);/* 发命令 */
		for (i = 0; i < 8; i++)
		{
			_id[i] = DS18B20_ReadByte_Choose(num);
		}

		DS18B20_Reset_Choose(num);		/* 总线复位 */				
		ucreturn = 1;
	}
	return ucreturn;
}


/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadTempByID
*	功能说明: 读指定ID的温度寄存器的值（原始数据）
*	形    参: 无
*	返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
unsigned short DS18B20_ReadTempByID_Choose(unsigned char *_id , unsigned char num)
{
	unsigned char temp1 = 0 , temp2 = 0;
	unsigned char i = 0;
	unsigned short ucretu = 0;

//	if(num == 0)
//	{
		DS18B20_Reset_Choose(num);		/* 总线复位 */

		DS18B20_WriteByte_Choose(0x55 , num);	/* 发命令 */
		for (i = 0; i < 8; i++)
		{
			DS18B20_WriteByte_Choose(_id[i] , num);
		}
		DS18B20_WriteByte_Choose(0x44 , num);	/* 发转换命令 */

		DS18B20_Reset_Choose(num);		/* 总线复位 */

		DS18B20_WriteByte_Choose(0x55 , num);	/* 发命令 */
		for (i = 0; i < 8; i++)
		{
			DS18B20_WriteByte_Choose(_id[i] , num);
		}	
		DS18B20_WriteByte_Choose(0xbe , num);

		temp1 = DS18B20_ReadByte_Choose(num);	/* 读温度值低字节 */
		temp2 = DS18B20_ReadByte_Choose(num);	/* 读温度值高字节 */

		ucretu = ((temp2 << 8) | temp1);	/* 返回16位寄存器值 */
//	}
//	else if(num == 1)
//	{
//		DS18B20_Reset_Choose(num);		/* 总线复位 */

//		DS18B20_WriteByte_Choose(0x55 , num);	/* 发命令 */
//		for (i = 0; i < 8; i++)
//		{
//			DS18B20_WriteByte_Choose(_id[i] , num);
//		}
//		DS18B20_WriteByte_Choose(0x44 , num);	/* 发转换命令 */

//		DS18B20_Reset_Choose(num);		/* 总线复位 */

//		DS18B20_WriteByte_Choose(0x55 , num);	/* 发命令 */
//		for (i = 0; i < 8; i++)
//		{
//			DS18B20_WriteByte_Choose(_id[i] , num);
//		}	
//		DS18B20_WriteByte_Choose(0xbe , num);

//		temp1 = DS18B20_ReadByte_Choose(num);	/* 读温度值低字节 */
//		temp2 = DS18B20_ReadByte_Choose(num);	/* 读温度值高字节 */

//		ucretu = ((temp2 << 8) | temp1);	/* 返回16位寄存器值 */
//	}
	return ucretu;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
