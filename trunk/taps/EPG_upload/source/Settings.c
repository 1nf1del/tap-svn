//------------------------------ atoi --------------------------------------
//
int	atoi( const char* sString )
{
	bool bNegate=FALSE;
	int iValue=0; 
	int iNdx;
	int iLen=strlen( sString );;

	for ( iNdx = 0; iNdx < iLen; iNdx += 1 )				
	{
		if ( sString[iNdx] == '-' || sString[iNdx] == '+')
		{
			bNegate = (sString[iNdx] == '-');
		}
		else
		{
			if ( sString[iNdx] < '0' || sString[iNdx] > '9' )
				return 0;
			iValue = ( iValue * 10 ) + ( sString[iNdx] - '0' );
		}
	}

	if ( bNegate == TRUE )
		iValue = -iValue;
	return iValue;
}

//------------------------------ GetSetting --------------------------------------
//
// windows is 0d 0a, unix is 0a
bool	GetSetting( char* sIniFile, char* sSrcSection, char* sSrcLabel, char* sResult )
{
	int	iNdx, iRecSize, ibNdx;
	char	sLabel[200], sParam[200], sBuff[200];
	bool	bEqual, bSectFound;

	memset( sLabel, 0, sizeof sLabel );
	memset( sParam, 0, sizeof sParam );
	memset( sResult, 0, sizeof sResult );
	memset( sBuff, 0, sizeof sBuff );

	iRecSize = strlen( sIniFile );
	bEqual = FALSE;
	bSectFound = FALSE;
	ibNdx = 0;

	for ( iNdx = 0; iNdx < iRecSize; iNdx += 1 )				
	{

		if (  sIniFile[iNdx]==0x0d || iNdx ==(iRecSize - 1) || sIniFile[iNdx] == 0x0a )	 //0x0d=CR
		{
			if ( bSectFound == FALSE )
			{
				if ( strcmp( sLabel, sSrcSection ) == 0 )
					bSectFound = TRUE;
			}
			else
			{
				if ( sLabel[0] == '[')
				{
					bSectFound = FALSE;
				}
				else
				{
					if ( strcmp( sLabel, sSrcLabel ) == 0 ) //i.e. they are equal
					{
					//	logf(4,"slabel:%s, srclabel:%s, sparam:%s",sLabel, sSrcLabel,sParam);
						strcpy( sResult, sParam );
						return TRUE;
					}
				}
			}

			memset (sLabel, 0, sizeof sLabel );
			memset (sParam, 0, sizeof sParam );
			bEqual = FALSE;
			ibNdx = 0;
		}
		else
		{
			if ( sIniFile[iNdx] != 0x0a ) //LF
			{
				if ( sIniFile[iNdx] == '=' )
				{
					ibNdx = 0;
					bEqual = TRUE;
				}
				else
				{
					if ( bEqual )
					{
						sParam[ibNdx] = sIniFile[iNdx];
					}
					else
					{
						sLabel[ibNdx] = sIniFile[iNdx];
					}
					if ( ibNdx < 200 )
						ibNdx += 1;		// Restrict to 200 characters
				}
			}
		}
	}	
	return FALSE;
}


