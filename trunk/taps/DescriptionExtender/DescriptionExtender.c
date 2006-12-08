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
#include "DescriptionExtender.h"
#include "EPG.h"
#include "Settings.h"


#define LOGFILE "DescriptionExtenderLog.txt"


//-----------------------------------------------------------------------------
typedef struct
{
    int		firmwareVersion;
	dword	getCurrentEvent;
	dword	eventTable;
	word	eventTableLength;
	word	eventLength;
	byte	extendedLengthOffset;
	byte	extendedEventNameOffset;
	byte	genreOffset;
} FirmwareDetail;


static FirmwareDetail firmware[] = 
{
	// FW ver,	GetCurEvent,	Event Table,	Size	Length	len,  name, genre
	// TF5800t - 456
	0x1326,		0x80181df0,		0x80335504,		14000,	0x44,	0x32, 0x2c, 0x40,	// 19 Sep 2006
	0x1299,		0x80181840,		0x80334f3c,		14000,	0x44,	0x32, 0x2c, 0x40,	// 18 Aug 2006
	0x1288,		0x8017fc08,		0x80333284,		14000,	0x44,	0x32, 0x2c, 0x40,	// 14 Jul 2006
	0x1225,		0x8017a3ec,		0x8032a7c8,		5000,	0x40,	0x2e, 0x30, 0,		// 08 Dec 2005
	0x1209,		0x801792f8,		0x80326c4c,		5000,	0x40,	0x2e, 0x30, 0,		// 15 Sep 2005 
	0x1205,		0x80178788,		0x8032e818,		5000,	0x40,	0x2e, 0x30, 0,		// 07 Sep 2005

	// TF5000t - 416
	0x1330,		0x801827b8,		0x802a0964,		5000,	0x40,	0x32, 0x2c, 0,		// 03 Nov 2006
	0x1306,		0x80181b08,		0x8029e8c4,		5000,	0x40,	0x32, 0x2c, 0,		// 12 Sep 2006
	0x1304,		0x8017d66c,		0x8029b1f4,		5000,	0x40,	0x32, 0x2c, 0,		// 05 Sep 2006
	0x1248,		0x80179378,		0x802961c4,		5000,	0x40,	0x2e, 0x30, 0,		// 20 Feb 2006
	0x1212,		0x8017a67c,		0x8028ebf4,		5000,	0x40,	0x2e, 0x30, 0,		// 05 Oct 2005

	// TF5000_5500 - 406
	0x1293,		0x801928f8,		0x802e1c20,		4000,	0x40,	0x32, 0x2c, 0,		// 20 Jul 2006
	0x1292,		0x80192e94,		0x802e3d80,		4000,	0x40,	0x32, 0x2c,	0,		// 13 Jul 2006
	0x1205,		0x8018d604,		0x802d6bb4,		4000,	0x40,	0x2e, 0x30, 0,		// 13 Sep 2005

	// TF5010_5510 - 436
	0x1212,		0x80188b0c,		0x802d84e0,		4000,	0x40,	0x2e, 0x30, 0,		// 05 Oct 2005

	// TF5010_MP - 1456
	0x1253,		0x80185a58,		0x802ee2f4,		4000,	0x40,	0x2e, 0x30, 0,		// 02 Mar 2006
	0x1218,		0x80182b0c,		0x802e64ac,		4000,	0x40,	0x2e, 0x30, 0,		// 15 Sep 2005

	// TF5100c - 12406
	0x1332,		0x8018e300,		0x802c0c88,		5000,	0x48,	0x32, 0x2c, 0,		// 10 Nov 2006
	0x1329,		0x8018e26c,		0x802c06e0,		5000,	0x48,	0x32, 0x2c, 0,		// 02 Nov 2005
	0x1328,		0x8018e15c,		0x802c04e8,		5000,	0x48,	0x32, 0x2c, 0,		// 22 Oct 2006
	0x1306,		0x8018de70,		0x802bf080,		5000,	0x48,	0x32, 0x2c, 0,		// 15 Sep 2006
	0x1281,		0x80190b5c,		0x802b74c4,		5000,	0x40,	0x2e, 0x30, 0,		// 12 Jun 2006
	0x1264,		0x8018ad58,		0x802b5be4,		5000,	0x40,	0x2e, 0x30, 0,		// 19 Apr 2006
	0x1260,		0x8018a628,		0x802b5498,		5000,	0x40,	0x2e, 0x30, 0,		// 15 Mar 2006
	0x1248,		0x8018a3e8,		0x802b5120,		5000,	0x40,	0x2e, 0x30, 0,		// 20 Feb 2006
	0x1205,		0x80187960,		0x802ad730,		5000,	0x40,	0x2e, 0x30, 0,		// 12 Sep 2005

	// TF5100c_MP - 1486
	0x1328,		0x8019487c,		0x802e1738,		5000,	0x48,	0x32, 0x2c, 0,		// 22 Oct 2006
	0x1327,		0x801946b0,		0x802e0360,		5000,	0x48,	0x32, 0x2c, 0,		// 25 Sep 2006
	0x1266,		0x801905cc,		0x802d823c,		5000,	0x40,	0x2e, 0x30, 0,		// 24 Apr 2006
	0x1260,		0x8018fde0,		0x802d7b60,		5000,	0x40,	0x2e, 0x30, 0,		// 15 Mar 2006

	// TF5100 - 13406
	0x1332,		0x8018be34,		0x802b5324,		5000,	0x40,	0x32, 0x2c, 0,		// 13 Nov 2006
	0x1300,		0x8018b578,		0x802b33ac,		5000,	0x40,	0x32, 0x2c, 0,		// ?? Sep 2006
	0x1264,		0x80187df4,		0x802ae9a0,		5000,	0x40,	0x2e, 0x30, 0,		// 19 Apr 2006
	0x1260,		0x80187778,		0x802ae474,		5000,	0x40,	0x2e, 0x30, 0,		// 15 Mar 2006
	0x1205,		0x80184650,		0x802a5cac,		5000,	0x40,	0x2e, 0x30, 0,		// 12 Sep 2005

	// TF5100t_MP - 1466
	0x1327,		0x801918ec,		0x802d6a64,		5000,	0x40,	0x32, 0x2c, 0,		// 13 Oct 2006
	0x1266,		0x8018d5f8,		0x802d1040,		5000,	0x40,	0x2e, 0x30, 0,		// 24 Apr 2006
	0x1260,		0x8018d024,		0x802d0b9c,		5000,	0x40,	0x2e, 0x30, 0,		// 15 Mar 2006
	0x1212,		0x801897cc,		0x802c7ab4,		5000,	0x40,	0x2e, 0x30, 0,		// 04 Oct 2005

	// TF5200c - 10446
	0x1328,		0x80191478,		0x802b9f2c,		5000,	0x40,	0x32, 0x2c, 0,		// 18 Oct 2006
	0x1306,		0x80191240,		0x802b9a04,		5000,	0x40,	0x32, 0x2c, 0,		// 12 Sep 2006
	0x1296,		0x80190d7c,		0x802b7710,		5000,	0x40,	0x2e, 0x30, 0,		// 29 Jul 2006
	0x1205,		0x80187114,		0x802ac8e8,		5000,	0x40,	0x2e, 0x30, 0,		// 13 Sep 2005

	// Dedicated Procaster firmware is no longer developed. Update with TF5100 firmwares instead
	// PC5101c_5102c - 10416
	0x1212,		0x801884e8,		0x802a8fe4,		5000,	0x40,	0x2e, 0x30, 0,		// 05 Oct 2005

	// PC5101t_5102t - 10426
	0x1212,		0x801851d4,		0x802a1578,		5000,	0x40,	0x2e, 0x30, 0,		// 05 Oct 2005
};


