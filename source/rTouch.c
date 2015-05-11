#include "rTouch.h"
#include "stm32f0xx.h"                  // Device header
#include "ARM_delay.h"


/*ADC DMA STUFF*/
void configureADC(void);
void startConversion(void);
int ADC_results[4];
int trasferComplete=0;

void floatPin(unsigned short GPIO_PINS);
void drivePin(unsigned short GPIO_PINS);
void configureSPLYPins(void);
void pullDownPin(unsigned short GPIO_PINS);
int detect_touch(void);
int rawX,rawY;
void readXY(void);
int detectAndRead(void);


void rTouch_init(void){
	configureSPLYPins();
	configureADC();
	NVIC_EnableIRQ(DMA1_Ch1_IRQn);
}

int rTouch_waitToPress(int timeout){
	if(timeout < 0){
		while(!detectAndRead());//wait forever
		return 1;
	}
	else if(timeout == 0){
		return detectAndRead();
	}
	else{
		for(int i = 0; i<timeout ; i++){
			if(detectAndRead()){
				return 1;
			}
			delay_ms(timeout);
		}
		return 0;
	} 
}

int rTouch_waitToRelease(int timeout){
	if(timeout < 0){
		while(detectAndRead());//wait forever
		return 1;
	}
	else if(timeout == 0){
		return !detectAndRead();
	}
	else{
		for(int i = 0; i<timeout ; i++){
			if(!detectAndRead()){
				return 1;
			}
			delay_ms(timeout);
		}
		return 0;
	}
}

int rTouch_readX(void){
	return rawX;
}

int rTouch_readY(void){
	return rawY;
}


void floatPin(unsigned short GPIO_PINS){
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Mode=GPIO_Mode_IN;
	myGPIO.GPIO_OType=GPIO_OType_PP;
	myGPIO.GPIO_Speed=GPIO_Speed_50MHz;
	myGPIO.GPIO_PuPd=GPIO_PuPd_NOPULL;
	myGPIO.GPIO_Pin=GPIO_PINS;
	GPIO_Init(GPIOA,&myGPIO);
}

void drivePin(unsigned short GPIO_PINS){
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);	
	myGPIO.GPIO_Mode=GPIO_Mode_OUT;
	myGPIO.GPIO_OType=GPIO_OType_PP;
	myGPIO.GPIO_Speed=GPIO_Speed_50MHz;
	myGPIO.GPIO_PuPd=GPIO_PuPd_NOPULL;
	myGPIO.GPIO_Pin=GPIO_PINS;
	GPIO_Init(GPIOA,&myGPIO);
}
void configureSPLYPins(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);	
	floatPin(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
}

void pullDownPin(unsigned short GPIO_PINS){
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);	
	myGPIO.GPIO_Mode=GPIO_Mode_IN;
	myGPIO.GPIO_OType=GPIO_OType_PP;
	myGPIO.GPIO_Speed=GPIO_Speed_50MHz;
	myGPIO.GPIO_PuPd=GPIO_PuPd_DOWN;
	myGPIO.GPIO_Pin=GPIO_PINS;
	GPIO_Init(GPIOA,&myGPIO);
}

int detect_touch(void){
	floatPin(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	pullDownPin(GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	drivePin(GPIO_Pin_6);
	delay_us(500);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)){
		floatPin(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
		return 1;
	}else{
		floatPin(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
		return 0;
	}
}



void readXY(void){	
	//Prepare Pins for reading X axis
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	floatPin(GPIO_Pin_4|GPIO_Pin_5);
	drivePin(GPIO_Pin_6|GPIO_Pin_7);
	delay_us(250);
	startConversion();
	while(!trasferComplete);
	rawX=ADC_results[0];

	//Prepare Pins for reading X axis
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	floatPin(GPIO_Pin_6|GPIO_Pin_7);
	drivePin(GPIO_Pin_4|GPIO_Pin_5);
	delay_us(250);
	startConversion();
	while(!trasferComplete);
	rawY=ADC_results[1];	
}

int detectAndRead(){
	if(detect_touch()){
		readXY();
		return 1;
	}else{
		return 0;
	}
}

void configureADC(){

	ADC_InitTypeDef myADC_Init;
	DMA_InitTypeDef myDMA_Init;
	//Configure PB0 and PB1 as analog input
	GPIO_InitTypeDef myAnalogPin;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);//Turn on Port B
	ADC_StructInit(&myADC_Init);
	myAnalogPin.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;//configure PB0 and PB1 at the same time
	myAnalogPin.GPIO_Mode=GPIO_Mode_AN;//Pins as anlog pins
	GPIO_Init(GPIOA,&myAnalogPin);//init gpio port with configuration
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//Turn on peripherial ADC1
	/*Calibrate ADC*/
	ADC_Cmd(ADC1,DISABLE);
	ADC_GetCalibrationFactor(ADC1);
	ADC_DMARequestModeConfig(ADC1,ADC_DMAMode_OneShot);
	ADC_DMACmd(ADC1,ENABLE);
	//
	
	//Configure Init Structure
	ADC_StructInit(&myADC_Init);
	myADC_Init.ADC_ScanDirection=ADC_ScanDirection_Upward;
	myADC_Init.ADC_DataAlign=ADC_DataAlign_Right;
	myADC_Init.ADC_Resolution=ADC_Resolution_10b;
	myADC_Init.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ADC_Init(ADC1,&myADC_Init);
	ADC_Cmd(ADC1,ENABLE);
	//
	ADC_ChannelConfig(ADC1,ADC_Channel_8,ADC_SampleTime_239_5Cycles);
	ADC_ChannelConfig(ADC1,ADC_Channel_9,ADC_SampleTime_239_5Cycles);
	//Enable EndOfConverison Interrupts generation
	//ADC_WaitModeCmd(ADC1,ENABLE);
	
	//
	//ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_StructInit(&myDMA_Init);
	myDMA_Init.DMA_M2M=DMA_M2M_Disable;
	myDMA_Init.DMA_Priority=DMA_Priority_Medium;
	myDMA_Init.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
	myDMA_Init.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
	myDMA_Init.DMA_MemoryInc=DMA_MemoryInc_Enable;
	myDMA_Init.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	myDMA_Init.DMA_Mode=DMA_Mode_Normal;
	myDMA_Init.DMA_DIR=DMA_DIR_PeripheralSRC;
	myDMA_Init.DMA_MemoryBaseAddr=(unsigned int)&ADC_results;
	myDMA_Init.DMA_PeripheralBaseAddr=(unsigned int)(&(ADC1->DR));
	myDMA_Init.DMA_BufferSize=2;

	DMA_Init(DMA1_Channel1,&myDMA_Init);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
}

void startConversion(void){
	trasferComplete=0;
	DMA_Cmd(DMA1_Channel1,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1,2);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_StartOfConversion(ADC1);
}

void DMA1_Channel1_IRQHandler(void){
	DMA_ClearITPendingBit(DMA_IT_TC);//clear trasfer complete flag
	trasferComplete=1;
} 

