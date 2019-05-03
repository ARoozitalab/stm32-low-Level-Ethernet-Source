/**
  ******************************************************************************
  * @file    main.c 
  * @author  alireza roozitalab
  * @version V1.0.0
  * @date    08 Feb. 2014
  * @brief   Main program body
	* @Email   alireza.roozitalab@gmail.com
  ******************************************************************************

  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4x7_eth_bsp.h"
#include "stm32f4x7_eth.h"
#include <string.h>
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB];/* Ethernet Rx MA Descriptor */
extern ETH_DMADESCTypeDef  DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Tx DMA Descriptor */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; /* Ethernet Receive Buffer */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; /* Ethernet Transmit Buffer */
uint8_t data[512]={0};
uint8_t *uip_buf;
uint8_t uip_buf2[1000]={0};
extern __IO ETH_DMADESCTypeDef  *DMATxDescToSet;
extern __IO ETH_DMADESCTypeDef  *DMARxDescToGet;
  __IO ETH_DMADESCTypeDef *DMARxNextDesc;
FrameTypeDef ali;
uint16_t ident=0;
#define MAC0   2
#define MAC1   0
#define MAC2   0
#define MAC3   0
#define MAC4   0
#define MAC5   0
 
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP0 	192
#define IP1 	168
#define IP2 	10
#define IP3 	10

#define TYPE_ARP 0x0806
#define TYPE_ICMP 0x0800
#define ICMP_PROTOCOL  0x01
#define TCP_PROTOCOL  0x06

void headercheksumn()
{
int i=0;
	uint32_t checksum=0;
	uint16_t data1=0;
	checksum=(data[14]<<8)|data[15];
	for(i=16;i<33;i=i+2)
	{
		data1=(data[i]<<8)|data[i+1];
	checksum=checksum+data1;
		if(checksum>65535)
		{
		checksum=(checksum) &(0xffff);
		checksum++;
		}
	}
	
checksum=~(checksum);	
data[24]=(checksum&0xff00)>>8;
data[25]=	(checksum&0xff);
}



void normcheksumn()
{
int i=0;
	uint32_t checksum=0;
	uint16_t data1=0;
	checksum=(data[34]<<8)|data[35];
	for(i=36;i<ali.length;i=i+2)
	{
		data1=(data[i]<<8)|data[i+1];
	checksum=checksum+data1;
		if(checksum>65535)
		{
		checksum=(checksum) &(0xffff);
		checksum++;
		}
	}
	
checksum=~(checksum);	
data[36]=(checksum&0xff00)>>8;
data[37]=	(checksum&0xff);
}






void arp_send(void)
{

/*MAC DEST*/
data[0]=uip_buf[6];
data[1]=uip_buf[7];
data[2]=uip_buf[8];
data[3]=uip_buf[9];
data[4]=uip_buf[10];
data[5]=uip_buf[11];
	
/*MAC SRC*/	
data[6]=MAC0;
data[7]=MAC1;
data[8]=MAC2;
data[9]=MAC3;
data[10]=MAC4;
data[11]=MAC5;
	
/*TYPE*/	
data[12]=((TYPE_ARP&0XFF00)>>8);
data[13]=(TYPE_ARP & 0XFF);
	
/*H TYPE*/
data[14]=0X00;
data[15]=0X01;

/*PRPTPCOL TYPE*/
data[16]=0X08;
data[17]=0X00;

/*H SIZE*/
data[18]=0X06;

/*PROTOCOL SIZE*/
data[19]=0X04;

/*OP CODE (RESPONDE)*/
data[20]=0X00;
data[21]=0X02;

/*MAC SRC*/	
data[22]=MAC0;
data[23]=MAC1;
data[24]=MAC2;
data[25]=MAC3;
data[26]=MAC4;
data[27]=MAC5;

/*IP SRC*/
data[28]=IP0;
data[29]=IP1;
data[30]=IP2;
data[31]=IP3;

/*TARGET MAC*/
data[32]=uip_buf[6];
data[33]=uip_buf[7];
data[34]=uip_buf[8];
data[35]=uip_buf[9];
data[36]=uip_buf[10];
data[37]=uip_buf[11];

/*TARGET IP*/
data[38]=uip_buf[28];
data[39]=uip_buf[29];
data[40]=uip_buf[30];
data[41]=uip_buf[31];
ETH_HandleTxPkt(data,42);
uip_buf[41]=0;
}




