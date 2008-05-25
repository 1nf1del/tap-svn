//
//	  MultiViewEPG jim16 Tue 1 Apr 12:12:00
//
//	  Copyright (C) 2008 Jim16
//
//	  This is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  The software is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this software; if not, write to the Free Software
//	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "settings.h"
#include "fontman.h"
#include "display.h"
#include "skin.h"
#include "screentools.h"
#include "favmanage.h"
#include "gridepg.h"

// Desc: Routine to close the displays.

void Display_Hide( void ){
	if( Display_BackgroundDrawn ){
		Display_PIGRestore();
		Screen_FadeOut(Display_RGN,Setting_FadeOut);
	}
	TAP_Osd_FillBox( Display_RGN, 0, 0, 720, 576, COLOR_None );
	TAP_Osd_FillBox( Display_MemRGN, 0, 0, 720, 576, COLOR_None );
	TAP_Osd_Delete(Display_RGN);
	TAP_Osd_Delete(Display_MemRGN);
	TAP_EnterNormal();
	Display_RGN = 0;
	Display_MemRGN = 0;
	Display_BackgroundDrawn = FALSE;
}

// Desc: Routine to initalise the displays. Calculates and stores the positions in the required variables

void Display_Init( void ){
	if( Display_Initalised ) return;
	
	// Define the locations for the top panel. Default is the PIG location
	Panel_Left_X = DISPLAY_X+DISPLAY_BORDER+DISPLAY_X_OFFSET;
	Panel_Left_Y = DISPLAY_Y+DISPLAY_BORDER+DISPLAY_HEADER+DISPLAY_Y_OFFSET;
	
	// Define the locations for the top panel. Default is the PIG location
	Panel_Top_X = DISPLAY_X+(DISPLAY_BORDER*2)+DISPLAY_ITEM_W+DISPLAY_X_OFFSET;
	Panel_Top_Y = DISPLAY_Y+DISPLAY_HEADER+DISPLAY_Y_OFFSET;
	Panel_Top_W = 269; //((720-DISPLAY_X) - (DISPLAY_X+(DISPLAY_BORDER*2)+DISPLAY_ITEM_W))-DISPLAY_BORDER;//+DISPLAY_X_OFFSET;
	Panel_Top_H = ((DISPLAY_Y+DISPLAY_BORDER+DISPLAY_HEADER + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 6))-(DISPLAY_Y+DISPLAY_BORDER+DISPLAY_HEADER))+DISPLAY_Y_OFFSET;
	
	// Define the locations for the bottom panel
	Panel_Bottom_X = DISPLAY_X+(DISPLAY_BORDER*2)+DISPLAY_ITEM_W+DISPLAY_X_OFFSET;
	Panel_Bottom_Y = ((DISPLAY_Y+DISPLAY_BORDER+DISPLAY_HEADER + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 6))+DISPLAY_BORDER)+DISPLAY_Y_OFFSET;
	Panel_Bottom_W = 269; //((720-DISPLAY_X) - (DISPLAY_X+(DISPLAY_BORDER*2)+DISPLAY_ITEM_W));//-DISPLAY_BORDER;//+DISPLAY_X_OFFSET;
	Panel_Bottom_H = ((576-DISPLAY_Y-DISPLAY_BORDER) - ((DISPLAY_Y+DISPLAY_BORDER+DISPLAY_HEADER + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 6))+DISPLAY_BORDER)-5)+DISPLAY_Y_OFFSET;
	
	DISPLAY_ROW_WIDTH = 603;
	// Look for font manager patch. If found flag for use
	if( PatchIsInstalled((dword*) 0x80000000, "Fm") ){
		Settings_NormalFonts = FALSE;
	} else {
		Settings_NormalFonts = TRUE;
	}
	Display_Initalised = TRUE;
}

// Desc: Routine to restore the scaled picture

void Display_PIGRestore( void ){
	if( Setting_PIG ){
		TAP_Channel_Scale( 1, 0, 0, 720, 576, FALSE );
	}
}

// Desc: Routine to scale the screen for the PIG

void Display_PIGScale( void ){
	if( !Setting_PIG ) return;
	TAP_Osd_FillBox(Display_RGN, Panel_Top_X, Panel_Top_Y, Panel_Top_W, Panel_Top_H, COLOR_None);
	TAP_Channel_Scale( 1, Panel_Top_X, Panel_Top_Y, Panel_Top_W, Panel_Top_H, FALSE );
}

// Desc: Routine to create the display regions for the graphics to be drawn to

void Display_CreateRegions( void ){
	if( !Display_RGN ){
		TAP_ExitNormal();
		if( isMasterpiece() ){
			Display_RGN = TAP_Osd_Create( 0, 0, 720, 576, 0, 0 );
		} else {
			Display_RGN = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
		}
		Display_MemRGN = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );
		Display_Initalised = FALSE;
	}
	Display_Init();
}