//--------------------------------ReadGMTINI--------------------------------
bool ReadGMTini()
{
    TYPE_File* fp;
    char * sGMTFile; //[512];
    char    sResult[256];
    unsigned char   tmp_date[5];
    word year;
    byte month,day;
    dword heapSize, freeHeapSize, availHeapSize;
	dword filelength;
	word dstactive=0;
	
    ChangeDir("Auto Start");
    if ( TAP_Hdd_Exist( GMT_OFFSET_FILE ) )
    {
        if ((fp = TAP_Hdd_Fopen( GMT_OFFSET_FILE ))!=0)
        {
			TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
			filelength=TAP_Hdd_Flen( fp );
			if (filelength==0)
			{
				TAP_Hdd_Fclose( fp );
				ChangeDir("ProgramFiles");
				return FALSE;
			}
			
			if (freeHeapSize<filelength+1) //oops, no ram left
			{
				TAP_Hdd_Fclose( fp );
				ShowMessageWin("Not enough RAM to process",GMT_OFFSET_FILE,200,FALSE);
				ShowMessageWin("Quitting TAP","",200,FALSE);
				TAP_Exit();
				return FALSE;
			}
			sGMTFile=TAP_MemAlloc(filelength+1);
			memset(sGMTFile,0,filelength+1);
			TAP_Hdd_Fread( sGMTFile, 1,filelength,fp );
			TAP_Hdd_Fclose( fp );
			
			GetSetting( sGMTFile, "[Time Settings]", "DstActive", sResult );
			if ( strlen( sResult ) > 0 )
			{
				if (!strcmp(sResult,"True")) 
				{	
					dstactive=60;
					gDstActive=TRUE;
				}
			}
			
			GetSetting( sGMTFile, "[Time Settings]", "GmtOffset", sResult );
			if ( strlen( sResult ) > 0 )
				g_GMToffst = atoi( sResult )*15 + dstactive;
			else 
				g_GMToffst = dstactive;
				
			GetSetting( sGMTFile, "[Time Settings]", "DstStart", sResult );
			memset( tmp_date, 0x0, 3 );
			day = atoi( strncpy( tmp_date, sResult, 2 ) );
	
			memset( tmp_date, 0x0, 3 );
			month = atoi( strncpy( tmp_date, sResult+3, 2 ) );
	
			memset( tmp_date, 0x0, 5 );
			year = atoi( strncpy( tmp_date, sResult+6, 4 ) );
			
			gDstStart=TAP_MakeMjd((word)year,(byte)month,(byte)day);
			
	#if DEBUG
			printf("GMTiniRead DSTSTART %02d %02d %04d",day,month,year);
	#endif           
			
			GetSetting( sGMTFile, "[Time Settings]", "DstEnd", sResult );
			memset( tmp_date, 0x0, 3 );
			day = atoi( strncpy( tmp_date, sResult, 2 ) );
	
			memset( tmp_date, 0x0, 3 );
			month = atoi( strncpy( tmp_date, sResult+3, 2 ) );
	
			memset( tmp_date, 0x0, 5 );
			year = atoi( strncpy( tmp_date, sResult+6, 4 ) );
			
			gDstEnd=TAP_MakeMjd((word)year,(byte)month,(byte)day);
			
	#if DEBUG
			printf("GMTiniRead DSTEND %02d %02d %04d\n",day,month,year);
	#endif 
			ChangeDir("ProgramFiles");
			TAP_MemFree(sGMTFile);
			return TRUE;
        }
        else
        {
            ChangeDir("ProgramFiles") ;
            return FALSE;
        }
    }
    else
    {
        printf(GMT_OFFSET_FILE" doesn't exist\n");  //can't do a logf as we are in the wrong directory
        ChangeDir("ProgramFiles");
        return FALSE;
    }
    
}



//------------------------------ SaveSettings --------------------------------------
//
void	SaveSettings(int IceSettings)
{
	TYPE_File*	fp;
	int		iMode;
	//char	* sIniFile;
	char		sIniFile[7680];
	char 		sBuff[256];
	char ini_filename[20];

	ChangeDir("ProgramFiles");

	memset( sIniFile, 0x0, sizeof sIniFile );

	if (IceSettings==LOADNORMALINI)	strcpy(ini_filename,INI_FILE);
	else if (IceSettings==LOADICEINI) strcpy(ini_filename,ICE_INI_FILE);
	else if (g_LoadIceIni==LOADICEINI)
		strcpy(ini_filename,ICE_INI_FILE);
	else 
		strcpy(ini_filename,INI_FILE);

	if ( TAP_Hdd_Exist( ini_filename ) )
		TAP_Hdd_Delete( ini_filename );

	TAP_Hdd_Create( ini_filename, ATTR_NORMAL );

	if ( (fp = TAP_Hdd_Fopen( ini_filename ))!=0 )
	{
#if DEBUG
		printf("saving inifile %s\n",ini_filename);
#endif

		sprintf( &sBuff, "[General]\r\n\r\n" );
		strcat( sIniFile, sBuff );
        
        sprintf( &sBuff,"USE_GMT_OffsetFILE=%s\r\n", (g_useGMT==GMTFILE)?"GMT:FILE":(g_useGMT==GMTTOPPY)?"GMT:Broadcaster":"GMT:TAPOffset");
        strcat( sIniFile, sBuff );
		   
		sprintf( &sBuff,"GMT_Offset=%d\r\n", g_GMToffst/15 );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Frequency File check No Timeshift=%d\r\n", g_freqMin[NTS] );
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Frequency File check Timeshift=%d\r\n", g_freqMin[TSH] );
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Email Volume=%s\r\n", (gVolume==VOLOFF)?"OFF":(gVolume==LOW)?"LOW":"HIGH" );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"AutoLoadEPGdata=%s\r\n", (gautoloadepg)?"YES":"NO" );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"AutoDeleteEPGdata at start=%s\r\n", (gautodeleteepg)?"YES":"NO" );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Next Update Time=%d\r\n", g_NextUpdateTime );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Reload Hour=%d\r\n", gReloadHour );
		strcat( sIniFile, sBuff );

        sprintf( &sBuff,"Load Number of Days=%d\r\n", gLoadDay );
        strcat( sIniFile, sBuff );

		sprintf( &sBuff,"TGD directory=%s\r\n", gTGDdirectory );
		strcat( sIniFile, sBuff );

		//sprintf( &sBuff,"Load 99999999 event details=%s\r\n", (gload9999epgevts)?"YES":"NO" );
		//strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Display LOADING OSD=%s\r\n", (g_ShowLoadingOSD==DISOFF)?"NO":(g_ShowLoadingOSD==SMALL)?"MINIMAL":"FULL" );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Logging Level (4everything 0nothing)=%d\r\n", g_loglevel );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Broadcaster EIT=%s\r\n", (gEITload)?"LOAD":"BLOCK" );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"EPG load wait=%d\r\n", g_loadtimewait );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Timer Set Delay=%d\r\n", g_timerdelay );
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"TEDS Recordings File=%s\r\n", (gTedRecordingsfile)?"YES":"NO" );
		strcat( sIniFile, sBuff );
		
