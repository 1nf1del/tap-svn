/************************************************************
				Part of the UK TAP Project
		This module handles configuration of the event information

Name	: EventInfoConfig.c
Author	: Darkmatter and IanP
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.1 kidhazy:    29-09-05	Extracted from the original EventInfo.c
**************************************************************/
//
//   X Max = 720
//   Y MAX = 576
//

#define EVENT_X_DEFAULT 306			    // 306
int EVENT_X;
#define EVENT_X_MIN 270

#define EVENT_Y_DEFAULT	350			    // 350
int EVENT_Y;

#define EVENT_W 370			    // 360 changed to 370 14-09-05
#define EVENT_H_DEFAULT 200	    // 200
int EVENT_H;

#define EVENT_H_MAX 200
#define EVENT_H_MIN 170

#define MAX_X   (EVENT_X+EVENT_W)  // 666
//#define CLOCK_X (EVENT_X + EVENT_W) // Define the x-coordinate for the onscreen clock
int CLOCK_X;
#define CLOCK_X_DEFAULT 675
#define CLOCK_Y_DEFAULT 320
#define CLOCK_W  80
#define CLOCK_H  26
//#define CLOCK_Y (EVENT_Y-30)    // Define the y-coordinate for the onscreen clock
int CLOCK_Y;

#define EVENT_NAME 45
#define EVENT_TIME 314
#define EVENT_LENGTH EVENT_TIME + 200
#define DWELL_EVENT_PROC	200			// 1 second


#define EVENT_PB_WIDTH 90  // Define the width of the progress bar in the Event Information Window.

