/*
*********************************************************************************************************
*
*	ģ������ : lwip����Э��ջ����
*	�ļ����� : app_lwip_lib.c
*	��    �� : V1.0
*	˵    �� : lwip�������򵥲���
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2019-12-06   Eric2013     �׷�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                                  �궨��
*********************************************************************************************************
*/
#define PORT_NUM       1001    /* TCP�����������˿ں� */


/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
struct netif netif;


/*
*********************************************************************************************************
*	�� �� ��: LwIPTest
*	����˵��: LwIPTestӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
unsigned char net_init(void) 
{
	unsigned char statr = 0;
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip4_addr ipaddr;						//ip��ַ
	struct ip4_addr netmask;					//��������
	struct ip4_addr gw;								//Ĭ������ 

	/* ����Э��ջ�߳� */
	tcpip_init(NULL, NULL);

#if LWIP_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

	Netif_Init_Flag = netif_add(&netif ,\
															&ipaddr ,\
															&netmask ,\
															&gw ,\
															NULL ,\
															&ethernetif_init ,
															&ethernet_input);//�������б������һ������
	/* ������ӿ���ӵ�netif_list�б� */
	if(Netif_Init_Flag == NULL){
		statr = 3;//�������ʧ�� 
	}else{
		/*  ע��Ĭ������ӿ� */
		netif_set_default(&netif);
		
		/* Set the link callback function, this function is called on change of link status*/
		netif_set_up(&netif);
		statr = 0;
	}	
	#if LWIP_DHCP
		dhcp_start (&netif);
	#endif
		return statr;//����OK.
}

/*
*********************************************************************************************************
*	�� �� ��: LwIPTest
*	����˵��: LwIPTestӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
void LwIPTest(void)
{
	udp_echoserver_init();
//	struct netconn *conn, *newconn;
//	err_t err;

//	/* ���������� */
//	conn = netconn_new(0X10);
//	netconn_bind(conn, IP_ADDR_ANY, PORT_NUM);

//	/* ���ü��� */
//	netconn_listen(conn);
//	
//	while (1) 
//	{
//		/* �ȴ������µ����� */
//		err = netconn_accept(conn, &newconn);

//		/* ���������� */
//		if (err == ERR_OK) 
//		{
//			struct netbuf *buf;
//			void *data;
//			u16_t len;

//			/* �򵥵����ݻػ� */
//			while ((err = netconn_recv(newconn, &buf)) == ERR_OK) 
//			{
//		        do {
//		             netbuf_data(buf, &data, &len);
//		             err = netconn_write(newconn, data, len, 0x01);
//		        } while (netbuf_next(buf) >= 0);
//				netbuf_delete(buf);
//			}
//			
//			/* ɾ������  */
//			netconn_close(newconn);
//			netconn_delete(newconn);
//		}

//	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