#if DEBUG		
		printf("gemailtransparency:%d\n",gemailtransparency);
#endif			
		sprintf( &sBuff,"ToppybiffTransparency=%d\r\n", gemailtransparency);
		strcat( sIniFile, sBuff );

		sprintf( &sBuff,"Scan ALL Events=%s\r\n", (g_scanALLevents)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		if (g_LoadIceIni!=NOICE)
		{
		sprintf( &sBuff,"LoadIceIni=%s\r\n", (g_LoadIceIni==LOADNORMALINI)?"NO":"YES" );
		strcat( sIniFile, sBuff );
		}
		
		sprintf( &sBuff,"Load Short Events=%s\r\n", (g_loadshortevents)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Set Manual Time at Startup=%s\r\n", (g_startupTimeSet)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Set Manual Time at power fail=%s\r\n", (g_powerFailTime)?"YES":"NO");
		strcat( sIniFile, sBuff );
	
		sprintf( &sBuff,"Manual Time GMT FIX=%s\r\n", (g_manualGMTTime)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Adjust GMT loading entries=%s\r\n", (g_GMTloadAdjust)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Set Manual Time on LCNs=%s\r\n", g_manualLCN);
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"Exit Key for Menu=%s\r\n", (g_buseExitKey)?"YES":"NO");
		strcat( sIniFile, sBuff );
		
		sprintf( &sBuff,"LCN TGD load=%s\r\n", (g_bLCNTGDload)?"YES":"NO");
		strcat( sIniFile, sBuff );
		

		/*
		if (g_NumServices==0) //something isn't right!
		{
			g_NumServices=5;  //lets create 5 dummy ones
			g_IniSvcPtr=(TYPE_IniSvcDetails *)TAP_MemAlloc(g_NumServices*sizeof(*g_IniSvcPtr));
			for (x=0; x<g_NumServices; x++)
			{
				g_IniSvcPtr[x].toppysatName[0]=0;
				strcpy(g_IniSvcPtr[x].tgdservice_name,"TGDChannelID\0");
				//strcpy(g_IniSvcPtr[x].toppyservice_name,"\0");
				g_IniSvcPtr[x].service_id=0;
				g_IniSvcPtr[x].transport_stream_id=0;
				g_IniSvcPtr[x].original_network_id=0;
				g_IniSvcPtr[x].topfieldinternalchannel=0;
			}
		}
		*/
		sprintf( &sBuff,"Number of Services=%d\r\n", g_NumServices );
		strcat( sIniFile, sBuff );

		sprintf( &sBuff, "\r\n\r\n[TV Parameters]\r\n" );
		strcat( sIniFile, sBuff );
		
#if DEBUG
		printf("Just before Writing Services\n");
#endif
		for ( iMode = 0; iMode < g_NumServices; iMode++ )				
		{
			sprintf( &sBuff, "\r\n\r\n[Channel %01d]\r\n\r\n", iMode+1 );
			strcat( sIniFile, sBuff );

			//if (strlen(g_IniSvcPtr[iMode].toppyservice_name)>0)
			//	sprintf( &sBuff,"service_name=%s,%s\r\n", g_IniSvcPtr[iMode].tgdservice_name,g_IniSvcPtr[iMode].toppyservice_name);
			//else
			sprintf( &sBuff,"service_name=%s\r\n", g_IniSvcPtr[iMode].tgdservice_name);
			strcat( sIniFile, sBuff );

			if (strlen(g_IniSvcPtr[iMode].toppysatName)>0)
			{
				sprintf( &sBuff,"satellite_name=%s\r\n", g_IniSvcPtr[iMode].toppysatName);
				strcat( sIniFile, sBuff );
			}

			if (g_IniSvcPtr[iMode].LCN_channel_id>0)
			{
				sprintf( &sBuff,"LCN_channel=%d\r\n", g_IniSvcPtr[iMode].LCN_channel_id);
				strcat( sIniFile, sBuff );
			}
			else
			{
				sprintf( &sBuff,"service_id=%d\r\n", g_IniSvcPtr[iMode].service_id);
				strcat( sIniFile, sBuff );

				sprintf( &sBuff,"transport_stream_id=%d\r\n", g_IniSvcPtr[iMode].transport_stream_id);
				strcat( sIniFile, sBuff );

				sprintf( &sBuff,"original_network_id=%d\r\n", g_IniSvcPtr[iMode].original_network_id);
				strcat( sIniFile, sBuff );
			}
		}
#if DEBUG
		printf("Just before Writing IniFile\n");
