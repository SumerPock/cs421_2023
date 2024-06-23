/*
*********************************************************************************************************
*
*	ģ������ : DS18B20 ����ģ��(1-wire �����¶ȴ�������
*	�ļ����� : bsp_ds18b20.h
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_DS18B20_H
#define _BSP_DS18B20_H

void bsp_InitDS18B20(void);

unsigned short DS18B20_ReadTempRegCRC_Choose(unsigned char num);

unsigned short DS18B20_ReadTempByID_Choose(unsigned char *_id , unsigned char num);


unsigned char DS18B20_ReadID_Choose(unsigned char *_id , unsigned char num);

unsigned short DS18B20_ReadTempRegCRC_Choose(unsigned char num);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
