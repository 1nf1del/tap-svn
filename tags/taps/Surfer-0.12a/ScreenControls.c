/************************************************************
			Part of the ukEPG project
		This module handles controlling transparency of screen regions

Name	: ScreenControls.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 07-09-05	Inception date

	Last change:  USE   
**************************************************************/


dword screenFadeTick;
static bool ScreenOn=TRUE;
static int  savedTransparency;  // Saves the original Transparency value.



//---------------------------------------------------
// Saves the current system wide transparency setting.
//
void SaveTransparency( void )
{
	savedTransparency = (*TAP_GetSystemVar)( SYSVAR_OsdAlpha );
}

                       
//---------------------------------------------------
// Sets the current system wide transparency setting to the default setting.
//
void SetTransparency( void )
{
     (*TAP_SetSystemVar)( SYSVAR_OsdAlpha, savedTransparency );
}

//---------------------------------------------------
// Fades in a region by cycling through the transparency.
//
// API 1.22 CAUTION : In TF5000PVR, you cannot set each transparency in the individual region. Only the total transparency
//                    can be changed with this function. (TAP_Osd_SetTransparency)
//
void FadeInRegion( word tempRgn )
{
     int i;
     // Transparency ranges from 0 (full transparency) to 255 (no transparency)
     for ( i=0; i<85; i++ )
	 {
         TAP_Osd_SetTransparency( tempRgn, i*3);  
         TAP_Delay(1); 
     }  
}     

//---------------------------------------------------
// Fades out a region by cycling through the transparency.
//
// API 1.22 CAUTION : In TF5000PVR, you cannot set each transparency in the individual region. Only the total transparency
//                    can be changed with this function. (TAP_Osd_SetTransparency)
//
void FadeOutRegion( word tempRgn )
{
     int i,start;
     start = (255 - savedTransparency) / 3;
     // Transparency ranges from 0 (full transparency) to 255 (no transparency)
     for ( i=start; i>0; i-- )
	 {
         TAP_Osd_SetTransparency( tempRgn, i*3);   
         TAP_Delay(1); 
     }  
}     

//---------------------------------------------------
// Sets transparency of a region to off.
//
// API 1.22 CAUTION : In TF5000PVR, you cannot set each transparency in the individual region. Only the total transparency
//                    can be changed with this function. (TAP_Osd_SetTransparency)
//
void SetTransparencyOffRegion( word tempRgn )
{
     // Transparency ranges from 0 (full transparency) to 255 (no transparency)
     TAP_Osd_SetTransparency( tempRgn, 255);     
}     


//---------------------------------------------------
// Sets transparency of a region to it's maximum.
//
// API 1.22 CAUTION : In TF5000PVR, you cannot set each transparency in the individual region. Only the total transparency
//                    can be changed with this function. (TAP_Osd_SetTransparency)
//
void SetTransparencyOnRegion( word tempRgn )
{
     // Transparency ranges from 0 (full transparency) to 255 (no transparency)
     TAP_Osd_SetTransparency( tempRgn, 0);     
}     

bool OsdActive (int startCol, int startRow, int endCol, int endRow)
{
    TYPE_OsdBaseInfo osdBaseInfo;
    dword* wScrn;
    dword iRow, iCol;

    TAP_Osd_GetBaseInfo( &osdBaseInfo );

    for ( iRow = startRow; iRow < endRow; iRow += 4 )  //every 4th line
    {
        wScrn = osdBaseInfo.eAddr + 720 * iRow;

        for ( iCol = startCol; iCol < endCol; iCol += 6 ) //may as well only scan every 6 pixels to save time and resource, stop at 350 as new f/w puts channel details to the right.
        {
            if ( (*(wScrn + iCol)) != 0 )
                return TRUE; // Do not allow Key Action
        }
    }
    return FALSE;
}


void HideScreen()
{
     if (ScreenOn)
     {
         ScreenOn = FALSE;
         FadeOutRegion(rgn);
     }
     else
     {
         ScreenOn = TRUE;
         FadeInRegion(rgn);
         screenFadeTick = TAP_GetTick();
     }         

}     
