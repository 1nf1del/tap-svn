/************************************************************
				Part of the UK TAP Project
		This module handles the loading of the logo files

Name	: logo.c
Author	: Darkmatter
Version	: 0.5
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	: Uses logos.dat file pre-processed by MakeLogos.
		  Makelogos originates from code written by IanP for the UK TAP project
		  and gif utilities by Topfield & Gershon Elber
History	: v0.0 Darkmatter: 06-06-05	Inception date
		  v0.1 Darkmatter: 08-06-05	Added routing to free memory on exit.
									Add : improved box response during loading of logos
		  v0.2 Darkmatter: 15-06-05	Dramatic speed increase by pre-compiling all logos in to a single logo.dat file
		  v0.3 Darkmatter: 02-07-05	Restore current working directory after use
		  v0.4 Darkmatter: 06-07-05	definition of FormatNameString made void (was undefined)
		  v0.5 Darkmatter: 26-07-05	Fixed problem where long 1st words weren't being displayed until 2nd line.
		  v0.6 Kidhazy:    21-08-05 Updated logo matching to allow matching on LCN as well as Service Name.
		  v0.7 Kidhazy:    22-08-05 New functions to allow for new virtual channel that selects last file that was played.
		  v0.8 Kidhazy:    13-09-05 New function to print name of the selected channel at the top of logos.

	Last change:  USE   7 Aug 105   11:01 pm
************************************************************/

#define MAX_SERVICES	2000

// Following 2 definitions moved to DisplayLogosConfig.c
//#define LOGO_W	60														// predefined logo width
//#define LOGO_H	39														// and height

#define NUM_BYTES	2													// number of bytes per pixel
#define LOGO_DATA_SIZE	LOGO_W * LOGO_H * NUM_BYTES						// number of bytes per logo (bitmap, so nice, fixed size)
#define CHANNEL_NAME_SIZE MAX_SvcName									// bytes allocated the the channel's name

#include "graphics/NarrowHighlight80x22.gd"

typedef struct															// how we store the logo data
{
	byte	svcName[CHANNEL_NAME_SIZE];
	byte	fileName[CHANNEL_NAME_SIZE];
	word    svcLCN;                     //KH match logos on LCN.
	word	svcNum;
	byte	svcType;
	bool	processedFlag;
	byte	*logo;
} TYPE_logoArray;

static TYPE_logoArray logoArrayTv[MAX_SERVICES/2];						// reserve space for all the TV serivces
static TYPE_logoArray logoArrayRadio[MAX_SERVICES/2];					// and radio services

static int maxTvSvc;
static int maxRadioSvc;
static int logoIndex;
static dword systemTickAtLastKey;

void CreateFileName( TYPE_logoArray *localPtr );          //KH Prototype for later.
#include "PlaybackChannel.c"

void FormatNameString( char *source, int *index, char *dest, int width)	// splits a string into that part that fits into the logo width
{																		// increments index, so next part can be printed on next line.
    int l, i, lastSpace;

	l=0; lastSpace=0; i = *index;
	
	do																	// break the string up in to sequences of words
	{																	// stop if any word goes over the allocated width
		if ( (*(dest + l) = *(source + i)) == 0 ) break;				// and rewind to the last space character
		*(dest + l + 1) = 0;											// (end of last word)

		if ( (*(source + i)) == ' ' ) lastSpace = l;
		
		i++; l++;
	}
	while ( TAP_Osd_GetW( dest, 0, FNT_Size_1419) < width );			// keep adding characters until over the box width

	if ( (*(source + i)) == 0 )											// naturally reached the end - nothing else to do
	{																	// just clean up and return
	    *index = i;
	     return;
	}
	
	if ( (*(source + i - 1)) == ' ' )									// finished on a space character
	{																	// overwrite with the string terminator
		*(dest + l - 1) = 0;											// and return
		*index = i;														// skip over the space character for next time
	     return;
	}

	if ( lastSpace == 0 )												// 1st word bigger than space available
	{																	// no choice, but to print it in part
		while ((( *(source + i)) != ' ' ) && (( *(source + i)) != 0 ))	// find the next space, or end of string
		{
			i++; l++;
		}

		*(dest + l - 1) = 0;											// insert the termination character at the end of 1st word
		*index = i+1;
		return;
	}


	*(dest + lastSpace) = 0;											// finished mid-word
	*index = *index + lastSpace + 1;									// go back to the last space character
}



