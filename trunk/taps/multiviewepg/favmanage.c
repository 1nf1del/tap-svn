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

#include "favmanage.h"
#include "fontman.h"
#include "skin.h"

void InsertFavItem( favitem fav ){
	if( favitem_ptr < sizeof(favourites) ){
		favourites[favitem_ptr] = TAP_MemAlloc(sizeof(favitem));
		memcpy(favourites[favitem_ptr], &fav, sizeof(favitem));
		favitem_ptr++;
	}
}

void FavManage_MemFree( void ){
	if (FavManage_ItemBase) TAP_MemFree(FavManage_ItemBase);
	if (FavManage_ItemHigh) TAP_MemFree(FavManage_ItemHigh);
	FavManage_ItemBase = NULL;
	FavManage_ItemHigh = NULL;
}

void FavManage_CreateGradients( void ){
	word rgn;
	FavManage_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEM);
		FavManage_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		FavManage_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		TAP_Osd_Delete(rgn);
	}
}

void FavManage_FreeMemory( void ){
	int i;
	for( i = 0; i < favitem_ptr; i++ ){
		TAP_MemFree(favourites[i]);
	}
	favitem_ptr = 0;
}	

void FavManage_ClearFav( favitem * fav ){
	memset(fav->term,0x0,sizeof(fav->term));
	memset(fav->channel,0x0,sizeof(fav->channel));
	memset(fav->day,0x0,sizeof(fav->day));
	fav->location = -1;
	fav->start = -1;
	fav->end = -1;
	fav->priority = 0;
	fav->adj = 0;
	fav->rpt = 0;
}

void FavManage_BuildFav( favitem *fav, const unsigned char *buf ){
	unsigned char tmp_field[1000];
	//unsigned char tmp[256];
	int  field_no, i;
	unsigned char *field_start;
	int len;
	field_no = 0;
	field_start = (unsigned char *)buf;
	memset(tmp_field, 0x0, 1);
	for(i=0; i<strlen(buf); i++){
		if ( *(buf+i) == '\t' ){
			strncat( tmp_field, field_start, ( (buf+i) - field_start ));
			field_no++;
			switch ( field_no ){
				case 1:	// Term
					if ((len=strlen(tmp_field))>127)
						len=127;
					strncat( fav->term, tmp_field, len);
					break;
				case 2: // Channel
					if ((len=strlen(tmp_field))>29)
						len=29;
					strncat( fav->channel, tmp_field, len);
					break;
				case 3: // Day
					if ((len=strlen(tmp_field))>9)
						len=9;
					strncat( fav->day, tmp_field, len);
					break;
				case 4: // Location
					if( strcmp(tmp_field,"TitleExact") == 0 ){
						fav->location = TITLEEXACT;
					} else {
						if( strcmp(tmp_field,"Title") == 0 ){
							fav->location = TITLE;
						} else {
							if( strcmp(tmp_field,"Subtitle") == 0 ){
								fav->location = SUBTITLE;
							} else {
								if( strcmp(tmp_field,"Description") == 0 ){
									fav->location = DESCRIPTION;
								} else {
									if( strcmp(tmp_field,"All") == 0 ){
										fav->location = ALL;
									} else {
										fav->location = TITLE;
									}
								}
							}
						}
					}
					break;
				case 5: // Start
					if( strcmp(tmp_field,"Any") == 0 ){
						fav->start = -1;
					} else {
						fav->start = atoi(tmp_field);
					}
					break;
				case 6: // End
					if( strcmp(tmp_field,"Any") == 0 ){
						fav->end = -1;
					} else {
						fav->end = atoi(tmp_field);
					}
					break;
				case 7: // Priority
					fav->priority = atoi(tmp_field);
					break;
				case 8: // Repeat
					fav->rpt = atoi(tmp_field);
					break;
				case 9: // Adjustable
					fav->adj = atoi(tmp_field);
					break;					
			}
			memset(tmp_field, 0x0, 1);
			field_start = (unsigned char *)(buf + i + 1);
		}
	}
}