void icmp_send()
{
	char i;
	char j;
data[0]=uip_buf[6];
data[1]=uip_buf[7];
data[2]=uip_buf[8];
data[3]=uip_buf[9];
data[4]=uip_buf[10];
data[5]=uip_buf[11];
	
/*MAC SRC*/	
data[6]=MAC0;
data[7]=MAC1;
data[8]=MAC2;
data[9]=MAC3;
data[10]=MAC4;
data[11]=MAC5; 

/*TYPE*/	
data[12]=((TYPE_ICMP&0XFF00)>>8);
data[13]=(TYPE_ICMP & 0XFF);
	
/* HEADER LENGHT*/
data[14]=0X45;

/* DIFFRENTIATED SERVVICE FILED*/
data[15]=0X00;

/*TOTAL LENGHT*/
data[16]=0X00;
data[17]=0X3C;

/*IDENTIFICATION*/
data[18]=uip_buf[18];
data[19]=uip_buf[19];

/*FLAG*/
data[20]=0;

/*FRAGMENT OFFSET*/
data[21]=0;

/*TIME TO LIVE*/
data[22]=uip_buf[22];

/*PROTOCOL*/
data[23]=ICMP_PROTOCOL;

/*PLACE OF CHECK SUM HEADER*/
data[24]=0;
data[25]=0;

/*SRC IP*/
data[26]=IP0;
data[27]=IP1;
data[28]=IP2;
data[29]=IP3;

/*DEC IP*/
data[30]=uip_buf[26];
data[31]=uip_buf[27];
data[32]=uip_buf[28];
data[33]=uip_buf[29];

/*TYPE (REPLY)*/
data[34]=0;

/*CODE*/
data[35]=0;

/*IDENTIFICATION BE*/
data[38]=0X00;
data[39]=0X01;

/*SEQUENCE CODE*/
data[40]=uip_buf[40];
data[41]=uip_buf[41];
j=0x61;
for(i=42;i<75;i++)
{
data[i]=uip_buf[i];
}
	data[36]=0;
	data[37]=0;	
/*CHECKSUM HEADER*/
headercheksumn();
/*NORM CHECK SUM*/
normcheksumn();

 ETH_HandleTxPkt(data,74);

		 uip_buf[1]=0x00;
}




