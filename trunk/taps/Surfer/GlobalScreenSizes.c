/************************************************************
				Part of the UK TAP Project
		This module handles the setting of the screen sizes and coordinates

Name	: GlobalScreenSizes.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 09/09/05	Inception Date
          v0.1 kidhazy: 29/09/05    Added capability to resize and reposition Main and PiP windows.


	Last change:  USE   9 Sept 2005    9:25 pm
**************************************************************/

//
// Define screen size and location variables
// 

#define NUMBER_OF_WINDOWS  6  // How many windows are we configuring.  1 = Main Window,  2 = PIP Window,  3 = Logo Window, 4 = Event Info, 
                              // 5 = Clock, 6 = Guide/Main Window, 7 = Overscan boundary
#define OVERSCAN_WINDOW    7  // The window number of the Overscan Boundary window                              
//
//   X Max = 720
//   Y MAX = 576
//
#define OSD_MAX_X 720
#define OSD_MAX_Y 576

// Original Main Screen values indicated below.
//#define SCREEN_X	120		// 120
//#define SCREEN_Y	20		// 20
//#define SCREEN_W	590		// 590										// 720 max
//#define SCREEN_H	332		// 332										// 576 max

int OVERSCAN_X;   // Use a variable to allow for repositioning on widescreen TVs
int OVERSCAN_Y;   // Use a variable to allow for repositioning on widescreen TVs
int OVERSCAN_W;   // Use a variable to allow for resizing on widescreen TVs
int OVERSCAN_H;   // Use a variable to allow for resizing on widescreen TVs

int SCREEN_X;   // Use a variable to allow for repositioning on widescreen TVs
int SCREEN_Y;   // Use a variable to allow for repositioning on widescreen TVs
int SCREEN_W;   // Use a variable to allow for resizing on widescreen TVs
int SCREEN_H;   // Use a variable to allow for resizing on widescreen TVs

int GDE_SCREEN_X;   // x co-ord of Main Screen when Guide Window active.
int GDE_SCREEN_Y;   // y co-ord of Main Screen when Guide Window active.
int GDE_SCREEN_W;   // width of Main Screen when Guide Window active.
int GDE_SCREEN_H;   // height of Main Screen when Guide Window active.

int NORM_SCREEN_X;   // Temp storage for x-coord of Main window.
int NORM_SCREEN_Y;   // Temp storage for y-coord of Main window.
int NORM_SCREEN_W;   // Temp storage for width of Main window.
int NORM_SCREEN_H;   // Temp storage for height of Main window.

#define SCREEN_X_MAX OSD_MAX_X
#define SCREEN_X_MIN (LOGO_WINDOW_W-5)  //120

#define SCREEN_Y_MAX (EVENT_Y-1)
#define SCREEN_Y_MIN 0

#define SCREEN_W_MAX (SCREEN_X_MAX - SCREEN_X_MIN)
#define SCREEN_H_MAX (SCREEN_Y_MAX - SCREEN_Y_MIN)


// Defines the REGION for the PiP Window.   
#define PIP_X	0		
#define PIP_Y	EVENT_Y	
#define PIP_W	EVENT_X		
#define PIP_H	(OSD_MAX_Y - EVENT_Y)
#define PIP_X_TEXT_OFFSET 40     // General offset to help centre text in PiP to allow for overscan
#define PIP_X_GRAPHIC_OFFSET ((PIP_X+((PIP_W-216)/2))+(PIP_X_TEXT_OFFSET/2))     // General offset to help centre PiP graphics.  PIP_X + (PiP_W - GraphicLength(216) ) / 2
#define PIP_MSG_H 90   // Height to cover the messages displayed in the PiP Window
//#define PIP_MSG_Y ((PIP_SCRY + ((PIP_H / 2) - (PIP_MSG_H / 2)))   // Starting Y coordinate for messages in the PiP window.
#define PIP_MSG_Y (EVENT_Y +50)

#define PIP_X_MAX EVENT_X    
#define PIP_X_MIN 0

#define PIP_Y_MAX OSD_MAX_Y
#define PIP_Y_MIN EVENT_Y

#define PIP_W_MAX (PIP_X_MAX - PIP_X_MIN)
#define PIP_H_MAX (PIP_Y_MAX - PIP_Y_MIN)

// Define placement of the actual PiP Window.
int PIP_SCRX;   // Use a variable to allow for repositioning on widescreen TVs
int PIP_SCRY;   // Use a variable to allow for repositioning on widescreen TVs
int PIP_SCRW;   // Use a variable to allow for resizing on widescreen TVs
int PIP_SCRH;   // Use a variable to allow for resizing on widescreen TVs

int SCREEN_RATIO;  // Stores the ratios for the TV screen MULTIPLIED by 1000 ( to prevent use of floating point arithmetic)
int WINDOW_RATIO=1250;

bool screenConfigShowing=FALSE;  // Flag to indicate the screen sizing config window is being displayed.
int  window=0; 
int  axis=0;
 
