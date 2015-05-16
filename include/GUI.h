#ifndef _GUI_H
#define _GUI_H

#define GUI_EVENT_PRESSED	0
#define GUI_EVENT_RELEASED	1
#define GUI_EVENT_CLICKED	2

typedef struct GUI_pushButton_struct{
	unsigned char x_pos;
	unsigned char y_pos;
	const char * text;
	void (*callBack)(struct GUI_pushButton_struct * handle ,int event);
}GUI_pushButton_t;

typedef struct GUI_label_struct{
	unsigned char x_pos;
	unsigned char y_pos;
	unsigned char maxChars;
	const char * text;
}GUI_label_t;

void GUI_init(void);
int GUI_calibrate(void);
int GUI_waitForPress(int timeout);
int GUI_readX(void);
int GUI_readY(void);

void GUI_drawPushButton(const char *a,unsigned char x, unsigned char y, unsigned char color,unsigned char state);
int GUI_pushButton_isPressed(const char *a,unsigned char x, unsigned char y, int readX, int readY);

void GUI_addButton(GUI_pushButton_t * newButton);
void GUI_addLabel(GUI_label_t * newLabel);

void GUI_scanScreen(void);

#endif