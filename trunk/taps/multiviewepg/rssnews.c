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

#include "rssnews.h"
#include "fontman.h"
#include "skin.h"
#include "display.h"
#include "screentools.h"
#include "timerlist.h"
#include "guide.h"
#include "tools.h"

void RssNews_MemFree( void ){
	if (RssNews_ItemBase) TAP_MemFree(RssNews_ItemBase);
	if (RssNews_ItemHigh) TAP_MemFree(RssNews_ItemHigh);
	RssNews_ItemBase = NULL;
	RssNews_ItemHigh = NULL;
}

void RssNews_CreateGradients( void ){
	word rgn;
	RssNews_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEM );
		RssNews_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED );
		RssNews_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		TAP_Osd_Delete(rgn);
	}
}


// Copied from EPG Uploader source so all credit to tonymy01 for this stuff
void InterpretRSSbuff( RSS_INFO *rss, const unsigned char *buf ){
	unsigned char tmp_field[1000];
	//unsigned char tmp[256];
	int  field_no, i;
	unsigned char *field_start;
	unsigned char	tmp_date[5];
	int len;
	field_no = 0;
	field_start = (unsigned char *)buf;
	memset(tmp_field, 0x0, 1);
	for(i=0; i<strlen(buf); i++){
		if ( *(buf+i) == '\t' ){
			strncat( tmp_field, field_start, ( (buf+i) - field_start ));
			field_no++;
			//logf(4,"field_no%02d %s", field_no, strncpy(tmp,tmp_field,115));
			//memset(tmp,0x0,1);
			switch ( field_no ){
				case FEEDNAME:
					if ((len=strlen(tmp_field))>99)
						len=99;
					strncat( rss->feedname, tmp_field, len);
					break;
				case FEEDDATE: // Date
					memset( tmp_date, 0x0, 5 );
					rss->year = atoi( strncpy( tmp_date, tmp_field, 4 ) );
					memset( tmp_date, 0x0, 3 );
					rss->month = atoi( strncpy( tmp_date, tmp_field+5, 2 ) );
					rss->day = atoi( strncpy( tmp_date, tmp_field+8, 2 ) );
					break;
				case FEEDTITLE: // Title
					if ((len=strlen(tmp_field))>99)
						len=99;
					strncat( rss->feedtitle, tmp_field, len);
					break;
				case FEEDDESC: // Description
					if ((len=strlen(tmp_field))>484)
						len=484;
					strncat( rss->feeddesc ,tmp_field, len);
					break;
			}
			memset(tmp_field, 0x0, 1);
			field_start = (unsigned char *)(buf + i + 1);
		}
	}
}

void clearRSS( RSS_INFO *rss ){
	memset(rss->feedname,0x0,100);
	rss->year = 0;
	rss->month = 0;
	rss->day = 0;
	memset( rss->feedtitle, 0x0, 100 );
	memset( rss->feeddesc,0x0, 400);
}

void RssNews_FreeMemory( void ){
	int i;
	for( i = 0; i < rssfeed_ptr; i++ ){
		TAP_MemFree(rssfeed[i]);
	}
	rssfeed_ptr = 0;
}


