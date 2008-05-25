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

#ifndef __EXTINFO__
  #define __EXTINFO__

#define __EXTINFO_VERSION__ "2008-01-15"

static int ExtInfo_X = 0;
static int ExtInfo_Y = 0;
static int ExtInfo_W = 0;
static int ExtInfo_H = 0;
static int ExtInfo_Row = 0;
static byte *ExtInfo_Base = NULL;
static word ExtInfo_RGN = 0;
static word ExtInfo_MemRGN = 0;
static bool ExtInfo_Showing = FALSE;
static bool ExtInfo_Initalised = FALSE;
static bool ExtInfo_ShowName = TRUE;

bool ExtInfo_IsActive( void );
void ExtInfo_Init( word, word, int, int, int, int );
void ExtInfo_Activate( int );
void ExtInfo_Close( void );
dword ExtInfo_Keyhandler( dword );
void ExtInfo_RenderDetails( int );
void ExtInfo_Init2( word rgn, word memrgn, int x, int y, int w, int h, bool name );

#endif