int FavManage_LoadFavFile(unsigned char *filename){
	unsigned char * in_buf;
	//unsigned char * memory;
	static unsigned char buf[1000];
	static unsigned char sBuff[64];
	TYPE_File* fp;
	favitem fav;
	int idx=0, i;
	int filelength=0;
	bool crfound=FALSE;

	FavManage_FreeMemory();	// Free the memory taken up by the current favourites
	// Lets begin the read    
	if (!(fp = TAP_Hdd_Fopen (filename)))
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
	favitem_ptr = 0;
	TAP_Hdd_Fread (in_buf, 1, filelength, fp);
	TAP_Hdd_Fclose (fp);
	*(in_buf+filelength)=0x0;
	*(in_buf+filelength+1)=0x0;
	FavManage_ClearFav( &fav );  // Clear the record and move on
	for(i=0; i<filelength; i++){
		// If we don't have a \n add the unsigned character to the current buffer
		if (idx<=1000-3) //3000 byte max record size.
		{
			buf[idx++] = *in_buf++;
		}
		else
		{
			while ( *in_buf != '\n' && *in_buf != '\r') //if record too large, will scan to the end of the line anyway
			{
				*in_buf++;
				i++; 
			}
			crfound=TRUE;  
		}
		while ( *in_buf == '\n' || *in_buf == '\r'|| *in_buf == '\0') //scan thru all CR/LF and EOF
		{
			crfound=TRUE;
			if (*in_buf == '\0') break; //end of file
			*in_buf++;
			i++;
		}
		if (crfound) 
		{
			crfound=FALSE;
			buf[idx++]='\t';  //add a tab to the end to make the InterpretEPGbuff easier to populate
			buf[idx]='\0';
			FavManage_BuildFav( &fav, (const unsigned char*)buf );  //use new buffer to fill epg
			InsertFavItem( fav );
			memset(sBuff,0,64);
			FavManage_ClearFav( &fav );  // Clear the record and move on
			idx = 0;
		}
	} //end while that counts thru the inbuf
	TAP_MemFree((unsigned char *) in_buf);
	memset(sBuff,0,64);
	FavManage_ReadFav = TRUE;
	return 0;
}

bool FavManage_IsActive( void ){
	return FavManage_Active;
}

// function used to hide
void FavManage_Hide( void ){
	FavManage_Active = FALSE;
	FavManage_Redraw = FALSE;
	FavManage_FreeMemory();
	FavManage_MemFree();	// Free the stored graphics
	FavManage_ReadFav = FALSE;
	Display_Hide();
}

void FavManage_SortPriority( void ){
	favitem *tmp;
	int tmpcmp = 0;
	int i, j, idx;
	for( i = 0; i < favitem_ptr; i++ ){
		idx = i;
		for( j = i; j < favitem_ptr; j++ ){
			// Sort by priority then by name
			if( favourites[idx]->priority < favourites[j]->priority ){
				idx = j;
			} else {
				if( favourites[idx]->priority == favourites[j]->priority ){
					// Check the term
					if( (tmpcmp = strcasecmp(favourites[idx]->term,favourites[j]->term)) > 0 ){	// Sort by the feed name
						idx = j;
					}
				}
			}
		}
		tmp = favourites[i];
		favourites[i] = favourites[idx];
		favourites[idx] = tmp;
	}
}

void FavManage_SortName( void ){
	favitem *tmp;
	int tmpcmp = 0;
	int i, j, idx;
	for( i = 0; i < favitem_ptr; i++ ){
		idx = i;
		for( j = i; j < favitem_ptr; j++ ){
			if( (tmpcmp = strcasecmp(favourites[idx]->term,favourites[j]->term)) > 0 ){	// Sort by the feed name
				idx = j;
			}
		}
		tmp = favourites[i];
		favourites[i] = favourites[idx];
		favourites[idx] = tmp;
	}
}


