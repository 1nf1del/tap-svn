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
#include "tools.h"
#include "favmanage.h"
#include "gridepg.h"
#include "settings.h"

void Settings_Save( void ){
	TYPE_File* fp;
	char sIniFile[1024];		// File should only need to be 512 characters Atm
	char sBuff[256];
	memset( sIniFile, 0, sizeof sIniFile );
	memset( sBuff, 0, sizeof sBuff );
	ChangeDirRoot();
	// File will be stored in /ProgramFiles/Settings/jimguide.ini
	TAP_Hdd_ChangeDir("ProgramFiles");
	if( !TAP_Hdd_Exist("Settings") ) TAP_Hdd_Create ("Settings", ATTR_FOLDER);
	TAP_Hdd_ChangeDir("Settings");
	if (!TAP_Hdd_Exist (OPTIONS_FILENAME)) TAP_Hdd_Create (OPTIONS_FILENAME, ATTR_NORMAL); // create file if not existing
	if ((fp = TAP_Hdd_Fopen (OPTIONS_FILENAME)))	{
		sprintf( &sBuff, "[General]\r\n\r\n" );
		strcat( sIniFile, sBuff );
		if ( Settings_LinearEPGTimes ) sprintf( &sBuff,"ShowTimes=True\r\n" );
		else sprintf( &sBuff,"ShowTimes=False\r\n" );
		strcat( sIniFile, sBuff );
		if ( Settings_ListAllTimers ) sprintf( &sBuff,"ListAllTimers=True\r\n" );
		else sprintf( &sBuff,"ListAllTimers=False\r\n" );
		strcat( sIniFile, sBuff );
		if ( Settings_TimerListLogos ) sprintf( &sBuff,"TimerListLogos=True\r\n" );
		else sprintf( &sBuff,"TimerListLogos=False\r\n" );
		strcat( sIniFile, sBuff );
		if ( Settings_GridEPGLogos ) sprintf( &sBuff,"GridEPGLogos=True\r\n" );
		else sprintf( &sBuff,"GridEPGLogos=False\r\n" );
		strcat( sIniFile, sBuff );			
		// Save the excluding channels
		sprintf( &sBuff, "GridEPGExcludes=%s\r\n", GridEPG_ExportExcludes() );
		strcat( sIniFile, sBuff );
		if ( Setting_ShowPartialTimers ) sprintf( &sBuff,"ShowPartial=True\r\n" );
		else sprintf( &sBuff,"ShowPartial=False\r\n" );
		strcat( sIniFile, sBuff );
		if ( Setting_PIG ) sprintf( &sBuff,"PIG=True\r\n" );
		else sprintf( &sBuff,"PIG=False\r\n" );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"DefaultOrder=%d\r\n", Setting_DefaultOrder );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"MessageTimeout=%d\r\n", Setting_MessageTimeout );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"SearchDelay=%d\r\n", Setting_SearchDelay );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"LinearEPGKey=%d\r\n", Settings_LinearEPGKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"TimerListKey=%d\r\n", Settings_TimerListKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"RSSListKey=%d\r\n", Settings_RSSListKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"FAVListKey=%d\r\n", Settings_FavListKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"GridEPGKey=%d\r\n", Settings_GridEPGKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"MultiListKey=%d\r\n", Settings_MultiListEPGKey );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"GridEPGChannels=%d\r\n", Settings_GridEPGChannels );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"StartUpDelay=%d\r\n", Settings_LoadDelay );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"ReloadHour=%d\r\n", Settings_ReloadHour );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"GridEPGSize=%d\r\n", Settings_GridEPGSize );
		strcat( sIniFile, sBuff );	
		if ( Setting_FadeIn ) sprintf( &sBuff,"FadeIn=True\r\n" );
		else sprintf( &sBuff,"FadeIn=False\r\n" );
		strcat( sIniFile, sBuff );
		if ( Setting_FadeOut ) sprintf( &sBuff,"FadeOut=True\r\n" );
		else sprintf( &sBuff,"FadeOut=False\r\n" );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"XOffset=%d\r\n", Settings_OffsetX );
		strcat( sIniFile, sBuff );	
		sprintf( &sBuff,"YOffset=%d\r\n", Settings_OffsetY );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"StartPadding=%d\r\n", Settings_StartBuffer );
		strcat( sIniFile, sBuff );	
		sprintf( &sBuff,"EndPadding=%d\r\n", Settings_EndBuffer );
		strcat( sIniFile, sBuff );
		sprintf( &sBuff,"GradientFactor=%d\r\n", Settings_GradientFactor );
		strcat( sIniFile, sBuff );
		if( Settings_AppendEpisode ) sprintf( &sBuff,"EpisodeName=True\r\n" );
		else sprintf( &sBuff,"EpisodeName=False\r\n" );
		strcat( sIniFile, sBuff );
		if( Settings_RenameTimers ) sprintf( &sBuff,"RenameTimers=True\r\n" );
		else sprintf( &sBuff,"RenameTimers=False\r\n" );
		strcat( sIniFile, sBuff );
		if( Settings_RenderBackground ) sprintf( &sBuff,"RenderBackground=True\r\n" );
		else sprintf( &sBuff,"RenderBackground=False\r\n" );
		strcat( sIniFile, sBuff );
		TAP_Hdd_Fwrite( sIniFile, sizeof( sIniFile ), 1, fp);
		TAP_Hdd_Fclose( fp );
	} else {
		// TO DO IMPLEMENT MESSAGE SAYING failed saving settings
	}
}

