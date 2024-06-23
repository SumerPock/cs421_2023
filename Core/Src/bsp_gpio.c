#include "bsp.h"

/* ���ƶ˿ڶ�Ӧ��RCCʱ�� */
#define GPIO_GPIO_CLK_ALLENABLE() {	  \
		__HAL_RCC_GPIOC_CLK_ENABLE(); \
		__HAL_RCC_GPIOD_CLK_ENABLE(); \
		__HAL_RCC_GPIOE_CLK_ENABLE(); \
		__HAL_RCC_GPIOA_CLK_ENABLE(); \
	};

	
/*SIG P1*/
#define GPIO_PORT_SIG1  	GPIOD
#define GPIO_PIN_SIG1		GPIO_PIN_10
	
/*SIG P2*/
#define GPIO_PORT_SIG2  	GPIOC
#define GPIO_PIN_SIG2		GPIO_PIN_9
	
/*SIG P3*/
#define GPIO_PORT_SIG3  	GPIOD
#define GPIO_PIN_SIG3		GPIO_PIN_11
	
/*SIG P4*/
#define GPIO_PORT_SIG4  	GPIOC
#define GPIO_PIN_SIG4		GPIO_PIN_8

/*SIG P5*/
#define GPIO_PORT_SIG5  	GPIOA
#define GPIO_PIN_SIG5		GPIO_PIN_8

/*SIG_CTRL3*/
#define GPIO_PORT_SIG_CTRL3   GPIOE
#define GPIO_PIN_SIG_CTRL3	  GPIO_PIN_4


#define GPIO_PORT_SIG_POW   GPIOE
#define GPIO_PIN_SIG_POW	GPIO_PIN_10

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitGpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* ��GPIOʱ�� */
	GPIO_GPIO_CLK_ALLENABLE();

	/*
	�������е�LEDָʾ��GPIOΪ�������ģʽ
	���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0����˻�����LED����.
	�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ���ǰ���ȹر�LEDָʾ��
	*/
	bsp_GpioOff(1);
	bsp_GpioOff(2);
	bsp_GpioOff(3);
	bsp_GpioOff(4);
	bsp_GpioOff(5);
	bsp_GpioOff(6);

	bsp_GpioOn(RK3588POWER);
	
	/* ����GPIO */
	GPIO_InitStruct.Pin = GPIO_PIN_SIG1 | GPIO_PIN_SIG3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* ����������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* ���������費ʹ�� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */
	HAL_GPIO_Init(GPIO_PORT_SIG3 , &GPIO_InitStruct);


	GPIO_InitStruct.Pin = GPIO_PIN_SIG2 | GPIO_PIN_SIG4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		
	GPIO_InitStruct.Pull = GPIO_NOPULL;               	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		
	HAL_GPIO_Init(GPIO_PORT_SIG2 , &GPIO_InitStruct);


	GPIO_InitStruct.Pin = GPIO_PIN_SIG5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		
	GPIO_InitStruct.Pull = GPIO_NOPULL;              
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		
	HAL_GPIO_Init(GPIO_PORT_SIG5 , &GPIO_InitStruct);


	GPIO_InitStruct.Pin = GPIO_PIN_SIG_CTRL3 | GPIO_PIN_SIG_POW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		
	GPIO_InitStruct.Pull = GPIO_NOPULL;              
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		
	HAL_GPIO_Init(GPIO_PORT_SIG_CTRL3 , &GPIO_InitStruct);	
}


void bsp_GpioOn(unsigned char _no)
{
	_no--;
	if (_no == 0)
	{
		GPIO_PORT_SIG1->BSRR = GPIO_PIN_SIG1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_SIG2->BSRR = GPIO_PIN_SIG2;
	}	
	else if (_no == 2)
	{
		GPIO_PORT_SIG3->BSRR = GPIO_PIN_SIG3;
	}
	else if (_no == 3)
	{
		GPIO_PORT_SIG4->BSRR = GPIO_PIN_SIG4;
	}
	else if (_no == 4)
	{
		GPIO_PORT_SIG5->BSRR = GPIO_PIN_SIG5;
	}
	else if (_no == 5)
	{
		GPIO_PORT_SIG_CTRL3->BSRR = GPIO_PIN_SIG_CTRL3;
	}
	else if(_no == 6)
	{
		GPIO_PORT_SIG_POW->BSRR = GPIO_PIN_SIG_POW;
	}
}


void bsp_GpioOff(unsigned char _no)
{
	_no--;
	if (_no == 0)
	{
		GPIO_PORT_SIG1->BSRR = (uint32_t)GPIO_PIN_SIG1 << 16U;
	}
	else if (_no == 1)
	{
		GPIO_PORT_SIG2->BSRR = (uint32_t)GPIO_PIN_SIG2 << 16U;
	}	
	else if (_no == 2)
	{
		GPIO_PORT_SIG3->BSRR = (uint32_t)GPIO_PIN_SIG3 << 16U;
	}
	else if (_no == 3)
	{
		GPIO_PORT_SIG4->BSRR = (uint32_t)GPIO_PIN_SIG4 << 16U;
	}
	else if (_no == 4)
	{
		GPIO_PORT_SIG5->BSRR = (uint32_t)GPIO_PIN_SIG5 << 16U;
	}
	else if (_no == 5)
	{
		GPIO_PORT_SIG_CTRL3->BSRR = (uint32_t)GPIO_PIN_SIG_CTRL3 << 16U;
	}	
	else if(_no == 6)
	{
		GPIO_PORT_SIG_POW->BSRR = (uint32_t)GPIO_PIN_SIG_POW << 16U;
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_GpioToggle(unsigned char _no)
{
	if (_no == 1)
	{
		GPIO_PORT_SIG1->ODR ^= GPIO_PIN_SIG1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_SIG2->ODR ^= GPIO_PIN_SIG2;
	}	
	else if (_no == 3)
	{
		GPIO_PORT_SIG3->ODR ^= GPIO_PIN_SIG3;
	}
	else if (_no == 4)
	{
		GPIO_PORT_SIG4->ODR ^= GPIO_PIN_SIG4;
	}
	else if (_no == 5)
	{
		GPIO_PORT_SIG5->ODR ^= GPIO_PIN_SIG5;
	}
	else if (_no == 6)
	{
		GPIO_PORT_SIG_CTRL3->ODR ^= GPIO_PIN_SIG_CTRL3;
	}
	else if (_no == 7)
	{
		GPIO_PORT_SIG_POW->ODR ^= GPIO_PIN_SIG_POW;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
unsigned char bsp_IsGpioOn(unsigned char _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_SIG1->ODR & GPIO_PIN_SIG1) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_SIG2->ODR & GPIO_PIN_SIG2) == 1)
		{
			return 1;
		}
		return 0;
	}	
	else if (_no == 3)
	{
		if ((GPIO_PORT_SIG3->ODR & GPIO_PIN_SIG3) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 4)
	{
		if ((GPIO_PORT_SIG4->ODR & GPIO_PIN_SIG4) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 5)
	{
		if ((GPIO_PORT_SIG5->ODR & GPIO_PIN_SIG5) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 6)
	{
		if ((GPIO_PORT_SIG_CTRL3->ODR & GPIO_PIN_SIG_CTRL3) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 7)
	{
		if ((GPIO_PORT_SIG_CTRL3->ODR & GPIO_PIN_SIG_CTRL3) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 6)
	{
		if ((GPIO_PORT_SIG_POW->ODR & GPIO_PIN_SIG_POW) == 1)
		{
			return 1;
		}
		return 0;
	}	
	return 0;
}

