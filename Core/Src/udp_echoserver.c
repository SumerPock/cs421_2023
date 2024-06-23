/**
  ******************************************************************************
  * @file    udp_echoserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
//#include "ip_addr.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    15   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    30   /* define the UDP remote connection port */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const struct ip4_addr *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
				//comSendBuf(4, "Set a receive callback for the upcb", 20);
      }
      else
      {
        udp_remove(upcb);
				//comSendBuf(4, "can not bind pcb", 20);
				//printf("can not bind pcb");
      }
   }
   else
   {
    //printf("can not create pcb");
		//comSendBuf(4, "can not create pcb", 20);
   } 
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
uint8_t receivebuf[30] = {0};
uint8_t send_buf[10] = {0x23 , 0x01 , 0X02 , 0X03 , 0X04 , 0X05 , 0X06 , 0X07 , 0XEE};
int receivelen = 0;
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,const struct ip4_addr *addr, u16_t port)
{
	/*这里面的顺序很重要，打乱会造成UDP发送失败*/
	struct pbuf *pq;
	receivelen = p->len;
	memcpy(receivebuf,p->payload,p->len);

  udp_connect(upcb , addr , UDP_CLIENT_PORT);
  pq = pbuf_alloc(PBUF_TRANSPORT , 8 , PBUF_POOL);//allocate memory
	pbuf_take(pq , (char*)send_buf , 8);//copy data to buf
	udp_send(upcb , pq);//send udp data
	udp_disconnect(upcb);
	pbuf_free(pq);   
  pbuf_free(p);	
	
//  /* Connect to the remote client */
//  udp_connect(upcb, addr, UDP_CLIENT_PORT);
//    
//  /* Tell the client that we have accepted it */
//  udp_send(upcb, p);

//  /* free the UDP connection, so we can accept new clients */
//  udp_disconnect(upcb);
//	
//  /* Free the p buffer */
//  pbuf_free(p);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
