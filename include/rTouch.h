#ifndef _RTOUCH_H
#define _RTOUCH_H

void rTouch_init(void);
int rTouch_waitToPress(int timeout);
int rTouch_waitToRelease(int timeout);
int rTouch_readX(void);
int rTouch_readY(void);
#endif
