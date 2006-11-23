/************************************************************
				Part of the UK TAP Project
		This module contains some generic tools

Name	: Tools.c
Author	: Kidhazy
Version	: 0.4
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Kidhazy: 10-09-05	Inception Date
	  v0.1 sl8:	20-01-06	All variable initialised
	  v0.2 sl8:	16-02-06	sysID removed
	  v0.3 sl8:	05-08-06	Modified to return result of change dir
	  v0.4 sl8:	28-09-06	GotoPath changed for TF5000.

**************************************************************/



//--------------------------------------------------------------------
//    D I R E C T O R Y    Utilities
//--------------------------------------------------------------------

char _tapDir[512];


//------------------------------ FindTapDir --------------------------------------
// Run once to locate the TAP's directory and stored it in the global _tapDir
//--------------------------------------------------------------------------------------

void FindTapDir(void)
{
	TYPE_File	fp;

	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_FindFirst( &fp );

	if ( strcmp( fp.name, "__ROOT__" ) == 0 )
	{
		strcpy(_tapDir, "ProgramFiles");
	}
	else
	{
		strcpy(_tapDir, "ProgramFiles/Auto Start");
	}
}


//----------------  DIRECTORY TOOLS        ---------------------------------------------
// 


//------------------------------ isFileRec --------------------------------------
//
bool	IsFileRec( char *recFile, 	dword	attr )
{
    //
    // Checks if a file is a valid recording.
    //
    
	char buff[128];

	strcpy( buff, recFile );       // Copy the filename into the buffer for checking.
	strlwr( buff );                // Convert the filename into all lowercase for easy comparison.
	
	if (( strcmp( buff + strlen( buff ) - 4, ".rec" ) == 0 ) && (attr == ATTR_TS) )
        	return TRUE;
	else
        	return FALSE;
}


//----------------  DIRECTORY TOOLS        ---------------------------------------------
// 
//--------------------------------------------------------------------------------------

bool GotoPath(char *fullPath)
{
	int	startPos = 0, i = 0;
	char	singlePath[128];
	bool	result = TRUE;

	if
	(
		(GotoRoot() == TRUE)
		&&
		(fullPath[0] != '/')
		&&
		(strlen(fullPath) > 0)
	)
	{
		for(i = 0; ((i < strlen(fullPath)) && (result == TRUE)); i++)
		{
			if(fullPath[i] == '/')
			{
				memset(singlePath, 0 , sizeof(singlePath));
				strncpy(singlePath, &fullPath[startPos], (i - startPos));
				if
				(
					(singlePath[0] != '/')
					&&
					(strlen(singlePath) > 0)
				)
				{

					if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
					{
						result = FALSE;
					}
				}
				else
				{
					result = FALSE;
				}

				startPos = i + 1;
			}
		}

		memset(singlePath, 0 , sizeof(singlePath));
		strncpy(singlePath, &fullPath[startPos], (i - startPos));
		if
		(
			(result == TRUE)
			&&
			(singlePath[0] != '/')
			&&
			(strlen(singlePath) > 0)
		)
		{
			if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
			{
				result = FALSE;
			}

		}
		else
		{
			result = FALSE;
		}
	}
	else
	{
		result = FALSE;
	}

	return result;
}

	
bool GotoTapDir()
{
	return GotoPath(_tapDir);
}

bool GotoProgramFiles(){
	return GotoPath("ProgramFiles");
}

bool GotoDataFiles(){
	return GotoPath("DataFiles");
}

bool GotoRoot(void)
{
	TYPE_File	tempFile;
	int	folderCount = 0;
	bool	result = FALSE;

	TAP_Hdd_FindFirst( &tempFile );

	//	Loop until Root found allow maximum of 20 levels

	while ( ( strcmp( tempFile.name, "__ROOT__" ) != 0 ) && ( folderCount < 20 ) )
	{
		TAP_Hdd_ChangeDir( ".." );
		TAP_Hdd_FindFirst( &tempFile );
	}

	if(folderCount < 20)
	{
		result = TRUE;
	}

	return result;
}



