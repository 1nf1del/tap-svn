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

#include "display.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "menu.h"

bool Menu_IsActive( void ){
	return Menu_Active;
}

// function used to hide menu
void Menu_Hide( void ){
	Menu_Active = FALSE;
	Menu_Min = 0;
	Menu_Max = MENU_ITEMS;
	Menu_Selected = 1;
	Menu_PrevSelected = 1;
	Menu_Redraw = TRUE;
	Settings_Save();
	Menu_MemFree();
	Display_Hide();
}

void Menu_MemFree( void ){
	if (Menu_ItemBase) TAP_MemFree(Menu_ItemBase);
	if (Menu_ItemHigh) TAP_MemFree(Menu_ItemHigh);
	Menu_ItemBase = NULL;
	Menu_ItemHigh = NULL;
}

void Menu_CreateGradients( void ){
	word rgn;
	Menu_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEM);
		Menu_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		Menu_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		TAP_Osd_Delete(rgn);
	}
}

// Function to render the item in the array on the screen
void Menu_RenderSectionTitle( word rgn, int position, char *option ){
	int x_pos = 0;
	int y_pos = 0;
	dword fc = DISPLAY_EVENT;
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position-Menu_Min));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	if( position-Menu_Min < 0 ) return;
	// ALter the text colour if required
	TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
	Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP, y_pos + 3, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP, option, fc, COLOR_None );
}

// Function to render the item in the array on the screen
void Menu_RenderItem( word rgn, int position, char *option, char *value, int selected ){
	int x_pos = 0;
	int y_pos = 0;
	dword fc = DISPLAY_EVENT;
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position-Menu_Min));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	if( position-Menu_Min < 0 ) return;
	// ALter the text colour if required
	if( position == selected ){
		if( Menu_ItemHigh != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Menu_ItemHigh );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		}
//		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, MENU_ITEM_H, MENU_SELECTEDITEM);
		TAP_Osd_FillBox(rgn, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP - MENU_OPTION_W, y_pos, MENU_GAP, MENU_ITEM_H, MENU_MAIN);
	} else {
		if( Menu_ItemBase != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Menu_ItemBase );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
		}
//		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, MENU_ITEM_H, MENU_ITEM);
		TAP_Osd_FillBox(rgn, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP - MENU_OPTION_W, y_pos, MENU_GAP, MENU_ITEM_H, MENU_MAIN);
	}
	Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP, y_pos + 3, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP - MENU_OPTION_W, option, fc, COLOR_None );
	Font_Osd_PutString1622( rgn, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP - MENU_OPTION_W+DISPLAY_GAP+DISPLAY_GAP+DISPLAY_GAP, y_pos + 3, x_pos + 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2) - DISPLAY_GAP, value, fc, COLOR_None);
}

void Menu_OptionInfo( word rgn, char *text ){
 	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_MAIN;
	// Sort out the X position
	int x = Panel_Left_X;
	int y = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * MENU_ITEMS);
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	// Render the display at the bottom
	TAP_Osd_FillBox(rgn, x, y, DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 3), DISPLAY_MAIN );
	// Allow for a really long message	
	WrapPutStr(rgn, text, x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP, 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-(DISPLAY_GAP+DISPLAY_GAP), fc, bc, 4, FNT_Size_1622, 5);
}