// Copied from EPG Uploader source so all credit to tonymy01 for this stuff
int LoadRSS(unsigned char *RSSfilename){
	unsigned char * in_buf;
	//unsigned char * memory;
	static unsigned char buf[1000];
	static unsigned char sBuff[64];
	TYPE_File* fp;
	RSS_INFO rss;
	int idx=0, i;
//	unsigned char str1[512];
	int filelength=0;
	bool crfound=FALSE;
	RssNews_FreeMemory();
	// Lets begin teh read    
	if (!(fp = TAP_Hdd_Fopen (RSSfilename)))
	{
		return 1;
	}
	filelength=TAP_Hdd_Flen( fp );
	if (!(filelength%512)) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
	if ((in_buf=(unsigned char *)TAP_MemAlloc(filelength+2))==NULL){
		TAP_Hdd_Fclose (fp);
		return 1;
	}
	//in_buf=memory;
	rssfeed_ptr = 0;
	TAP_Hdd_Fread (in_buf, 1, filelength, fp);
	TAP_Hdd_Fclose (fp);
	*(in_buf+filelength)=0x0;
	*(in_buf+filelength+1)=0x0;
	clearRSS( &rss );  // Clear the record and move on
	for(i=0; i<filelength; i++){
		// If we don't have a \n add the unsigned character to the current buffer
		if (idx<=1000-3) //3000 byte max record size.
		{
			buf[idx++] = *in_buf++;
//			gbytestransferred++;  //countbytes
		}
		else
		{
			while ( *in_buf != '\n' && *in_buf != '\r') //if record too large, will scan to the end of the line anyway
			{
				*in_buf++;
//				gbytestransferred++;  //event count carriage returns to make the byte count identical to file size
				i++; 
			}
			crfound=TRUE;  
		}
		while ( *in_buf == '\n' || *in_buf == '\r'|| *in_buf == '\0') //scan thru all CR/LF and EOF
		{
			crfound=TRUE;
			if (*in_buf == '\0') break; //end of file
			*in_buf++;
//			gbytestransferred++;  //event count carriage returns to make the byte count identical to file size
			i++;
			//TAP_SystemProc();
		}
		if (crfound) 
		{
			crfound=FALSE;
			buf[idx++]='\t';  //add a tab to the end to make the InterpretEPGbuff easier to populate
			buf[idx]='\0';
			// Fill the RSS record and then add to the fixed array of news
			InterpretRSSbuff( &rss, (const unsigned char*)buf );  //use new buffer to fill epg
			if( rssfeed_ptr < RSS_MAX_FEEDS ){
				rssfeed[rssfeed_ptr] = TAP_MemAlloc(sizeof(RSS_INFO));
				memcpy(events[eventsPointer], &rss, sizeof(RSS_INFO));
				rssfeed_ptr++;
			}
			memset(sBuff,0,64);
			clearRSS( &rss );  // Clear the record and move on
			idx = 0;
//			gbSystemProc=TRUE;
//			TAP_SystemProc();//let toppy do other things (such as channel changing)
//			gbSystemProc=FALSE;
		}
	} //end while that counts thru the inbuf
	TAP_MemFree((unsigned char *) in_buf);
	memset(sBuff,0,64);
	RSS_Fileread = TRUE;
	return 0;
}

bool RSS_IsActive( void ){
	return RSS_Active;
}

// function used to hide TimerList
void RSS_Hide( void ){
	RSS_Active = FALSE;
	Guide_Redraw = TRUE;
	RSS_Redraw = FALSE;
	RSS_Fileread = FALSE;
	RssNews_MemFree();
	Display_Hide();
}


void RSS_Activate( void ){
	RSS_Min = 0;
	RSS_Max = RSS_MAXLIST;
	RSS_Selected = 0;
	RSS_PrevSelected = 0;
	if( !RSS_Fileread ){
		// Haven't read the file then we need to read and store in memory
		ChangeDirRoot();
		TAP_Hdd_ChangeDir("ProgramFiles");
		if( TAP_Hdd_Exist(RSS_FILE) ){
			LoadRSS(RSS_FILE);
		}
	}
	if( RSS_Max > rssfeed_ptr -1 ){
		RSS_Max = rssfeed_ptr;
	}
	if( rssfeed_ptr > 0 ){
		RSS_Sort();
		RssNews_CreateGradients();
		RSS_Render();
	}
}

// Function to render the item in the array on the screen
void RSS_RenderItem( word rgn, int row, int selected, int prevselected, bool fresh ){
	int x_pos = 0;
	int y_pos = 0;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;

	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (row-RSS_Min));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	// Draw background for item it previous or selected otherwise the background on text will do the job
	if( row == selected ){
		if( RssNews_ItemHigh != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, RssNews_ItemHigh );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
		}