// Define temporary variables to hold window config.
int current_SCREEN_X;   
int current_SCREEN_Y;   
int current_SCREEN_W;   
int current_SCREEN_H;   
int current_GDE_SCREEN_X;   
int current_GDE_SCREEN_Y;   
int current_GDE_SCREEN_W;   
int current_GDE_SCREEN_H;
int current_PIP_SCRX;   
int current_PIP_SCRY;   
int current_PIP_SCRW;   
int current_PIP_SCRH;   
int current_SCREEN_RATIO;
int current_BASE_X;
int current_BASE_Y;
int current_GDE_W;
int current_EVENT_X;
int current_EVENT_Y;
int current_EVENT_H;
int current_CLOCK_X;
int current_CLOCK_Y;
int current_OVERSCAN_X;
int current_OVERSCAN_Y;
int current_OVERSCAN_H;
int current_OVERSCAN_W;

// Prototype definitions.
void SaveConfigurationToFile( void ); 
void ActivateLogos( int svcType, int svcNum );
void DisplayEventInfo( int svcType, int svcNum );
void DisplayGuideWindow(int svcType, int svcNum);
void ActivateGuideWindow( void );
void CloseGuideWindow();
void LoadGuideEventInfo(int svcType, int svcNum);
void RedrawMainMenu( void );
void DisplayScreenConfigWindow(int window);
void UpdateClock(word tempRgn, int xCoord, int yCoord);
void ShowMessageWin( word msgRgn, char* msg1, char* msg2, char* msg3, int delay );
void ShrinkPictureWindows( void );
// End Prototype definitions.

void SaveNormalScreenSettings()
{
     // Save the normal screen setting.
     NORM_SCREEN_X = SCREEN_X;
     NORM_SCREEN_Y = SCREEN_Y;
     NORM_SCREEN_W = SCREEN_W;
     NORM_SCREEN_H = SCREEN_H;
}

void InitialiseOverScanSizes()
{
    OVERSCAN_X = 40;   
    OVERSCAN_Y = 30;   
    OVERSCAN_W = 645;  
    OVERSCAN_H = 516;  
}

     
void SetOverScanSizes()
{
      BASE_X = OVERSCAN_X + 10;
      BASE_Y = OVERSCAN_Y + HEADER_H;
      
      EVENT_H = EVENT_H_DEFAULT;
      EVENT_X = OVERSCAN_X + (OVERSCAN_W - EVENT_W);
      EVENT_Y = OVERSCAN_Y + (OVERSCAN_H - EVENT_H);
      
      CLOCK_X = OVERSCAN_X + OVERSCAN_W;
      CLOCK_Y = EVENT_Y - CLOCK_H - 1;
     
      SCREEN_W = OVERSCAN_W - (10+LOGO_W+10);
      SCREEN_H = (SCREEN_W * 1000) / SCREEN_RATIO;
      if (SCREEN_H > (EVENT_Y - OVERSCAN_Y))
      {
           SCREEN_H = EVENT_Y - OVERSCAN_Y;
           SCREEN_W = (SCREEN_H * SCREEN_RATIO) / 1000;  
      }                    
      SCREEN_X = OVERSCAN_X + (OVERSCAN_W - SCREEN_W);
      SCREEN_Y = OVERSCAN_Y + ((EVENT_Y - OVERSCAN_Y) - SCREEN_H) / 2; ;  // Centre the screen vertically.

      PIP_SCRW = EVENT_X - OVERSCAN_X;
      PIP_SCRH = (PIP_SCRW * 1000) / SCREEN_RATIO;
      if (PIP_SCRH > EVENT_H)
      {
           PIP_SCRH = EVENT_H;
           PIP_SCRW = (PIP_SCRH * SCREEN_RATIO) / 1000;
      }
      PIP_SCRX = EVENT_X - PIP_SCRW;
      PIP_SCRY = EVENT_Y + (((OVERSCAN_Y + OVERSCAN_H) - EVENT_Y) - PIP_SCRH) / 2; // Centre the screen vertically.
      
      GDE_SCREEN_X = (BASE_X + LOGO_W + 10 + GDE_W);  
      GDE_SCREEN_W = OVERSCAN_W - GDE_SCREEN_X + BASE_X - 10;
      GDE_SCREEN_H = (GDE_SCREEN_W * 1000) / SCREEN_RATIO;
      if (GDE_SCREEN_H > (EVENT_Y - OVERSCAN_Y))
      {
          GDE_SCREEN_H = EVENT_Y - OVERSCAN_Y;
          GDE_SCREEN_W = (GDE_SCREEN_H * SCREEN_RATIO) / 1000;
          GDE_SCREEN_X = OVERSCAN_X + (OVERSCAN_W - GDE_SCREEN_W);  // Reposition Guide/Main Screen on hard right side.
      }
      GDE_SCREEN_Y = OVERSCAN_Y + ((EVENT_Y - OVERSCAN_Y) - GDE_SCREEN_H) / 2;  // Centre the screen vertically.
}


