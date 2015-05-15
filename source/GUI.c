#include "GUI.h"
#include "rTouch.h"
#include "GLCD.h"
#include "ARM_delay.h"

#define CAL_DOT_SIZE 3
#define CAL_DOT_OFFSET 10

float const preset_x_offset= 86.96;
float const preset_x_scale= 5.17;
float const preset_y_offset= 110.25;
float const preset_y_scale= 9.7;

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
	glcd_fill_screen(OFF);
	glcd_load_buffer();
	rTouch_waitToRelease(-1);
}

int GUI_calibrate(void){
	int x_pos[4]={CAL_DOT_OFFSET,GLCD_WIDTH-CAL_DOT_OFFSET,GLCD_WIDTH-CAL_DOT_OFFSET,CAL_DOT_OFFSET};
	int y_pos[4]={CAL_DOT_OFFSET,CAL_DOT_OFFSET,GLCD_HEIGHT-CAL_DOT_OFFSET,GLCD_HEIGHT-CAL_DOT_OFFSET};
	int x_read[4];
	int y_read[4];
	float dr;
	float dp;
	
	for(int i = 0; i<4 ; i++){
		getCalibrationPoint(x_pos[i],y_pos[i],&(x_read[i]),&(y_read[i]));
		delay_ms(500);
	}
	
	dp=(((x_pos[1]+x_pos[2])-(x_pos[0]+x_pos[3]))/2);
	dr=(((x_read[1]+x_read[2])-(x_read[0]+x_read[3]))/2);
	x_scale=dr/dp;
	
	dp=(((y_pos[2]+y_pos[3])-(y_pos[0]+y_pos[1]))/2);
	dr=(((y_read[2]+y_read[3])-(y_read[0]+y_read[1]))/2);
	y_scale=dr/dp;
	
	x_offset=0;
	y_offset=0;
	
	for(int i = 0; i < 4; i++){
		x_offset+=x_read[i]-x_pos[i]*x_scale;
		y_offset+=y_read[i]-y_pos[i]*y_scale;;
	}
	x_offset/=4;
	y_offset/=4;
	
	return 1;
}

int validX=0;
int validY=0;

#define MAX_READS 7
#define MIN_READS 5

int getValidPoint(void){
	int readingsX[MAX_READS];
	int readingsY[MAX_READS];
	int sumX=0, sumY=0;
	int readingsTaken=0;
	for(int i=0; i < MAX_READS; i++){
		if(rTouch_waitToPress(0)){
			readingsX[readingsTaken]=rTouch_readX();
			readingsY[readingsTaken]=rTouch_readY();
			readingsTaken++;
		}
	}
	if(readingsTaken >= MIN_READS){
		for(int i=0; i<readingsTaken; i++){
			sumX+=readingsX[i];
			sumY+=readingsY[i];
		}
		validX=sumX/readingsTaken;
		validY=sumY/readingsTaken;
		return 1;
	}
	else{
		return 0;
	}
}

int GUI_waitForPress(int timeout){
	rTouch_waitToPress(timeout);
	return getValidPoint(); 
}

int GUI_readX(){
	return (validX - x_offset)/x_scale;
}
int GUI_readY(){
	return (validY - y_offset)/y_scale;
}

void GUI_drawPushButton(const char *a,unsigned char x, unsigned char y, unsigned char color,unsigned char state){
    char c;
    int i=0;
    while((c=a[i])!='\0')i++;
	glcd_rect(x-3,y-3,x+(i*6)+1,y+8+1,YES,!state);
    glcd_text57(x,y,a,1,state);
	glcd_rect(x-3,y-3,x+(i*6)+1,y+8+1,NO,state);
	glcd_rect(x-4,y-4,x+(i*6)+2,y+8+2,NO,!color);
}

int GUI_pushButton_isPressed(const char *a,unsigned char x, unsigned char y, int readX, int readY){
	int x_min, x_max, y_min, y_max;
	int i=0;
	
	while((a[i])!='\0') i++;
	
	x_min=x-5;
	y_min=y-5;
	x_max=x+(i*6)+3;
	y_max=y+8+3;
	
	return ((x_min<readX && readX<x_max) && (y_min<readY && readY<y_max));
}

#define MAX_BUTTONS	10

GUI_pushButton_t * registered_button[MAX_BUTTONS];
int nOfButtons=0;

void GUI_addButton(GUI_pushButton_t * newButton){
	registered_button[nOfButtons]=newButton;
	nOfButtons++;
}

int lastPressedButton=-1;
int lastSreenWasPressed=1;

void GUI_scanScreen(void){
	int currentPressedButton = -1;
	int currentScreenIsPressed = 0;
	int currentXPos,currentYPos;
	
	//Scan for changes
	currentScreenIsPressed=GUI_waitForPress(0);
	if(currentScreenIsPressed){
		currentXPos=GUI_readX();
		currentYPos=GUI_readY();
		for(int i=0; i<nOfButtons; i++){
			if(GUI_pushButton_isPressed(registered_button[i]->text,
					registered_button[i]->x_pos,registered_button[i]->y_pos,
					currentXPos, currentYPos)){
				currentPressedButton=i;
			}
		}
	}
	
	//update screen if changes detected
	if((currentPressedButton != lastPressedButton) || (currentScreenIsPressed != lastSreenWasPressed)){
		glcd_fill_screen(OFF);
		for(int i=0; i<nOfButtons; i++){
			if(i == currentPressedButton){
				GUI_drawPushButton(registered_button[i]->text,registered_button[i]->x_pos,registered_button[i]->y_pos,OFF,ON);
			}else{
				GUI_drawPushButton(registered_button[i]->text,registered_button[i]->x_pos,registered_button[i]->y_pos,OFF,OFF);	
			}
		}
		glcd_load_buffer();
	}
	
	//execute call backs
	if(currentPressedButton != lastPressedButton){
		if(lastPressedButton >= 0)
			registered_button[lastPressedButton]->callBack(
				registered_button[lastPressedButton],
					((!currentScreenIsPressed) ? GUI_EVENT_CLICKED :  GUI_EVENT_RELEASED));
		if(currentPressedButton >= 0) 
			registered_button[currentPressedButton]->callBack(
				registered_button[currentPressedButton],GUI_EVENT_PRESSED);
	}
	
	lastPressedButton=currentPressedButton;
	lastSreenWasPressed=currentScreenIsPressed;
	
}