//		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		bc = DISPLAY_ITEMSELECTED;
	} else {
		if( row == prevselected ){
			if( RssNews_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, RssNews_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
			}
//			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
			bc = COLOR_None;
		} else {
			if( fresh ){
				if( RssNews_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, RssNews_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
				}
//				TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
				bc = COLOR_None;
			}
		}
	}
	// Lets place the text on the item now
	Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP, y_pos + 3, 720-((Panel_Left_X-DISPLAY_X_OFFSET)), rssfeed[row]->feedtitle, fc, COLOR_None );
}

void RSS_RenderDetails( int selected ){
	int x_pos = 0;
	int y_pos = 0;
	char text[256];
	memset( text, '\0', sizeof(text) );
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * RSS_MAXLIST);
	TAP_Osd_FillBox(Display_MemRGN, x_pos+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * RSS_MAXLIST), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) ,(( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5), DISPLAY_MAIN);
	// Print the timer duration
	TAP_SPrint( text, "%s ~ %02d/%02d\0", rssfeed[selected]->feedname, rssfeed[selected]->day, rssfeed[selected]->month);
	Font_Osd_PutString1622( Display_MemRGN, x_pos+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * RSS_MAXLIST)+3, DISPLAY_ROW_WIDTH, text, DISPLAY_EVENT, COLOR_None );
	WrapPutStr(Display_MemRGN, rssfeed[selected]->feeddesc, x_pos+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_MAXLIST+1))+3, DISPLAY_ROW_WIDTH, DISPLAY_EVENT, DISPLAY_MAIN, 5, FNT_Size_1622, DISPLAY_GAP);
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, x_pos+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * RSS_MAXLIST), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) ,(( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5), x_pos+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * RSS_MAXLIST), FALSE);
}

// Function to render the item in the array on the screen
void RSS_RenderBlankItem( word rgn, int position ){
	int x_pos = 0;
	int y_pos = 0;
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	if( RssNews_ItemBase != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, RssNews_ItemBase );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
}


void RSS_Sort( void ){
	RSS_INFO *tmp;
	int tmpcmp = 0;
	int i, j, idx;
	for( i = 0; i < rssfeed_ptr; i++ ){
		idx = i;
		for( j = i; j < rssfeed_ptr; j++ ){
			if( (tmpcmp = strcasecmp(rssfeed[idx]->feedname,rssfeed[j]->feedname)) > 0 ){	// Sort by the feed name
				idx = j;
			} else {
				if( tmpcmp == 0 ){	// Same feed so sort by the date
					if( TAP_MakeMjd( rssfeed[idx]->year, rssfeed[idx]->month, rssfeed[idx]->day )
					> TAP_MakeMjd( rssfeed[j]->year, rssfeed[j]->month, rssfeed[j]->day )){
						// Swap as the date is after
						idx = j;
					} else {
						// Same date so sort by the feedtitle
						if( TAP_MakeMjd( rssfeed[idx]->year, rssfeed[idx]->month, rssfeed[idx]->day ) == TAP_MakeMjd( rssfeed[j]->year, rssfeed[j]->month, rssfeed[j]->day )){
							if( (tmpcmp = strcasecmp(rssfeed[idx]->feedtitle,rssfeed[j]->feedtitle)) > 0 ){	// Sort by the feed name
								idx = j;
							}
						}
					}
				}
			}
		}
		tmp = rssfeed[i];
		rssfeed[i] = rssfeed[idx];
		rssfeed[idx] = tmp;
	}
}

