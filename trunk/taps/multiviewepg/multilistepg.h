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

#ifndef __MULTILIST__
  #define __MULTILIST__

#define __MULTILIST_VERSION__ "2008-01-15"

#define MultiList_MaxEvents 6
#define MultiList_Cols 3
#define MultiList_Rows 2

static int MultiList_SelectedRow = 0;
static int MultiList_SelectedCol = 0;
static int MultiList_PrevSelectedRow = 0;
static int MultiList_PrevSelectedCol = 0;
static dword MultiList_StartTime = 0;
static bool MultiList_Active = FALSE;
static bool MultiList_Redraw = FALSE;
static int MultiList_Min = 0;
//static int MultiList_Max = MultiList_MaxEvents;
static int MultiList_Mode = SVC_TYPE_Tv;

void MultiList_Deactivate( void );
dword MultiList_Keyhandler( dword );
void MultiList_RenderItem( word, char *, int, int, int, int, int, int, int, int, bool );
void MultiList_RenderChannel( word, word, int, int );
void MultiList_Render( void );
void MultiList_Activate( void );
void MultiList_Deactivate( void );
bool MultiList_isActive( void );


#endif
