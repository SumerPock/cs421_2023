#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: AppTaskPCCommun
*	����˵��: ��λ��
*	��    ��: ��
*	�� �� ֵ: ��
* 	�� �� ��:   
*********************************************************************************************************
*/
//void AppTaskPCCommun(void *argument)
//{	
////	const uint16_t usFrequency = 50; /* �ӳ����� */
////	uint32_t tick;
////	/* ��ȡ��ǰʱ�� */
////	tick = osKernelGetTickCount();		
////	osStatus_t os_Status;
////	unsigned int Flag = 0;
////	unsigned int GetPCcomFlag = 0;
////	
////	static unsigned char msg_PcGetNetwork_Number = 0;
////	static unsigned char msg_PcGetTime_Number = 0;
////	
////	PcSetNetwork pcgetnet = {0};
////	PcSetTimer pcgettim   = {0};
////	
////	unsigned char GetPCcommBuff[13] = {0xEA , 0xED , 0x30 , 0x0D , 0X01 , 0X01 , 0X00 , 0X00 , 0X00  , 0X00 , 0X00 , 0X00 , 0X00};
//	
//	while(1)
//	{			
////		unsigned char getnum = 0;
////		Flag = osEventFlagsGet(event_usart1_ID);	
////		GetPCcomFlag = osEventFlagsGet(event_SetPcCommon_ID);
////		
////		msg_PcGetNetwork_Number =  osMessageQueueGetCount(msgQueue_PcGetNetwork_Data);
////		msg_PcGetTime_Number =  osMessageQueueGetCount(msgQueue_PcGetTime_Data);
//		
////		if(Flag == 1)
////		{
////			getnum = Get_PC_Data(g_usart1_rx_buf , g_tBspUsart1.rxLen);
////			switch(getnum)
////			{
////				case 0:
////					break;
////				
////				case 1://��ʱ��
////					osEventFlagsSet(event_RK3588Sinal_ID , 0x06U);
////					break;
////				
////				case 2://���ַ
////					osEventFlagsSet(event_RK3588Sinal_ID , 0x08U);
////					break;
////				
////				case 3://��ѯ����
////					break;
////				
////				case 4://����IP
////					osEventFlagsSet(event_RK3588Sinal_ID , 0x09U);
////					break;
////				
////				case 5://����ʱ��
////					osEventFlagsSet(event_RK3588Sinal_ID , 0x0AU);
////					break;
////				
////				case 6://���ô���
////					break;
////				
////				default:
////					break;
////			}
////			memset(g_usart1_rx_buf, zero, sizeof(g_usart1_rx_buf));
////			if(osEventFlagsClear(event_usart1_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource))
////			{
////				
////			}
////		}
//		
//		
//		//if(GetPCcomFlag != 0)
//		{
//			static unsigned char loop = 0;
//			//switch(GetPCcomFlag)
//			{
////				case 0x01://��ʱ
////					if(msg_PcGetTime_Number > 0)
////					{
////						os_Status = osMessageQueueGet(msgQueue_PcGetTime_Data,/* ��Ϣ���е����� */
////													  &pcgettim,    		  /* �����Ϣ���ڴ� */
////													  NULL,              	  /* ȡ����Ϣ�����ȼ� */
////													  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
////						
////						*(GetPCcommBuff + 4) = loop++;
////						*(GetPCcommBuff + 5) = 0x02;
////						*(GetPCcommBuff + 6) = pcgettim.year;
////						*(GetPCcommBuff + 7) = pcgettim.moon;
////						*(GetPCcommBuff + 8) = pcgettim.day;
////						*(GetPCcommBuff + 9) = pcgettim.hour;
////						*(GetPCcommBuff + 10) = pcgettim.point;
////						*(GetPCcommBuff + 11) = pcgettim.second;
////						
////						*(GetPCcommBuff + 12) = checksum(GetPCcommBuff, sizeof(GetPCcommBuff) - 1);
////						
////						//BSP_UART_Transmit_IT(&husart1 , &GetPCcommBuff[0] , sizeof(GetPCcommBuff));
////					}
////					
////					#if DEBUG_MODE == 1  
////						printf("time");
////					#endif						
////					
////					if(osEventFlagsClear(event_SetPcCommon_ID , 0x01U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
////					break;
//				
////				case 0x02://����ip
////					if(msg_PcGetNetwork_Number > 0)
////					{
////						os_Status = osMessageQueueGet(msgQueue_PcGetNetwork_Data,/* ��Ϣ���е����� */
////													  &pcgetnet,    		  /* �����Ϣ���ڴ� */
////													  NULL,              	  /* ȡ����Ϣ�����ȼ� */
////													  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
////						*(GetPCcommBuff + 4) = loop++;
////						*(GetPCcommBuff + 5) = 0x01;
////						*(GetPCcommBuff + 6) = pcgetnet.ip_1;
////						*(GetPCcommBuff + 7) = pcgetnet.ip_2;
////						*(GetPCcommBuff + 8) = pcgetnet.ip_3;
////						*(GetPCcommBuff + 9) = pcgetnet.ip_4;
////						*(GetPCcommBuff + 10) = pcgetnet.local_port >> 8;
////						*(GetPCcommBuff + 11) = pcgetnet.local_port;
////						
////						*(GetPCcommBuff + 12) = checksum(GetPCcommBuff, sizeof(GetPCcommBuff) - 1);
////						
////						//BSP_UART_Transmit_IT(&husart1 , &GetPCcommBuff[0] , sizeof(GetPCcommBuff));
////					}		
////					#if DEBUG_MODE == 1  
////						printf("set ip");
////					#endif	
////					
////					if(osEventFlagsClear(event_SetPcCommon_ID , 0x02U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
////					break;
//				
////				case 0x03://��ѯip
////					if(msg_PcGetNetwork_Number > 0)
////					{
////						os_Status = osMessageQueueGet(msgQueue_PcGetNetwork_Data,/* ��Ϣ���е����� */
////													  &pcgetnet,    		  /* �����Ϣ���ڴ� */
////													  NULL,              	  /* ȡ����Ϣ�����ȼ� */
////													  osWaitForever  	      /* ��ʱʱ�䣬���жϺ��������osMessageQueueGetʱ���˲���������NULL */);
////						*(GetPCcommBuff + 4) = loop++;
////						*(GetPCcommBuff + 5) = 0x01;
////						*(GetPCcommBuff + 6) = pcgetnet.ip_1;
////						*(GetPCcommBuff + 7) = pcgetnet.ip_2;
////						*(GetPCcommBuff + 8) = pcgetnet.ip_3;
////						*(GetPCcommBuff + 9) = pcgetnet.ip_4;
////						*(GetPCcommBuff + 10) = pcgetnet.local_port >> 8;
////						*(GetPCcommBuff + 11) = pcgetnet.local_port;
////						
////						*(GetPCcommBuff + 12) = checksum(GetPCcommBuff, sizeof(GetPCcommBuff) - 1);
////						
////						//BSP_UART_Transmit_IT(&husart1 , &GetPCcommBuff[0] , sizeof(GetPCcommBuff));
////					}
////					
////					#if DEBUG_MODE == 1  
////						printf("chick ip");
////					#endif						
//					
////					if(osEventFlagsClear(event_SetPcCommon_ID , 0x03U) != (osFlagsErrorUnknown|osFlagsErrorParameter|osFlagsErrorResource)){}
////					break;	

