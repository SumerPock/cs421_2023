/*
*********************************************************************************************************
*
*	ģ������ : FDCAN����ģ��
*	�ļ����� : bsp_can.c
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "bsp.h"

#define CAN_IT_TME                  ((uint32_t)CAN_IER_TMEIE)   /*!< Transmit mailbox empty interrupt */

 /* Error Interrupts */                                                                                                            
 #define CAN_IT_EWG                  ((uint32_t)CAN_IER_EWGIE) /*!< Error warning interrupt   */     
 #define CAN_IT_EPV                  ((uint32_t)CAN_IER_EPVIE) /*!< Error passive interrupt   */     
 #define CAN_IT_BOF                  ((uint32_t)CAN_IER_BOFIE) /*!< Bus-off interrupt         */     
 #define CAN_IT_LEC                  ((uint32_t)CAN_IER_LECIE) /*!< Last error code interrupt */     
 #define CAN_IT_ERR                  ((uint32_t)CAN_IER_ERRIE) /*!< Error Interrupt           */ 

/*��ʼ��CAN GPIO*/
void Init_HardCanGpioConfig(void);
/*����CAN�������*/
unsigned char CAN_Mode_Init(CAN_InitTypeDef* can_Init , CAN_HandleTypeDef* canx_handler , CAN_TypeDef* CanChanNum);
/*����CAN�˲���*/
unsigned char CAN_Filter_Config(CAN_HandleTypeDef* canx_handler , CAN_FilterTypeDef* canx_filter);
/*CAN���緢�ͺ���*/
unsigned char CAN_Send_Mesage(CAN_HandleTypeDef can_hand , CAN_TxHeaderTypeDef* cse ,unsigned char(*ucp)[8]);

void ConfigStm32HalCan(void);

void bsp_InitCan1(void);
void bsp_InitCan2(void);
void bsp_DeInitCan1(void);
void bsp_DeInitCan2(void);

void can1_SendPacket(uint8_t *_DataBuf, uint8_t _Len);
void can2_SendPacket(uint8_t *_DataBuf, uint8_t _Len);

/*��ʼ��CAN GPIO*/
extern void Init_HardCanGpioConfig(void);
/*����CAN�������*/
unsigned char CAN_Mode_Init(CAN_InitTypeDef* can_Init , CAN_HandleTypeDef* canx_handler , CAN_TypeDef* CanChanNum);
/*����CAN�˲���*/
extern unsigned char CAN_Filter_Config(CAN_HandleTypeDef* canx_handler , CAN_FilterTypeDef* canx_filter);
/*CAN���緢�ͺ���*/
unsigned char CAN_Send_Mesage(CAN_HandleTypeDef can_hand , CAN_TxHeaderTypeDef* cse ,unsigned char(*ucp)[8]);

extern void ConfigStm32HalCan(void);

#endif 