void SetWideScreenSizes()
{
        SCREEN_RATIO = 1333;  // 4:3

        // Logo positions.
        BASE_Y=BASE_Y_DEFAULT;
        BASE_X=BASE_X_DEFAULT;
        GDE_W =GDE_W_DEFAULT;
        
        // Event window
        EVENT_X = EVENT_X_DEFAULT;
        EVENT_Y = EVENT_Y_DEFAULT;
        EVENT_H = EVENT_H_DEFAULT;

        // Clock Window
        CLOCK_X = CLOCK_X_DEFAULT;
        CLOCK_Y = CLOCK_Y_DEFAULT;

        SCREEN_X = 193;  //193 to center it.
        SCREEN_Y = 20; 
        SCREEN_H = (EVENT_Y-SCREEN_Y-1);
        SCREEN_W = (SCREEN_H * SCREEN_RATIO) / 1000;  
        
        PIP_SCRH = 200;
        PIP_SCRW = (PIP_SCRH * SCREEN_RATIO) / 1000;
        PIP_SCRX = PIP_X_MAX - PIP_SCRW;   //73 306-200=106
        PIP_SCRY = EVENT_Y;   // 375
        
        // Main Window with Guide Window Active
        GDE_SCREEN_X = (LOGO_WINDOW_W + GDE_W -5);  
        GDE_SCREEN_Y = 33; 
//        GDE_SCREEN_H = (EVENT_Y-GDE_SCREEN_Y-1);
        GDE_SCREEN_H = 316;
        GDE_SCREEN_W = (GDE_SCREEN_H * SCREEN_RATIO) / 1000;  

        SaveNormalScreenSettings();  // Save the Main screen settings as the norm.
}

void Set43ScreenSizes()
{
        SCREEN_RATIO = 1777;  // 16:9  

        // Logo positions.
        BASE_Y=BASE_Y_DEFAULT;
        BASE_X=BASE_X_DEFAULT;
        GDE_W =GDE_W_DEFAULT;
        
        // Event window
        EVENT_X = EVENT_X_DEFAULT;
        EVENT_Y = EVENT_Y_DEFAULT;
        EVENT_H = EVENT_H_DEFAULT;
        
        // Clock Window
        CLOCK_X = CLOCK_X_DEFAULT;
        CLOCK_Y = CLOCK_Y_DEFAULT;

        SCREEN_X = 120;
        SCREEN_Y = 20; 
        SCREEN_H = (EVENT_Y-SCREEN_Y-1);  // 329/332
        SCREEN_W = (SCREEN_H * SCREEN_RATIO) / 1000;  // 585/590

        PIP_SCRH = 150;
        PIP_SCRW = (PIP_SCRH * SCREEN_RATIO) / 1000;
        PIP_SCRX = PIP_X_MAX - PIP_SCRW;  //40   306-266=40
        PIP_SCRY = EVENT_Y+30;     // 380

        // Main Window with Guide Window Active
        GDE_SCREEN_X = (LOGO_WINDOW_W + GDE_W-5);  
        GDE_SCREEN_Y = 75;                               

//        GDE_SCREEN_H = (EVENT_Y-GDE_SCREEN_Y-1);  
        GDE_SCREEN_H = 232;  
        GDE_SCREEN_W = (GDE_SCREEN_H * SCREEN_RATIO) / 1000;  

        SaveNormalScreenSettings();  // Save the Main screen settings as the norm.

}

void ShowPictureWindows( int window )
{
    switch (window)
    {
	     case 1: // Main Window.
                TAP_Osd_FillBox( rgn, SCREEN_X_MIN, SCREEN_Y_MIN, SCREEN_W_MAX, SCREEN_H_MAX, 0 ); // Clear the entire Main window area.
                TAP_Channel_Scale( CHANNEL_Main, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, FALSE );
                if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum );
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Red );
                break;
                
	     case 2: // PiP Window.
                TAP_Osd_FillBox( rgn, 0, PIP_SCRY-1, PIP_X_MAX, OSD_MAX_Y-(PIP_SCRY-1), 0 ); // Clear the entire PIP window area.
	            TAP_Channel_Scale( CHANNEL_Sub, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, FALSE );			// define the PIP window
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Red );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                break;

	     case 3: // Logo Window.
                TAP_Osd_FillBox( rgn, SCREEN_X_MIN, SCREEN_Y_MIN, SCREEN_W_MAX, SCREEN_H_MAX, 0 ); // Clear the entire Main window area.
                ActivateLogos( currentSvcType, currentSvcNum );                    // draw the logos	
                if (guideWindowDisplayed) 
                {
                    CloseGuideWindow();    // Close current Guide window
                    ActivateGuideWindow(); // Reactivate with new coordinates.
                    DisplayGuideWindow( currentSvcType, currentSvcNum );
                }
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Red );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, LOGO_WINDOW_H, LOGO_WINDOW_W-BASE_X+10, 2, 1, COLOR_Black );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Red );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                break;

	     case 4: // Event Info Window
                DisplayScreenConfigWindow(window);   // Redraw Event Window with new coordinates.
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Red );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                break;

	     case 5:  // Clock Window
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Red );	
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                break;

	     case 6: // Main Window with Guide Window Active
                TAP_Osd_FillBox( rgn, LOGO_WINDOW_W+GDE_W, 0, OSD_MAX_X-(LOGO_WINDOW_W+GDE_W), EVENT_Y-1, 0 ); // Clear the entire Main window area.
                TAP_Channel_Scale( CHANNEL_Main, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, FALSE );
                if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum );
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Red );
                break;

	     case OVERSCAN_WINDOW: // Overscan window
                SetOverScanSizes();

                TAP_Osd_FillBox( rgn, 0, 0, OSD_MAX_X, OSD_MAX_Y, 0 ); // Clear the entire  window area.
                ActivateLogos( currentSvcType, currentSvcNum );                    // draw the logos	
                DisplayScreenConfigWindow(window);   // Redraw Event Window with new coordinates.
                TAP_Channel_Scale( CHANNEL_Main, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, FALSE );
	            TAP_Channel_Scale( CHANNEL_Sub, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, FALSE );			// define the PIP window
                if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum );
                UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.
                TAP_Osd_DrawRectangle( rgn, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, BASE_X-10, BASE_Y-HEADER_H, LOGO_WINDOW_W-BASE_X+10, LOGO_WINDOW_H-BASE_Y+HEADER_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, GDE_X-1, GDE_Y, GDE_W, GDE_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, CLOCK_X-CLOCK_W, CLOCK_Y, CLOCK_W, CLOCK_H, 2, COLOR_Yellow );	
                TAP_Osd_DrawRectangle( rgn, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, GDE_SCREEN_X, GDE_SCREEN_Y, GDE_SCREEN_W, GDE_SCREEN_H, 2, COLOR_Yellow );
                TAP_Osd_DrawRectangle( rgn, OVERSCAN_X, OVERSCAN_Y, OVERSCAN_W, OVERSCAN_H, 2, COLOR_Red );
                break;
    }
}