//------------
//
void DisplayLogo( word WindowRgn, dword Xcoord, dword Ycoord, int svcNum, byte svcType )
{
	char	str[80];
	dword	width;
	int		i;

	TYPE_logoArray	*localPtr;
	TYPE_TapChInfo	currentChInfo;
    //KH Make sure the channel selected is valid if we haven't selected the Playback channel, 
    if ((svcNum != Playback_svcnum) &&
       ( !TAP_Channel_GetInfo( svcType, svcNum, &currentChInfo ) )) return;		// service doesn't exist !

	
	if ( svcType == SVC_TYPE_Tv ) localPtr = &logoArrayTv[svcNum];		// recover the correct pointer - TV, or Radio
	else localPtr = &logoArrayRadio[svcNum];

	if ( localPtr->processedFlag == TRUE )								// is this logo in our cache?
	{																	// yes -> display it
		TAP_Osd_RestoreBox( WindowRgn, Xcoord, Ycoord, LOGO_W, LOGO_H, localPtr->logo );
	}
	else
	{	
		TAP_Osd_FillBox( rgn, Xcoord, Ycoord, LOGO_W, LOGO_H, COLOR_Black );

		i=0;															// in place of the logo
		FormatNameString( localPtr->svcName, &i, str, LOGO_W - 5);
		TAP_Osd_PutStringAf1419( WindowRgn, Xcoord, Ycoord, Xcoord + LOGO_W + 5, str, COLOR_White, 0 );

		FormatNameString( localPtr->svcName, &i, str, LOGO_W - 5);
		TAP_Osd_PutStringAf1419( WindowRgn, Xcoord, Ycoord + (LOGO_H/2), Xcoord + LOGO_W + 5, str, COLOR_White, 0 );
	}
}



//------------
//
void DisplaySvcName( word WindowRgn, dword Xcoord, dword Ycoord, dword width, int svcNum, byte svcType )
{
	char	str[80];
	int	    strW;
	int		i;

	TYPE_logoArray	*localPtr;
	TYPE_TapChInfo	currentChInfo;
    //KH Make sure the channel selected is valid if we haven't selected the Playback channel, 
    if ((svcNum != Playback_svcnum) &&
       ( !TAP_Channel_GetInfo( svcType, svcNum, &currentChInfo ) )) return;		// service doesn't exist !

	
	if ( svcType == SVC_TYPE_Tv ) localPtr = &logoArrayTv[svcNum];		// recover the correct pointer - TV, or Radio
	else localPtr = &logoArrayRadio[svcNum];

    TAP_Osd_PutGd( rgn, BASE_X-10, BASE_Y-HEADER_H, &_narrowhighlight80x22Gd, TRUE );	// draw the green bar
    i=0;	// Grab the first line of the Channel name.
	FormatNameString( localPtr->svcName, &i, str, width );
    PrintCenter( WindowRgn, Xcoord, Ycoord, Xcoord + width, str, COLOR_White, 0, FNT_Size_1419);
}



//------------
//
void CreateFileName( TYPE_logoArray *localPtr )
{
	int len, c, cf;
	static char tmp[CHANNEL_NAME_SIZE];

	len = strlen(localPtr->svcName);
	for( c = cf = 0; c < len; c++)									//filter out punctuation from channel name to create file name
	{
		if (!ispunct(localPtr->svcName[c]))
		{
			tmp[cf] = localPtr->svcName[c];
			cf++;
		}
	}
	tmp[cf] = 0;

	strcpy(localPtr->fileName, tmp);								// save the file name
}



//------------
//
void InitLogoMemory( void )
{
	int i, count;
    int countTvSvc, countRadioSvc;
	TYPE_TapChInfo	currentChInfo;

	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );				// how many services are defined on this box?

	if ( countTvSvc > MAX_SERVICES/2 )
	{                                                                   // must quit if too many services
//	    ErrorPrint("Too many services");                                // TF5800 won't be affected	- so hack ok for us 
//		return;															// temp - must fix.
		countTvSvc = MAX_SERVICES/2;
	}
	if ( countRadioSvc > MAX_SERVICES/2 )
	{
		countRadioSvc = MAX_SERVICES/2;
	}


	for ( i=0; i<=countTvSvc; i++)										// for each TV service
	{
		TAP_Channel_GetInfo( SVC_TYPE_Tv, i, &currentChInfo );			// get the info pointer
		strcpy( logoArrayTv[i].svcName, currentChInfo.chName);			// and copy the channel's name

		logoArrayTv[i].svcLCN = currentChInfo.logicalChNum;				// service LCN
		logoArrayTv[i].svcNum = i;										// service number
		logoArrayTv[i].svcType = SVC_TYPE_Tv;							// service type (TV, or Radio)
		logoArrayTv[i].processedFlag = FALSE;							// no logo cached yet
		logoArrayTv[i].logo = TAP_MemAlloc( LOGO_DATA_SIZE );			// allocate space for the logo
		CreateFileName( &logoArrayTv[i] );								// filter out puntuation and generate a file name
	}

	for ( i=0; i<=countRadioSvc; i++)									// do the same for each radio service on this box
	{
		TAP_Channel_GetInfo( SVC_TYPE_Radio, i, &currentChInfo );
		strcpy( logoArrayRadio[i].svcName, currentChInfo.chName);

		logoArrayRadio[i].svcLCN = currentChInfo.logicalChNum;				// service LCN
		logoArrayRadio[i].svcNum = i;
		logoArrayRadio[i].svcType = SVC_TYPE_Radio;
		logoArrayRadio[i].processedFlag = FALSE;
		logoArrayRadio[i].logo = TAP_MemAlloc( LOGO_DATA_SIZE );
		CreateFileName( &logoArrayRadio[i] );							// filter out puntuation and generate a file name
	}
	
    CreatePlaybackChannel( &countTvSvc );  //KH Create logo and pointers for virtual Playback Channel.

	maxTvSvc = countTvSvc;												// save the number of services for later
	maxRadioSvc = countRadioSvc;

	logoIndex = 0;
}




