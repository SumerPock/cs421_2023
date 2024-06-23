/*
*********************************************************************************************************
*
*	ģ������ : DS18B20 ����ģ��(1-wire �����¶ȴ�������
*	�ļ����� : bsp_ds18b20.c
*	��    �� : V1.0
*	˵    �� : DS18B20��CPU֮�����1��GPIO�ӿڡ�
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2014-01-24  armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	DS18B20 ����ֱ�Ӳ鵽STM32-V5�������U16 (3P) ����.

	DS18B20     STM32F407������
	VCC   ------  3.3V
	DQ    ------  PB1   (���������� 4.7K ��������)
	GND   ------  GND
*/

/* ����GPIO�˿� */
#define RCC_DQ		__HAL_RCC_GPIOD_CLK_ENABLE()
#define PORT_DQ		GPIOD
#define PIN_DQ		GPIO_PIN_1


#if 1 /* �⺯����ʽ */
	#define DQ_0()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ , GPIO_PIN_RESET)
	#define DQ_1()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ , GPIO_PIN_SET)

	/* �ж�DQ�����Ƿ�Ϊ�� GPIO_PIN_RESET*/
	#define DQ_IS_LOW()	(HAL_GPIO_ReadPin(PORT_DQ, PIN_DQ) == GPIO_PIN_RESET)	
	
#else	/* ֱ�Ӳ����Ĵ���������ٶ� */
//	#define DQ_0()		PORT_DQ->BSRRH = PIN_DQ
//	#define DQ_1()		PORT_DQ->BSRR = PIN_DQ
	#define DQ_0()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ, GPIO_PIN_SET)
	#define DQ_1()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ, GPIO_PIN_RESET)
	/* �ж�DQ�����Ƿ�Ϊ�� */
	#define DQ_IS_LOW()	((PORT_DQ->IDR & PIN_DQ) == 0)
#endif


/* ����GPIO�˿� */
#define PIN_DQ1		GPIO_PIN_0


#if 1 /* �⺯����ʽ */
	#define DQ1_0()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1 , GPIO_PIN_RESET)
	#define DQ1_1()		HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1 , GPIO_PIN_SET)

	/* �ж�DQ�����Ƿ�Ϊ�� GPIO_PIN_RESET*/
	#define DQ1_IS_LOW()	(HAL_GPIO_ReadPin(PORT_DQ, PIN_DQ1) == GPIO_PIN_RESET)	
	
#else	/* ֱ�Ӳ����Ĵ���������ٶ� */
//	#define DQ_0()		PORT_DQ->BSRRH = PIN_DQ
//	#define DQ_1()		PORT_DQ->BSRR = PIN_DQ
	#define DQ1_0()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1, GPIO_PIN_SET)
	#define DQ1_1()	HAL_GPIO_WritePin(PORT_DQ, PIN_DQ1, GPIO_PIN_RESET)
	/* �ж�DQ�����Ƿ�Ϊ�� */
	#define DQ1_IS_LOW()	((PORT_DQ->IDR & PIN_DQ1) == 0)
#endif


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDS18B20
*	����˵��: ����STM32��GPIO��SPI�ӿڣ��������� DS18B20
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDS18B20(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	RCC_DQ;

	DQ_1();
	DQ1_1();
	
	GPIO_InitStruct.Pin       = PIN_DQ | PIN_DQ1;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;	/* ��Ϊ��©ģʽ */
	GPIO_InitStruct.Pull      = GPIO_NOPULL;			/* ���������費ʹ�� */
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_DQ, &GPIO_InitStruct);	
}

