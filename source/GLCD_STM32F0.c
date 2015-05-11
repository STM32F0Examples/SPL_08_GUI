#include "GLCD_instance.h"
#include "stm32f0xx.h"     
#include "stm32f0xx_gpio.h" 
//Pin Map for this implementation on STM32F051K6 (TQFP32)
//All the listed below are 5V tolerant pins other microcontrollers may have more or less
//uC			GLCD
//***************
//PA8			D4
//PA9			D5
//PA10		D6
//PA11		D7
//PA12		RS
//PA13		SDIO	
//PA14 		SWCLK
//PA15		R/W
//PB3			E
//PB4			D0
//PB5			D1
//PB6			D2
//PB7			D3
//PF0			(AVAILABE FOR SWAPING)
//PF1     (AVAILABE FOR SWAPING)



void GLCD_LDATA(unsigned int L_NIBBLE){
		GPIO_SetBits(GPIOB,(L_NIBBLE<<4)&(0xF0));//Sets the 1's bits on low nibble 
		GPIO_ResetBits(GPIOB,(~(L_NIBBLE<<4))&(0xF0));//Resets the 0's bits on low nibble
}

void GLCD_HDATA(unsigned int H_NIBBLE){
	GPIO_SetBits(GPIOA,(H_NIBBLE<<8)&(0xF00));//Sets the 1's bits on high nibble
	GPIO_ResetBits(GPIOA,(~(H_NIBBLE<<8))&(0xF00));//Resets the 0's bits on low nibble
}

void __GLCD_Put_Data(char data){
	 GLCD_LDATA(data);//writes the data low nibble
	 GLCD_HDATA(data>>4);	//writes the data high nibble
}

void __GLCD_CTRL_Pin_Write(unsigned char control_pin,unsigned char pin_state){
		if(control_pin==GLCD_PIN_E){
			if(pin_state!=0){
				GPIO_SetBits(GPIOB,GPIO_Pin_3);	
			}else{
				GPIO_ResetBits(GPIOB,GPIO_Pin_3);
			}
		}else if(control_pin==GLCD_PIN_RW){
			if(pin_state!=0){
				GPIO_SetBits(GPIOA,GPIO_Pin_15);	
			}else{
				GPIO_ResetBits(GPIOA,GPIO_Pin_15);
			}
		}else if(control_pin==GLCD_PIN_RS){
			if(pin_state!=0){
				GPIO_SetBits(GPIOA,GPIO_Pin_12);	
			}else{
				GPIO_ResetBits(GPIOA,GPIO_Pin_12);
			}
		}
}

void __GLCD_Configure_Pins(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Pin B3 B4 B5 B6 B7 open drain output*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;//opendrain outputs
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//No pull up/down resistors
	GPIO_Init(GPIOB,&GPIO_InitStruct);//init the pins on PORT B
	
	//Pin A8 PA9 PA10 PA11 PA12 PA15 as open drain outputs
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;//opendrain outputs
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//No pull up/down resistors
	GPIO_Init(GPIOA,&GPIO_InitStruct);//init the pins on PORT A
}

