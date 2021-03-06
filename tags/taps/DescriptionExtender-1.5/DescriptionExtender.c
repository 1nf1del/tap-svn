/*
	Copyright (C) 2005-2006 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <tap.h>
#include <string.h>
#include <firmware.h>
#include <OPCodes.h>
#include <messagewin.h>
#include <model.h>
#include "EPG.h"

#if defined(CT)
#define POSTFIX " CT"
#elif defined(NG)
#define POSTFIX " NG"
#else
#define POSTFIX ""
#endif

TAP_ID( 0x81243235 );
TAP_PROGRAM_NAME("DescriptionExtender 1.6" POSTFIX);
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Provides long TAP EPG descriptions");
TAP_ETCINFO(__DATE__);


#include <TSRCommander.h>


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
#ifdef DEBUG
	TAP_Print("Exiting DescriptionExtender\n");
#endif
	UndoFirmwareHacks();
	return TRUE;
}


//-----------------------------------------------------------------------------
// This function displays a config menu
void TSRCommanderConfigDialog()
{
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_KEY )
	{
#ifdef DEBUG
		if ( param1 == RKEY_Power )
		{
			TSRCommanderExitTAP();
			TAP_Exit();
			return 0;
		}
#endif
	}
	return param1;
}


//-----------------------------------------------------------------------------
typedef struct {
	word	model;
    int		firmwareVersion;
	dword	eventTable;
	word	eventTableLength;
	word	eventLength;
	dword	eventTableType;
} FirmwareDetail;


FirmwareDetail firmware[] = 
{
	// Model, FW ver,	Event Table,	Size	Length	Type
	// TF5800t
	456,	0x1326,		0x80335504,		14000,	0x44,	2,		// 19 Sep 2006
	456,	0x1288,		0x80333284,		14000,	0x44,	2,		// 14 Jul 2006
	456,	0x1225,		0x8032a7c8,		5000,	0x40,	1,		// 08 Dec 2005
	456,	0x1209,		0x80326c4c,		5000,	0x40,	1,		// 15 Sep 2005 
	456,	0x1205,		0x8032e818,		5000,	0x40,	1,		// 07 Sep 2005
	456,	0x1204,		0x8032e698,		5000,	0x40,	1,		// 01 Sep 2005

	// TF5000t
	//416,	0x1304,		0x8029b1f4,		5000,	0x40,	1,		// 05 Sep 2006		
	416,	0x1248,		0x802961c4,		5000,	0x40,	1,		// 20 Feb 2006
	416,	0x1212,		0x8028ebf4,		5000,	0x40,	1,		// 05 Oct 2005

	// TF5000_5500
	406,	0x1205,		0x802d6bb4,		5000,	0x40,	1,		// 13 Sep 2005

	// TF5010_5510
	436,	0x1212,		0x802d84e0,		4000,	0x40,	1,		// 05 Oct 2005

	// TF5100c
	//12406,	0x1306,		0x802bf080,		5000,	0x48,	1,		// 15 Sep 2006
	12406,	0x1281,		0x802b74c4,		5000,	0x40,	1,		// 12 Jun 2006
	12406,	0x1264,		0x802b5be4,		5000,	0x40,	1,		// 19 Apr 2006
	12406,	0x1260,		0x802b5498,		5000,	0x40,	1,		// 15 Mar 2006
	12406,	0x1248,		0x802b5120,		5000,	0x40,	1,		// 20 Feb 2006
	12406,	0x1205,		0x802ad730,		5000,	0x40,	1,		// 12 Sep 2005
	12406,	0x1170,		0x802c27b0,		5000,	0x40,	1,		// 04 May 2005

	// TF5100
	//13406,	0x1300,		0x802b33ac,		5000,	0x40,	1,		// ?? Sep 2006
	13406,	0x1264,		0x802ae9a0,		5000,	0x40,	1,		// 19 Apr 2006
	13406,	0x1260,		0x802ae474,		5000,	0x40,	1,		// 15 Mar 2006
	13406,	0x1205,		0x802a5cac,		5000,	0x40,	1,		// 12 Sep 2005

	// TF5100c_MP
	//1486,	0x1327,		0x802e0360,		5000,	0x48,	1,		// 25 Sep 2006
	1486,	0x1266,		0x802d823c,		5000,	0x40,	1,		// 24 Apr 2006
	1486,	0x1260,		0x802d7b60,		5000,	0x40,	1,		// 15 Mar 2006
	1486,	0x1212,		0x802cf550,		5000,	0x40,	1,		// 05 Oct 2005

	// TF5100t_MP
	1466,	0x1266,		0x802d1040,		5000,	0x40,	1,		// 24 Apr 2006
	1466,	0x1260,		0x802d0b9c,		5000,	0x40,	1,		// 15 Mar 2006
	1466,	0x1212,		0x802c7ab4,		5000,	0x40,	1,		// 04 Oct 2005

	// TF5200c
	10446,	0x1296,		0x802b7710,		5000,	0x40,	1,		// 29 Jul 2006
	10446,	0x1205,		0x802ac8e8,		5000,	0x40,	1,		// 13 Sep 2005

	// Dedicated Procaster firmware is no longer developed. Update with TF5100 firmwares instead
	// PC5101c_5102c
	//10416,	0x1281,		0x802b74c4,		5000,	0x40,	1,		// TF5100c 12 Jun 2006 (crossflashed)
	10416,	0x1260,		0x802b5498,		5000,	0x40,	1,		// TF5100c 15 Mar 2006 (crossflashed)
	10416,	0x1212,		0x802a8fe4,		5000,	0x40,	1,		// 05 Oct 2005

	// PC5101t_5102t
	10426,	0x1260,		0x802ae474,		5000,	0x40,	1,		// TF5100 15 Mar 2006 (crossflashed)
	10426,	0x1212,		0x802a1578,		5000,	0x40,	1,		// 05 Oct 2005

	// Other crossflashed firmwares - will be removed if they cause problems
	416,	0x1260,		0x802ae474,		5000,	0x40,	1		// TF5100 15 Mar 2006
};


void* FW_MemAlloc(dword size)
{
}
void FW_sprintf(void *s, const void *fmt, ...)
{
}
void FW_Print(const void *fmt, ...)
{
}


FirmwareDetail* GetFirmwareDetail()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}

typedef struct
{
    byte genre;
    byte subgenre;
    byte description[0x40];
} GenreTable;

GenreTable genres[] = {
    { 0x0,0x00,"Unclassified" },

    { 0x1,0x00,"Movie/Drama" },
    { 0x1,0x01,"detective/thriller" },
    { 0x1,0x02,"adventure/western/war" },
    { 0x1,0x03,"science fiction/fantasy/horror" },
    { 0x1,0x04,"comedy" },
    { 0x1,0x05,"soap/melodrama/folkloric" },
    { 0x1,0x06,"romance" },
    { 0x1,0x07,"serious/classical/religious/historical movie/drama" },
    { 0x1,0x08,"adult movie/drama" },

    { 0x2,0x00,"News/Current Affairs" },
    { 0x2,0x01,"news/weather report" },
    { 0x2,0x02,"news magazine" },
    { 0x2,0x03,"documentary" },
    { 0x2,0x04,"discussion/interview/debate" },

    { 0x3,0x00,"Show/Game Show" },
    { 0x3,0x01,"game show/quiz/contest" },
    { 0x3,0x02,"variety show" },
    { 0x3,0x03,"talk show" },

    { 0x4,0x00,"Sports" },
    { 0x4,0x01,"special sports events" },
    { 0x4,0x02,"sports magazines" },
    { 0x4,0x03,"football/soccer" },
    { 0x4,0x04,"tennis/squash" },
    { 0x4,0x05,"team sports" },
    { 0x4,0x06,"athletics" },
    { 0x4,0x07,"motor sport" },
    { 0x4,0x08,"water sport" },
    { 0x4,0x09,"winter sports" },
    { 0x4,0x0a,"equestrian" },
    { 0x4,0x0b,"martial sports" },

    { 0x5,0x00,"Children's/Youth Programmes" },
    { 0x5,0x01,"pre-school children's programmes" },
    { 0x5,0x02,"entertainment programmes for 6 to 14" },
    { 0x5,0x03,"entertainment programmes for 10 to 16" },
    { 0x5,0x04,"informational/educational/school programmes" },
    { 0x5,0x05,"cartoons/puppets" },

    { 0x6,0x00,"Music/Ballet/Dance" },
    { 0x6,0x01,"rock/pop" },
    { 0x6,0x02,"serious music/classical music" },
    { 0x6,0x03,"folk/traditional music" },
    { 0x6,0x04,"jazz" },
    { 0x6,0x05,"musical/opera" },
    { 0x6,0x06,"ballet" },

    { 0x7,0x00,"Arts/Culture (without music)" },
    { 0x7,0x01,"performing arts" },
    { 0x7,0x02,"fine arts" },
    { 0x7,0x03,"religion" },
    { 0x7,0x04,"popular culture/traditional arts" },
    { 0x7,0x05,"literature" },
    { 0x7,0x06,"film/cinema" },
    { 0x7,0x07,"experimental film/video" },
    { 0x7,0x08,"broadcasting/press" },
    { 0x7,0x09,"new media" },
    { 0x7,0x0a,"arts/culture magazines" },
    { 0x7,0x0b,"fashion" },

    { 0x8,0x00,"Social/Political Issues/Economics" },
    { 0x8,0x01,"magazines/reports/documentary" },
    { 0x8,0x02,"economics/social advisory" },
    { 0x8,0x03,"remarkable people" },

    { 0x9,0x00,"Education/Science/Factual Topics" },
    { 0x9,0x01,"nature/animals/environment" },
    { 0x9,0x02,"technology/natural sciences" },
    { 0x9,0x03,"medicine/physiology/psychology" },
    { 0x9,0x04,"foreign countries/expeditions" },
    { 0x9,0x05,"social/spiritual sciences" },
    { 0x9,0x06,"further education" },
    { 0x9,0x07,"languages" },

    { 0xa,0x00,"Leisure Hobbies" },
    { 0xa,0x01,"tourism/travel" },
    { 0xa,0x02,"handicraft" },
    { 0xa,0x03,"motoring" },
    { 0xa,0x04,"fitness & health" },
    { 0xa,0x05,"cooking" },
    { 0xa,0x06,"advertisement/shopping" },
    { 0xa,0x07,"gardening" },

    { 0xb,0x00,"Original Language" },
    { 0xb,0x01,"black and white" },
    { 0xb,0x02,"unpublished" },
    { 0xb,0x03,"live broadcast" },

	{ 0xff, 0xff, "" }
};


GenreTable* GetGenreTable()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}



char* GetGenreName( byte genre, byte subgenre )
{
	int i;
	if (subgenre>0)
		FW_Print("%d %d\n", genre,subgenre);
    for ( i=0; genres[i].genre != 0xff; ++i )
	{
        if ( genres[i].genre == genre && genres[i].subgenre == subgenre )
            return genres[i].description;
    }
    return genres[0].description;
}


// Read description from the firmware's EPG event table
byte* GetEventDescriptionv1( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		if ( parameters )
		{
			type_eventtable_v1* e = (type_eventtable_v1*)parameters->eventTable;
			int eventTableLength = parameters->eventTableLength;
			int eventLength = parameters->eventLength;

			// Try each entry in the event table. Not very efficient, but the firmware does exactly the same
			int i;
			for ( i=0; i<eventTableLength; ++i, (byte*)e+=eventLength )
			{
				// On valid events, match evtId
				if ( (e->char00 & 0xc0) && (e->duration_hour | e->duration_min) && event->evtId == e->event_id )
				{
					// Match satIdx, orgNetId, tsId, svcId, evtId
					if ( event->svcId == e->service_id && event->tsId == e->transport_stream_id &&
						 event->orgNetId == e->original_network_id && event->satIdx == e->sat_index )
					{
						int outputLength = 0;
						int descriptionLength = 0;
						bool addSpace = FALSE;

						// Calculate how much memory will be needed for the text
						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte* description = e->event_name;
						if ( description )
						{
							description += e->event_name_length;
							descriptionLength = strlen(description);
							if ( descriptionLength > 127 )
							{
								outputLength += descriptionLength;
								addSpace = TRUE;
							}
						}

						// Add on the extended info length, plus space for a zero terminator
						if ( e->extended_length > 0 )
						{
#ifndef CT
							// only append an additional space if there is already some extended info to append
							if ( addSpace )
								++outputLength;
#endif
							outputLength += e->extended_length + 1;
							addSpace = FALSE;
						}

						// Add one more for an additional terminator
						++outputLength;

						// If there's going to be something worth returning
						if ( outputLength > 0 )
						{
							bool addSpace = FALSE;

							// allocate memory, plus space for a zero terminator
							result = FW_MemAlloc( outputLength+1 );
							if ( result )
							{
								byte* p = result;
								// Append any long description text
								if ( descriptionLength > 127 )
								{
									memcpy( p, description, descriptionLength );
									p += descriptionLength;
									addSpace = TRUE;
								}

								// Append the existing extended infomation
								if ( e->extended_length > 0 )
								{
#ifndef CT
									// only append an additional space if there is extended info to append
									if ( addSpace )
										*p++ = ' ';
#endif
									memcpy( p, e->extended_event_name, e->extended_length );
									p += e->extended_length;
									addSpace = FALSE;
								}

								// And terminate with an additional terminator
								p[0] = '\0';
							}
						}
						break;
					}
				}
			}
		}
	}

	return result;
}


// Read description from the firmware's EPG event table
byte* GetEventDescriptionv2( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		if ( parameters )
		{
			type_eventtable_v2* e = (type_eventtable_v2*)parameters->eventTable;
			int eventTableLength = parameters->eventTableLength;
			int eventLength = parameters->eventLength;

			// Try each entry in the event table. Not very efficient, but the firmware does exactly the same
			int i;
			for ( i=0; i<eventTableLength; ++i, (byte*)e+=eventLength )
			{
				// On valid events, match evtId
				if ( (e->char00 & 0xc0) && (e->duration_hour | e->duration_min) && event->evtId == e->event_id )
				{
					// Match satIdx, orgNetId, tsId, svcId, evtId
					if ( event->svcId == e->service_id && event->tsId == e->transport_stream_id &&
						 event->orgNetId == e->original_network_id && event->satIdx == e->sat_index )
					{
						int outputLength = 0;
						int descriptionLength = 0;
						char* genre = 0;
						int genreLength = 0;
						bool addSpace = FALSE;

						// Calculate how much memory will be needed for the text
						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte* description = e->event_name;
						if ( description )
						{
							description += e->event_name_length;
							descriptionLength = strlen(description);
							if ( descriptionLength > 127 )
							{
								outputLength += descriptionLength;
								addSpace = TRUE;
							}
						}

						// Add on the extended info length, plus space for a zero terminator
						if ( e->extended_length > 0 )
						{
#ifndef CT
							// only append an additional space if there is already some extended info to append
							if ( addSpace )
								++outputLength;
#endif
//							FW_Print("%d %s\n", e->extended_length, e->extended_event_name);
							outputLength += e->extended_length + 1;
							addSpace = FALSE;
						}

#ifndef NG
						// Add on the genre length, plus space for brackets and a zero terminator
						genre = GetGenreName( e->genre, e->subgenre );
						if ( genre )
						{
							genreLength = strlen(genre);
							if ( addSpace )
								++outputLength;
							outputLength += genreLength + 3;
						}
#endif

						// Add one more for an additional terminator
						++outputLength;

						// If there's going to be something worth returning
						if ( outputLength > 0 )
						{
							bool addSpace = FALSE;

							// allocate memory, plus space for a zero terminator
							result = FW_MemAlloc( outputLength+1 );
							if ( result )
							{
								byte* p = result;
								// Append any long description text
								if ( descriptionLength > 127 )
								{
									memcpy( p, description, descriptionLength );
									p += descriptionLength;
									addSpace = TRUE;
								}

								// Append the existing extended infomation
								if ( e->extended_length > 0 )
								{
#ifndef CT
									// only append an additional space if there is extended info to append
									if ( addSpace )
										*p++ = ' ';
#endif
									memcpy( p, e->extended_event_name, e->extended_length );
									p += e->extended_length;
									addSpace = FALSE;
								}

								// Append the genre
								if ( genre )
								{
									if ( addSpace )
										*p++ = ' ';

									*p++ = '[';
									memcpy( p, genre, genreLength );
									p += genreLength;
									*p++ = ']';
									*p++ = '\0';
								}

								// And terminate with an additional terminator
								p[0] = '\0';
							}
						}
						break;
					}
				}
			}
		}
	}

	return result;
}

int TAP_Main(void)
{
	dword eventTable;
	bool supported = FALSE;
	int index;

	// Look up the current firmware
	for (index=0; index<sizeof(firmware)/sizeof(FirmwareDetail); ++index)
	{
		if ( *sysID == firmware[index].model && _appl_version == firmware[index].firmwareVersion )
		{
			supported = TRUE;
			break;
		}
	}

	if ( !supported )
	{
		// Try and get some helpful information of users with unsupported firmware
		char buffer[300];
		sprintf( buffer, "Description Extender is not compatible with your firmware\n"
			"For an update, please contact the author quoting\n"
			"%d, %04X, %X and %X for an update", *sysID, _appl_version, TAP_GetCurrentEvent-0x80000000, TAP_EPG_GetExtInfo-0x80000000 );
		ShowMessage( buffer, 750 );
		return 0;
	}

	// We need to know the address of the firmware event table without needing to use $gp
	((word*)GetFirmwareDetail)[1] = ((dword)(firmware+index) >> 16) & 0xffff;
	((word*)GetFirmwareDetail)[3] = (dword)(firmware+index) & 0xffff;

	((word*)GetGenreTable)[1] = ((dword)genres >> 16) & 0xffff;
	((word*)GetGenreTable)[3] = ((dword)genres) & 0xffff;

	// Replace TAP_EPG_GetExtInfo with a jump to our GetEventDescription function
	HackFirmware( (dword*)TAP_EPG_GetExtInfo, firmware[index].eventTableType == 1 ? J(GetEventDescriptionv1) : J(GetEventDescriptionv2) );
	HackFirmware( ((dword*)TAP_EPG_GetExtInfo)+1, NOP_CMD ); // MUST insert a nop after the jump

	// Set up TAP API wrappers that don't require the use of $gp to be called
	((dword*)FW_MemAlloc)[0] = J(TAP_MemAlloc);
	((dword*)FW_MemAlloc)[1] = NOP_CMD;
	// Useful for debugging
	((dword*)FW_sprintf)[0] = J(TAP_SPrint);
	((dword*)FW_sprintf)[1] = NOP_CMD;
	((dword*)FW_Print)[0] = J(TAP_Print);
	((dword*)FW_Print)[1] = NOP_CMD;

	return 1;
}