#endif
		TAP_Hdd_Fwrite( sIniFile, sizeof( sIniFile ), 1, fp);
		TAP_Hdd_Fclose( fp );
	}
	else
	{
		//logf("can't open "ini_filename);
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("ERROR:Save: kann nicht geöffnet werden", ini_filename,150,FALSE);
		else
			ShowMessageWin("ERROR:Save: can't open", ini_filename,150,FALSE);
	}
	ChangeDir(gTGDdirectory);
}

//------------------------------ LoadSettings --------------------------------------
//
bool	LoadSettings(bool bFirstSettings)
{
	TYPE_File* fp;
	char	* sIniFile; //[7680];
	char	sResult[256];
	char	sSection[128];
//	char 	sBuff[256];
	char    ini_filename[20];
	TYPE_TapChInfo chInfo;
	bool bsavesettings=FALSE;
    bool inifileread=FALSE;
	dword filelength;
	//TYPE_TimerRAM tempbuff;
	dword heapSize, freeHeapSize, availHeapSize;
	
	int total_tvSvc,total_radSvc;
	int i,x;

	int	iMode;
//	bool	bLoadOk;
	bool found=FALSE;

	//memset( sIniFile, 0, sizeof sIniFile );
	memset( sResult, 0, sizeof sResult );

	if (!g_btapdirinifile)
	{
		ChangeDir("ProgramFiles");
	}
	if (bFirstSettings)
		strcpy(ini_filename,INI_FILE);
	else if (g_LoadIceIni==LOADICEINI)
		strcpy(ini_filename,ICE_INI_FILE);
	else 
		strcpy(ini_filename,INI_FILE);
		
#if DEBUG
		printf("Just Prior to opening inifile %s\n",ini_filename);
#endif
	if ((fp = TAP_Hdd_Fopen( ini_filename ))!=0)
	{

#if DEBUG
		printf("loading inifile %s\n",ini_filename);
#endif
		if ((filelength=TAP_Hdd_Flen( fp ))<40) //something must be wrong, a zero byte file perhaps too.
		{
#if DEBUG
		printf("inifile too small\n");
#endif	
			TAP_Hdd_Fclose( fp );
			goto noinifile;    //yes, I know, sacriledge, a goto, but this is the best way of also coping with a dodgy file.
		}

		TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
		if (freeHeapSize<filelength+1) //oops, no ram left
		{
			TAP_Hdd_Fclose( fp );
			ShowMessageWin("Not enough RAM to process",ini_filename,200,FALSE);
			ShowMessageWin("Quitting TAP","",200,FALSE);
			TAP_Exit();
			return FALSE;
		}
		sIniFile=TAP_MemAlloc(filelength+1);
		memset( sIniFile, 0, filelength+1 );
		TAP_Hdd_Fread( sIniFile, 1, filelength, fp );
		TAP_Hdd_Fclose( fp );

		if (g_btapdirinifile)  //if inifile was originally detected with tap
		{
			TAP_Hdd_Delete(ini_filename);  //delete it
		}

		GetSetting( sIniFile, "[General]", "LoadIceIni", sResult );
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"YES")) g_LoadIceIni=LOADICEINI;
			else if (!strcmp(sResult,"NO")) g_LoadIceIni=LOADNORMALINI;
			else g_LoadIceIni=NOICE;

			if (bFirstSettings && g_LoadIceIni==LOADICEINI) 
			{
#if DEBUG
				printf("inifile reloading due to ini params suggesting to load ice\n");
#endif
				TAP_MemFree(sIniFile);
				return TRUE;   //if our loaded params suggest to load the ice file, need to reload
			}
		}
		else g_LoadIceIni=NOICE;
        
		GetSetting( sIniFile, "[General]", "USE_GMT_OffsetFILE", sResult );
        if ( strlen( sResult ) > 0 )
		{			
			if (!strcmp(sResult,"GMT:FILE")) g_useGMT=GMTFILE;
			else if (!strcmp(sResult,"GMT:Broadcaster")) g_useGMT=GMTTOPPY;
			else g_useGMT=GMTTAP;
        }
        if (g_useGMT==GMTFILE)  //now to immediately load the GMToffset details
        {
            inifileread=ReadGMTini();  
        }
        if (!inifileread || g_useGMT!=GMTFILE) //if we don't want to load the inifile or it couldn't be read above
        {
		    GetSetting( sIniFile, "[General]", "GMT_Offset", sResult );
		    if ( strlen( sResult ) > 0 )
			    g_GMToffst = atoi( sResult )*15;
        }

		GetSetting( sIniFile, "[General]", "Frequency File check No Timeshift", sResult );
		if ( strlen( sResult ) > 0 )
			g_freqMin[NTS] = atoi( sResult );
		if (g_freqMin[NTS]<=1 || g_freqMin[NTS]>720)	g_freqMin[NTS]=720;
		
		GetSetting( sIniFile, "[General]", "Frequency File check Timeshift", sResult );
		if ( strlen( sResult ) > 0 )
			g_freqMin[TSH] = atoi( sResult );
		if (g_freqMin[TSH]<=1 || g_freqMin[TSH]>720)	g_freqMin[TSH]=5;
		
		
		GetSetting( sIniFile,"[General]","Email Volume", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"OFF")) gVolume=VOLOFF;
			else if (!strcmp(sResult,"LOW")) gVolume=LOW;
			else gVolume=HIGH;
		}

		GetSetting( sIniFile, "[General]", "AutoLoadEPGdata", sResult );
		if ( strlen( sResult ) > 0 )
			gautoloadepg = !strcmp(sResult,"YES");

		GetSetting( sIniFile, "[General]", "AutoDeleteEPGdata at start", sResult );
		if ( strlen( sResult ) > 0 )
			gautodeleteepg = !strcmp(sResult,"YES");

		GetSetting( sIniFile, "[General]","Next Update Time", sResult );
		if ( strlen( sResult ) > 0 )
			g_NextUpdateTime = atoi( sResult );
		if (g_NextUpdateTime<-30 || g_NextUpdateTime >30)
			g_NextUpdateTime=31; //31 is "NO" option.

		GetSetting( sIniFile, "[General]","Reload Hour", sResult );
		if ( strlen( sResult ) > 0 )
			gReloadHour = atoi( sResult );
            else
            gReloadHour = 24; //used to indicate "don't bother reloading tap"
            
        GetSetting( sIniFile, "[General]","Load Number of Days", sResult );
        if ( strlen( sResult ) > 0 )
            gLoadDay = atoi( sResult );           

		GetSetting( sIniFile, "[General]","TGD directory", sResult );
		strcpy(gTGDdirectory,sResult);
		if (gTGDdirectory[0]==0) 
		{
			strcpy(gTGDdirectory,"ProgramFiles");
			bsavesettings=TRUE;
		}
		g_binifiledirectoryknown=TRUE;
		ChangeDirRoot();
		if (!TAP_Hdd_Exist( gTGDdirectory ))
		{
			printf("Created %s directory in the root directory\n",gTGDdirectory);
			TAP_Hdd_Create( gTGDdirectory, ATTR_FOLDER );
		}
		ChangeDir(gTGDdirectory);
	    if (!TAP_Hdd_Exist(LOG_FILE)) {   //first thing we will do is create a logfile if it doesn't exist
            TAP_Hdd_Create(LOG_FILE,ATTR_NORMAL);
        }
		//GetSetting( sIniFile,"[General]","Load 99999999 event details", sResult);
		//if ( strlen( sResult ) > 0 )
		//	gload9999epgevts = !strcmp(sResult,"YES");

		GetSetting( sIniFile,"[General]","Display LOADING OSD", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"MINIMAL")) g_ShowLoadingOSD=SMALL;
			else if (!strcmp(sResult,"NO")) g_ShowLoadingOSD=DISOFF;
			else g_ShowLoadingOSD=LARGE;
		}
		if (g_ShowLoadingOSD==DISOFF) g_timerdelay=380; else g_timerdelay=350;

		GetSetting( sIniFile,"[General]","Logging Level (4everything 0nothing)",sResult);
		if ( strlen( sResult ) > 0 )
			g_loglevel = atoi( sResult );

		GetSetting( sIniFile,"[General]","Broadcaster EIT", sResult);
		if ( strlen( sResult ) > 0 )
			gEITload = !strcmp(sResult,"LOAD");

		GetSetting( sIniFile,"[General]","EPG load wait", sResult);
		if ( strlen( sResult ) > 0 )
			g_loadtimewait = atoi(sResult);

		GetSetting( sIniFile,"[General]","Timer Set Delay", sResult);
		if ( strlen( sResult ) > 0 )
			g_timerdelay = atoi(sResult);
		
		GetSetting( sIniFile,"[General]","TEDS Recordings File", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"YES")) gTedRecordingsfile=TRUE;
			else
				gTedRecordingsfile=FALSE;
		}	
		GetSetting( sIniFile,"[General]","ToppybiffTransparency", sResult);
		if ( strlen( sResult ) > 0 )
			gemailtransparency = atoi(sResult);			
			
		GetSetting( sIniFile,"[General]","Scan ALL Events", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"YES")) g_scanALLevents=TRUE;
			else
				g_scanALLevents=FALSE;
		}
		
		GetSetting( sIniFile,"[General]","Load Short Events", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"YES")) g_loadshortevents=TRUE;
			else
				g_loadshortevents=FALSE;
		}
		
		GetSetting( sIniFile,"[General]","Set Manual Time at Startup", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_startupTimeSet=FALSE;
			else
				g_startupTimeSet=TRUE;
		}		
		
		
		GetSetting( sIniFile,"[General]","Set Manual Time at power fail", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_powerFailTime=FALSE;
			else
				g_powerFailTime=TRUE;
		}
		
		GetSetting( sIniFile,"[General]","Manual Time GMT FIX", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_manualGMTTime=FALSE;
			else
				g_manualGMTTime=TRUE;
		}
		
		GetSetting( sIniFile,"[General]","Adjust GMT loading entries", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_GMTloadAdjust=FALSE;
			else
				g_GMTloadAdjust=TRUE;
		}
		
		GetSetting( sIniFile,"[General]","Set Manual Time on LCNs", sResult);
		if ( strlen( sResult ) > 0 )
			strcpy(g_manualLCN,sResult);
		else
			memset(g_manualLCN,0,sizeof g_manualLCN);
			
		GetSetting( sIniFile,"[General]","Exit Key for Menu", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_buseExitKey=FALSE;
			else
				g_buseExitKey=TRUE;
		}	
		
		GetSetting( sIniFile,"[General]","LCN TGD load", sResult);
		if ( strlen( sResult ) > 0 )
		{
			if (!strcmp(sResult,"NO")) g_bLCNTGDload=FALSE;
			else
				g_bLCNTGDload=TRUE;
		}	
		
		GetSetting( sIniFile, "[General]", "Number of Services", sResult );
		if ( strlen( sResult ) > 0 )
			g_NumServices = atoi( sResult );
		if (g_NumServices>MAX_INI_SVCS) g_NumServices=MAX_INI_SVCS;

		TAP_Channel_GetTotalNum( &total_tvSvc, &total_radSvc );

		if (g_total_tvSvc!=total_tvSvc || g_total_radSvc!=total_radSvc) //therefore first time tap has loaded and/or channel rescan has happened
		{
#if DEBUG
			logf(4,"_total_tvSvc!=total_tvSvc || g_total_radSvc!=total_radSvc");
#endif
			if (g_IniSvcPtr!=0)	TAP_MemFree((char *) g_IniSvcPtr);
			if (g_NumServices!=0) 
				g_IniSvcPtr=(TYPE_IniSvcDetails *)TAP_MemAlloc(g_NumServices*sizeof(*g_IniSvcPtr));
		}	
