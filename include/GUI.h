#ifndef _GUI_H
#define _GUI_H

void GUI_init(void);
int GUI_calibrate(void);
int GUI_waitForPress(int timeout);
int GUI_readX(void);
int GUI_readY(void);
#endif