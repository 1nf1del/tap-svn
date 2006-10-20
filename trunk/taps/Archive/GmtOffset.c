/************************************************************
				Part of the ukEPG project
	This module handles the reading of information from the GMT_Offset.ini file.
	This file is created by the PBK TAP to provide an indication of daylight savings times.
 
Name	: GmtOffset.c
Author	: Peter Gillepsie
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 20/10/2006  Code from Peter G moved into this routine.


************************************************************/


//------------------------------ GetInteger --------------------------------------
//
int	GetInteger( char* sString )
{
    
    int   iValue;
	int    iNdx, iLen;
	bool	bNegate;

	bNegate = FALSE;
	iLen = strlen( sString );
	iValue = 0;

	for ( iNdx = 0; iNdx < iLen; iNdx += 1 )				
	{
		if ( sString[iNdx] == 45 )
		{
			bNegate = TRUE;
		}
		else
		{
			if ( sString[iNdx] < 48 || sString[iNdx] > 57 )
				return 0;
			iValue = ( iValue * 10 ) + ( sString[iNdx] - 48 );
		}
	}

	if ( bNegate == TRUE )
		iValue = iValue * -1;

	return iValue;
}


//------------------------------ GetSetting --------------------------------------
//

bool	GetSetting ( char* sIniFile, char* sSrcSection, char* sSrcLabel, char* sResult )
{
	int	iNdx, iRecSize, ibNdx;
	char	sLabel[200], sParam[200], sBuff[200], intAlpha[20];
	bool	bEqual, bSectFound, bParam;

	memset( sLabel, 0, sizeof sLabel );
	memset( sParam, 0, sizeof sParam );
	memset( sResult, 0, sizeof sResult );

	iRecSize = strlen( sIniFile );
	bEqual = FALSE;
	bSectFound = FALSE;
	ibNdx = 0;
	
	for ( iNdx = 0; iNdx < iRecSize; iNdx += 1 )				
	{
		if (  ( sIniFile[iNdx] == 10 ) || ( sIniFile[iNdx] == 13 ) || ( iNdx == ( iRecSize - 1 ) ) )	
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
					if ( strcmp( sLabel, sSrcLabel ) == 0 )
					{
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
			if ( sIniFile[iNdx] != 10 )
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



//------------------------------ ReadGmtOffset --------------------------------------------
//

void ReadGmtOffset()
{
    TYPE_File* fp;
    char    sIniFile[1024];
    char    sResult[256];
    char    intAlpha[100];
    int     PBKgmtOffset;
 
    memset( sIniFile, 0, sizeof sIniFile );
    memset( sResult, 0, sizeof sResult );

    PBKgmtOffset = 1000;
    ChangeDirRoot();
    TAP_Hdd_ChangeDir( "ProgramFiles" );
    TAP_Hdd_ChangeDir( "Auto Start" );
    if ( fp = TAP_Hdd_Fopen( "GMT_Offset.ini" ) )
    {
        TAP_Hdd_Fread( sIniFile, sizeof( sIniFile ), 1, fp );
        TAP_Hdd_Fclose( fp );
        if ( strlen ( sIniFile ) < 100 ) return;
        GetSetting( sIniFile, "[Time Settings]", "GmtOffset", sResult );
        PBKgmtOffset = GetInteger( sResult );
        PBKgmtOffset = PBKgmtOffset * 15;
        GetSetting( sIniFile, "[Time Settings]", "DstActive", sResult );
        if ( strcmp ( sResult, "True" ) == 0 )  PBKgmtOffset= PBKgmtOffset + 60;
    }
    if (PBKgmtOffset != 1000) // Then we found a GMT Offset setting in GMT_Offset.ini, so update our global setting to match that.
       GMToffsetOption = PBKgmtOffset;
       
}
