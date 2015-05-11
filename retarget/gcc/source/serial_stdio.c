#include "serial_stdio.h"

void serial_puts(Serial_t serial ,const char * pString){
	char newChar; 
	int i ;
	for(i = 0 ; i < 81; i++){
		newChar = pString[i];
		if( newChar != '\0' ){
			serial.sendChar(newChar);
		}else{
			break;
		}
	}
}

int serial_gets(Serial_t  serial,char * pBuffer, int bufferSize){
	char newChar; 
	int i ;
	for(i = 0 ; i < (bufferSize - 1); i++){
		newChar = serial.getChar();
		if( (newChar != '\r') ){
			pBuffer[i]=newChar;
		}else{
			pBuffer[i]='\0';
			return i;
		}
	}
	pBuffer[bufferSize-1]='\0';
	return bufferSize-1;
}

void serial_printf(Serial_t  serial, const char * format ,...){
	char * tempBuffer;
	va_list args;
	va_start(args, format);
	vasprintf(&tempBuffer,format, args);
	va_end(args);
	serial_puts(serial,tempBuffer);
	free(tempBuffer);
}
