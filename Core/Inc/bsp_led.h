/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

/* 供外部调用的函数声明 */
void bsp_InitLed(void);

void bsp_LedOn(unsigned char _no);

void bsp_LedOff(unsigned char _no);

void bsp_LedToggle(unsigned char _no);

unsigned char bsp_IsLedOn(unsigned char _no);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