/*
*********************************************************************************************************
*	�� �� ��: DS18B20_Reset
*	����˵��: ��λDS18B20�� ����DQΪ�ͣ���������480us��Ȼ��ȴ�
*	��    ��: ѡ��18B20���
*	�� �� ֵ: 0 ʧ�ܣ� 1 ��ʾ�ɹ�
*********************************************************************************************************
*/
unsigned char DS18B20_Reset_Choose(unsigned char num)
{
	/*
		��λʱ��, ��DS18B20 page 15

		������������DQ���������� 480us
		Ȼ���ͷ�DQ���ȴ�DQ�������������ߣ�Լ 15-60us
		DS18B20 ������DQΪ�� 60-240us�� ����źŽ� presence pulse  (��λ����,��ʾDS18B20׼������ ���Խ�������)
		���������⵽�����Ӧ���źţ���ʾDS18B20��λ�ɹ�
	*/
	uint8_t i = 0;
	uint16_t k = 0;

	DISABLE_INT();/* ��ֹȫ���ж� */
	
	if(num == 0)
	{
		/* ��λ�����ʧ���򷵻�0 */
		for (i = 0; i < 1; i++)
		{
			DQ_0();				/* ����DQ */
			bsp_DelayUS(520);	/* �ӳ� 520uS�� Ҫ������ӳٴ��� 480us */
			DQ_1();				/* �ͷ�DQ */

			bsp_DelayUS(15);	/* �ȴ�15us */

			/* ���DQ��ƽ�Ƿ�Ϊ�� */
			for (k = 0; k < 10; k++)
			{
				if (DQ_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* �ȴ�65us */
			}
			if (k >= 10)
			{
				continue;		/* ʧ�� */
			}

			/* �ȴ�DS18B20�ͷ�DQ */
			for (k = 0; k < 30; k++)
			{
				if (!DQ_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* �ȴ�65us */
			}
			if (k >= 30)
			{
				continue;		/* ʧ�� */
			}

			break;
		}
	}
	else if(num == 1)
	{
				/* ��λ�����ʧ���򷵻�0 */
		for (i = 0; i < 1; i++)
		{
			DQ1_0();				/* ����DQ */
			bsp_DelayUS(520);	/* �ӳ� 520uS�� Ҫ������ӳٴ��� 480us */
			DQ1_1();				/* �ͷ�DQ */

			bsp_DelayUS(15);	/* �ȴ�15us */

			/* ���DQ��ƽ�Ƿ�Ϊ�� */
			for (k = 0; k < 10; k++)
			{
				if (DQ1_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* �ȴ�65us */
			}
			if (k >= 10)
			{
				continue;		/* ʧ�� */
			}

			/* �ȴ�DS18B20�ͷ�DQ */
			for (k = 0; k < 30; k++)
			{
				if (!DQ1_IS_LOW())
				{
					break;
				}
				bsp_DelayUS(10);	/* �ȴ�65us */
			}
			if (k >= 30)
			{
				continue;		/* ʧ�� */
			}

			break;
		}	
	}
	
	ENABLE_INT();	/* ʹ��ȫ���ж� */
	bsp_DelayUS(5);
	if (i >= 1)
	{
		return 0;
	}

	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: DS18B20_WriteByte
*	����˵��: ��DS18B20д��1�ֽ�����
*	��    ��: ѡ��18B20���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DS18B20_WriteByte_Choose(unsigned char _val , unsigned char num)
{
	/*
		д����ʱ��, ��DS18B20 page 16
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
*	�� �� ��: DS18B20_ReadByte
*	����˵��: ��DS18B20��ȡ1�ֽ�����
*	��    ��: ѡ��DS18B20���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static unsigned char DS18B20_ReadByte_Choose(unsigned char num)
{
	unsigned char ucreturn = 0;
	/*
		д����ʱ��, ��DS18B20 page 16
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
*	�� �� ��: DS18B20_ReadTempReg
*	����˵��: ���¶ȼĴ�����ֵ��ԭʼ���ݣ�
*	��    ��: ѡ��DS18B20���
*	�� �� ֵ: �¶ȼĴ������� ������16�õ� 1���϶ȵ�λ, Ҳ����С����ǰ�������)
*********************************************************************************************************
*/
unsigned short DS18B20_ReadTempReg_Choose(unsigned char num)
{
	uint8_t temp1, temp2;

	/* ���߸�λ */
	if (DS18B20_Reset_Choose(num) == 0)
	{
		return 0;
	}	
	DS18B20_WriteByte_Choose(0xcc , num);
	DS18B20_WriteByte_Choose(0x44 , num);

	DS18B20_Reset_Choose(num);

	DS18B20_WriteByte_Choose(0xcc , num);
	DS18B20_WriteByte_Choose(0xbe , num);

	temp1 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */
	temp2 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */

	return ((temp2 << 8) | temp1);	/* ����16λ�Ĵ���ֵ */
}



/*
*********************************************************************************************************
*	�� �� ��: DS18B20_ReadTempRegCRC
*	����˵��: ���¶ȼĴ�����ֵ��ԭʼ���ݣ�����CRCУ�顣
*	��    ��: ��
*	�� �� ֵ: �¶ȼĴ������� ������16�õ� 1���϶ȵ�λ, Ҳ����С����ǰ�������)
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
	
	/* ���߸�λ */
	if (DS18B20_Reset_Choose(num) == 0)
	{
		return 0;
	}		

	DS18B20_WriteByte_Choose(0xcc , num);	/* ������ */
	DS18B20_WriteByte_Choose(0x44 , num);	/* ��ת������ */

	DS18B20_Reset_Choose(num);		/* ���߸�λ */

	DS18B20_WriteByte_Choose(0xcc , num);	/* ������ */
	DS18B20_WriteByte_Choose(0xbe , num);

	temp[0] = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */
	temp[1] = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */

	temp[2] = DS18B20_ReadByte_Choose(num);	/* Alarm High Byte    */
	temp[3] = DS18B20_ReadByte_Choose(num);	/* Alarm Low Byte     */
	temp[4] = DS18B20_ReadByte_Choose(num);	/* Reserved Byte 1    */
	temp[5] = DS18B20_ReadByte_Choose(num);	/* Reserved Byte 2    */
	temp[6] = DS18B20_ReadByte_Choose(num);	/* Count Remain Byte  */
	temp[7] = DS18B20_ReadByte_Choose(num);	/* Count Per degree C */
    crc = DS18B20_ReadByte_Choose(num);

	/* ��ߵ�125���Ӧ0x07D0�����ﷵ��0x0800��ʾ����*/
	if(crc != dallas_crc8(temp, 8))
	{
		return 0x800;
	}
	
	return ((temp[1] << 8) | temp[0]);	/* ����16λ�Ĵ���ֵ */
}

/*
*********************************************************************************************************
*	�� �� ��: DS18B20_ReadID
*	����˵��: ��DS18B20��ROM ID�� �����ϱ���ֻ��1��оƬ
*	��    ��: _id �洢ID
*	�� �� ֵ: 0 ��ʾʧ�ܣ� 1��ʾ��⵽��ȷID
*********************************************************************************************************
*/
unsigned char DS18B20_ReadID_Choose(unsigned char *_id , unsigned char num)
{
	unsigned char i = 0;
	unsigned char ucreturn = 0;
	if(num == 0)
	{
		/* ���߸�λ */
		if (DS18B20_Reset_Choose(num) == 0)
		{
			ucreturn = 0; 
		}
		DS18B20_WriteByte_Choose(0x33 , num);/* ������ */
		for (i = 0; i < 8; i++)
		{
			_id[i] = DS18B20_ReadByte_Choose(num);
		}

		DS18B20_Reset_Choose(num);		/* ���߸�λ */
		
		ucreturn = 1;
	}
	else if(num == 1)
	{
		/* ���߸�λ */
		if (DS18B20_Reset_Choose(num) == 0)
		{
			ucreturn = 0; 
		}
		DS18B20_WriteByte_Choose(0x33 , num);/* ������ */
		for (i = 0; i < 8; i++)
		{
			_id[i] = DS18B20_ReadByte_Choose(num);
		}

		DS18B20_Reset_Choose(num);		/* ���߸�λ */				
		ucreturn = 1;
	}
	return ucreturn;
}


/*
*********************************************************************************************************
*	�� �� ��: DS18B20_ReadTempByID
*	����˵��: ��ָ��ID���¶ȼĴ�����ֵ��ԭʼ���ݣ�
*	��    ��: ��
*	�� �� ֵ: �¶ȼĴ������� ������16�õ� 1���϶ȵ�λ, Ҳ����С����ǰ�������)
*********************************************************************************************************
*/
unsigned short DS18B20_ReadTempByID_Choose(unsigned char *_id , unsigned char num)
{
	unsigned char temp1 = 0 , temp2 = 0;
	unsigned char i = 0;
	unsigned short ucretu = 0;

//	if(num == 0)
//	{
		DS18B20_Reset_Choose(num);		/* ���߸�λ */

		DS18B20_WriteByte_Choose(0x55 , num);	/* ������ */
		for (i = 0; i < 8; i++)
		{
			DS18B20_WriteByte_Choose(_id[i] , num);
		}
		DS18B20_WriteByte_Choose(0x44 , num);	/* ��ת������ */

		DS18B20_Reset_Choose(num);		/* ���߸�λ */

		DS18B20_WriteByte_Choose(0x55 , num);	/* ������ */
		for (i = 0; i < 8; i++)
		{
			DS18B20_WriteByte_Choose(_id[i] , num);
		}	
		DS18B20_WriteByte_Choose(0xbe , num);

		temp1 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */
		temp2 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */

		ucretu = ((temp2 << 8) | temp1);	/* ����16λ�Ĵ���ֵ */
//	}
//	else if(num == 1)
//	{
//		DS18B20_Reset_Choose(num);		/* ���߸�λ */

//		DS18B20_WriteByte_Choose(0x55 , num);	/* ������ */
//		for (i = 0; i < 8; i++)
//		{
//			DS18B20_WriteByte_Choose(_id[i] , num);
//		}
//		DS18B20_WriteByte_Choose(0x44 , num);	/* ��ת������ */

//		DS18B20_Reset_Choose(num);		/* ���߸�λ */

//		DS18B20_WriteByte_Choose(0x55 , num);	/* ������ */
//		for (i = 0; i < 8; i++)
//		{
//			DS18B20_WriteByte_Choose(_id[i] , num);
//		}	
//		DS18B20_WriteByte_Choose(0xbe , num);

//		temp1 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */
//		temp2 = DS18B20_ReadByte_Choose(num);	/* ���¶�ֵ���ֽ� */

//		ucretu = ((temp2 << 8) | temp1);	/* ����16λ�Ĵ���ֵ */
//	}
	return ucretu;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
