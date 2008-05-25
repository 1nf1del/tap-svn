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

#include "screentools.h"

void Screen_Set( word rgn ){
	savedTransparency = (*TAP_GetSystemVar)( SYSVAR_OsdAlpha );
	TAP_Osd_SetTransparency( rgn, 0);
}

// Items rendering stuff
void Screen_FadeIn( word rgn, bool fade ){
	int i;
	if( fade ){
		for ( i=0; i<28; i++ ){
			TAP_Osd_SetTransparency( rgn, i*9);  // Was 3
			TAP_Delay(1); 
		}
		TAP_Osd_SetTransparency( rgn, 255);  // Was 3
	} else {
		TAP_Osd_SetTransparency( rgn, 255);  // Was 3
	     (*TAP_SetSystemVar)( SYSVAR_OsdAlpha, savedTransparency );	// Reset the transparency
	}
}

void Screen_FadeOut( word rgn, bool fade ){
	int i,start;
	if( fade ){
		start = (255 - savedTransparency) / 12;
		// Transparency ranges from 0 (full transparency) to 255 (no transparency)
		for ( i=start; i>0; i-- ){
			TAP_Osd_SetTransparency( rgn, i*12);   
			TAP_Delay(1); 
		}
		TAP_Osd_SetTransparency( rgn, 0);
	}
     (*TAP_SetSystemVar)( SYSVAR_OsdAlpha, savedTransparency );	// Reset the transparency
     savedTransparency = 0;
}

