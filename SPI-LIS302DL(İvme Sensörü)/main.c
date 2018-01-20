#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int8_t x,y,z;
char  str[50]; 
uint32_t i;
//Tx SPI param: adress, data
void SPI_Tx(uint8_t adress, uint8_t data)
{
	// adress:  Open the LIS302DL datasheet to find out the adress of a certain register.
  GPIO_ResetBits(GPIOE,GPIO_Pin_3);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,adress);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
	
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,data);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
	
  GPIO_SetBits(GPIOE,GPIO_Pin_3);
   
}
 
//Rx SPI param: adress, return data
 uint8_t SPI_Rx(uint8_t adress)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_3); //CS pini lojik 0 a çekildi
  adress=(0x80) | (adress); //  this tells the sensor to read and not to write, that's where the (0x80 | adress) comes from.
	
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,adress);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
	
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,0x00);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1); 
  GPIO_SetBits(GPIOE,GPIO_Pin_3); //CS pini lojik 1 e çekildi
  return SPI_I2S_ReceiveData(SPI1);
}
 void USART_Puts(USART_TypeDef* USARTx,volatile char *s) // char karakter sayisi kadar döndürüyor
{
 while(*s)
 {
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	 USART_SendData(USARTx,*s);
	 *s++;
 }	 
}

int main()
{
GPIO_InitTypeDef GPIO_InitTypeStructure;
USART_InitTypeDef USART_InitStructure;
SPI_InitTypeDef SPI_InitTypeDefStruct;
	
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOD, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // transmitter tx A2 GPIOya bagli usartta

/*-------- Configuring SCK, MISO, MOSI --------*/

GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_AF;
GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
GPIO_Init(GPIOA, &GPIO_InitTypeStructure);
 
 
/*-------- Configuring ChipSelect-Pin PE3 --------*/ 
	
GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_3;
GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_OUT;
GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
GPIO_Init(GPIOE, &GPIO_InitTypeStructure);
 
GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_OUT;
GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
GPIO_Init(GPIOD, &GPIO_InitTypeStructure);
 
GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
 

SPI_InitTypeDefStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2; //Defines the clock speed of our SPI - set it to maximum here (Prescaler 2)
SPI_InitTypeDefStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex; //Unidirectional or bidirectional. We need bidirectional here for read and write.
SPI_InitTypeDefStruct.SPI_Mode=SPI_Mode_Master; //Master or Slave - our stm32f4 shoulb be the master here!
SPI_InitTypeDefStruct.SPI_DataSize=SPI_DataSize_8b; //You can send 8 Bits and 16 Bits - we use 8 Bits.
SPI_InitTypeDefStruct.SPI_FirstBit=SPI_FirstBit_MSB; //Starting with MSB or LSB ? - LIS302DL datasheet -> its MSB !
SPI_InitTypeDefStruct.SPI_CPOL=SPI_CPOL_High; //Clock polarity - I set this to High, as it is in the read&write protocol in LIS302DL datasheet.
SPI_InitTypeDefStruct.SPI_CPHA=SPI_CPHA_2Edge; //Defines the edge for bit capture - I use 2nd edge.
SPI_InitTypeDefStruct.SPI_NSS=SPI_NSS_Soft|SPI_NSSInternalSoft_Set; // Chip select hardware/sofware - I set this so software
SPI_Init(SPI1,&SPI_InitTypeDefStruct);
SPI_Cmd(SPI1,ENABLE);


GPIO_InitTypeStructure.GPIO_Pin = GPIO_Pin_2;
GPIO_InitTypeStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitTypeStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitTypeStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitTypeStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_Init(GPIOA, & GPIO_InitTypeStructure);

GPIO_PinAFConfig(GPIOA , GPIO_PinSource2, GPIO_AF_USART2);

USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Tx;
USART_InitStructure.USART_Parity = USART_Parity_No ;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_Init(USART2,& USART_InitStructure);
USART_Cmd(USART2, ENABLE);
 
GPIO_SetBits(GPIOE,GPIO_Pin_3);

  SPI_Tx(0x20, 0x67); //100hz 9.2g
  //SPI_Tx(0x20, 0x47); //100hz 2.3g
// SPI_Tx(0x24, 0x48);
 

while(1)
 {
  //Rx SPI data
  x=SPI_Rx(0x29);
  y=SPI_Rx(0x2B);
	z=SPI_Rx(0x2D);
	 if(x<-20) GPIO_SetBits(GPIOD,GPIO_Pin_12);
	 else GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	 if(x>20) GPIO_SetBits(GPIOD,GPIO_Pin_14);
	 else GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	 if(x<-20) GPIO_SetBits(GPIOD,GPIO_Pin_15);
	 else GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	 if(x>20) GPIO_SetBits(GPIOD,GPIO_Pin_13);
	 else GPIO_ResetBits(GPIOD,GPIO_Pin_13);
  //use data
	// sprintf(str,"x=%d , y=%d ,z=%d \n", x, y,z);
	 
	sprintf(str,"%d \n", x);
	USART_Puts(USART2,str);
	sprintf(str,"%d \n", y);
  USART_Puts(USART2,str);
	sprintf(str,"%d \n", z);
  USART_Puts(USART2,str);
	 	
	 i=600000;
		while(i)
			i--;
  
 }
}