////				default:
////					break;				
//			}
//		}
//		
////		tick += usFrequency;                          
////		osDelayUntil(tick);	
//	}
//}


/*
*********************************************************************************************************
*	�� �� ��: Get_PC_Data
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
* 	�� �� ��:   
*********************************************************************************************************
*/
//unsigned char Get_PC_Data(unsigned char *GetBuff , unsigned char GetBLen)
//{
////	unsigned char ucreturn = 0;
////	unsigned char getbuff[25] = {0};
////	memcpy(getbuff , GetBuff , GetBLen);
////	
////	osStatus_t os_Status;
////	unsigned char getcheck = 0;
////	
////	PcSetNetwork pcsetnet = {0};
////	PcSetTimer pcsettim = {0};
////	
////	if(*(getbuff + 0) == 0x55 && *(getbuff + 1) == 0xaa)
////	{
////		switch(*(getbuff + 2))
////		{
////			case 0x10://��ѯ
////				if(*(getbuff + 3) == GetBLen)
////				{
////					switch(*(getbuff + 5))
////					{
////						case 0x01://��ѯʱ��
////							getcheck = checksum(getbuff, GetBLen - 1);
////							if(getcheck == *(getbuff + 6))
////							{
////								ucreturn = 1;
////								
////								#if DEBUG_MODE == 1  
////									printf("this is time");
////								#endif									
////								
////							}
////							break;
////						
////						case 0x03://��ѯIP��ַ
////							getcheck = checksum(getbuff, GetBLen - 1);
////							if(getcheck == *(getbuff + 6))
////							{
////								ucreturn = 2;
////								
////								#if DEBUG_MODE == 1  
////									printf("this is ip");
////								#endif	
////							}
////							break;
////						
////						default:
////							ucreturn = 3;
////							break;
////					}
////				}
////				break;
////			
////			case 0x20://����
////				if(*(getbuff + 3) == GetBLen)
////				{
////					switch(*(getbuff + 5))
////					{
////						case 0x01://ip
////							getcheck = checksum(getbuff, GetBLen - 1);
////							if(getcheck == *(getbuff + 12))
////							{
////								/*��Ϣ���*/
////								pcsetnet.ip_1 = *(getbuff + 6);
////								pcsetnet.ip_2 = *(getbuff + 7);
////								pcsetnet.ip_3 = *(getbuff + 8);
////								pcsetnet.ip_4 = *(getbuff + 9);
////								pcsetnet.local_port = ((*(getbuff + 10)) << 8) | *(getbuff + 11);
////								
////								os_Status = osMessageQueuePut(msgQueue_PcSetNetwork_Data, 	/* ��Ϣ���е����� */
////															  &pcsetnet, 					/* ������Ϣ���еı�������Ϣ�� */
////															  NULL,      					/* ������Ϣ�����ȼ� */
////															  NULL      	 				/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
////															 );	
////								#if DEBUG_MODE == 1  
////									printf("set is ip");
////								#endif	
////								ucreturn = 4;
////							}
////							break;
////						
////						case 0x02://ʱ��
////							getcheck = checksum(getbuff, GetBLen - 1);
////							if(getcheck == *(getbuff + 12))
////							{
////								/*��Ϣ���*/
////								pcsettim.year = *(getbuff + 6);
////								pcsettim.moon = *(getbuff + 7);
////								pcsettim.day = *(getbuff + 8);
////								pcsettim.hour =  *(getbuff + 9);
////								pcsettim.point = *(getbuff + 10);
////								pcsettim.second =  *(getbuff + 11);
////								
////								os_Status = osMessageQueuePut(msgQueue_PcSetTime_Data, 	/* ��Ϣ���е����� */
////															  &pcsettim, 				/* ������Ϣ���еı�������Ϣ�� */
////															  NULL,      				/* ������Ϣ�����ȼ� */
////															  NULL      	 			/* ��ʱʱ��Ϊ0�����osMessageQueuePut���жϻص���������ʱ���˲���������NULL */
////															  );									
////								#if DEBUG_MODE == 1  
////									printf("set is ip");
////								#endif	
////								
////								ucreturn = 5;
////							}
////							break;
////						
////						default:
////							ucreturn = 6;
////							break;
////					}
////				}
////				break;
////			
////			default:
////				break;	
////		}
////	}
////	return ucreturn;
//}