void DisplayScreenConfigWindow(int window)
{
    dword startX;
    char str[10];

	TAP_Osd_FillBox( rgn, EVENT_X-1, EVENT_Y-1, OSD_MAX_X-EVENT_X+1, OSD_MAX_Y-EVENT_Y+1, 0 );	// clear the bottom portion of the screen
	TAP_Osd_PutGd( rgn, EVENT_X, EVENT_Y, &_narrowhighlightGd, TRUE );	// draw the green bar
    startX = EVENT_X+3;   // Set the initial x-coord for the Event Name.

    //
    // Clear the Event Information box.
    // 
	TAP_Osd_FillBox( rgn, EVENT_X, EVENT_Y+35, EVENT_W, EVENT_H-35, EVENT_FILL_COLOUR );	// Fill the event window

    if (window != OVERSCAN_WINDOW) TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+40, MAX_X, "OK - Switches to other windows.", MAIN_TEXT_COLOUR, 0 );
    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+62, MAX_X, "Record - Saves window positions.", MAIN_TEXT_COLOUR, 0 );
    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+84, MAX_X, "Exit - Quits without saving positions.", MAIN_TEXT_COLOUR, 0 );
    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+106, MAX_X, "Recall - Reset to starting positions.", MAIN_TEXT_COLOUR, 0 );
    if (window != OVERSCAN_WINDOW) TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+128, MAX_X, "<< / >> - Decrease/Increase Size.", MAIN_TEXT_COLOUR, 0 );
    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+150, MAX_X, "Vol -/+  - Move Window Left/Right.", MAIN_TEXT_COLOUR, 0 );
    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+172, MAX_X, "Ch -/+  - Move Window Down/Up.", MAIN_TEXT_COLOUR, 0 );
    if (window == OVERSCAN_WINDOW) 
    {
       switch (axis)
       {
            case 0: // Top/Bottom
                    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+40, MAX_X, "OK - Switches to Left/Right setting.", MAIN_TEXT_COLOUR, 0 );
                    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+128, MAX_X, "<< / >> - Decrease/Increase Top/Bottom Size.", MAIN_TEXT_COLOUR, 0 );
                    break;
                    
            case 1: // Left/Right
                    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+40, MAX_X, "OK - Switches to Top/Bottom setting.", MAIN_TEXT_COLOUR, 0 );
                    TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+128, MAX_X, "<< / >> - Decrease/Increase Left/Right Size.", MAIN_TEXT_COLOUR, 0 );
                    break;
       }             
    }
       
    switch (window)
    {
           case 1:    
                      strcpy( str, "Main");
                      break;
           case 2:    
                      strcpy( str, "PiP");
                      break;
           case 3:    
                      strcpy( str, "Logo");
                      break;
           case 4:    
                      strcpy( str, "Event");
                      break;
           case 5:    
                      strcpy( str, "Clock");
                      break;
           case 6:    
                      strcpy( str, "Main/Guide");
                      break;
           case OVERSCAN_WINDOW:    
                      strcpy( str, "Quick");
                      break;
    }
    sprintf(gstr,"%s Window Size & Position Settings.",str);
	TAP_Osd_PutStringAf1622( rgn, startX, EVENT_Y+7, MAX_X, gstr, MAIN_TEXT_COLOUR, 0 );
    
}

void CloseScreenConfigWindow()
{
    // Need to turn off the Guide Window if it was activated.     
    if (guideWindowDisplayed)
	{
        CloseGuideWindow();
		guideWindowDisplayed = FALSE;
    }
    redrawWindow = TRUE;					// will cause a redraw of windows
    screenConfigShowing=FALSE;
    RedrawMainMenu();
       
}


void SaveScreenConfig()
{
    SaveConfigurationToFile();    
   	SaveNormalScreenSettings();            // Save the Normal screen settings.

}

