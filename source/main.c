#include "stm32f0xx.h"                  // Device header
#include "retarget_STM32F0.h"
#include "serial_stdio.h"
#include "GUI.h"
#include "rTouch.h"
#include "GLCD.h"

void setToMaxSpeed(void);
Serial_t UART2_serial = {UART2_getChar, UART2_sendChar};

#define IN_BUFFER_SIZE 80
char inputBuffer[IN_BUFFER_SIZE];

extern float x_offset;
extern float x_scale;
extern float y_offset;
extern float y_scale;

void myCallback(GUI_pushButton_t * handle ,int event);

#define nButtons 10
GUI_pushButton_t buttons[nButtons]={{10,10,"B1",myCallback},{35,10,"B2",myCallback},{60,10,"B3",myCallback},
	{10,30,"B4",myCallback},{35,30,"B5",myCallback},{60,30,"B6",myCallback},
	{10,50,"B7",myCallback},{35,50,"B8",myCallback},{60,50,"B9",myCallback},
	{10,70,"    B0    ",myCallback}};


int main(void)
{
	setToMaxSpeed();
	UART2_init(9600);
	serial_printf(UART2_serial,"\nUSART2 ready\n");
	GUI_init();
	//GUI_calibrate();
	serial_printf(UART2_serial,"GUI Environment ready\n");
	serial_printf(UART2_serial,"xOff = %f\n",  x_offset);
	serial_printf(UART2_serial,"xScale = %f\n", x_scale);
	serial_printf(UART2_serial,"yOff = %f\n",  y_offset);
	serial_printf(UART2_serial,"yScale = %f\n", y_scale);
	
	for(int i=0; i<(sizeof(buttons)/sizeof(GUI_pushButton_t)); i++){
		GUI_addButton(&buttons[i]);
	}
	
	while(1){
		GUI_scanScreen();
	}	
}

void myCallback(GUI_pushButton_t * handle ,int event){
	const char * message;
	
	switch(event){
		case GUI_EVENT_PRESSED:
			message="pressed";
			break;
		case GUI_EVENT_RELEASED:
			message="released";
			break;
		case GUI_EVENT_CLICKED:
			message="clicked";
			break;
		default :
			message="unknown event";
	}
	
	for(int i=0; i<(sizeof(buttons)/sizeof(GUI_pushButton_t)); i++){
		if(handle==&buttons[i]){
			serial_printf(UART2_serial,"button \"%s\" %s\n",handle->text,message);
		}
	}
}

void setToMaxSpeed(void){
		int internalClockCounter;
		RCC_PLLCmd(DISABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
		RCC_HSEConfig(RCC_HSE_OFF);
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
		RCC_PREDIV1Config(RCC_PREDIV1_Div1);
		RCC_PLLCmd(ENABLE);
		while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		for(internalClockCounter=0;internalClockCounter<1024;internalClockCounter++){
			if(RCC_GetSYSCLKSource()==RCC_SYSCLKSource_PLLCLK){
				SystemCoreClockUpdate();
				break;
			}
		}		
}