void Menu_ItemDesc( word rgn, int item ){
	switch( item ){
		case DEFAULT_SORT:
			Menu_OptionInfo(rgn,"This is a option for the default sorting method when guide is first displayed. Currently only Name, Channel, Start Time and Rating are avaliable.");
			return;
			break;
		case MESSAGE_TIMEOUT:
			Menu_OptionInfo(rgn,"This is a option for the timeout period for messages.");
			return;
			break;
		case SEARCH_DELAY:
			Menu_OptionInfo(rgn,"This is a option for the number of ticks from the last keypress to activate searching when using the keyboard feature.");
			return;
			break;
		case FADE_OUT:
			Menu_OptionInfo(rgn,"This is a option which toggles whether to fade the display out when closing.");
			return;
			break;
		case FADE_IN:
			Menu_OptionInfo(rgn,"This is a option which toggles whether to fade the display in when opening.");
			return;
			break;
		case PIG:
			Menu_OptionInfo(rgn,"This is a option which toggles whether to display the current channel or playing file when displaying. If no then the next occuring timers will be placed on screen to replace the PIG");
			return;
			break;
		case TIMERLISTMODE:
			Menu_OptionInfo(rgn,"Option to display all timers when the Timer List is activated");
			return;
			break;
		case TIMERLISTLOGOS:
			Menu_OptionInfo(rgn,"Display channel logos in the timerlist");
			return;
			break;		
		case SHOW_TIMES:
			Menu_OptionInfo(rgn,"Show times in the Linear EPG");
			return;
			break;
		case SHOW_PARTIAL:
			Menu_OptionInfo(rgn,"Select Yes to show partial timers with an orange circle.");
			return;
			break;
		case TAP_EXIT:
			Menu_OptionInfo(rgn,"Press OK to exit TAP");
			return;
			break;
		case LINEAREPGKEY:
			Menu_OptionInfo(rgn,"Set the activation key for the Linear EPG. Press the key you wish to activate the guide with. Press 0 to remove key");
			return;
			break;
		case XOFFSET:
			Menu_OptionInfo(rgn,"Set the X offset for the display");
			return;
			break;
		case YOFFSET:
			Menu_OptionInfo(rgn,"Set the Y offset for the display");
			return;
			break;
		case TIMERPADDINGSTART:
			Menu_OptionInfo(rgn,"Padding to add to the start of a timer set by this guide");
			return;
			break;
		case TIMERPADDINGEND:
			Menu_OptionInfo(rgn,"Padding to add to the end of a timer set by this guide");
			return;
			break;
		case APPENDEPISODE:
			Menu_OptionInfo(rgn,"Whether to append the episode name if present. Currently under testing and may contain bugs");
			return;
			break;
		case TIMERRENAME:
			Menu_OptionInfo(rgn,"Toggle whether to allow this TAP to rename the timers when setting and deletion occurs");
			return;
			break;	
		case RENDERBACKGROUND:
			Menu_OptionInfo(rgn,"Toggle whether to render background of the display");
			return;
			break;
		case TIMERLISTKEY:
			Menu_OptionInfo(rgn,"Key assigned to launch the timer list. Press 0 to remove key");
			return;
			break;
		case RSSLISTKEY:
			Menu_OptionInfo(rgn,"Key assigned to launch the RSS news list. Press 0 to remove key");
			return;
			break;
		case MULTILISTKEY:
			Menu_OptionInfo(rgn,"Preview of new mode call multilist. Same layout as JustEPG but currently has no navigation");
			return;
			break;
		case GRIDEPGCHANNELS:
			Menu_OptionInfo(rgn,"Number of channels to display in the Grid EPG View");
			return;
			break;
		case GRIDEPGLOGOS:
			Menu_OptionInfo(rgn,"Display channel logos in the Grid EPG View.");
			return;
			break;		
		case FAVLISTKEY:
			Menu_OptionInfo(rgn,"Key assigned to launch the Favourites editor. Press 0 to remove key");
			return;
			break;	
		case GRIDEPGKEY:
			Menu_OptionInfo(rgn,"Key assigned to launch the Grid EPG. Press 0 to remove key");
			return;
			break;
		case GRIDEPGSIZE:
			Menu_OptionInfo(rgn,"Default size of the Grid EPG view.");
			return;
			break;
		case GRADIENT_FACTOR:
			Menu_OptionInfo(rgn,"Factor to be used for the gradient. 0 for no gradient max is 5");
			return;
			break;
		case RELOADHOUR:
			Menu_OptionInfo(rgn,"The hour for the events to be reloaded.");
			return;
			break;
		case LOADDELAY:
			Menu_OptionInfo(rgn,"The delay in seconds for events to be loaded at startup.");
			return;
			break;
		default:
			Menu_OptionInfo(rgn,"");
			return;
			break;
	}
}

