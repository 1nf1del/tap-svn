/************************************************************
				Part of the ukEPG project
		This module displays the current time in a bar

Name	: TimeBar.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 27-07-05	Split from TimerDisplay.c

	Last change:  USE  31 Jul 105    2:34 pm
**************************************************************/

//------------
//
void UpdateListClock(void)
{
	char	str[80], str2[20], str3[20];
	byte 	hour, min, sec, month, day, weekDay;
	word 	year, mjd;
	dword	width;

	TAP_GetTime( &mjd, &hour, &min, &sec);
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

//	weekDay=2; month=8; day=28; hour=12; min=58;		// use for testing width
	
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

//	TAP_SPrint( str, "%s %d %s %02d:%02d:%02d", str2, day, str3, hour, min, sec);		// includes seconds - not normally shown
//	TAP_SPrint( str, "%s %d %s %02d:%02d", str2, day, str3, hour, min);
	TAP_SPrint( str, "%s %d %s %d:%02d%cm", str2, day, str3, (hour+23)%12 + 1, min, hour >= 12 ? 'p':'a'); //Display a 12 hour clock with am/pm indicator
	width = TAP_Osd_GetW( str, 0, FNT_Size_1622 );

	TAP_Osd_PutGd( rgn, 494, 34, &_timebarGd, TRUE );
	TAP_Osd_PutStringAf1622( rgn, 581-(width/2), 39, 660, str, TIME_COLOUR, 0 );
}


