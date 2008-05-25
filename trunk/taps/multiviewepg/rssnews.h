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

#ifndef __RSSNEWS__
  #define __RSSNEWS__

#define __RSSNEWS_VERSION__ "2008-01-15"

//http://web.resource.org/rss/1.0/spec#s5.3 using those values

typedef struct _RSS_INFO
{
	char feedname[100];
	int	year;
	int	month;
	int day;
	char feedtitle[100];
	char feeddesc[500];
} RSS_INFO;


#define RSS_MAX_FEEDS 500

#define RSS_FILE "rssnews.dat"

enum {
	FEEDBLANK,
	FEEDNAME,
	FEEDDATE,
	FEEDTITLE,
	FEEDDESC,
	FEEDFORMAT
};

static RSS_INFO *rssfeed[RSS_MAX_FEEDS];	// Array of news items
static int rssfeed_ptr = 0;		// Location of the next insert event

static byte* RssNews_ItemBase = NULL;
static byte* RssNews_ItemHigh = NULL;


#define RSS_MAXLIST 8

static int RSS_Selected = 0;
static int RSS_PrevSelected = 0;
static int RSS_Min = 0;
static int RSS_Max = RSS_MAXLIST;
static bool RSS_Redraw = FALSE;
static bool RSS_Active = FALSE;
static bool RSS_Fileread = FALSE;

void RSS_RenderItem( word, int, int, int, bool );
void RSS_Activate( void );
void RSS_Render( void );
dword RSS_Keyhandler( dword );
int LoadRSS(unsigned char * );
void clearRSS( RSS_INFO * );
void InterpretRSSbuff( RSS_INFO *, const unsigned char * );
void RSS_RenderDetails( int );
void RSS_RenderBlankItem( word, int );
void RSS_Sort( void );
void RssNews_FreeMemory( void );

#endif