// Desc: Routine to render the background. If the unit is a masterpiece then the round corners are not used

void Display_DrawBackground( void ){
	// Fill the background
	if( Settings_RenderBackground ){
		TAP_Osd_FillBox(Display_RGN, 0, 0, 720, 576, DISPLAY_BACKGROUND);
	} else {
		TAP_Osd_FillBox(Display_RGN, 0, 0, 720, 576, COLOR_None);
	}
	if( isMasterpiece() ){
		// Masterpiece doesn't support round corners so just render square
		TAP_Osd_FillBox(Display_RGN, (DISPLAY_X+DISPLAY_X_OFFSET),(DISPLAY_Y+DISPLAY_Y_OFFSET),720-((DISPLAY_X-DISPLAY_X_OFFSET)*2),576-((DISPLAY_Y-DISPLAY_Y_OFFSET)*2),DISPLAY_MAIN);
		Display_BackgroundDrawn = TRUE;
		return;
	}
	// Render the main section of the display
	gxRoundRectangle(Display_RGN, (DISPLAY_X-(DISPLAY_CORNER/2))+DISPLAY_X_OFFSET, (DISPLAY_Y-(DISPLAY_CORNER/2))+DISPLAY_Y_OFFSET, DISPLAY_X+DISPLAY_W+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, DISPLAY_Y+DISPLAY_H+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_MAIN, TRUE, DISPLAY_CORNER);
	// Top left
	gxCircleQuadrant(Display_RGN, DISPLAY_X+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, DISPLAY_Y+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 1, TRUE);
	// Top right
	gxCircleQuadrant(Display_RGN, DISPLAY_X+DISPLAY_W-(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, DISPLAY_Y+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 2, TRUE);
	// Bottom left 3
	gxCircleQuadrant(Display_RGN, DISPLAY_X+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, DISPLAY_Y+DISPLAY_H-(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 3, TRUE);
	// Bottom right 4
	gxCircleQuadrant(Display_RGN, DISPLAY_X+DISPLAY_W-(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, DISPLAY_Y+DISPLAY_H-(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 4, TRUE);
	// Flag background as being drawn
	Display_BackgroundDrawn = TRUE;
}

// Desc: Routine to render the background with a particular size difference. Not used atm

void Display_DrawBackgroundSize( int sizediff ){
	if( isMasterpiece() ){
		// Masterpiece doesn't support round corners so just render square
		TAP_Osd_FillBox(Display_RGN, ((DISPLAY_X+sizediff)+DISPLAY_X_OFFSET),((DISPLAY_Y+sizediff)+DISPLAY_Y_OFFSET),720-((DISPLAY_X-DISPLAY_X_OFFSET)*2),576-(((DISPLAY_Y+sizediff)-DISPLAY_Y_OFFSET)*2),DISPLAY_MAIN);
		Display_BackgroundDrawn = TRUE;
		return;
	}
	// Render the main section of the display
	gxRoundRectangle(Display_RGN, ((DISPLAY_X+sizediff)-(DISPLAY_CORNER/2))+DISPLAY_X_OFFSET, ((DISPLAY_Y+sizediff)-(DISPLAY_CORNER/2))+DISPLAY_Y_OFFSET, (DISPLAY_X+sizediff)+(DISPLAY_W-(sizediff*2))+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, (DISPLAY_Y+sizediff)+(DISPLAY_H-(sizediff*2))+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_MAIN, TRUE, DISPLAY_CORNER);
	// Top left
	gxCircleQuadrant(Display_RGN, (DISPLAY_X+sizediff)+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, (DISPLAY_Y+sizediff)+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 1, TRUE);
	// Top right
	gxCircleQuadrant(Display_RGN, (DISPLAY_X+sizediff)+(DISPLAY_W-(sizediff*2))-(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, (DISPLAY_Y+sizediff)+(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 2, TRUE);
	// Bottom left 3
	gxCircleQuadrant(Display_RGN, (DISPLAY_X+sizediff)+(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, (DISPLAY_Y+sizediff)+(DISPLAY_H-(sizediff*2))-(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 3, TRUE);
	// Bottom right 4
	gxCircleQuadrant(Display_RGN, (DISPLAY_X+sizediff)+(DISPLAY_W-(sizediff*2))-(DISPLAY_CORNER/2)+DISPLAY_X_OFFSET, (DISPLAY_Y+sizediff)+(DISPLAY_H-(sizediff*2))-(DISPLAY_CORNER/2)+DISPLAY_Y_OFFSET, DISPLAY_CORNER, DISPLAY_MAIN, 4, TRUE);
	// Flag background as being drawn
	Display_BackgroundDrawn = TRUE;
}

// Desc: Routine to render the time in all displays. Must be in the main idle routine

void Display_DrawTime( void ){
	static dword ticks = 0;
	static int prevmin = -1;
	word mjd;
	byte hour, min, sec;
	char text[40];
	if( !Display_ClockEnabled ) return;	// Clock disabled so don't render
	if( Display_BackgroundDrawn ){	// Need to render the time so lets start
		if( GridEPG_IsActive() ){
			if( ticks + 100 < TAP_GetTick() ){
				TAP_GetTime( &mjd, &hour, &min, &sec );
				if( prevmin >= 0 ){
					if( prevmin != min ){	// Minute has changed so lets redraw the time
						TAP_SPrint( text, "%02d:%02d\0", hour, min );
						TAP_Osd_FillBox( Display_MemRGN, Panel_Left_X+550, Panel_Left_Y-28-8, 55, 22, DISPLAY_MAIN);
						Font_Osd_PutString1622( Display_MemRGN, Panel_Left_X+550+50-(TAP_Osd_GetW(text,0,FNT_Size_1622)), Panel_Left_Y-28-8, 720-DISPLAY_X-DISPLAY_BORDER+DISPLAY_X_OFFSET, text, DISPLAY_TITLETEXT, DISPLAY_MAIN);
						TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X+550, Panel_Left_Y-28-8, 55, 22, Panel_Left_X+550, Panel_Left_Y-28-8, FALSE);
						GridEPG_RenderTimeLoc();
						prevmin = min;
					}
				} else {
					// Just render anyway as thisis the first time
					TAP_SPrint( text, "%02d:%02d\0", hour, min );
					TAP_Osd_FillBox( Display_MemRGN, Panel_Left_X+550, Panel_Left_Y-28-8, 55, 22, DISPLAY_MAIN);
					Font_Osd_PutString1622( Display_MemRGN, Panel_Left_X+550+50-(TAP_Osd_GetW(text,0,FNT_Size_1622)), Panel_Left_Y-28-8, 720-DISPLAY_X-DISPLAY_BORDER+DISPLAY_X_OFFSET, text, DISPLAY_TITLETEXT, DISPLAY_MAIN);
					TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X+550, Panel_Left_Y-28-8, 55, 22, Panel_Left_X+550, Panel_Left_Y-28-8, FALSE);
					GridEPG_RenderTimeLoc();
					prevmin = min;
				}
				ticks = TAP_GetTick();
			}
		} else {
			if( ticks + 100 < TAP_GetTick() ){
				TAP_GetTime( &mjd, &hour, &min, &sec );
				if( prevmin >= 0 ){
					if( prevmin != min ){	// Minute has changed so lets redraw the time
						TAP_SPrint( text, "%s - %02d:%02d\0", Time_DOWDD(mjd), hour, min );
						TAP_Osd_FillBox( Display_MemRGN, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, 140, 22, DISPLAY_MAIN);
						Font_Osd_PutString1622( Display_MemRGN, Panel_Left_X+460+140-(TAP_Osd_GetW(text,0,FNT_Size_1622)), DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, Panel_Left_X+460+140, text, DISPLAY_TITLETEXT, DISPLAY_MAIN);
						TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, 140, 22, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, FALSE);
						prevmin = min;
					}
				} else {
					// Just render anyway as thisis the first time
					TAP_SPrint( text, "%s - %02d:%02d\0", Time_DOWDD(mjd), hour, min );
					TAP_Osd_FillBox( Display_MemRGN, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, 140, 22, DISPLAY_MAIN);
					Font_Osd_PutString1622( Display_MemRGN, Panel_Left_X+460+140-(TAP_Osd_GetW(text,0,FNT_Size_1622)), DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, Panel_Left_X+460+140, text, DISPLAY_TITLETEXT, DISPLAY_MAIN);
					TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, 140, 22, Panel_Left_X+460, DISPLAY_Y+DISPLAY_BORDER+7+DISPLAY_Y_OFFSET, FALSE);
					prevmin = min;
				}
				ticks = TAP_GetTick();
			}
		}
	} else {
		prevmin = -1;
	}
}

// Desc: Routine to render the title of the display.

void Display_RenderTitle( char * text ){
	TAP_Osd_FillBox( Display_MemRGN, Panel_Left_X, DISPLAY_Y+DISPLAY_BORDER+4+DISPLAY_Y_OFFSET, DISPLAY_ITEM_W, 26, DISPLAY_MAIN );
	Font_Osd_PutString1926( Display_MemRGN, Panel_Left_X, DISPLAY_Y+DISPLAY_BORDER+4+DISPLAY_Y_OFFSET, Panel_Left_X+DISPLAY_ITEM_W, text, DISPLAY_TITLETEXT, DISPLAY_MAIN );
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, DISPLAY_Y+DISPLAY_BORDER+4+DISPLAY_Y_OFFSET, DISPLAY_ITEM_W, 26, Panel_Left_X, DISPLAY_Y+DISPLAY_BORDER+4+DISPLAY_Y_OFFSET, FALSE);
	Display_TitleDrawn = TRUE;
}