// Function to render the item in the array on the screen
void FavManage_RenderBlankItem( word rgn, int position ){
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
	if( FavManage_ItemBase != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, FavManage_ItemBase );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
	TAP_Osd_FillBox(rgn, x_pos+30, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+100, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
}

char * FavManage_LocationText( int location ){
	static char text[30];
	// Location
	switch( location ){
		case TITLEEXACT:
			sprintf( text, "TitleEx\0");
		 	break;
		case TITLE:
			sprintf( text, "Title\0");
		 	break;
		case SUBTITLE:
			sprintf( text, "Subtitle\0");
		 	break;
		case DESCRIPTION:
			sprintf( text, "Description\0");
		 	break;
		case ALL:
			sprintf( text, "All\0");
		 	break;
		 default:
			sprintf( text, "Unknown\0");
			break;
	}
	return text;
}

// Function to render the item in the array on the screen
void FavManage_RenderItem( word rgn, int row, int selected, int prevselected, bool fresh ){
	int x_pos = 0;
	int y_pos = 0;
	char text[128];
	int width = 0;
	int textOffset = 0;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;

	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (row-FavManage_Min));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	// Draw background for item it previous or selected otherwise the background on text will do the job
	if( row == selected ){
		if( FavManage_ItemHigh != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, FavManage_ItemHigh );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		}
//		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		bc = DISPLAY_ITEMSELECTED;
	} else {
		if( row == prevselected ){
			if( FavManage_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, FavManage_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
			}
			bc = COLOR_None;
		} else {
			if( fresh ){
				if( FavManage_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, FavManage_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
				}
				bc = COLOR_None;
			}
		}
	}
	TAP_Osd_FillBox(rgn, x_pos+30, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+100, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	if( favourites[row] == NULL ) return;	// Incase we are nothing return to avoid any freezing
	// Lets place the text on the item now
	// Lets place the text on the items now align the first 2 columns in the middle of there cells
	// Priority
	TAP_SPrint( text, "%d\0", favourites[row]->priority);
	width = TAP_Osd_GetW( text, 0, FNT_Size_1622);
	textOffset = (30 - width)/2;	// Cell width - string width / 2
	Font_Osd_PutString1622( rgn, x_pos+textOffset, y_pos + 3, x_pos + 30 - textOffset, text, fc, COLOR_None );
	
	// Location
	switch( favourites[row]->location ){
		case TITLEEXACT:
			sprintf( text, "TitleEx\0");
		 	break;
		case TITLE:
			sprintf( text, "Title\0");
		 	break;
		case SUBTITLE:
			sprintf( text, "Subtitle\0");
		 	break;
		case DESCRIPTION:
			sprintf( text, "Description\0");
		 	break;
		case ALL:
			sprintf( text, "All\0");
		 	break;
		 default:
			sprintf( text, "Unknown\0");
			break;
	}
	width = TAP_Osd_GetW( text, 0, FNT_Size_1622);
	textOffset = (60 - width)/2;	// Cell width - string width / 2
	if( textOffset < 0 ) textOffset =0;
	Font_Osd_PutString1622( rgn, x_pos+textOffset+35, y_pos + 3, x_pos + 95-textOffset, text, fc, COLOR_None );

	// Start
	sprintf(text,"%s to %s\0", MinsToHHMM(favourites[row]->start), MinsToHHMM2(favourites[row]->end));
	width = TAP_Osd_GetW( text, 0, FNT_Size_1622);
	textOffset = (143 - width)/2;	// Cell width - string width / 2
	if( textOffset < 0 ) textOffset =0;
	Font_Osd_PutString1622( rgn, x_pos+textOffset+95, y_pos + 3, x_pos + 238-textOffset, text, fc, COLOR_None );
	
	// The term to search for
	Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP+235, y_pos + 3, 720-((Panel_Left_X-DISPLAY_X_OFFSET-DISPLAY_X_OFFSET)), favourites[row]->term, fc, COLOR_None );
}