static void* FW_MemAlloc(dword size)
{
}
static void FW_sprintf(void *s, const void *fmt, ...)
{
}
static void FW_Print(const void *fmt, ...)
{
}


static FirmwareDetail* GetFirmwareDetail()
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
byte* GetEventDescription( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		Options* options = (Options*)parameters->firmwareVersion;
		if ( parameters )
		{
			type_eventtable* e = (type_eventtable*)parameters->eventTable;
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
						word extendedLength = *((word*)(((byte*)e)+parameters->extendedLengthOffset));

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
						if ( extendedLength > 0 )
						{
							// if additional space is enabled and there is some extended info to append
							if ( options->insertSpace && addSpace )
								++outputLength;
							outputLength += extendedLength + 1;
							addSpace = FALSE;
						}

						// if the genre is enabled and the firmware supports it
						if ( options->addGenre && parameters->genreOffset )
						{
							// Add on the genre length, plus space for brackets and a zero terminator
							genre = GetGenreName( *(((byte*)e)+parameters->genreOffset),
												  *(((byte*)e)+parameters->genreOffset+1));
							if ( genre )
							{
								genreLength = strlen(genre);
								if ( addSpace )
									++outputLength;
								outputLength += genreLength + 3;
							}
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
								if ( extendedLength > 0 )
								{
									byte* extText = (((byte*)e)+parameters->extendedEventNameOffset);
									// if additional space is enabled and there is some extended info to append
									if ( options->insertSpace && addSpace )
										*p++ = ' ';
									memcpy( p, (void*)*((dword*)extText), extendedLength );
									p += extendedLength;
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

bool DescriptionExtender_Init()
{
	dword eventTable;
	bool supported = FALSE;
	int index;

	// Look up the current firmware
	// Since Topfield release different firmware for different models with the same ID, we have to use some other easily
	// accessible unique value to allow users to crossflash their firmware (popular amongst 5100 and 5200 users).
	// TAP_GetCurrentEvent's address appears to be unique.
	for (index=0; index<sizeof(firmware)/sizeof(FirmwareDetail); ++index)
	{
		if ( _appl_version == firmware[index].firmwareVersion && (dword)TAP_GetCurrentEvent == firmware[index].getCurrentEvent  )
		{
			supported = TRUE;
			break;
		}
	}

	if ( !supported )
	{
		// Try and get some helpful information from users with unsupported firmware
		TYPE_File* fp;
		char buffer[512];

		TAP_Hdd_ChangeDir("..");
		TAP_Hdd_ChangeDir("..");
		TAP_Hdd_ChangeDir("ProgramFiles");
		TAP_Hdd_Delete( LOGFILE );
		if ( !TAP_Hdd_Exist( LOGFILE ) )
			TAP_Hdd_Create( LOGFILE, ATTR_NORMAL );
		if ( fp = TAP_Hdd_Fopen( LOGFILE ) )
		{
			memset( buffer, ' ', sizeof(buffer) );
			sprintf( buffer, "%d, 0x%04x, 0x%x, 0x%x\n", *sysID, _appl_version, TAP_GetCurrentEvent, TAP_EPG_GetExtInfo );
			buffer[strlen(buffer)] = ' ';
			TAP_Hdd_Fwrite( buffer, 1, sizeof(buffer), fp );
			TAP_Hdd_Fclose( fp );
		}

		sprintf( buffer,
			"Description Extender is not compatible with your firmware\n"
			"For an update, please contact the author for an update\n"
			"quoting your model type, firmware date and the following:\n"
			"%d, %04X, %X and %X\n"
			"A log file has been created in ProgramFiles",
			*sysID, _appl_version, TAP_GetCurrentEvent-0x80000000, TAP_EPG_GetExtInfo-0x80000000 );
		ShowMessage( buffer, 1000 );
		return FALSE;
	}
	else
	{
		// We need to know various settings without needing to use $gp
		// firmwareVersion is no longer needed, use it to pass the options structure
		firmware[index].firmwareVersion = (int)&options;
		((word*)GetFirmwareDetail)[1] = ((dword)(firmware+index) >> 16) & 0xffff;
		((word*)GetFirmwareDetail)[3] = (dword)(firmware+index) & 0xffff;

		// Replace TAP_EPG_GetExtInfo with a jump to our GetEventDescription function
		HackFirmware( (dword*)TAP_EPG_GetExtInfo, J(GetEventDescription) );
		HackFirmware( ((dword*)TAP_EPG_GetExtInfo)+1, NOP_CMD ); // MUST insert a nop after the jump

		// Set up TAP API wrappers that don't require the use of $gp to be called
		((dword*)FW_MemAlloc)[0] = J(TAP_MemAlloc);
		((dword*)FW_MemAlloc)[1] = NOP_CMD;
		// Useful for debugging
		((dword*)FW_sprintf)[0] = J(TAP_SPrint);
		((dword*)FW_sprintf)[1] = NOP_CMD;
		((dword*)FW_Print)[0] = J(TAP_Print);
		((dword*)FW_Print)[1] = NOP_CMD;
	}
}