void ResetScreenConfig()
{
     // Reset the configuration to the original one.
     SCREEN_X = current_SCREEN_X;   
     SCREEN_Y = current_SCREEN_Y;   
     SCREEN_W = current_SCREEN_W;   
     SCREEN_H = current_SCREEN_H;   
     GDE_SCREEN_X = current_GDE_SCREEN_X;   
     GDE_SCREEN_Y = current_GDE_SCREEN_Y;   
     GDE_SCREEN_W = current_GDE_SCREEN_W;   
     GDE_SCREEN_H = current_GDE_SCREEN_H;   
     PIP_SCRX = current_PIP_SCRX;   
     PIP_SCRY = current_PIP_SCRY;   
     PIP_SCRW = current_PIP_SCRW;   
     PIP_SCRH = current_PIP_SCRH;
     SCREEN_RATIO = current_SCREEN_RATIO;
     BASE_X   = current_BASE_X;
     BASE_Y   = current_BASE_Y;
     GDE_W    = current_GDE_W;
     EVENT_X  = current_EVENT_X;
     EVENT_Y  = current_EVENT_Y;
     EVENT_H  = current_EVENT_H;
     CLOCK_X  = current_CLOCK_X;
     CLOCK_Y  = current_CLOCK_Y;
     OVERSCAN_X = current_OVERSCAN_X;
     OVERSCAN_Y = current_OVERSCAN_Y;
     OVERSCAN_H = current_OVERSCAN_H;
     OVERSCAN_W = current_OVERSCAN_W;

}


