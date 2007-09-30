/************************************************************
				Part of the UK TAP Project
		This module handles configuration of the logos

Name	: DisplayLogosConfig.c
Author	: Darkmatter and IanP
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.1 kidhazy:    29-09-05	Extracted from the original DisplayLogos.c and Logo.c
**************************************************************/

#define LOGO_W	60														// predefined logo width
#define LOGO_H	39														// and height

#define NUM_LOGO_ON_Y_AXIS	7
#define MID_POINT 			3
#define BASE_Y_DEFAULT		51 // 48 was 38  changed 14 Sept 2005 after adding channel name to the top.
int BASE_Y=BASE_Y_DEFAULT;
#define BASE_X_DEFAULT		55
int BASE_X=BASE_X_DEFAULT;
#define LOGO_SPACE_CAROUSEL	41
#define LOGO_SPACE_LINEAR	42

#define HEADER_H            22  // Defines the height of the green header bar on top of the logo window.
#define LOGO_WINDOW_W       (BASE_X + LOGO_W + 10)  // Define the overall width of the Logo window.
#define LOGO_WINDOW_H       (BASE_Y + (NUM_LOGO_ON_Y_AXIS * LOGO_SPACE_LINEAR))  // Define the overall height of the Logo window.

//#define LEFT_BAR_COLOUR ARGB(1, 0, 0, 16)   // Original colour defined by darkmatter.
#define LEFT_BAR_COLOUR COLOR_Black           // Changed to Black by kidhazy for readability.

#define GDE_X  (BASE_X + LOGO_W + 10) // Starting x-coordiante for the Guide window.  Calculated from the Logo Window position
#define GDE_Y  (BASE_Y - HEADER_H)    // Starting y-coordiante for the Guide window.  Calculated from the Logo Window position
#define GDE_W_DEFAULT  170                    // Overall width of the Guide window.
int GDE_W = GDE_W_DEFAULT;
#define GDE_H  (NUM_LOGO_ON_Y_AXIS * LOGO_SPACE_LINEAR + HEADER_H)   // Overall height of the Guide window.  
#define GDE_W_MIN  100
#define GDE_W_MAX  170

#define PBWIDTH  (GDE_W - 42)    // Width of the progressbar in the Guide Window.
#define PBHEIGHT 19     // Height of the progressbar in the Guide Window.
