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

#ifndef __FavManage__
  #define __FavManage__

#define __FavManage_VERSION__ "2008-01-16"

#define FavManage_MaxLIST 13

#include "favsortorder.h"

enum {
	TITLEEXACT,
	TITLE,
	SUBTITLE,
	DESCRIPTION,
	ALL
};

typedef struct {
	char term[128];
	char channel[30];
	char day[10];
	int location;
	int start;
	int end;
	int priority;
	int adj;
	int rpt;
} favitem;

static int FavManage_SortOrder = SORTFAVPRIORITY;
static favitem *favourites[200];
static int favitem_ptr = 0;

static bool FavManage_ReadFav = FALSE;
static bool FavManage_Active = FALSE;
static bool FavManage_Redraw = FALSE;
static bool FavManage_Changed = FALSE;
static int FavManage_Min = 0;
static int FavManage_Max = 0;
static int FavManage_Selected = 0;
static int FavManage_PrevSelected = 0;
static int FavManage_Column = 0;
static byte* FavManage_ItemBase = NULL;
static byte* FavManage_ItemHigh = NULL;

void FavManage_ClearFav( favitem * fav );
void FavManage_BuildFav( favitem *fav, const unsigned char *buf );
int FavManage_LoadFavFile(unsigned char *filename);
bool FavManage_IsActive( void );
void FavManage_Hide( void );
void FavManage_SortPriority( void );
void FavManage_SortName( void );
void FavManage_RenderBlankItem( word rgn, int position );
void FavManage_RenderItem( word rgn, int row, int selected, int prevselected, bool fresh );
void FavManage_Render( void );
void FavManage_Activate( void );
void FavManage_DeleteCallback( int option );
void FavManage_WriteFavs(char *FileName, char *Text);
void FavManage_SaveFile( void );
void FavManage_ResetLocation( void );
dword FavManage_KeyHandler( dword param1 );
void FavManage_SortCallback( bool yes, int selected );
void FavManage_ReloadCallback( bool yes );
char * FavManage_LocationText( int location );
void FavManage_Append( void );
void FavManage_RenderHelp( void );
char * Search_FavTime2( int time );
void FavManage_MemFree( void );
void FavManage_CreateGradients( void );

#endif