// Function to set the defaults for the TAP
void Settings_Defaults( void ){
	Setting_ShowPartialTimers = TRUE;
	Setting_PIG = TRUE;
	Setting_DefaultOrder = SORT_CHANNEL;
	Setting_MessageTimeout = 200;
	Setting_SearchDelay = 100;
	Setting_FadeIn = TRUE;
	Setting_FadeOut = TRUE;
	Settings_LinearEPGKey = RKEY_Fav;
	DISPLAY_X_OFFSET = Settings_OffsetX = 0;
	DISPLAY_Y_OFFSET = Settings_OffsetY = 0;
	Settings_StartBuffer = 3;
	Settings_EndBuffer = 10;
	Settings_AppendEpisode = FALSE;
	Settings_RenameTimers = TRUE;
	Settings_RenderBackground = TRUE;
	Settings_TimerListKey = RKEY_TvRadio;
	Settings_RSSListKey = RKEY_Teletext;
	Settings_FavListKey = 0;
	Settings_GridEPGKey = 0;
	Settings_MultiListEPGKey = 0;
	Settings_GridEPGSize = 180;
	Settings_GradientFactor = 2;
	Settings_LinearEPGTimes = FALSE;
	Settings_ListAllTimers = FALSE;
	Settings_LoadDelay = -1;
	Settings_ReloadHour = 3;
	Settings_GridEPGChannels = 10;
	Settings_GridEPGLogos = FALSE;
	Settings_TimerListLogos = FALSE;
	memset( Settings_ChannelExcludes, 0, sizeof(Settings_ChannelExcludes) );
}