void FavManage_Render( void ){
	int i = 0;
	int c = 0;
	char text[128];
	Display_CreateRegions();
	if( !FavManage_Active ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		Display_DrawTime();
		FavManage_RenderHelp();
		if( FavManage_SortOrder == SORTFAVPRIORITY ){
			sprintf(text, "Favourites (%d) - [ Priority ]\0", favitem_ptr );
			Display_RenderTitle(text);
		} else {
			sprintf(text, "Favourites (%d) - [ Name ]\0", favitem_ptr );
			Display_RenderTitle(text);
		}
		c = 0;
		for( i = FavManage_Min; i < FavManage_Max && i < favitem_ptr; i++ ){
			FavManage_RenderItem(Display_RGN,i,FavManage_Selected, FavManage_PrevSelected,TRUE);
			c++;
		}
		for( i = c; i < FavManage_MaxLIST; i++ ){
			FavManage_RenderBlankItem( Display_RGN, i );
		}
		// Draw the template on the display by fading in
		Screen_FadeIn(Display_RGN,Setting_FadeIn);
	} else {
		if( FavManage_Redraw ){
			if( !Display_BackgroundDrawn ){
				Display_DrawBackground();
				Display_DrawTime();
				FavManage_RenderHelp();
			}
			TAP_Osd_Copy( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			c = 0;
			for( i = FavManage_Min; i < FavManage_Max && i < favitem_ptr; i++ ){
				FavManage_RenderItem(Display_MemRGN,i,FavManage_Selected, FavManage_PrevSelected,TRUE);
				c++;
			}
			for( i = c; i < FavManage_MaxLIST; i++ ){
				FavManage_RenderBlankItem( Display_MemRGN, i );
			}
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			if( FavManage_SortOrder == SORTFAVPRIORITY ){
				sprintf(text, "Favourites (%d) - [ Priority ]\0", favitem_ptr );
				Display_RenderTitle(text);
			} else {
				sprintf(text, "Favourites (%d) - [ Name ]\0", favitem_ptr );
				Display_RenderTitle(text);
			}
			FavManage_Redraw = FALSE;
		} else {
			FavManage_RenderItem(Display_MemRGN,FavManage_Selected,FavManage_Selected, FavManage_PrevSelected,TRUE);
			FavManage_RenderItem(Display_MemRGN,FavManage_PrevSelected,FavManage_Selected, FavManage_PrevSelected,TRUE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_PrevSelected-FavManage_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_PrevSelected-FavManage_Min)), FALSE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_Selected-FavManage_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_Selected-FavManage_Min)), FALSE);
		}
	}
	FavManage_Active = TRUE;
}

void FavManage_Activate( void ){
	FavManage_Min = 0;
	FavManage_Max = FavManage_MaxLIST;
	FavManage_Selected = 0;
	FavManage_PrevSelected = 0;
	FavManage_Column = 0;
	FavManage_Redraw = TRUE;
	
	if( !FavManage_ReadFav ){	// Haven't read the file yet so we will read and flag as no changes
		TAP_Hdd_ChangeDir("/ProgramFiles/");
		FavManage_LoadFavFile("favourites.ini");
		FavManage_Changed = FALSE;
	}
	if( FavManage_Max > favitem_ptr -1 ){
		FavManage_Max = favitem_ptr;
	}
	if( favitem_ptr > 0 ){
		if( FavManage_SortOrder == SORTFAVPRIORITY ){
			FavManage_SortPriority();
		} else {
			FavManage_SortName();
		}
	}
	FavManage_CreateGradients();
	FavManage_Render();
}

void FavManage_SortCallback( bool yes, int selected ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		FavManage_SortOrder = selected;
		if( FavManage_SortOrder == SORTFAVPRIORITY ){
			FavManage_SortPriority();
		} else {
			FavManage_SortName();
		}
		FavManage_Redraw = TRUE;
		FavManage_Render();
	}
}

void FavManage_ReloadCallback( bool yes ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		FavManage_FreeMemory(); // Free the currently used memory
		TAP_Hdd_ChangeDir("/ProgramFiles/");
		FavManage_LoadFavFile("favourites.ini");
		FavManage_Min = 0;
		FavManage_Max = FavManage_MaxLIST;
		if( FavManage_Max > favitem_ptr -1 ){
			FavManage_Max = favitem_ptr;
		}
		if( favitem_ptr > 0 ){
			switch( FavManage_SortOrder ){
				case SORTFAVNAME:
					FavManage_SortName();
					break;
				case SORTFAVPRIORITY:
					FavManage_SortPriority();
					break;
			}
		}
		FavManage_Changed = FALSE;
		FavManage_Redraw = TRUE;
		FavManage_Render();
	}
}

void Search_FavSaveCallback( bool yes ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		FavManage_SaveFile();
	}
	FavManage_Hide();
}

bool FavMange_isDisabled( char * term ){
	if( strlen(term) > 0 ){
		if( term[0] == '#' ){
			return TRUE;
		}
	}
	return FALSE;
}

