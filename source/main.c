#include "stm32f0xx.h"                  // Device header
#include "retarget_STM32F0.h"
#include "serial_stdio.h"
#include "GUI.h"
#include "rTouch.h"

void setToMaxSpeed(void);
Serial_t UART2_serial = {UART2_getChar, UART2_sendChar};

#define IN_BUFFER_SIZE 80
char inputBuffer[IN_BUFFER_SIZE];

extern float x_offset;
extern float x_scale;
extern float y_offset;
extern float y_scale;

int main(void)
{
	setToMaxSpeed();
	UART2_init(9600);
	serial_printf(UART2_serial,"\nUSART2 ready\n");
	GUI_init();
	GUI_calibrate();
	serial_printf(UART2_serial,"GUI Environment ready\n");
	serial_printf(UART2_serial,"xOff = %d\n", (int) x_offset);
	serial_printf(UART2_serial,"xScale = %d\n", (int) x_scale);
	serial_printf(UART2_serial,"yOff = %d\n", (int) y_offset);
	serial_printf(UART2_serial,"yScale = %d\n", (int) y_scale);
	while(1){
		GUI_waitForPress(-1);
		serial_printf(UART2_serial,"X=%d\tY=%d\n",GUI_readX(),GUI_readY());
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