//------------
//
void ReleaseLogoMemory()
{
    int i;
	
	for ( i=0; i<=maxTvSvc; i++)
	{
		TAP_MemFree( logoArrayTv[i].logo );
	}

	for ( i=0; i<=maxRadioSvc; i++)
	{
		TAP_MemFree( logoArrayRadio[i].logo );
	}
}



//------------
//
bool findChannelPointer( char *channelName, TYPE_logoArray **localPtr )
{
    int i;
    char LCNstr[10];     //KH temportay storage of LCN as a string.

	for ( i=0; i<=maxTvSvc; i++)										// first compare all the TV services
	{
        sprintf(LCNstr, "%d", logoArrayTv[i].svcLCN);       //KH Convert integer LCN to string to allow comparison to channel 'filename'

		if (( strcmp( logoArrayTv[i].fileName, channelName ) == 0 ) ||
             (strcmp( LCNstr, channelName ) == 0 )) 		             //KH we match logos based on service name or LCN
		{
			*localPtr = &logoArrayTv[i];
			return TRUE;
		}
	}
	for ( i=0; i<=maxRadioSvc; i++)										// then compare all the Radio services
	{
        sprintf(LCNstr, "%d", logoArrayRadio[i].svcLCN);       //KH Convert integer LCN to string to allow comparison to channel 'filename'

		if (( strcmp( logoArrayRadio[i].fileName, channelName ) == 0) ||
     		(strcmp( LCNstr, channelName ) == 0 ))                     //KH we match logos based on service name or LCN
		{
			*localPtr = &logoArrayRadio[i];
			return TRUE;
		}
	}

	return FALSE;														// didn't find any service name that match
}



//------------
//
void ReadLogoFile()
{
	TYPE_File	*logoFile;
	TYPE_logoArray *logoPointer;
	dword		dataSize;
	byte		*junkBuffer;
	char		channelName[CHANNEL_NAME_SIZE];
	int i; 

//  ChangeDirRoot();                      // Change to the root directory.
//  TAP_Hdd_ChangeDir("ProgramFiles");	  // Let's go to the program files directory.
//	TAP_Hdd_ChangeDir("UK TAP Project");  // Change to the UK TAP Project Directory.
	if ( ! TAP_Hdd_Exist( "logo.dat" ) ) return;						// check our logo file exits in the current directory
	
	logoFile = TAP_Hdd_Fopen( "logo.dat" );
	if ( logoFile == NULL ) return;										// and we can open it ok

    junkBuffer = TAP_MemAlloc(LOGO_DATA_SIZE);							// create a place to read in unwanted logos
	
	while ( TAP_Hdd_Ftell( logoFile ) < TAP_Hdd_Flen( logoFile ) )		// Keep reading logos until we reach the end of file
	{
		TAP_Hdd_Fread( channelName, CHANNEL_NAME_SIZE, 1, logoFile );	// read the channel name
		if ( findChannelPointer( channelName, &logoPointer ) )			// cross referece the array to find this entry
		{
			TAP_Hdd_Fread( logoPointer->logo, LOGO_DATA_SIZE, 1, logoFile );	// read the logo bitmap
			logoPointer->processedFlag = TRUE;							// mark this channel as having a logo in the cache
		}
		else
		{																// else channel doesn't appear in service list ->
			TAP_Hdd_Fread( junkBuffer, LOGO_DATA_SIZE, 1, logoFile );	// must read the logo, otherwise read pointer will get stuck
		}
	}

	TAP_Hdd_Fclose( logoFile );
//	TAP_Hdd_ChangeDir("..");											// return to original directory
	TAP_MemFree( junkBuffer );
}
	

//------------
//
void CacheLogos( void )
{
	InitLogoMemory();
	ReadLogoFile();
}