void FavManage_EnableFav( int item ){
	char tmp[128];
	memset(tmp, 0, sizeof(tmp) );
	strcpy(tmp, favourites[item]->term );
	memset(favourites[item]->term, 0, sizeof(favourites[item]->term) );
	strcpy(favourites[item]->term, tmp+1 );
	FavManage_Changed = TRUE;
}

void FavManage_DisableFav( int item ){
	char tmp[128];
	memset(tmp, 0, sizeof(tmp) );
	strcat(tmp,"#");
	strcat(tmp, favourites[item]->term );
	memset(favourites[item]->term, 0, sizeof(favourites[item]->term) );
	strcpy(favourites[item]->term, tmp );
	FavManage_Changed = TRUE;
}


void FavManage_DeleteCallback( int option ){
	int i = 0;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( favitem_ptr < 1 ) return;	// Can't delete as we have nothing in our array so return
	if( option == THREEOPTION_OPTION1 ){
		// Remove the favourite item from the array of favourites
		for( i = FavManage_Selected; i < favitem_ptr -1; i++ ){
			// Move the items above us down in the array
			favourites[i] = favourites[i+1];
		}
		favitem_ptr--;
		if( favitem_ptr < FavManage_MaxLIST ){	// Less than a page then adjust the end render item
			FavManage_Max = favitem_ptr;
		}
		if( FavManage_Selected > favitem_ptr-1 ){
			FavManage_Selected = 0;
			FavManage_Min = 0;
			FavManage_Max = FavManage_MaxLIST;
			if( FavManage_Max > favitem_ptr -1 ){
				FavManage_Max = favitem_ptr;
			}
		}
		FavManage_Redraw = TRUE;
		FavManage_Render();
		FavManage_Changed = TRUE;
	} else {
		// If disable then add # at the start of the search term
		if( option == THREEOPTION_OPTION2 ){
			if( FavMange_isDisabled(favourites[FavManage_Selected]->term) ){
				FavManage_EnableFav(FavManage_Selected);
			} else {
				FavManage_DisableFav(FavManage_Selected);
			}
			FavManage_Redraw = TRUE;
			FavManage_Render();
			FavManage_Changed = TRUE;
		}
	}
}

void FavManage_WriteFavs(char *FileName, char *Text){
  TYPE_File             *File;
  dword                 LogSize;
  char                  CRLF [] = {'\r', '\n'};

  if (FileName && FileName [0])
  {
    InitTAPAPIFix();

    TAP_Hdd_Create (FileName, ATTR_NORMAL);
    if ((File = TAP_Hdd_Fopen (FileName)) != NULL)
    {
      LogSize = TAP_Hdd_Flen (File);
      TAP_Hdd_Fseek (File, 0, SEEK_END);
      if (Text && Text [0]) TAP_Hdd_Fwrite (Text, strlen (Text), 1, File);
      TAP_Hdd_Fwrite (CRLF, 2, 1, File);
      HDD_TruncateFile (File, LogSize + strlen (Text) + 2);
      HDD_TouchFile (File);
      TAP_Hdd_Fclose (File);

      //The following Create/Delete ensures that the FAT is flushed onto the HDD
      TAP_Hdd_Create("tmp", ATTR_NORMAL);
      TAP_Hdd_Delete("tmp");
    }
  }
}

// HELP PANEL
void FavManage_RenderHelp( void ){
	int x_pos = 0;
	int y_pos = 0;
	int width = 0;
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST))+10;
	width = DISPLAY_ROW_WIDTH;
	// Fill the background
	TAP_Osd_FillBox(Display_RGN, x_pos, y_pos, width, 30, DISPLAY_MAIN);
	// Draw the keyboard and Timer edit help
	TAP_Osd_PutGd( Display_RGN, x_pos+5, y_pos, &_redGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30, y_pos, x_pos+width, "New Favourite", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+130, y_pos, &_whiteGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+130, y_pos, x_pos+width, "Delete Favourite", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+270, y_pos, &_okGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+270, y_pos, x_pos+width, "Edit Favourite", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+395, y_pos, &_pauseGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+395, y_pos, x_pos+width, "Sort Order", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+503, y_pos, &_recallGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+35+503, y_pos, x_pos+width, "Reload", DISPLAY_EVENT, COLOR_None);
}