void tcp_send(void)
{
	char i;
	uint32_t ack;
data[0]=uip_buf[6];
data[1]=uip_buf[7];
data[2]=uip_buf[8];
data[3]=uip_buf[9];
data[4]=uip_buf[10];
data[5]=uip_buf[11];
	
/*MAC SRC*/	
data[6]=MAC0;
data[7]=MAC1;
data[8]=MAC2;
data[9]=MAC3;
data[10]=MAC4;
data[11]=MAC5; 

/*TYPE*/	
data[12]=((TYPE_ICMP&0XFF00)>>8);
data[13]=(TYPE_ICMP & 0XFF);
	
/* HEADER LENGHT*/
data[14]=0X45;

/* DIFFRENTIATED SERVVICE FILED*/
data[15]=0X00;

/*TOTAL LENGHT*/
if(uip_buf[47]==0x02){
data[16]=0X00;
data[17]=0X2C;
}
if(uip_buf[47]==0x18){
data[16]=0X00;
data[17]=0X28;
}
/*IDENTIFICATION*/
data[18]=((ident&0XFF00)>>8);
data[19]=(ident & 0XFF);
ident++;
/*FLAG*/
data[20]=0;

/*FRAGMENT OFFSET*/
data[21]=0;

/*TIME TO LIVE*/
data[22]=0xff;//uip_buf[22];

/*PROTOCOL*/	
data[23]=TCP_PROTOCOL;

/*PLACE OF CHECK SUM HEADER*/


/*SRC IP*/
data[26]=IP0;
data[27]=IP1;
data[28]=IP2;
data[29]=IP3;

/*DEC IP*/
data[30]=uip_buf[26];
data[31]=uip_buf[27];
data[32]=uip_buf[28];
data[33]=uip_buf[29];

/*port src*/
data[34]=0x01;
data[35]=0xf6;

/*port dec*/
data[36]=uip_buf[34];
data[37]=uip_buf[35];


ack=(uip_buf[38]<<24)|(uip_buf[39]<<16)|(uip_buf[40]<<8)|(uip_buf[41]);
if(uip_buf[47]==0x02){
/*seq num*/
data[38]=0x51;
data[39]=0xC7;
data[40]=0x02;
data[41]=0x00;
ack=ack+1;
}
if(uip_buf[47]==0x18){
/*seq num*/
data[38]=uip_buf[42];
data[39]=uip_buf[43];
data[40]=uip_buf[44];
data[41]=uip_buf[45];
ack=ack+12;
}
/*ack num*/
data[42]=((ack&0xff000000)>>24);
data[43]=((ack&0xff0000)>>16);
data[44]=((ack&0xff00)>>8);
data[45]=((ack&0xff));

/*header lenght*/
if(uip_buf[47]==0x02)
data[46]=0x60;
if(uip_buf[47]==0x18)
data[46]=0x50;
/*flag*/
if(uip_buf[47]==0x02)
data[47]=0x12;
if(uip_buf[47]==0x18)
data[47]=0x10;

/*windows size*/
data[48]=0x02;
data[49]=0x00;


/*place of the check sum*/
data[52]=0x00;
data[53]=0x00;
/*options*/
if(uip_buf[47]==0x02){
data[54]=0x02;
data[55]=0x04;
data[56]=0x05;
data[57]=0xb4;
}
if(uip_buf[47]==0x18){
data[54]=0x00;
data[55]=0x00;
data[56]=0x00;
data[57]=0x00;
}


}



int main(void)
{
	uint8_t hwaddr[6];
	int i;
	hwaddr[0] =  MAC0;
  hwaddr[1] =  MAC1;
  hwaddr[2] =  MAC2;
  hwaddr[3] =  MAC3;
  hwaddr[4] =  MAC4;
  hwaddr[5] =  MAC5;
  ETH_BSP_Config();

	ETH_MACAddressConfig(ETH_MAC_Address0,hwaddr); 
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
	  ETH_Start();
  while (1)
  {
		
		
   if (ETH_CheckFrameReceived())
    {
			ali=ETH_Get_Received_Frame();
			uip_buf=(u8 *)ali.buffer;
			 memcpy(uip_buf2,uip_buf,ali.length);
			 
	if((uip_buf[0]==0xff)&(uip_buf[1]==0xff)&(uip_buf[2]==0xff)&(uip_buf[3]==0xff)&
	(uip_buf[4]==0xff)&(uip_buf[5]==0xff)&(uip_buf[38]==IP0)&(uip_buf[39]==IP1)&&
	(uip_buf[40]==IP2)&(uip_buf[41]==IP3))
	{
	 arp_send();
	}
	else if((uip_buf[0]==MAC0)&(uip_buf[1]==MAC1)&(uip_buf[2]==MAC2)&(uip_buf[3]==MAC3)&(uip_buf[4]==MAC4)&
		(uip_buf[5]==MAC5)&(uip_buf[23]==ICMP_PROTOCOL))
	{
		icmp_send();
	}

  if (DMA_RX_FRAME_infos->Seg_Count > 1)
  {
    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
  }
  else
  {
    DMARxNextDesc = ali.descriptor;
  }
  
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++)
  {  
    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
  }
  
  /* Clear Segment_Count */
  DMA_RX_FRAME_infos->Seg_Count =0;
  
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
	}
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 