void Guide_MenuItem( word rgn, int item, int selected ){
	switch( item ){
		case LINEAREPG_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Linear View - Settings" );
			return;
			break;
		case GRIDEPG_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Grid View - Settings" );
			return;
			break;
		case TIMERLIST_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Timerlist - Settings" );
			return;
			break;
		case RSSNEWS_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "RSS News - Settings" );
			return;
			break;		
		case FAVMANGE_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Favourites Manager - Settings" );
			return;
			break;
		case MULTILIST_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Multilist - Settings" );
			return;
			break;			
		case MAIN_OPTIONS:
			Menu_RenderSectionTitle( rgn, item, "Main - Settings" );
			return;
			break;
		case DEFAULT_SORT:
			Menu_RenderItem(rgn,item,"Default sorting",Events_SortOrderText(Setting_DefaultOrder), selected);
			return;
			break;
		case MESSAGE_TIMEOUT:
			Menu_RenderItem(rgn,item,"Timeout for messages",NumberToString(Setting_MessageTimeout),selected);
			return;
			break;
		case SEARCH_DELAY:
			Menu_RenderItem(rgn,item,"Timeout for search",NumberToString(Setting_SearchDelay),selected);
			return;
			break;
		case FADE_OUT:
			Menu_RenderItem(rgn,item,"Fade Out on close",Setting_FadeOut?"Yes":"No",selected);
			return;
			break;
		case FADE_IN:
			Menu_RenderItem(rgn,item,"Fade In on opening",Setting_FadeIn?"Yes":"No",selected);
			return;
			break;
		case TIMERLISTMODE:
			Menu_RenderItem(rgn,item,"List all timers by default",Settings_ListAllTimers?"Yes":"No",selected);
			return;
			break;
		case TIMERLISTLOGOS:
			Menu_RenderItem(rgn,item,"Show logos in the timer list",Settings_TimerListLogos?"Yes":"No",selected);
			return;
			break;		
		case PIG:
			Menu_RenderItem(rgn,item,"Display Picture In Graphics",Setting_PIG?"Yes":"No",selected);
			return;
			break;
		case SHOW_TIMES:
			Menu_RenderItem(rgn,item,"Show Times",Settings_LinearEPGTimes?"Yes":"No",selected);
			return;
			break;
		case SHOW_PARTIAL:
			Menu_RenderItem(rgn,item,"Show partial recordings/timers",Setting_ShowPartialTimers?"Yes":"No",selected);
			return;
			break;
		case TAP_EXIT:
			Menu_RenderItem(rgn,item,"Exit TAP","OK",selected);
			return;
			break;
		case LINEAREPGKEY:
			Menu_RenderItem(rgn,item,"Activation key for guide",RKEY_NameShort(Settings_LinearEPGKey),selected);
			return;
			break;
		case XOFFSET:
			Menu_RenderItem(rgn,item,"X Offset for the display",NumberToString(Settings_OffsetX),selected);
			return;
			break;
		case YOFFSET:
			Menu_RenderItem(rgn,item,"Y Offset for the display",NumberToString(Settings_OffsetY),selected);
			return;
			break;
		case GRIDEPGCHANNELS:
			Menu_RenderItem(rgn,item,"Grid EPG Channels",NumberToString(Settings_GridEPGChannels),selected);
			return;
			break;
		case GRIDEPGLOGOS:
			Menu_RenderItem(rgn,item,"Display logos in Grid EPG",Settings_GridEPGLogos?"Yes":"No", selected);
			return;
			break;
		case TIMERPADDINGSTART:
			Menu_RenderItem(rgn,item,"Timer start padding",NumberToString(Settings_StartBuffer),selected);
			return;
			break;
		case TIMERPADDINGEND:
			Menu_RenderItem(rgn,item,"Timer end padding",NumberToString(Settings_EndBuffer),selected);
			return;
			break;
		case APPENDEPISODE:
			Menu_RenderItem(rgn,item,"Append episode name",Settings_AppendEpisode?"Yes":"No", selected);
			return;
			break;
		case TIMERRENAME:
			Menu_RenderItem(rgn,item,"Allow timer rename",Settings_RenameTimers?"Yes":"No", selected);
			return;
			break;	
		case RENDERBACKGROUND:
			Menu_RenderItem(rgn,item,"Render background",Settings_RenderBackground?"Yes":"No", selected);
			return;
			break;
		case TIMERLISTKEY:
			Menu_RenderItem(rgn,item,"Activation key for timerlist",RKEY_Name(Settings_TimerListKey), selected);
			return;
			break;
		case RELOADHOUR:
			Menu_RenderItem(rgn,item,"Reload Hour", Settings_ReloadHour==-1?"Disabled":NumberToString(Settings_ReloadHour), selected);
			return;
			break;
		case LOADDELAY:
			Menu_RenderItem(rgn,item,"Startup Delay", Settings_LoadDelay==-1?"Disabled":SecsToString(Settings_LoadDelay/100), selected);
			return;
			break;
		case RSSLISTKEY:
			Menu_RenderItem(rgn,item,"Activation key for RSS news list",RKEY_Name(Settings_RSSListKey), selected);
			return;
			break;
		case FAVLISTKEY:
			Menu_RenderItem(rgn,item,"Activation key for Favourites list",RKEY_Name(Settings_FavListKey), selected);
			return;
			break;	
		case GRIDEPGKEY:
			Menu_RenderItem(rgn,item,"Activation key for Grid EPG",RKEY_Name(Settings_GridEPGKey), selected);
			return;
			break;	
		case MULTILISTKEY:
			Menu_RenderItem(rgn,item,"Activation key for MultiList EPG",RKEY_Name(Settings_MultiListEPGKey), selected);
			return;
			break;				
		case GRIDEPGSIZE:
			Menu_RenderItem(rgn,item,"Default size of EPG grid",NumberToString(Settings_GridEPGSize), selected);
			return;
			break;
		case GRADIENT_FACTOR:
			Menu_RenderItem(rgn,item,"Gradient Factor",NumberToString(Settings_GradientFactor), selected);
			return;
			break;			
		default:
			Menu_RenderItem(rgn,item,"","",selected);
			return;
			break;
	}
}