char * Search_FavLocation( int location ){
	static char tmp[30];
	memset( tmp, '\0', sizeof(tmp) );	// Clear out garbage
	switch( location ){
		case TITLEEXACT:
			sprintf(tmp,"TitleExact");
			break;
		case TITLE:
			sprintf(tmp,"Title");
			break;
		case SUBTITLE:
			sprintf(tmp,"Subtitle");
			break;
		case DESCRIPTION:
			sprintf(tmp,"Description");
			break;
		case ALL:
			sprintf(tmp,"All");
			break;
		default:
			sprintf(tmp,"Title");
			break;
	}
	return tmp;
}

char * Search_FavTime( int time ){
	static char tmp[10];
	memset( tmp, '\0', sizeof(tmp) );	// Clear out garbage
	if( time == -1 ){
		sprintf(tmp,"Any");
	} else {
		sprintf(tmp,"%d", time);
	}
	return tmp;
}

char * Search_FavTime2( int time ){
	static char tmp[10];
	memset( tmp, '\0', sizeof(tmp) );	// Clear out garbage
	if( time == -1 ){
		sprintf(tmp,"Any");
	} else {
		sprintf(tmp,"%d", time);
	}
	return tmp;
}

void FavManage_Append( void ){
	char tmp[512];
	dword timeout;
	timeout = TAP_GetTick();
	TAP_Hdd_ChangeDir("/ProgramFiles/");
	memset( tmp, '\0', sizeof(tmp) );	// Clear of garbage
	sprintf(tmp, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\0", favourites[favitem_ptr-1]->term, favourites[favitem_ptr-1]->channel, favourites[favitem_ptr-1]->day, Search_FavLocation(favourites[favitem_ptr-1]->location), Search_FavTime(favourites[favitem_ptr-1]->start), Search_FavTime2(favourites[favitem_ptr-1]->end), favourites[favitem_ptr-1]->priority, favourites[favitem_ptr-1]->rpt, favourites[favitem_ptr-1]->adj);
	FavManage_WriteFavs("favourites.ini",tmp);
}

void FavManage_SaveFile( void ){
	char tmp[300];
	int i = 0;
	dword timeout;
	char text[256];
	char filebuf[10000];
	timeout = TAP_GetTick();
	TAP_Hdd_ChangeDir("/ProgramFiles/");
    TAP_Hdd_Delete("favourites.ini");
	Message_Display("Please Wait. Saving favourites");
	memset(text,0x0,sizeof(text));
	memset(filebuf,0x0,sizeof(filebuf));
	for( i = 0; i < favitem_ptr; i++ ){
		memset( tmp, '\0', sizeof(tmp) );	// Clear of garbage
		sprintf(tmp, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\0", favourites[i]->term, favourites[i]->channel, favourites[i]->day, Search_FavLocation(favourites[i]->location), Search_FavTime(favourites[i]->start), Search_FavTime2(favourites[i]->end), favourites[i]->priority, favourites[i]->rpt==0?"0":"1", favourites[i]->adj==0?"0":"1");
		// Append to buffer. If greater then the buffer then writeout buffer and start again
		if( strlen(filebuf) + strlen(tmp) < sizeof(filebuf) ){
			strcat(filebuf, tmp );
			strcat(filebuf, "\r\n" );
		} else {
			// We need to write the buffer to file and start using the buffer from the start again
			FavManage_WriteFavs("favourites.ini",filebuf);	
			memset(filebuf,0x0,sizeof(filebuf));
			strcat(filebuf, tmp );
			strcat(filebuf, "\r\n" );
		}
		if( timeout + 20 < TAP_GetTick() ){
			if( strlen(text) < 50 ){	// Make sure we don't overflow our array
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			} else {
				memset(text,0x0,sizeof(text));
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			}
		}
		TAP_SystemProc();			// Allow system to do what it needs to do
	}
	if( strlen(filebuf) > 0 ){
		FavManage_WriteFavs("favourites.ini",filebuf);	
	}
	FavManage_Changed = FALSE;
	TAP_Delay(50);
	Message_DisplayClear();
	TAP_Delay(50);
}

void FavManage_ResetLocation( void ){
	FavManage_Selected = 0;
	FavManage_Min=0;
	FavManage_Max = FavManage_MaxLIST;
	if( favitem_ptr < FavManage_MaxLIST ){
		FavManage_Max = favitem_ptr;
	}
	FavManage_Redraw = TRUE;
}

void Search_FavEditReturn( favitem a, bool cancel ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( !cancel ){	// User didnt cancel
		TAP_MemFree(favourites[FavManage_Selected]);	// Free the selected favourite from the array as we will replace with the returned favitem
		favourites[FavManage_Selected] = TAP_MemAlloc(sizeof(favitem));
		memcpy(favourites[FavManage_Selected], &a, sizeof(favitem));
		FavManage_Redraw = TRUE;
		if( FavManage_SortOrder == SORTFAVPRIORITY ){
			FavManage_SortPriority();
		} else {
			FavManage_SortName();
		}
		FavManage_Changed = TRUE;
		FavManage_Render();	
	}
}

void Search_FavNewReturn( favitem a, bool cancel ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( !cancel ){	// User didnt cancel
		InsertFavItem( a );
		if( favitem_ptr < FavManage_MaxLIST ){
			FavManage_Max = favitem_ptr;
		}
		FavManage_Redraw = TRUE;
		if( FavManage_SortOrder == SORTFAVPRIORITY ){
			FavManage_SortPriority();
		} else {
			FavManage_SortName();
		}
		FavManage_Changed = TRUE;
		FavManage_Render();	
	}
}

// Keyhandler for the Fav screen
dword FavManage_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( YesNo_IsActive() ){
		return YesNo_KeyHandler(param1);
	}
	if (ThreeOption_IsActive() ) {
		return ThreeOption_KeyHandler(param1);
	}
	if( FavSortOrder_IsActive() ){
		return FavSortOrder_KeyHandler(param1);
	}
	if( FavEdit_IsActive() ){
		return FavEdit_KeyHandler(param1);
	}
	if( FavNew_IsActive() ){
		return FavNew_KeyHandler(param1);
	}
	switch( param1 ){
		case RKEY_Menu:
			FavManage_Hide();
			Menu_Render();
			return 0;
			break;
		case RKEY_Pause:
			FavSortOrder_Init( Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
			FavSortOrder_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !FavSortOrder_Activate("Sort Order",&FavManage_SortCallback) ){
				Message_Display( "Option box failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_ChDown:
			FavManage_PrevSelected = FavManage_Selected;
			FavManage_Selected++;
			if( FavManage_Selected > FavManage_Max-1 ){
				FavManage_Min = FavManage_Selected;
				FavManage_Max = FavManage_Min + FavManage_MaxLIST;
				FavManage_Redraw = TRUE;
				if( FavManage_Max > favitem_ptr-1 ){
					FavManage_Min = favitem_ptr-FavManage_MaxLIST;
					FavManage_Max = favitem_ptr;
				}
			}
			if( FavManage_Selected > favitem_ptr-1 ){
				FavManage_Selected = 0;
				FavManage_Min = 0;
				FavManage_Max = FavManage_MaxLIST;
				if( FavManage_Max > favitem_ptr -1 ){
					FavManage_Max = favitem_ptr;
				}
				FavManage_Redraw = TRUE;
			}
			FavManage_Render();
			return 0;
			break;
		case RKEY_VolUp:
			FavManage_PrevSelected = FavManage_Selected;
			FavManage_Selected+=FavManage_MaxLIST;
			FavManage_Min +=FavManage_MaxLIST;
			FavManage_Max +=FavManage_MaxLIST;
			if( FavManage_Max > favitem_ptr-1 ){
				FavManage_Max = favitem_ptr;
				FavManage_Min = FavManage_Max - FavManage_MaxLIST;
			}
			if( FavManage_Selected > favitem_ptr-1 ){
				FavManage_Selected = 0;
				FavManage_Min = 0;
				FavManage_Max = FavManage_MaxLIST;
				if( FavManage_Max > favitem_ptr-1 ) FavManage_Max = favitem_ptr;
			}
			if( FavManage_Min < 0 ) FavManage_Min = 0;
			FavManage_Redraw = TRUE;
			FavManage_Render();
			return 0;
			break;
		case RKEY_VolDown:
			FavManage_PrevSelected = FavManage_Selected;
			FavManage_Selected-=FavManage_MaxLIST;
			FavManage_Min -=FavManage_MaxLIST;
			FavManage_Max -=FavManage_MaxLIST;
			if( FavManage_Min < 0 ){
				FavManage_Min = 0;
				FavManage_Max = FavManage_MaxLIST;
				if( FavManage_Max > favitem_ptr-1 ) FavManage_Max = favitem_ptr;
			}
			if( FavManage_Selected < 0 ){
				FavManage_Selected = favitem_ptr-1;
				FavManage_Max = favitem_ptr;
				FavManage_Min = FavManage_Max-FavManage_MaxLIST;
				if( FavManage_Min < 0 ) FavManage_Min = 0;
			}
			FavManage_Redraw = TRUE;
			FavManage_Render();
			return 0;
			break;
		case RKEY_ChUp:
			FavManage_PrevSelected = FavManage_Selected;
			FavManage_Selected--;
			if( FavManage_Selected < FavManage_Min ){
				FavManage_Min = FavManage_Selected-FavManage_MaxLIST+1;
				FavManage_Max = FavManage_Selected+1;
				FavManage_Redraw = TRUE;
				if( FavManage_Min < 0 ){
					FavManage_Min = 0;
					FavManage_Max = FavManage_MaxLIST;
					if( FavManage_Max > favitem_ptr-1 ){
						FavManage_Max = favitem_ptr;
					}
				}
			}
			if( FavManage_Selected < 0 ){
				FavManage_Selected = favitem_ptr-1;
				FavManage_Max = favitem_ptr;
				FavManage_Min = FavManage_Max-FavManage_MaxLIST;
				if( FavManage_Min < 0 ){
					FavManage_Min = 0;
				}
				FavManage_Redraw = TRUE;
			}
			FavManage_Render();
			return 0;
			break;
		case RKEY_Recall:
			if( favitem_ptr < 1 ) return 0;
			YesNo_Init( Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !YesNo_Activate("Reload favourites file?",&FavManage_ReloadCallback) ){
				Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_Ab:
			if( favitem_ptr < 1 ) return 0;
			ThreeOption_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150);
			ThreeOption_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if ( !ThreeOption_Activate("Delete or disable favourite?","Delete","Disable",THREEOPTION_OPTION1,&FavManage_DeleteCallback) ) {
			}
//			YesNo_Init( Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
//			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
//			if( !YesNo_Activate("Delete favourite?",&FavManage_DeleteCallback) ){
//				Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
//			}
			return 0;
			break;
		case RKEY_0:
			FavManage_ResetLocation();
			FavManage_Render();
			return 0;
			break;
		case RKEY_Ok:
			FavEdit_Init( Display_RGN, Display_MemRGN, (720/2)-(350/2)+DISPLAY_X_OFFSET, (576/2)-(385/2)+DISPLAY_Y_OFFSET+5, 350, 385 );
			FavEdit_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !FavEdit_Activate(FavManage_Selected,&Search_FavEditReturn) ){
				Message_Display( "Favourite Editing Failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_NewF1:
			FavNew_Init( Display_RGN, Display_MemRGN, (720/2)-(350/2)+DISPLAY_X_OFFSET, (576/2)-(385/2)+DISPLAY_Y_OFFSET+5, 350, 385 );
			FavNew_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !FavNew_Activate(&Search_FavNewReturn) ){
				Message_Display( "Favourite New Dialog Failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_Sat:
			if( FavMange_isDisabled(favourites[FavManage_Selected]->term) ){
				FavManage_EnableFav(FavManage_Selected);
			} else {
				FavManage_DisableFav(FavManage_Selected);
			}
			FavManage_Redraw = TRUE;
			FavManage_Render();
			FavManage_Changed = TRUE;
			return 0;
			break;
	}
	if( param1 == RKEY_Exit || param1 == Settings_FavListKey ){
		if( FavManage_Changed ){
			if( favitem_ptr < 1 ) return 0;
			YesNo_Init( Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !YesNo_Activate("Save changes?",&Search_FavSaveCallback) ){
				Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
			}
		} else {
			FavManage_Hide();
		}
		return 0;
	}
	return 0;
}

