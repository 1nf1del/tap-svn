
#include "tap.h"
#include "tools.h"


//---------------------------------------  ShowMessageWin --------------------------------
//
void ShowMessageWin (char* lpMessage, char* lpMessage1, dword dwDelay)
{
	dword rgn;															// stores rgn-handle for osd
	dword w;															// stores width of message-text

	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );					// create rgn-handle
	//rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_Plane2 );					// create rgn-handle
	w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;				// calculate width of message
	if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
		w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;			// if second message is larger, calculate new width of message
	
	if (w > 720) w = 720;												// if message is to long
	TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );	// draw background-box for border
		
	TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, lpMessage,		
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 1. message
	TAP_Osd_PutS(rgn, (720-w)/2, 270+26, (720+w)/2, lpMessage1,	
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 2. message
	TAP_Delay(dwDelay);													// show it for dwDelay /100 seconds
	TAP_Osd_Delete(rgn);												// release rgn-handle
}