#if DEBUG
			logf(4,"MemAlloc numservices:%d sizeofptr:%d addr:0x%x",g_NumServices,sizeof(*g_IniSvcPtr),g_IniSvcPtr);
#endif	
		for ( iMode = 0; iMode < g_NumServices; iMode++ )				
		{
			//char * str;
			sprintf( &sSection, "[Channel %01d]", iMode+1 );
			GetSetting( sIniFile, sSection, "service_name", sResult );
			//memset(g_IniSvcPtr[iMode].tgdservice_name,0x0,128);
			strcpy(g_IniSvcPtr[iMode].tgdservice_name,sResult);

			GetSetting( sIniFile, sSection, "satellite_name", sResult );
			//g_IniSvcPtr[iMode].toppysatName[0]=0;
			strcpy(g_IniSvcPtr[iMode].toppysatName,sResult);

			//printf("num:%d ServiceName:%s result:%s \n",iMode,service_name[iMode],sResult);

			if (GetSetting( sIniFile, sSection, "LCN_channel", sResult ))
				g_IniSvcPtr[iMode].LCN_channel_id=atoi( sResult );
			else
				g_IniSvcPtr[iMode].LCN_channel_id=0;
#if DEBUG
			logf(4,"LCN_channel:%d",g_IniSvcPtr[iMode].LCN_channel_id);
#endif
//preset these to 0 before reading true values, either from LCN or from inifile settings
			g_IniSvcPtr[iMode].service_id = 0;
			g_IniSvcPtr[iMode].transport_stream_id = 0;
			g_IniSvcPtr[iMode].original_network_id = 0;
			//g_IniSvcPtr[iMode].topfieldinternalchannel=9999;
			
			if (g_IniSvcPtr[iMode].LCN_channel_id != 0)
			{
				for ( i = 0; i < total_tvSvc; i++ )				
				{
					TAP_Channel_GetInfo( SVC_TYPE_Tv,  i, &chInfo );
					if (chInfo.logicalChNum==g_IniSvcPtr[iMode].LCN_channel_id) //if toppy LCN matches iniLCN
					{
						g_IniSvcPtr[iMode].service_id = chInfo.svcId;           //set service ts etc params
						g_IniSvcPtr[iMode].transport_stream_id = chInfo.tsId;
						g_IniSvcPtr[iMode].original_network_id = chInfo.orgNetId;
						g_IniSvcPtr[iMode].topfieldinternalchannel=i;
						break;
					}
				}
			}
			else //using svcids rather than LCN in inifile
			{
				//g_IniSvcPtr[iMode].LCN_channel_id=0; 
				GetSetting( sIniFile, sSection, "service_id", sResult );
				g_IniSvcPtr[iMode].service_id = atoi( sResult );
				//printf("ServiceId %d\n",service_id[iMode]);

				GetSetting( sIniFile, sSection, "transport_stream_id", sResult );
				g_IniSvcPtr[iMode].transport_stream_id = atoi( sResult );

				GetSetting( sIniFile, sSection, "original_network_id", sResult );
				g_IniSvcPtr[iMode].original_network_id = atoi( sResult );
			}
			
#if DEBUG			
			ChangeDir(gTGDdirectory);
			
			logf(4,"inifile TGDName:%s LCN:%d SVC:%d TSID:%d ORNID:%d satname:%s",
			g_IniSvcPtr[iMode].tgdservice_name,
			g_IniSvcPtr[iMode].LCN_channel_id,g_IniSvcPtr[iMode].service_id,
			g_IniSvcPtr[iMode].transport_stream_id,g_IniSvcPtr[iMode].original_network_id,g_IniSvcPtr[iMode].toppysatName);
			logf(4,"got here,g_total_tvSvc %d, total_tvSvc %d",g_total_tvSvc,total_tvSvc);
#endif
			found=FALSE;
			if (g_total_tvSvc!=total_tvSvc || g_total_radSvc!=total_radSvc)  //have to do this only at startup or channel count
			//change as can have a lot of sat channels, and takes a while to scan
			{
				for (i=0;i<total_tvSvc;i++) //find toppy channel number for serviceID and service name if it is included in the ini.
				{
					TAP_Channel_GetInfo( SVC_TYPE_Tv,  i, &chInfo );  //0=tvsvc
					if (chInfo.svcId==g_IniSvcPtr[iMode].service_id  && 
						chInfo.orgNetId==g_IniSvcPtr[iMode].original_network_id && 
						chInfo.tsId==g_IniSvcPtr[iMode].transport_stream_id)
						//(!strlen(g_IniSvcPtr[iMode].toppyservice_name)	|| 
						//strstr(chInfo.chName,g_IniSvcPtr[iMode].toppyservice_name)))
					{
						g_IniSvcPtr[iMode].topfieldinternalchannel=i;
						logf(4,"Toppyinternal_channel:%d found for ini_svcId:%d LCN:%d inifile_entry:%d",g_IniSvcPtr[iMode].topfieldinternalchannel,g_IniSvcPtr[iMode].service_id,g_IniSvcPtr[iMode].LCN_channel_id,iMode+1);
						found=TRUE;
						break; //added break now as we don't want to scan all the channels, lets find the 1st.
					}
				}
				if (!found)
				{
					logf(2,"ERROR:Can't find inifile service_id:%d LCN:%d for any Topfield channels",g_IniSvcPtr[iMode].service_id,g_IniSvcPtr[iMode].LCN_channel_id);
					g_IniSvcPtr[iMode].topfieldinternalchannel=9999;
				}
			}
		} //for ( iMode = 0; iMode < g_NumServices; iMode++ )	
		g_total_tvSvc=total_tvSvc;
		g_total_radSvc=total_radSvc;
		if (g_btapdirinifile || bsavesettings)  //if inifile was originally found with tap
		{
			g_btapdirinifile=FALSE;    //never worry again as was deleted earlier
			SaveSettings(NOICE);            //save to progfiles folder
		}
		ChangeDir(gTGDdirectory);
		TAP_MemFree(sIniFile);
	}
	else
	{
noinifile:		
#if DEBUG
		printf("problems opening "INI_FILE", setting default options\n");
#endif		
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("ERROR:Load: kann nicht geöffnet werden inifile", "Einstellung von Standardannahmen bei Unterlassungen",150,FALSE);
		else
			ShowMessageWin("ERROR:Load: can't open inifile", "Setting default options",150,FALSE);

		g_GMToffst=0;
		g_freqMin[NTS]=720;
		g_freqMin[TSH]=5;
		g_NumServices=5;
		gautoloadepg=TRUE;
		gautodeleteepg=TRUE;
		//gload9999epgevts=TRUE;
		g_ShowLoadingOSD=LARGE;
		g_loglevel=3;
		g_NextUpdateTime=-1;
		gReloadHour=24;
		g_loadtimewait=3;
		gEITload=FALSE;
		g_scanALLevents=FALSE;

		g_IniSvcPtr=(TYPE_IniSvcDetails *)TAP_MemAlloc(g_NumServices*sizeof(*g_IniSvcPtr));
		for (x=0; x<g_NumServices; x++)
		{
			g_IniSvcPtr[x].toppysatName[0]=0;
			strcpy(g_IniSvcPtr[x].tgdservice_name,"TGDChannelID\0");
			//strcpy(g_IniSvcPtr[x].toppyservice_name,"\0");
			g_IniSvcPtr[x].service_id=0;
			g_IniSvcPtr[x].transport_stream_id=0;
			g_IniSvcPtr[x].original_network_id=0;
			g_IniSvcPtr[x].topfieldinternalchannel=9999;
		}
		SaveSettings(NOICE);
	}
	return FALSE;

	//for ( iMode = 1; iMode <= g_NumServices; iMode += 1 )				
	//{
	//		printf("iMode:%d servicename:%s \n",iMode,service_name[iMode]);
	//}
} //LoadSettings