// Function used to load options from the saved file
void Settings_Load( void ){
	TYPE_File* fp;
	char sIniFile[1024];
	char sResult[256];
	char sSection[128];
	char sBuff[256];
	memset( sIniFile, 0, sizeof sIniFile );
   	memset( sResult, 0, sizeof sResult );
	memset( sSection, 0, sizeof sSection );
	memset( sBuff, 0, sizeof sBuff );
	Settings_Defaults();
	ChangeDirRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");
	// New version has settings in different location. Read existing
	if( !TAP_Hdd_Exist(OPTIONS_FILENAME) ){
		if( !TAP_Hdd_Exist("Settings") ) TAP_Hdd_Create ("Settings", ATTR_FOLDER);
		TAP_Hdd_ChangeDir("Settings");
	}
	if ((fp = TAP_Hdd_Fopen (OPTIONS_FILENAME))){
		TAP_Hdd_Fread( sIniFile, sizeof( sIniFile ), 1, fp );
		TAP_Hdd_Fclose( fp );
		GetSetting( sIniFile, "[General]", "ShowTimes", sResult );
		Settings_LinearEPGTimes  = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "ShowPartial", sResult );
		Setting_ShowPartialTimers  = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "ListAllTimers", sResult );
		Settings_ListAllTimers  = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "TimerListLogos", sResult );
		Settings_TimerListLogos  = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "GridEPGLogos", sResult );
		Settings_GridEPGLogos  = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "PIG", sResult );
		Setting_PIG = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "FadeIn", sResult );
		Setting_FadeIn = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "FadeOut", sResult );
		Setting_FadeOut = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "DefaultOrder", sResult );
		Setting_DefaultOrder = GetInteger( sResult );
		if( Setting_DefaultOrder < 0 || Setting_DefaultOrder > (SORT_TYPES-1)){
			Setting_DefaultOrder = SORT_CHANNEL;
		}
		sortOrder = Setting_DefaultOrder;
		GetSetting( sIniFile, "[General]", "LinearEPGKey", sResult );
		Settings_LinearEPGKey = GetInteger( sResult );
		if( Settings_LinearEPGKey < 0 ){
			Settings_LinearEPGKey = RKEY_Fav;
		}
		GetSetting( sIniFile, "[General]", "GridEPGExcludes", sResult );
		strcpy( Settings_ChannelExcludes, sResult );
		GetSetting( sIniFile, "[General]", "TimerListKey", sResult );
		Settings_TimerListKey = GetInteger( sResult );
		if( Settings_TimerListKey < 0 ){
			Settings_TimerListKey = RKEY_TvRadio;
		}
		GetSetting( sIniFile, "[General]", "RSSListKey", sResult );
		Settings_RSSListKey = GetInteger( sResult );
		if( Settings_RSSListKey < 0 ){
			Settings_RSSListKey = RKEY_Teletext;
		}
		GetSetting( sIniFile, "[General]", "FAVListKey", sResult );
		Settings_FavListKey = GetInteger( sResult );
		if( Settings_FavListKey < 0 ){
			Settings_FavListKey = 0;
		}
		GetSetting( sIniFile, "[General]", "MultiListKey", sResult );
		Settings_MultiListEPGKey = GetInteger( sResult );
		if( Settings_MultiListEPGKey < 0 ){
			Settings_MultiListEPGKey = 0;
		}
		GetSetting( sIniFile, "[General]", "GridEPGKey", sResult );
		Settings_GridEPGKey = GetInteger( sResult );
		if( Settings_GridEPGKey < 0 ){
			Settings_GridEPGKey = 0;
		}
		GetSetting( sIniFile, "[General]", "GridEPGSize", sResult );
		Settings_GridEPGSize = GetInteger( sResult );
		if( Settings_GridEPGSize <= 0 ){
			Settings_GridEPGSize = 180;
		}
		GridEPG_Size = Settings_GridEPGSize;	// Set the grid size to the users preference	
		GetSetting( sIniFile, "[General]", "GridEPGChannels", sResult );
		Settings_GridEPGChannels = GetInteger( sResult );
		if( Settings_GridEPGChannels != 10 && Settings_GridEPGChannels != GridEPG_AltNum ){
			Settings_GridEPGChannels = 10;
		}
		GetSetting( sIniFile, "[General]", "MessageTimeout", sResult );
		Setting_MessageTimeout = GetInteger( sResult );
		if( Setting_MessageTimeout < 100 ){
			Setting_MessageTimeout = 100;
		}
		GetSetting( sIniFile, "[General]", "SearchDelay", sResult );
		Setting_SearchDelay = GetInteger( sResult );
		if( Setting_SearchDelay < 100 ){
			Setting_SearchDelay = 100;
		}
		GetSetting( sIniFile, "[General]", "XOffset", sResult );
		Settings_OffsetX = GetInteger( sResult );
		if( Settings_OffsetX < -50 || Settings_OffsetX > 50 ){
			Settings_OffsetX = 0;
		}
		DISPLAY_X_OFFSET = Settings_OffsetX;
		GetSetting( sIniFile, "[General]", "YOffset", sResult );
		Settings_OffsetY = GetInteger( sResult );
		if( Settings_OffsetY < -50 || Settings_OffsetY > 50 ){
			Settings_OffsetY = 0;
		}
		DISPLAY_Y_OFFSET = Settings_OffsetY;
		GetSetting( sIniFile, "[General]", "GradientFactor", sResult );
		Settings_GradientFactor = GetInteger( sResult );
		if( Settings_GradientFactor < 0 || Settings_GradientFactor > 5 ){
			Settings_GradientFactor = 2;
		}
		GetSetting( sIniFile, "[General]", "StartUpDelay", sResult );
		Settings_LoadDelay = GetInteger( sResult );
		if( Settings_LoadDelay < -1 || Settings_LoadDelay > 30000 ){
			Settings_LoadDelay = -1;
		}
		GetSetting( sIniFile, "[General]", "ReloadHour", sResult );
		Settings_ReloadHour = GetInteger( sResult );
		if( Settings_ReloadHour < -1 || Settings_ReloadHour > 23 ){
			Settings_ReloadHour = -1;
		}
		GetSetting( sIniFile, "[General]", "StartPadding", sResult );
		Settings_StartBuffer = GetInteger( sResult );
		GetSetting( sIniFile, "[General]", "EndPadding", sResult );
		Settings_EndBuffer = GetInteger( sResult );
		GetSetting( sIniFile, "[General]", "EpisodeName", sResult );
		Settings_AppendEpisode = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "RenameTimers", sResult );
		Settings_RenameTimers = ( strcmp ( sResult, "True" ) == 0 );
		GetSetting( sIniFile, "[General]", "RenderBackground", sResult );
		Settings_RenderBackground = ( strcmp ( sResult, "True" ) == 0 );
	} else {
		Settings_Defaults();
	}
	Settings_Save();
	Settings_LoadFile = FALSE;
}
