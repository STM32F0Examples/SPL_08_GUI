#include "GUI.h"
#include "rTouch.h"
#include "GLCD.h"
#include "ARM_delay.h"

#define CAL_DOT_SIZE 3
#define CAL_DOT_OFFSET 10

float const preset_x_offset= 120;
float const preset_x_scale= 0.1;
float const preset_y_offset= 180;
float const preset_y_scale= 0.1;

float x_offset;
float x_scale;
float y_offset;
float y_scale;



void GUI_init(void){
	glcd_init(GRAPHIC);
	rTouch_init();
	
	x_offset=preset_x_offset;
	x_scale=preset_x_scale;
	y_offset=preset_y_offset;
	y_scale=preset_y_scale;
}

#define N_READINGS 10
#define READING_TIME 1000 //in ms
#define readingsVariace .05 //in ms

void getCalibrationPoint(int x_cord,int y_cord,int * x_read,int * y_read){
	int readingsX[N_READINGS];
	int readingsY[N_READINGS];
	int sumX=0, sumY=0;
	int readingsTaken=0;
	glcd_fill_screen(OFF);
	glcd_circle(x_cord,y_cord,CAL_DOT_SIZE,YES,ON);	
	glcd_load_buffer();
	while(readingsTaken < N_READINGS){
		if(rTouch_waitToPress(0)){
			readingsX[readingsTaken]=rTouch_readX();
			readingsY[readingsTaken]=rTouch_readY();
			readingsTaken++;
		}else{
			readingsTaken=0;
		}
		delay_ms(READING_TIME/N_READINGS);
	}
	for(int i=0; i<N_READINGS; i++){
		sumX+=readingsX[i];
		sumY+=readingsY[i];
	}
	*x_read=sumX/N_READINGS;
	*y_read=sumY/N_READINGS;
}

int GUI_calibrate(void){
	int x_pos[4]={CAL_DOT_OFFSET,GLCD_WIDTH-CAL_DOT_OFFSET,GLCD_WIDTH-CAL_DOT_OFFSET,CAL_DOT_OFFSET};
	int y_pos[4]={CAL_DOT_OFFSET,CAL_DOT_OFFSET,GLCD_HEIGHT-CAL_DOT_OFFSET,GLCD_HEIGHT-CAL_DOT_OFFSET};
	int x_read[4];
	int y_read[4];
	for(int i = 0; i<4 ; i++){
		getCalibrationPoint(x_pos[i],y_pos[i],&x_read[i],&y_read[i]);
		delay_ms(500);
	}
	return 0;
}