void ShowIniFile(void)
{
//	char	sIniFile[7680];
	char sBuff[90]; //,str2[50],str3[60],str4[50];
	int i;
	int num_items=NEXTTAP+g_NumServices+2;//TGD directory, transparency  don't appear in menu

	//ChangeDir("ProgramFiles");

	if (!_gFullRegion)
	{
		TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
		_gFullRegion= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
	}
	TAP_Win_SetDefaultColor (&g_winOpts);
	//TAP_Win_Create (&g_winOpts, _gFullRegion, 60, ((278-(num_items+2)*10)<30)?30:(278-(num_items+2)*10), 550, (((num_items+2)*18)>546)?546:(num_items+2)*18, FALSE, FALSE);
	TAP_Win_Create (&g_winOpts, _gFullRegion, 40, ((241-num_items*11)<20)?20:(241-num_items*11), 590, ((num_items*22)>440)?440:num_items*22, FALSE, TRUE);

	if (g_LoadIceIni==LOADICEINI)
		TAP_Win_SetTitle (&g_winOpts, "ICE INI file Info", 0, FNT_Size_1622);
	else
		TAP_Win_SetTitle (&g_winOpts, "INI file Info", 0, FNT_Size_1622);

	sprintf( sBuff, "GMT offset=%d (15min intervals)", g_GMToffst/15);
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff, "Frequency File check no TS=%d", g_freqMin[NTS]);
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff, "Frequency File check Timeshift=%d", g_freqMin[TSH]);
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Email Volume=%s", (gVolume==VOLOFF)?"OFF":(gVolume==LOW)?"LOW":"HIGH" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"AutoLoadEPGdata=%s", (gautoloadepg)?"YES":"NO" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"AutoDeleteEPGdata at start=%s", (gautodeleteepg)?"YES":"NO" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Next Update Time=%d", g_NextUpdateTime );
	TAP_Win_AddItem (&g_winOpts, (g_NextUpdateTime==31)?"Now/Next update: NO":sBuff);
	sprintf( sBuff,"Reload Hour=%d", gReloadHour );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"TGD directory=%s", gTGDdirectory );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	//sprintf( sBuff,"Load 99999999 event details=%s", (gload9999epgevts)?"YES":"NO" );
	//TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Display LOADING OSD=%s\r\n", (g_ShowLoadingOSD==DISOFF)?"NO":(g_ShowLoadingOSD==SMALL)?"MINIMAL":"FULL" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Logging Level=%d\r\n", g_loglevel );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Broadcaster EIT=%s\r\n", (gEITload)?"LOAD":"BLOCK" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"EPG load wait=%d\r\n", g_loadtimewait );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Timer Set Delay=%d\r\n", g_timerdelay );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"TEDS Recordings File=%s\r\n", (gTedRecordingsfile)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Load ICE INI=%s\r\n", (g_LoadIceIni==NOICE)?"NONE":(g_LoadIceIni==LOADNORMALINI)?"NO":"YES" );
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"ToppybiffTransparency=%d\r\n", gemailtransparency);
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( sBuff,"Scan ALL Events=%s\r\n", (g_scanALLevents)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Set Manual Time at Startup=%s\r\n", (g_startupTimeSet)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Set Manual Time at power fail=%s\r\n", (g_powerFailTime)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Manual Time GMT FIX=%s\r\n", (g_manualGMTTime)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Adjust GMT loading entries=%s\r\n", (g_GMTloadAdjust)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Set Manual Time on LCNs=%s\r\n", g_manualLCN);
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"Exit Key for Menu=%s\r\n", (g_buseExitKey)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	sprintf( &sBuff,"LCN TGD load=%s\r\n", (g_bLCNTGDload)?"YES":"NO");
	TAP_Win_AddItem (&g_winOpts, sBuff);
	
	sprintf( sBuff, "Number of Services=%d", g_NumServices);
	TAP_Win_AddItem (&g_winOpts, sBuff);

	for ( i = 0; i < g_NumServices; i += 1 )				
	{

		sprintf( sBuff, "[%01d] LCN:%03d srvcId:%04d tsId:%04d orignetId:%04d name:%s",
			i+1, g_IniSvcPtr[i].LCN_channel_id, g_IniSvcPtr[i].service_id,g_IniSvcPtr[i].transport_stream_id,g_IniSvcPtr[i].original_network_id,
			g_IniSvcPtr[i].tgdservice_name);//,(strlen(g_IniSvcPtr[i].toppyservice_name)>0)?",":"",g_IniSvcPtr[i].toppyservice_name);
		//logf("%s-%s",service_name[i],toppyservice_name[i]);
		TAP_Win_AddItem (&g_winOpts, sBuff);
	}

	g_bIniWinShown = TRUE;
	//ChangeDir(gTGDdirectory);
}//ShowIniFile
