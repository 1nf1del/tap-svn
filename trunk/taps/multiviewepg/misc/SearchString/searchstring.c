#include <stdio.h>
#include <string.h>

#define debug 1

// C program on the PC to be ported to the topfield and my guide TAP
enum {
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

typedef struct
{
	int	evtId;				// event ID
	int	startTime;			// start time((MJD << 16) | (Hour << 8) | Min)
	int	endTime;
	int	duration;			// duration((Hour << 8) | Min)
	int	runningStatus;		// running status
	char	eventName[128];
	char	description[128];
	int	parentalRating; 	// parental Rating
	int	svcId;				
} TYPE_TapEvent;

static favitem favourites[200];
static int favitem_ptr = 0;
static TYPE_TapEvent searchresult[200];
static int searchresult_ptr = 0;

static int GridEPG_ChannelExcludes[200];
static int GridEPG_ChannelExcludes_ptr = 0;
static char Settings_ChannelExcludes[256];

int	atoi( const char* sString )
{
	int bNegate=0;
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

	if ( bNegate == 1 )
		iValue = -iValue;
	return iValue;
}


void InsertFavItem( char * term, char * channel, char * day, int location, int start, int end, int priority, int adj, int rpt ){
	if( favitem_ptr < sizeof(favourites) ){
		strcpy(favourites[favitem_ptr].term, term);
		strcpy(favourites[favitem_ptr].channel, channel);
		strcpy(favourites[favitem_ptr].day, day);
		favourites[favitem_ptr].location = location;
		favourites[favitem_ptr].start = start;
		favourites[favitem_ptr].end = end;
		favourites[favitem_ptr].priority = priority;
		favourites[favitem_ptr].adj = adj;
		favourites[favitem_ptr].rpt = rpt;
		favitem_ptr++;
	}
}

void SetupEvent( TYPE_TapEvent * event, int start, int end, int duration, char * name, char * desc, int channel ){
	event->startTime = start;
	event->endTime = end;
	event->duration = duration;
	strcpy(event->eventName, name);
	strcpy(event->description, desc);
	event->svcId = channel;
}

int SearchTimeOk( int evtstart, int favstart, int favend ){
	if( favstart == -1 && favend == -1 ) return 1;	// If any time then return OK
	if( favstart == -1 ){
		if( favend <= evtstart ) return 1;	// Show finishes after or at the same time that the end time is important
	} else {
		// Start time is needed
		if( evtstart >= favstart ){
			if( favend == -1 ) return 1;
			if( evtstart <= favend ) return 1;	// Show finishes after or at the same time that the end time is important
		}
	}
	return 0;
}

int SearchDayOk( TYPE_TapEvent * event, char * day ){
	if( strcmp(day,"All") == 0 ) return 1;
	// Add code on the topfield as the dword structure is on there and we can get the DOW from that to compare
	return 0;
}

int SearchString( char * term, char * string ){
	// We need to add code to search the string here
	// Allow for the + ! in the term
	char *p;
	char *token;
	char *plus = "+";
	char *minus = "!";
	
	p = strchr(term, '+');
	// Fix at something+somethingelse atm
	if( p != NULL ){
	    token = strtok(term, plus);
		p = strstr(string, token);
		if( p != NULL ){
			if( debug ) printf("found %s\n", token);
			// Search for the next item of the +
		    token = strtok(NULL, plus);
			p = strstr(string, token);
			if( p != NULL ){
				// We are finished so lets end
				return 1;
			}
		}
		return 0;
	}
	
	p = strchr(term, '!');
	// Fix at something+somethingelse atm
	if( p != NULL ){
	    token = strtok(term, minus);
		p = strstr(string, token);
		if( p != NULL ){
			if( debug ) printf("found %s\n", token);
			// Search for the next item of the +
		    token = strtok(NULL, minus);
			p = strstr(string, token);
			if( p == NULL ){
				// We are finished so lets end
				if( debug ) printf("Didn't find %s\n", token);
				return 1;
			}
	
		}
		return 0;
	}
	
	// Normal text search
	p = strstr(string, term);
	if( p != NULL ){
		return 1;
	}
	// Look for the Something and Not something	
	return 0;
}

int SearchEvent( TYPE_TapEvent * event, int favitem ){
	// Check for the correct channel
	if(	atoi(favourites[favitem].channel) == event->svcId ){
		// Check that the start and end time is ok
		if( SearchTimeOk( event->startTime, favourites[favitem].start, favourites[favitem].end ) ){
			// Check for the correct day
			if( SearchDayOk( event, favourites[favitem].day ) ){
				switch( favourites[favitem].location ){
					case TITLE:
						return SearchString( favourites[favitem].term, event->eventName );
						break;
					case SUBTITLE:
						return SearchString( favourites[favitem].term, event->description );
						break;
					case DESCRIPTION:
						// TO DO WHEN TAP IMPLEMENTS
						break;
					case ALL:
						break;
				}
			}
		}
	}
	return 0;
}

void splitExcludes( char * channelstring ){
	char *divider = ",";
	int channels[200];
	int channels_ptr = 0;
	int i;
	char *sub_string;

	sub_string = strtok(channelstring, divider);
	channels[channels_ptr] = atoi(sub_string);
	channels_ptr++;

	while ( (sub_string=strtok(NULL, divider)) != NULL)
	{
	channels[channels_ptr] = atoi(sub_string);
	channels_ptr++;
	}
	  
	for( i = 0; i < channels_ptr; i++ ){
	printf( "%d\n", channels[i] );
	}

}

void GridEPG_BuildExcludes( void ){
	int count = 100;	// Maximum number of excludes
	char *ptr;
	char store[256];//static so each iteration reads ok.
	int i,j,k=0;
	strcpy(store,Settings_ChannelExcludes);
	for (j=0;j<=count;j++) //have to keep getting next comma seperated LCN from the last iteration when this routine is called
	{
		if ((ptr = strchr(store,',')) == NULL) //no more comma's for null
		{
			GridEPG_ChannelExcludes[GridEPG_ChannelExcludes_ptr] = atoi(store);
			GridEPG_ChannelExcludes_ptr++;
			break;
		}
		else  //pointer will be set to point to the comma at this point
		{
			k=count+1;
			*ptr='\0';
			GridEPG_ChannelExcludes[GridEPG_ChannelExcludes_ptr] = atoi(store);
			GridEPG_ChannelExcludes_ptr++;
			strcpy(store,ptr+1);
		}
	}
}

char * GridEPG_ExportExcludes( void ){
	int i;
	static char text[128];
	char buff[10];
	memset(text, 0, sizeof(text));
	
	for( i = 0; i < GridEPG_ChannelExcludes_ptr; i++ ){
		printf( "Channel Excluded - %d\n", GridEPG_ChannelExcludes[i] );
	}
	
	for( i = 0; i < GridEPG_ChannelExcludes_ptr; i++ ){
		sprintf( buff, "%d\0", GridEPG_ChannelExcludes[i]);
		strcat( text, buff );
		if( i+1 < GridEPG_ChannelExcludes_ptr ){	// If not the last add the divider
			strcat( text, "," );
		}
	}
	return text;
}

void extractRGBValues( int colour ){
	printf("Colour = %d\n", colour);

	printf("%d\n", colour>>5&0x1F);
	printf("%d\n", colour>>0&0x1F);
// 44474 = 1010110110111010
// ARRRRRGGGGGBBBBB
// 0111110000000000 for red
// 1
// 01011 = 11 * 8 = 88
// 01101 = 13 * 8 = 104
// 11010 = 26 * 8 = 208
}

int extractRGBValue( int type, dword colour ){
	switch( type ){
		case RED:
			return (colour>>10&0x1F)*8;
		case GREEN:
			return (colour>>5&0x1F)*8;
		case BLUE:
			return (colour>>0&0x1F)*8;
	}
	return 0;
}

int main()
{
//	TYPE_TapEvent event;
//	char test_string[50]="2,22,3,5";
//	
//	InsertFavItem( "Formula+Grand Prix", "5", "All", TITLE, -1, -1, 6, 0 , 0 );
//	SetupEvent( &event, 800, 950, 150, "The Oprah Winfrey Show", "", 5 );
//	if( SearchEvent( &event, 0 ) ){
//		printf( "Item has been found\n" );
//	} else {
//		printf( "Item was not found\n" );
//	}
//	
//	// Lets us the token stuff to divide a string of , and place in an array
//	printf("Will call build excludes\n");
//
//	memset( Settings_ChannelExcludes, 0, sizeof(Settings_ChannelExcludes));
//	sprintf(Settings_ChannelExcludes, "%d,%d,%d\0", 3, 5, 60 );
//	GridEPG_BuildExcludes();
//	
//	printf( GridEPG_ExportExcludes() );
//	printf( "\n" );
	
	extractRGBValues(44474);
	
	return 0;
}

