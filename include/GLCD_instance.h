#ifndef __GLCD_INSTANCE_H__
	#define __GLCD_INSTANCE_H__
	
	/*******************************************************************************/
	/*******************************************************************************/
	//Constantes necesarias para interfaz GLCD
	#define GLCD_PIN_E 0 // GLCD Enable strobe
	#define GLCD_PIN_RW 1 //GLCD Write/Read PIN
	#define GLCD_PIN_RS 2 //GLCD Register Select

	#define GLCD_RW_WRITE 0	//
	#define GLCD_RW_READ 1	//
	#define GLCD_RS_DATA 0	//
	#define GLCD_RS_INSTRUCTION 1	//

	
	
	
	/**
	* @brief This function must write a byte on the pins conected on the data bus of the GLCD  D[0..7]
	*/
	#ifdef __GLCD_C__ 
	extern
	#endif	
	void __GLCD_Put_Data(char data);
	/**
	* @brief This function must write the value "pin_state" bit on the "control_pin" of the GLCD display 
	* @param control_pin is a 
	*/
	#ifdef __GLCD_C__ 
	extern
	#endif
	void __GLCD_CTRL_Pin_Write(unsigned char control_pin,unsigned char pin_state);
	/**
	*
	*/
	#ifdef __GLCD_C__ 
	extern
	#endif
	void __GLCD_Configure_Pins(void);
	
#endif
	