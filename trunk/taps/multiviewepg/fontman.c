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

#include "fontman.h"
#include "settings.h"

void Font_Osd_PutString1622( dword rgn, dword x, dword y, dword maxX, char * str, dword fcolour, dword bcolour ){
	if( Settings_NormalFonts ){
		TAP_Osd_PutStringAf1622( rgn, x, y, maxX, str, fcolour, bcolour );
	} else {
		// Font Manager Support
		TAP_Osd_PutS( rgn, x, y, maxX, str, fcolour, bcolour, 0, FNT_Size_1622, TRUE, ALIGN_LEFT );
	}
}

void Font_Osd_PutString1926( dword rgn, dword x, dword y, dword maxX, char * str, dword fcolour, dword bcolour ){
	if( Settings_NormalFonts ){
		TAP_Osd_PutStringAf1926( rgn, x, y, maxX, str, fcolour, bcolour );
	} else {
		// Font Manager Support
		TAP_Osd_PutS( rgn, x, y, maxX, str, fcolour, bcolour, 0, FNT_Size_1926, TRUE, ALIGN_LEFT );
	}
}

void Font_Osd_PutString1419( dword rgn, dword x, dword y, dword maxX, char * str, dword fcolour, dword bcolour ){
	if( Settings_NormalFonts ){
		TAP_Osd_PutStringAf1419( rgn, x, y, maxX, str, fcolour, bcolour );
	} else {
		// Font Manager Support
		TAP_Osd_PutS( rgn, x, y, maxX, str, fcolour, bcolour, 0, FNT_Size_1419, TRUE, ALIGN_LEFT );
	}
}