//------------
//
void ScreenConfigKeyHandler(dword key)
{
	int  i, oldEditLine, tempPIP_X;

	switch ( key )
	{
		case RKEY_ChDown : // Move Down
                           switch (window)
                           {
		                   case 1: 
                                if (SCREEN_Y + SCREEN_H < (EVENT_Y-1))
		                        { 	  
                                    SCREEN_Y++; 
                                    ShowPictureWindows(window); 
                                }
						        break;
						        
		                   case 2: 
                                if ((PIP_SCRY + PIP_SCRH < OSD_MAX_Y) && (PIP_SCRY < OSD_MAX_Y - 90))
		                        {
                                   PIP_SCRY++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 3: 
                                if (LOGO_WINDOW_H < EVENT_Y)
		                        {
                                   BASE_Y++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 4: 
                                if ((EVENT_Y+EVENT_H < OSD_MAX_Y) && (EVENT_Y < PIP_SCRY))
		                        {
                                   EVENT_Y++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 5: 
                                if (CLOCK_Y + CLOCK_H < OSD_MAX_Y)
		                        {
                                   CLOCK_Y++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 6: 
                                if (GDE_SCREEN_Y + GDE_SCREEN_H < (EVENT_Y-1))
		                        { 	  
                                    GDE_SCREEN_Y++; 
                                    ShowPictureWindows(window); 
                                }
						        break;

		                   case 7: 
                                if (OVERSCAN_Y + OVERSCAN_H < (OSD_MAX_Y-1))
		                        { 	  
                                    OVERSCAN_Y++; 
                                    ShowPictureWindows(window); 
                                }
						        break;
                           }
                           break;

							
		case RKEY_ChUp :   // Move Up
                           switch (window)
                           {
		                   case 1: 
                                if (SCREEN_Y > 0)
                                {
                                    SCREEN_Y--; 
                                    ShowPictureWindows(window); 
                                }
						        break;
						           
		                   case 2: 
                                if (PIP_SCRY > PIP_Y_MIN)
		                        {
                                   PIP_SCRY--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 3: 
                                if (BASE_Y > HEADER_H)
		                        {
                                   BASE_Y--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 4: 
                                if ((EVENT_Y > SCREEN_Y+SCREEN_H) && (EVENT_Y > LOGO_WINDOW_H))
		                        {
                                   EVENT_Y--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 5: 
                                if (CLOCK_Y > 0)
		                        {
                                   CLOCK_Y--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 6: 
                                if (GDE_SCREEN_Y > 0)
                                {
                                    GDE_SCREEN_Y--; 
                                    ShowPictureWindows(window); 
                                }
						        break;

		                   case 7: 
                                if (OVERSCAN_Y > 0)
		                        { 	  
                                    OVERSCAN_Y--; 
                                    ShowPictureWindows(window); 
                                }
						        break;
						           
                           }
                           break;

							
		case RKEY_VolUp :  // Move Right
                           switch (window)
                           {
		                   case 1: 
                                if (SCREEN_X + SCREEN_W < SCREEN_X_MAX)
		                        {
                                   SCREEN_X++;
                                   ShowPictureWindows(window); 
                                }
                                break;
                                
		                   case 2: 
                                if (PIP_SCRX + PIP_SCRW < EVENT_X)
                                {
                                   PIP_SCRX++;
                                   ShowPictureWindows(window); 
                                }
                                break;
		                   
                           case 3: 
                                if (LOGO_WINDOW_W < SCREEN_X)
		                        {
                                   BASE_X++;
                                   ShowPictureWindows(window); 
                                }
                                break;
		                   
                           case 4: 
                                if (EVENT_X + EVENT_W < OSD_MAX_X)
		                        {
                                   EVENT_X++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 5: 
                                if (CLOCK_X < OSD_MAX_X)
		                        {
                                   CLOCK_X++;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 6: 
                                if (GDE_SCREEN_X + GDE_SCREEN_W < SCREEN_X_MAX)
		                        {
                                   GDE_SCREEN_X++;
                                   ShowPictureWindows(window); 
                                }
                                break;


		                   case 7: 
                                if (OVERSCAN_X + OVERSCAN_W < (OSD_MAX_X-1))
		                        { 	  
                                    OVERSCAN_X++; 
                                    ShowPictureWindows(window); 
                                }
						        break;
                          }
                          break;
		
		case RKEY_VolDown : // Move Left
                           switch (window)
                           {
		                   case 1: 
                                if (SCREEN_X > SCREEN_X_MIN)
		                        {
                                   SCREEN_X--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 2: 
                                   if (PIP_SCRX > 0 )
                                   {
                                       PIP_SCRX--;
                                       ShowPictureWindows(window); 
                                   }
						           break;

		                   case 3: 
                                if (BASE_X-10 > 0)
		                        {
                                   BASE_X--;
                                   ShowPictureWindows(window); 
                                }
                                break;
		                   
                           case 4: 
                                if ((EVENT_X > PIP_SCRX + PIP_SCRW) && (EVENT_X > EVENT_X_MIN))
		                        {
                                   EVENT_X--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 5: 
                                if (CLOCK_X - CLOCK_W > 0 )
		                        {
                                   CLOCK_X--;
                                   ShowPictureWindows(window); 
                                }
                                break;                          

		                   case 6: 
                                if (GDE_SCREEN_X > (LOGO_WINDOW_W + GDE_W)-20 )
		                        {
                                   GDE_SCREEN_X--;
                                   ShowPictureWindows(window); 
                                }
                                break;

		                   case 7: 
                                if (OVERSCAN_X > 0)
		                        { 	  
                                    OVERSCAN_X--; 
                                    ShowPictureWindows(window); 
                                }
						        break;
                          }
                          break;


		case RKEY_Forward:  // Increase Window size
                           switch (window)
                           {
		                   case 1: 
                                   if ((SCREEN_Y > SCREEN_Y_MIN) && ((SCREEN_X + SCREEN_W + (SCREEN_RATIO/1000) + 1) < SCREEN_X_MAX ) )
		                           { 	  
                                       SCREEN_H++; 
                                       SCREEN_W = (SCREEN_RATIO * SCREEN_H) / 1000;
                                       SCREEN_Y--; 
                                       ShowPictureWindows(window); 
                                   }
						           break;
		                   case 2: 
                                   if ((PIP_SCRH + PIP_SCRY < OSD_MAX_Y) && ((PIP_SCRX - (SCREEN_RATIO/1000) - 1) > 0 ) )
		                           { 
                                       tempPIP_X = PIP_SCRX + PIP_SCRW;	  // temp store of current right-hand x-coord.
                                       PIP_SCRH++; 
                                       PIP_SCRW = (SCREEN_RATIO * PIP_SCRH) / 1000;
                                       PIP_SCRX = tempPIP_X - PIP_SCRW;  // Keep the window still on right-handside and grow to the left.
                                       ShowPictureWindows(window);   
                                   }
						           break;

		                   case 3: 
                                if (GDE_W < GDE_W_MAX) 
                                {
                                     GDE_W++;
                                     ShowPictureWindows(window);   
                                }
                                break;
		                   
                           case 4: 
                                if ((EVENT_H < EVENT_H_MAX) && ( EVENT_Y + EVENT_H < OSD_MAX_Y))
                                {
                                      EVENT_H++;
                                      ShowPictureWindows(window);   
                                }
                                break;

		                   case 6: 
                                   if ((GDE_SCREEN_Y > SCREEN_Y_MIN) && ((GDE_SCREEN_X + GDE_SCREEN_W + (SCREEN_RATIO/1000) + 1) < SCREEN_X_MAX ) )
		                           { 	  
                                       GDE_SCREEN_H++; 
                                       GDE_SCREEN_W = (SCREEN_RATIO * GDE_SCREEN_H) / 1000;
                                       GDE_SCREEN_Y--; 
                                       ShowPictureWindows(window); 
                                   }
						           break;

		                   case 7: 
                                   switch (axis)
                                   {
                                          case 0:  // Top/Bottom
                                                 if (OVERSCAN_H < (OSD_MAX_Y - OVERSCAN_Y) )
		                                         { 	  
                                                    OVERSCAN_H++; 
                                                    ShowPictureWindows(window); 
                                                 }
						                         break;
                                          case 1:  // Left/Right
                                                 if (OVERSCAN_W < (OSD_MAX_X - OVERSCAN_X))
		                                         { 	  
                                                    OVERSCAN_W++; 
                                                    ShowPictureWindows(window); 
                                                 }
						                         break;
                                   }              
/*                                   
                                   if ((OVERSCAN_Y + OVERSCAN_H < OSD_MAX_Y) && ((OVERSCAN_X + OVERSCAN_W + (SCREEN_RATIO/1000) + 1) < OSD_MAX_X ) )
		                           { 	  
                                       OVERSCAN_H++; 
                                       OVERSCAN_W = (WINDOW_RATIO * OVERSCAN_H) / 1000;
                                       ShowPictureWindows(window); 
                                   }
						           break;
*/
                          }
                          break;

             
        case RKEY_Rewind:  // Decrease Window size.
                           switch (window)
                           {
		                   case 1: 
                                if ((SCREEN_Y < (EVENT_Y-1)) && (SCREEN_H > 100))
		                           { 	  
                                       SCREEN_H--; 
                                       SCREEN_W = (SCREEN_RATIO * SCREEN_H) / 1000;
                                       SCREEN_Y++; 
TAP_Print("X=%d Y=%d W=%d H=%d \r\n",SCREEN_X,SCREEN_Y,SCREEN_W,SCREEN_H);                                       
                                       ShowPictureWindows(window); 
                                   }
						           break;

		                   case 2: 
                                   if (PIP_SCRH > 100)
		                           { 	  
                                       tempPIP_X = PIP_SCRX + PIP_SCRW;	  // temp store of current right-hand x-coord.
                                       PIP_SCRH--; 
                                       PIP_SCRW = (SCREEN_RATIO * PIP_SCRH) / 1000;
                                       PIP_SCRX = tempPIP_X - PIP_SCRW;  // Keep the window still on right-handside and grow to the left.
                                       ShowPictureWindows(window); 
                                   }
						           break;

		                   case 3: 
                                if (GDE_W > GDE_W_MIN)
                                {
                                     GDE_W--;
                                     ShowPictureWindows(window);   
                                }
                                break;
		                   
                           case 4: 
                                if (EVENT_H > EVENT_H_MIN) 
                                {
                                      EVENT_H--;
                                      ShowPictureWindows(window);   
                                }
                                break;

		                   case 6: 
                                if (GDE_SCREEN_Y < (EVENT_Y-1))
		                           { 	  
                                       GDE_SCREEN_H--; 
                                       GDE_SCREEN_W = (SCREEN_RATIO * GDE_SCREEN_H) / 1000;
                                       GDE_SCREEN_Y++; 
                                       ShowPictureWindows(window); 
                                   }
						           break;

		                   case 7: 
                                   switch (axis)
                                   {
                                          case 0:  // Top/Bottom
                                                 if (OVERSCAN_H > (GDE_H + EVENT_H) )
		                                         { 	  
                                                    OVERSCAN_H--; 
                                                    ShowPictureWindows(window); 
                                                 }
						                         break;
                                          case 1:  // Left/Right
                                                 if (OVERSCAN_W > (10 + LOGO_W + 10 + GDE_W + EVENT_W))
		                                         { 	  
                                                    OVERSCAN_W--; 
                                                    ShowPictureWindows(window); 
                                                 }
						                         break;
                                   }              
/*                                                  
                                   if (OVERSCAN_H > (GDE_H + EVENT_H) )
		                           { 	  
                                       OVERSCAN_H--; 
                                       OVERSCAN_W = (WINDOW_RATIO * OVERSCAN_H) / 1000;
                                       ShowPictureWindows(window); 
                                   }
						           break;
*/
                          }
                          break;

             
        case RKEY_Ok :      if (window==OVERSCAN_WINDOW)  // Change the axis that we will change.
                            {
                                axis = !axis;
		                        DisplayScreenConfigWindow(window);
		                        ShowPictureWindows(window);
                                break; 
                            }    
                            if (window < NUMBER_OF_WINDOWS) window++;
		                    else window=1;
		                    DisplayScreenConfigWindow(window);
		                    ShowPictureWindows(window);
		                    break;
		                    

		case RKEY_Recall :	ResetScreenConfig();				// Reload the original data
                            DisplayScreenConfigWindow(window); 
                            ShowPictureWindows(window);        
							break;
							
		case RKEY_Record :	SaveScreenConfig();					// Save the modified config info
							CloseScreenConfigWindow();			// Close the config window
							break;

        case RKEY_Guide:    //guideFormat = 0;  // Reset the Guide Display Format to Now/Next Display Format
                            //displayStartTime = 0;
                            if (!guideWindowDisplayed)
					        {
						         ActivateGuideWindow();
						         LoadGuideEventInfo(currentSvcType, currentSvcNum);
						         DisplayGuideWindow(currentSvcType, currentSvcNum);
					        }
					        else
					        {
						         CloseGuideWindow();
						         guideWindowDisplayed = FALSE;
			                }
			                ShowPictureWindows(window); 
                            break;							


		case RKEY_Exit : 	ResetScreenConfig();				// Reload the original data
                            CloseScreenConfigWindow();			// Close the config window
							break;
							

		default :
			break;
	}
}

void InitialiseScreenSizes()
{
     //  If the screen size and position information was not read from the INI file then we will have all zeroes.
     //  So we need to check for this and apply some defaults and save to config file for later reuse.

	if ((SCREEN_X <= 0) || (SCREEN_Y <= 0) || (SCREEN_W <= 0) || (SCREEN_H <= 0) || 
        (PIP_SCRY <= 0) || (PIP_SCRX < 0) || (PIP_SCRW <= 0) || (PIP_SCRH <= 0) ||
        (BASE_X < 10) || (BASE_Y <= 0) || (GDE_W <= 0) ||
        (GDE_SCREEN_X <=0) || (GDE_SCREEN_Y <= 0) || (GDE_SCREEN_H <= 0) || (GDE_SCREEN_W <=0)||
        (EVENT_X <=0) || (EVENT_Y <= 0) || (EVENT_H <= 0) ||
        (CLOCK_X <= 0) || (CLOCK_Y <= 0) ||
        (OVERSCAN_W <= 0) || (OVERSCAN_H <= 0) ) // The Screen config variables may not have been set, so take the defaults.
    {  
       Set43ScreenSizes();
       InitialiseOverScanSizes();
       SaveScreenConfig();
    }

    if (BASE_Y <= 0) 
    {    
        // Logo positions.
        BASE_Y=BASE_Y_DEFAULT;
        BASE_X=BASE_X_DEFAULT;
        GDE_W =GDE_W_DEFAULT;
        SaveScreenConfig();
    }
    
    if (GDE_W <= 0)
    {
        GDE_W =GDE_W_DEFAULT;
        SaveScreenConfig();
    }
    
    if ((EVENT_Y <= 0) || (EVENT_X < EVENT_X_MIN) || (EVENT_H > EVENT_H_MAX) || (EVENT_H < EVENT_H_MIN))
    {
        // Event window
        EVENT_X = EVENT_X_DEFAULT;
        EVENT_Y = EVENT_Y_DEFAULT;
        EVENT_H = EVENT_H_DEFAULT;
        SaveScreenConfig();
    }

    if ((CLOCK_X <= 0) || (CLOCK_Y <= 0))
    {
        CLOCK_X = CLOCK_X_DEFAULT;
        CLOCK_Y = CLOCK_Y_DEFAULT;
        SaveScreenConfig();
    }
    
}

void ResetScreenConfigWindow()
{
     char str[100], gstr[100];
     if (TvRatioOption == 0) 
     {
         Set43ScreenSizes();
         strcpy(str,"Standard Screen");
     }
     else 
     {
         SetWideScreenSizes();
         strcpy(str,"Wide Screen");
     }
     InitialiseOverScanSizes();             // Set default Overscan sizes.
     SaveScreenConfig();
     sprintf(gstr,"set for %s.",str);
     ShowMessageWin(rgn, "Resetting Window Size & Positions.", "Window sizes & positions have been", gstr, 200);
     redrawWindow = TRUE;					// will cause a redraw of windows
}     

void SwitchScreenToGuideWindow()
{
     // Set the Main Channel screen settings to the Guide Window Screen Settings.
     SCREEN_X = GDE_SCREEN_X;
     SCREEN_Y = GDE_SCREEN_Y;
     SCREEN_W = GDE_SCREEN_W;
     SCREEN_H = GDE_SCREEN_H;
}

void SwitchScreenToNormal()
{
     // Set the Main Channel screen settings back to the original Main Window Screen Settings.
     SCREEN_X = NORM_SCREEN_X;
     SCREEN_Y = NORM_SCREEN_Y;
     SCREEN_W = NORM_SCREEN_W;
     SCREEN_H = NORM_SCREEN_H;
}

void InitialiseScreenConfigWindow(startWindow)
{
     screenConfigShowing = TRUE;
     window = startWindow;  // Set the global starting window.
     axis = 0; // Start with the Top/Bottom axis.

     // Need to turn off the Guide Window first so that we have the correct SCREEN_* settings.     
     if (guideWindowDisplayed)
	 {
		SwitchScreenToNormal();
        ShrinkPictureWindows();
        CloseGuideWindow();
		guideWindowDisplayed = FALSE;
     }

     // Save the current configuration.
     current_SCREEN_X = SCREEN_X;   
     current_SCREEN_Y = SCREEN_Y;   
     current_SCREEN_W = SCREEN_W;   
     current_SCREEN_H = SCREEN_H;   
     current_GDE_SCREEN_X = GDE_SCREEN_X;   
     current_GDE_SCREEN_Y = GDE_SCREEN_Y;   
     current_GDE_SCREEN_W = GDE_SCREEN_W;   
     current_GDE_SCREEN_H = GDE_SCREEN_H;   
     current_PIP_SCRX = PIP_SCRX;   
     current_PIP_SCRY = PIP_SCRY;   
     current_PIP_SCRW = PIP_SCRW;   
     current_PIP_SCRH = PIP_SCRH;     
     current_SCREEN_RATIO = SCREEN_RATIO;
     current_BASE_X   = BASE_X;
     current_BASE_Y   = BASE_Y;
     current_GDE_W    = GDE_W;
     current_EVENT_X  = EVENT_X;
     current_EVENT_Y  = EVENT_Y;
     current_EVENT_H  = EVENT_H;
     current_CLOCK_X  = CLOCK_X;
     current_CLOCK_Y  = CLOCK_Y;
     current_OVERSCAN_X = OVERSCAN_X;
     current_OVERSCAN_Y = OVERSCAN_Y;
     current_OVERSCAN_W = OVERSCAN_W;
     current_OVERSCAN_H = OVERSCAN_H;
     
//     TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// clear the screen
     TAP_Osd_FillBox( rgn, 0, 0, 720, 576, 0 );				// clear the screen

     TAP_Channel_Scale( CHANNEL_Main, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, FALSE );
	 TAP_Channel_Scale( CHANNEL_Sub, PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, FALSE );			// define the PIP window
     
//     ActivateLogos( currentSvcType, currentSvcNum );                    // draw the logos	
//	 DisplayEventInfo( currentSvcType, currentSvcNum );                 // draw the event information window.

     UpdateClock( rgn, CLOCK_X, CLOCK_Y );  // Draw the clock.

     ActivateLogos( currentSvcType, currentSvcNum );                    // draw the logos	
     DisplayScreenConfigWindow(window);   
     ShowPictureWindows(window);
}