void Menu_RenderExisting( void ){
	int i;
	Display_CreateRegions();
	Menu_CreateGradients();
	if( !Menu_IsActive() ){
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		// Render the template as the loading will take some time
		TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
		Menu_Min = 0;
		Menu_Max = MENU_ITEMS;
		Menu_Selected = 1;
		Menu_PrevSelected = 1;
		for( i = Menu_Min; i < Menu_Max; i++ ){
			Guide_MenuItem(Display_MemRGN,i,Menu_Selected);
		}
		TAP_Osd_Copy( Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0, FALSE);
		Menu_ItemDesc( Display_RGN, Menu_Selected );
		Menu_Redraw = FALSE;
		Display_DrawTime();
		// Draw the new title and flag that it has changed		
		Display_RenderTitle("Settings - MultiViewEPG");
		Display_TitleDrawn = FALSE;
	}
	Menu_Active = TRUE;
}

void Menu_Render( void ){
	int i;
	Display_CreateRegions();
	Menu_CreateGradients();
	if( !Menu_IsActive() ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		// Render the template as the loading will take some time
		TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
		Menu_Min = 0;
		Menu_Max = MENU_ITEMS;
		Menu_Selected = 1;
		Menu_PrevSelected = 1;
		for( i = Menu_Min; i < Menu_Max; i++ ){
			Guide_MenuItem(Display_MemRGN,i,Menu_Selected);
		}
		TAP_Osd_Copy( Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0, FALSE);
		Menu_ItemDesc( Display_RGN, Menu_Selected );
		Menu_Redraw = FALSE;
		Display_DrawTime();
		// Draw the new title and flag that it has changed		
		Display_RenderTitle("Settings - MultiViewEPG");
		Display_TitleDrawn = FALSE;
		// Draw the template on the display by fading in
		Screen_FadeIn(Display_RGN,Setting_FadeIn);
	} else {
		if( Menu_Redraw ){
			if( !Display_BackgroundDrawn ){
				Display_DrawBackground();
				Display_DrawTime();
			}
			// Draw the new title and flag that it has changed
			Display_RenderTitle("Settings - MultiViewEPG");
			Display_TitleDrawn = FALSE;
			TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
			for( i = Menu_Min; i < Menu_Max; i++ ){
				Guide_MenuItem(Display_MemRGN,i,Menu_Selected);
			}
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0, FALSE);
			Menu_ItemDesc( Display_RGN, Menu_Selected );
			Menu_Redraw = FALSE;
		} else {
			Guide_MenuItem(Display_MemRGN,Menu_PrevSelected,Menu_Selected);
			Guide_MenuItem(Display_MemRGN,Menu_Selected,Menu_Selected);
			Menu_ItemDesc( Display_MemRGN, Menu_Selected );
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Menu_PrevSelected-Menu_Min)), DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Menu_PrevSelected-Menu_Min)), FALSE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Menu_Selected-Menu_Min)), DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Menu_Selected-Menu_Min)), FALSE);
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * MENU_ITEMS), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 3), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * MENU_ITEMS), FALSE);
		}
	}
	Menu_Active = TRUE;
}

