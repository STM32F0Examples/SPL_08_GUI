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

void myCallback(GUI_pushButton_t * handle, int event);

#define B_X_START 10
#define B_Y_START 28
#define B_X_INC 22
#define B_Y_INC 18

GUI_pushButton_t buttons[] = {
	{B_X_START + B_X_INC*0, B_Y_START + B_Y_INC*0, "1", myCallback},
	{B_X_START + B_X_INC*1, B_Y_START + B_Y_INC*0, "2", myCallback},
	{B_X_START + B_X_INC*2, B_Y_START + B_Y_INC*0, "3", myCallback},
	{B_X_START + B_X_INC*0, B_Y_START + B_Y_INC*1, "4", myCallback},
	{B_X_START + B_X_INC*1, B_Y_START + B_Y_INC*1, "5", myCallback},
	{B_X_START + B_X_INC*2, B_Y_START + B_Y_INC*1, "6", myCallback},
	{B_X_START + B_X_INC*0, B_Y_START + B_Y_INC*2, "7", myCallback},
	{B_X_START + B_X_INC*1, B_Y_START + B_Y_INC*2, "8", myCallback},
	{B_X_START + B_X_INC*2, B_Y_START + B_Y_INC*2, "9", myCallback},
	{B_X_START + B_X_INC*3, B_Y_START + B_Y_INC*0, "0", myCallback},
	{B_X_START + B_X_INC*3, B_Y_START + B_Y_INC*1, "-", myCallback},
	{B_X_START + B_X_INC*3, B_Y_START + B_Y_INC*2, "+", myCallback},
	{B_X_START + B_X_INC*4, B_Y_START + B_Y_INC*0, "V", myCallback},
	{B_X_START + B_X_INC*4, B_Y_START + B_Y_INC*1, "S", myCallback},
	{B_X_START + B_X_INC*4, B_Y_START + B_Y_INC*2, "R", myCallback},
	{B_X_START + B_X_INC*5, B_Y_START + B_Y_INC*0, "ENTER", myCallback},
	{B_X_START + B_X_INC*5, B_Y_START + B_Y_INC*1, " DEL ", myCallback},
};

int main(void) {
	setToMaxSpeed();
	UART2_init(9600);
	serial_printf(UART2_serial, "\nUSART2 ready\n");
	GUI_init();
	//GUI_calibrate();
	serial_printf(UART2_serial, "GUI Environment ready\n");
	serial_printf(UART2_serial, "xOff = %d.%d\n", (int) x_offset, (int) (x_offset * 100 - ((int) x_offset)*100));
	serial_printf(UART2_serial, "xScale = %d.%d\n", (int) x_scale, (int) (x_scale * 100 - ((int) x_scale)*100));
	serial_printf(UART2_serial, "yOff = %d.%d\n", (int) y_offset, (int) (y_offset * 100 - ((int) y_offset)*100));
	serial_printf(UART2_serial, "yScale = %d.%d\n", (int) y_scale, (int) (y_scale * 100 - ((int) y_scale)*100));

	for (int i = 0; i < (sizeof (buttons) / sizeof (GUI_pushButton_t)); i++) {
		GUI_addButton(&buttons[i]);
	}

	while (1) {
		GUI_scanScreen();
	}
}

void myCallback(GUI_pushButton_t * handle, int event) {
	const char * message;

	switch (event) {
		case GUI_EVENT_PRESSED:
			message = "pressed";
			break;
		case GUI_EVENT_RELEASED:
			message = "released";
			break;
		case GUI_EVENT_CLICKED:
			message = "clicked";
			break;
		default:
			message = "unknown event";
	}

	for (int i = 0; i < (sizeof (buttons) / sizeof (GUI_pushButton_t)); i++) {
		if (handle == &buttons[i]) {
			if (event == GUI_EVENT_CLICKED) serial_printf(UART2_serial, "button \"%s\" %s\n", handle->text, message);
		}
	}
}

void setToMaxSpeed(void) {
	int internalClockCounter;
	RCC_PLLCmd(DISABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
	RCC_PREDIV1Config(RCC_PREDIV1_Div1);
	RCC_PLLCmd(ENABLE);
	while (!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	for (internalClockCounter = 0; internalClockCounter < 1024; internalClockCounter++) {
		if (RCC_GetSYSCLKSource() == RCC_SYSCLKSource_PLLCLK) {
			SystemCoreClockUpdate();
			break;
		}
	}
}
