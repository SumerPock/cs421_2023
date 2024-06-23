/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

/* ���ⲿ���õĺ������� */
void bsp_InitLed(void);

void bsp_LedOn(unsigned char _no);

void bsp_LedOff(unsigned char _no);

void bsp_LedToggle(unsigned char _no);

unsigned char bsp_IsLedOn(unsigned char _no);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