// Keyhandler for the menu
dword Menu_Keyhandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		Menu_Hide();
		return 0;
	}
	if( Menu_Selected == LINEAREPGKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_LinearEPGKey = 0;
			} else {
				Settings_LinearEPGKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	if( Menu_Selected == TIMERLISTKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_TimerListKey = 0;
			} else {
				Settings_TimerListKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	if( Menu_Selected == RSSLISTKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_RSSListKey = 0;
			} else {
				Settings_RSSListKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	if( Menu_Selected == FAVLISTKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_FavListKey= 0;
			} else {
				Settings_FavListKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	if( Menu_Selected == GRIDEPGKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_GridEPGKey = 0;
			} else {
				Settings_GridEPGKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	if( Menu_Selected == MULTILISTKEY ){
		// If we are no a nav key the user has pressed a button for guide to activate
		if( param1 != RKEY_VolUp && param1 != RKEY_VolDown && param1 != RKEY_ChUp && param1 != RKEY_ChDown ){
			if( param1 == RKEY_0 ){
				Settings_MultiListEPGKey = 0;
			} else {
				Settings_MultiListEPGKey = param1;
			}
			Menu_Render();
			return 0;
		}
	}
	switch( param1 ){
		case RKEY_ChDown:
			Menu_PrevSelected = Menu_Selected;
			Menu_Selected++;
			switch( Menu_Selected ){
				case LINEAREPG_OPTIONS:
				case GRIDEPG_OPTIONS:
				case TIMERLIST_OPTIONS:
				case RSSNEWS_OPTIONS:
				case FAVMANGE_OPTIONS:
				case MAIN_OPTIONS:
				case MULTILIST_OPTIONS:
					Menu_Selected++;
			}
			if( Menu_Selected > Menu_Max-1 ){
				Menu_Min = Menu_Selected;
				Menu_Max = Menu_Selected + MENU_ITEMS;
				Menu_Redraw = TRUE;
				if( Menu_Max > MENU_OPTIONS-1 ){
					Menu_Min = MENU_OPTIONS-MENU_ITEMS;
					Menu_Max = MENU_OPTIONS;
				}
			}
			if( Menu_Selected > MENU_OPTIONS-1 ){
				Menu_Selected = 1;
				Menu_Min = 0;
				Menu_Max = MENU_ITEMS;
				if( Menu_Max > MENU_OPTIONS -1 ){
					Menu_Max = MENU_OPTIONS;
				}
				Menu_Redraw = TRUE;
			}
			Menu_Render();
			return 0;
			break;
		case RKEY_ChUp:
			Menu_PrevSelected = Menu_Selected;
			Menu_Selected--;
			switch( Menu_Selected ){
				case LINEAREPG_OPTIONS:
				case GRIDEPG_OPTIONS:
				case TIMERLIST_OPTIONS:
				case RSSNEWS_OPTIONS:
				case FAVMANGE_OPTIONS:
				case MAIN_OPTIONS:
				case MULTILIST_OPTIONS:
					Menu_Selected--;
			}
			if( Menu_Selected < Menu_Min ){
				Menu_Min = Menu_Selected-MENU_ITEMS+1;
				Menu_Max = Menu_Selected+1;
				Menu_Redraw = TRUE;
				if( Menu_Min < 0 ){
					Menu_Min = 0;
					Menu_Max = MENU_ITEMS;
					if( Menu_Max > MENU_OPTIONS-1 ){
						Menu_Max = MENU_OPTIONS;
					}
				}
			}
			if( Menu_Selected < 0 ){
				Menu_Selected = MENU_OPTIONS-1;
				Menu_Max = MENU_OPTIONS;
				Menu_Min = Menu_Max-MENU_ITEMS;
				if( Menu_Min < 0 ){
					Menu_Min = 0;
				}
				Menu_Redraw = TRUE;
			}
			Menu_Render();
			return 0;
			break;
		case RKEY_VolUp:
		case RKEY_VolDown:
		case RKEY_Ok:
			Menu_PrevSelected = Menu_Selected;
			switch( Menu_Selected ){
				case SHOW_TIMES:
					Settings_LinearEPGTimes = !Settings_LinearEPGTimes;
					break;
				case SHOW_PARTIAL:
					Setting_ShowPartialTimers = !Setting_ShowPartialTimers;
					break;
				case TIMERLISTMODE:
					Settings_ListAllTimers = !Settings_ListAllTimers;
					break;
				case GRIDEPGCHANNELS:
					if( Settings_GridEPGChannels == 10 ){
						Settings_GridEPGChannels = GridEPG_AltNum;
					} else {
						Settings_GridEPGChannels = 10;
					}
					break;
				case GRIDEPGLOGOS:
					Settings_GridEPGLogos = !Settings_GridEPGLogos;
					break;	
				case TAP_EXIT:
					Menu_Hide();
					TSRCommanderExitTAP();
					TAP_Exit();
					return 0;
					break;
				case DEFAULT_SORT:
					Setting_DefaultOrder++;
					if( Setting_DefaultOrder >= SORT_TYPES ){
						Setting_DefaultOrder = 0;
					}
					break;
				case MESSAGE_TIMEOUT:
					if( param1 == RKEY_VolUp ){
						Setting_MessageTimeout+=50;
					} else {
						if( param1 == RKEY_VolDown ){
							Setting_MessageTimeout-=50;
							if(Setting_MessageTimeout < 0 ){
								Setting_MessageTimeout = 0;
							}
						}
					}
					break;
				case RELOADHOUR:
					if( param1 == RKEY_VolUp ){
						Settings_ReloadHour++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_ReloadHour--;
						}
					}
					if( Settings_ReloadHour > 23 ){
						Settings_ReloadHour = -1;
					}
					if( Settings_ReloadHour < -1 ){
						Settings_ReloadHour = 23;
					}
					break;
				case LOADDELAY:
					if( param1 == RKEY_VolUp ){
						Settings_LoadDelay += 100;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_LoadDelay -= 100;
						}
					}
					if( Settings_LoadDelay > 30000 ){
						Settings_LoadDelay = -1;
					}
					if( Settings_LoadDelay < -99 ){
						Settings_LoadDelay = 30000;
					}
					if( Settings_LoadDelay < -1 ){
						Settings_LoadDelay = -1;
					}
					break;
				case SEARCH_DELAY:
					if( param1 == RKEY_VolUp ){
						Setting_SearchDelay+=50;
					} else {
						if( param1 == RKEY_VolDown ){
							Setting_SearchDelay-=50;
							if(Setting_SearchDelay < 0 ){
								Setting_SearchDelay = 0;
							}
						}
					}
					break;
				case GRIDEPGSIZE:
					if( param1 == RKEY_VolUp ){
						Settings_GridEPGSize+=60;
						if( Settings_GridEPGSize > 360 ){
							Settings_GridEPGSize = 60;
						}
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_GridEPGSize-=60;
							if(Settings_GridEPGSize < 60 ){
								Settings_GridEPGSize = 360;
							}
						}
					}
					GridEPG_Size = Settings_GridEPGSize;
					break;
				case FADE_OUT:
					Setting_FadeOut = !Setting_FadeOut;
					break;
				case TIMERLISTLOGOS:
					Settings_TimerListLogos = !Settings_TimerListLogos;
					break;
				case FADE_IN:
					Setting_FadeIn = !Setting_FadeIn;
					break;
				case PIG:
					Setting_PIG = !Setting_PIG;
					if( !Setting_PIG ){
						Guide_ShowingTimers = FALSE;
					}
					break;
				case XOFFSET:
					if( param1 == RKEY_VolUp ){
						Settings_OffsetX++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_OffsetX--;
						}
					}
					DISPLAY_X_OFFSET = Settings_OffsetX;
					Display_Initalised = FALSE;
					Menu_Redraw = TRUE;
					Display_BackgroundDrawn = FALSE;
					break;
				case YOFFSET:
					if( param1 == RKEY_VolUp ){
						Settings_OffsetY++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_OffsetY--;
						}
					}
					DISPLAY_Y_OFFSET = Settings_OffsetY;
					Display_Initalised = FALSE;
					Menu_Redraw = TRUE;
					Display_BackgroundDrawn = FALSE;
					break;
				case TIMERPADDINGSTART:
					if( param1 == RKEY_VolUp ){
						Settings_StartBuffer++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_StartBuffer--;
						}
					}
					break;
				case TIMERPADDINGEND:
					if( param1 == RKEY_VolUp ){
						Settings_EndBuffer++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_EndBuffer--;
						}
					}
					break;
				case GRADIENT_FACTOR:
					if( param1 == RKEY_VolUp ){
						Settings_GradientFactor++;
					} else {
						if( param1 == RKEY_VolDown ){
							Settings_GradientFactor--;
						}
					}
					if( Settings_GradientFactor < 0 ){
						Settings_GradientFactor = 5;
					} else {
						if( Settings_GradientFactor> 5 ){
							Settings_GradientFactor = 0;
						}
					}
					Menu_CreateGradients();
					Menu_Redraw = TRUE;
					break;
				case APPENDEPISODE:
					Settings_AppendEpisode = !Settings_AppendEpisode;
					break;
				case TIMERRENAME:
					Settings_RenameTimers =!Settings_RenameTimers;
					break;
				case RENDERBACKGROUND:
					Settings_RenderBackground = !Settings_RenderBackground;
					break;
			}
			Menu_Render();
			return 0;
			break;
		case RKEY_Record:
			Settings_Save();
			Menu_OptionInfo( Display_RGN, "Settings have been saved." );
			TAP_Delay(100);
			Menu_ItemDesc( Display_RGN, Menu_Selected );			
			return 0;
			break;
	}
	return 0;
}

