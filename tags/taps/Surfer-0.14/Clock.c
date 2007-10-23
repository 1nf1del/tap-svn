/************************************************************
				Part of the ukEPG project
		This module displays the current time 

Name	: Clock.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 01-09-05	Split from Timebar.c
          v0.1 kidhazy: 16-09-05    Defined parameters to allow for easy movement/placement of clock.

	Last change:  USE  31 Jul 105    2:34 pm
**************************************************************/

//------------
//
void UpdateClock(word tempRgn, int xCoord, int yCoord)
{
	char	str[80], str2[20], str3[20];
	byte 	hour, min, sec, month, day, weekDay;
	word 	year, mjd;
	dword	width;

	TAP_GetTime( &mjd, &hour, &min, &sec);
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

    //	weekDay=2; month=8; day=28; hour=12; min=58;		// use for testing width
	
    // Following code can be used if we decide to display the day of week, and month details.
	switch ( weekDay )
	{
		case 0:	TAP_SPrint(str2,"Mon"); break;
		case 1:	TAP_SPrint(str2,"Tue"); break;
		case 2:	TAP_SPrint(str2,"Wed"); break;
		case 3:	TAP_SPrint(str2,"Thu"); break;
		case 4:	TAP_SPrint(str2,"Fri"); break;
		case 5:	TAP_SPrint(str2,"Sat"); break;
		case 6:	TAP_SPrint(str2,"Sun"); break;
		default : TAP_SPrint(str2,"BAD"); break;
	}

	switch ( month )
	{
		case 1:	TAP_SPrint(str3,"Jan"); break;
		case 2:	TAP_SPrint(str3,"Feb"); break;
		case 3:	TAP_SPrint(str3,"Mar"); break;
		case 4:	TAP_SPrint(str3,"Apr"); break;
		case 5:	TAP_SPrint(str3,"May"); break;
		case 6:	TAP_SPrint(str3,"Jun"); break;
		case 7:	TAP_SPrint(str3,"Jul"); break;
		case 8:	TAP_SPrint(str3,"Aug"); break;
		case 9:	TAP_SPrint(str3,"Sep"); break;
		case 10:TAP_SPrint(str3,"Oct"); break;
		case 11:TAP_SPrint(str3,"Nov"); break;
		case 12:TAP_SPrint(str3,"Dec"); break;
		default : TAP_SPrint(str3,"BAD"); break;
	}

    //	sprintf( str, "%s %d %s %02d:%02d:%02d", str2, day, str3, hour, min, sec);	// Includes seconds - not normally shown
    //	sprintf( str, "%s %d %s %02d:%02d", str2, day, str3, hour, min);            // Includes day and month.
    sprintf( str, " %d:%02d%cm", (hour+23)%12 + 1, min, hour >= 12 ? 'p':'a');       //Display a 12 hour clock with am/pm indicator
	width = TAP_Osd_GetW( str, 0, FNT_Size_1926 );

    //	TAP_Osd_PutGd( tempRgn, xCoord, yCoord, &_timebarGd, TRUE );       // Print a background if you want.
    //	TAP_Osd_PutStringAf1622( tempRgn, 581-(width/2), yCoord, 660, str, TIME_COLOUR, 0 );  // Center the time display.
    TAP_Osd_PutStringAf1926( tempRgn, xCoord-width, yCoord, xCoord, str, MAIN_TEXT_COLOUR, 88 );  
//  TAP_Osd_PutStringAf1926( tempRgn, xCoord-width, yCoord, xCoord, str, MAIN_TEXT_COLOUR, 0);
	
}