// Create the display for the rss news
void RSS_Render( void ){
	int i = 0;
	int c = 0;
	Display_CreateRegions();
	if( !Guide_Rendered && !RSS_Active ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		Display_DrawTime();
		Display_RenderTitle("RSS News");
		c = 0;
		for( i = RSS_Min; i < RSS_Max && i < rssfeed_ptr; i++ ){
			RSS_RenderItem(Display_RGN,i,RSS_Selected, RSS_PrevSelected,TRUE);
			c++;
		}
		for( i = c; i < RSS_MAXLIST; i++ ){
			RSS_RenderBlankItem( Display_RGN, i );
		}
		RSS_RenderDetails(RSS_Selected);
		// Draw the template on the display by fading in
		Screen_FadeIn(Display_RGN,Setting_FadeIn);
	} else {
		if( RSS_Redraw ){
			if( !Display_BackgroundDrawn ){
				Display_DrawBackground();
				Display_DrawTime();
			}
			TAP_Osd_Copy( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_MAXLIST-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			c = 0;
			for( i = RSS_Min; i < RSS_Max && i < rssfeed_ptr; i++ ){
				RSS_RenderItem(Display_MemRGN,i,RSS_Selected, RSS_PrevSelected,TRUE);
				c++;
			}
			for( i = c; i < RSS_MAXLIST; i++ ){
				RSS_RenderBlankItem( Display_MemRGN, i );
			}
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_MAXLIST-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			RSS_RenderDetails(RSS_Selected);
			Display_RenderTitle("RSS News");
			RSS_Redraw = FALSE;
		} else {
			RSS_RenderItem(Display_MemRGN,RSS_Selected,RSS_Selected, RSS_PrevSelected,TRUE);
			RSS_RenderItem(Display_MemRGN,RSS_PrevSelected,RSS_Selected, RSS_PrevSelected,TRUE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_PrevSelected-RSS_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_PrevSelected-RSS_Min)), FALSE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_Selected-RSS_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (RSS_Selected-RSS_Min)), FALSE);
			RSS_RenderDetails(RSS_Selected);
		}
	}
	RSS_Active = TRUE;
}

// Keyhandler for the RSS
dword RSS_Keyhandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		RSS_Hide();
		return 0;
	}
	switch( param1 ){
		case RKEY_ChDown:
			RSS_PrevSelected = RSS_Selected;
			RSS_Selected++;
			if( RSS_Selected > RSS_Max-1 ){
				RSS_Min = RSS_Selected;
				RSS_Max = RSS_Min + RSS_MAXLIST;
				RSS_Redraw = TRUE;
				if( RSS_Max > rssfeed_ptr-1 ){
					RSS_Min = rssfeed_ptr-RSS_MAXLIST;
					RSS_Max = rssfeed_ptr;
				}
			}
			if( RSS_Selected > rssfeed_ptr-1 ){
				RSS_Selected = 0;
				RSS_Min = 0;
				RSS_Max = RSS_MAXLIST;
				if( RSS_Max > rssfeed_ptr -1 ){
					RSS_Max = rssfeed_ptr;
				}
				RSS_Redraw = TRUE;
			}
			RSS_Render();
			return 0;
			break;
		case RKEY_ChUp:
			RSS_PrevSelected = RSS_Selected;
			RSS_Selected--;
			if( RSS_Selected < RSS_Min ){
				RSS_Min = RSS_Selected-RSS_MAXLIST+1;
				RSS_Max = RSS_Selected+1;
				RSS_Redraw = TRUE;
				if( RSS_Min < 0 ){
					RSS_Min = 0;
					RSS_Max = RSS_MAXLIST;
					if( RSS_Max > rssfeed_ptr-1 ){
						RSS_Max = rssfeed_ptr;
					}
				}
			}
			if( RSS_Selected < 0 ){
				RSS_Selected = rssfeed_ptr-1;
				RSS_Max = rssfeed_ptr;
				RSS_Min = RSS_Max-RSS_MAXLIST;
				if( RSS_Min < 0 ){
					RSS_Min = 0;
				}
				RSS_Redraw = TRUE;
			}
			RSS_Render();
			return 0;
			break;
		case RKEY_Recall:
			RssNews_FreeMemory();
			TAP_Hdd_ChangeDir("/ProgramFiles/");
			LoadRSS(RSS_FILE);
			if( RSS_Max > rssfeed_ptr -1 ){
				RSS_Max = rssfeed_ptr;
			}
			if( rssfeed_ptr > 0 ){
				RSS_Sort();
			}
			RSS_Redraw = TRUE;
			RSS_Render();
			return 0;
			break;
	}
	return 0;
}

