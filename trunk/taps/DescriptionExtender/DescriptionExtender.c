#include <tap.h>
#include <string.h>
#include <OPCodes.h>
#include <messagewin.h>


TAP_ID( 0x81243234 );
TAP_PROGRAM_NAME("DescriptionExtender 0.3 beta");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Makes longer programme descriptions");
TAP_ETCINFO("descriptions available to TAPs");


#include <TSRCommander.h>


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
	TAP_Print("Exiting DescriptionExtender\n");
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


void* FW_MemAlloc(dword size)
{
}
int FW_MemFree(const void *addr)
{
}
TYPE_TapEvent* FW_GetEvent( int svcType, int svcNum, int *eventNum )
{
}
int FW_Channel_GetFirstInfo( int svcType, TYPE_TapChInfo *chInfo )
{
}
int FW_Channel_GetNextInfo( TYPE_TapChInfo *chInfo )
{
}
void FW_Print(const void *fmt, ...)
{
}
void* AddressOfHack()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}


// Get the Topfield channel number, given a service id
bool FindChannel( int svcId, int* svcType, int* svcNum )
{
	if ( !svcType || !svcNum )
		return FALSE;

	for ( *svcType = 0; *svcType <= 1; ++*svcType )
	{
		TYPE_TapChInfo chInfo;
		*svcNum = 0;
		if ( FW_Channel_GetFirstInfo( *svcType, &chInfo ) )
		do
		{
			if ( chInfo.svcId == svcId )
			{
				return TRUE;
			}
			++*svcNum;
		}
		while ( FW_Channel_GetNextInfo( &chInfo ) );
	}

	return FALSE;
}


#define ADDR(addr, offset) ((word*)((word*)addr)+1+((offset)>>1))


byte* GetEventDescription( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		int svcType;
		int svcNum;
		if ( FindChannel( event->svcId, &svcType, &svcNum ) )
		{
			int eventNum;
			TYPE_TapEvent* allEvents;

			// Sanity check before we temporarily modify TAP_GetEvent
			if ( *ADDR(AddressOfHack(), 0) != 0x007f ||
				 *ADDR(AddressOfHack(), 0x4c) != 0x007f ||
				 *ADDR(AddressOfHack(), 0x58) != 0x008f ||
				 *ADDR(AddressOfHack(), 0x60) != 0x008f )
			{
				FW_Print( "TAP_GetEvent failed\n" );
				return result;
			}

			*ADDR(AddressOfHack(), 0) = 0;				// copy 0 characters of name
			*ADDR(AddressOfHack(), 0x4c) = 0x00ff;		// copy 255 characters of description
			*ADDR(AddressOfHack(), 0x58) = 0x000f;		// start of description=start of name
			*ADDR(AddressOfHack(), 0x60) = 0x000f;

			allEvents = FW_GetEvent( svcType, svcNum, &eventNum );
			if ( allEvents )
			{
				int i;
				for( i=0; i < eventNum; ++i )
				{
					if ( allEvents[i].evtId == event->evtId )
					{
						int length =  strlen(allEvents[i].eventName);
						if ( length > 127 )
						{
							// split description between description and extended info
							//result = FW_MemAlloc( length-126 );
							//strcpy( result, allEvents[i].eventName+127 );

							// entire description returned in extended info
							result = FW_MemAlloc( length+1 );
							strcpy( result, allEvents[i].eventName );
						}
						break;
					}
				}
				FW_MemFree( allEvents );
			}
			else
			{
				FW_Print("FW_GetEvent returned no events\n");
			}

			// restore original firmware values
			*ADDR(AddressOfHack(), 0) = 0x007f;
			*ADDR(AddressOfHack(), 0x4c) = 0x007f;
			*ADDR(AddressOfHack(), 0x58) = 0x008f;
			*ADDR(AddressOfHack(), 0x60) = 0x008f;
		}
	}

	return result;
}



dword eventFillSignature[] =
{
	0x2418007f,
	0x0302082b,
	0x10200003,
	0x24190118,
	0x03001025,
	0x24190118,
	0x00000000,
	0x02ff0019,
	0x00009812,
	0x02ffff21,
	0xffc4000f,
	0x02802825,
	0x0c00ffff,
	0x00403025,
	0x02802025,
	0x0c05ffff,
	0xffc02825,
	0x0c000dff,
	0x02802025,
	0x2419007f,
	0x0322082b,
	0x10200003,
	0xffc4008f,
	0x03201025,
	0xffc4008f
};


int TAP_Main(void)
{
	dword addr;

	TAP_Print("Starting DescriptionExtender\n");

	// Verify that the hack will work when it gets called
	addr = FindFirmwareFunction( eventFillSignature, sizeof(eventFillSignature), TAP_GetEvent, ((dword*)TAP_GetEvent) + 0x100 );
	if ( addr ==0 ||
		 *ADDR(addr, 0) != 0x007f ||
		 *ADDR(addr, 0x4c) != 0x007f ||
		 *ADDR(addr, 0x58) != 0x008f ||
		 *ADDR(addr, 0x60) != 0x008f )
	{
		ShowMessage( "Sorry, this TAP is not compatible with your firmware", 200 );
	}

	// Replace TAP_EPG_GetExtInfo with a jump to our GetEventDescription function
	HackFirmware( TAP_EPG_GetExtInfo, J(GetEventDescription) );

	// Set up TAP API wrappers that don't require the use of $gp to be called
	*((dword*)FW_MemAlloc) = J(TAP_MemAlloc);
	*((dword*)FW_MemFree) = J(TAP_MemFree);
	*((dword*)FW_GetEvent) = J(TAP_GetEvent);
	*((dword*)FW_Channel_GetFirstInfo) = J(TAP_Channel_GetFirstInfo);
	*((dword*)FW_Channel_GetNextInfo) = J(TAP_Channel_GetNextInfo);
	((dword*)FW_Print)[0] = J(TAP_Print);
	((dword*)FW_Print)[1] = NOP_CMD;
	// We also need to know the address of TAP_GetEvent without needing to use $gp
	((word*)AddressOfHack)[1] = ((dword)addr >> 16) & 0xffff;
	((word*)AddressOfHack)[3] = (dword)addr & 0xffff;

	return 1;
}
