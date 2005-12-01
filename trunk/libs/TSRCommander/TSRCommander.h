// Improved version of TSRCommander.inc
// Include TSRCommander.h at the top of your TAP's main source file after <tap.h>
// implement TSRCommanderConfigDialog to create your configuration menu or leave the function body
// empty if there is no configuration
// Implement TSRCommanderExitTAP to clean up when your TAP exits. Return TRUE to allow the TAP to
// close, or FALSE if
// In your TAP_Main, return TAP_TSR | TAP_HASCONFIG to make TSRCommander display a config menu item

#ifndef __TSRCOMMANDER_H
#define __TSRCOMMANDER_H


// Prototypes for TSR Commander 
// These *must* be implemented by a TSR Commander compliant TAP 
void TSRCommanderConfigDialog();
bool TSRCommanderExitTAP();

// Flags returned to TSRCommander from TAP_Main
// Intentionally starting at 2 for backwards compatibilty with old TAP_Mains
#define TAP_HASCONFIGMENU 2

typedef struct
{
	char	landingzone[256];
	char	signature[20];
	char	*TAPName;
	dword	ExitTAP;
	dword	HasConfigDialog;
	dword	ShowConfigDialog;
	dword	FutureExtensions[64];
} TYPE_TSRCommander;

static TYPE_TSRCommander TSRCommander;


void TSRCommanderErase()
{
	int i;

	for ( i=0 ; i < 256 ; i++ )					// erase landingzone
	{
		TSRCommander.landingzone[i] = ' ';
	}
	for ( i=0 ; i < 20 ; i++ )					// erase signature
	{
		TSRCommander.signature[i] = ' ';
	}
}


// Prototype for TAP
dword TSR_EventHandler(word event, dword param1, dword param2 );

// Main event handler
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_IDLE )
	{
		if (TSRCommander.ShowConfigDialog)
		{
			TSRCommander.ShowConfigDialog = FALSE;
			TSRCommanderConfigDialog();
		}
		if (TSRCommander.ExitTAP == TRUE)
		{
			TSRCommander.ExitTAP = FALSE;
			if (TSRCommanderExitTAP() == TRUE)
			{
				TSRCommanderErase();				// destroy TSRCommander-signature
				TAP_Exit();					// usual exit-call for TSR-TAP
			}
		}
	}
	
	// Call TAP's event handler
	return TSR_EventHandler( event, param1, param2 );
}

// Prototype for TAP
int TSR_Main();

// Main entry point
int TAP_Main()
{
	int result;
	int i;

	for ( i=0 ; i < 256 ; i++ )					// preparing landingzone for faster RAM-scann
	{
		TSRCommander.landingzone[i] = 'J';
	}
	TSRCommander.signature[0] = 'J';				// set signature-value
	TSRCommander.signature[1] = 'u';
	TSRCommander.signature[2] = 'd';
	TSRCommander.signature[3] = 'y';
	TSRCommander.signature[4] = 's';
	TSRCommander.signature[5] = '_';
	TSRCommander.signature[6] = 'S';
	TSRCommander.signature[7] = 'i';
	TSRCommander.signature[8] = 'g';
	TSRCommander.signature[9] = 'n';
	TSRCommander.signature[10] = 'a';
	TSRCommander.signature[11] = 't';
	TSRCommander.signature[12] = 'u';
	TSRCommander.signature[13] = 'r';
	TSRCommander.signature[14] = 'e';
	TSRCommander.signature[15] = '_';
	TSRCommander.signature[16] = 'X';
	TSRCommander.signature[17] = '7';
	TSRCommander.signature[18] = '0';
	TSRCommander.signature[19] = 'E';
	TSRCommander.TAPName = (char *) __tap_program_name__;		// set tap-name pointer
	TSRCommander.ExitTAP = FALSE;					// dont stop tap on startup
	TSRCommander.HasConfigDialog = FALSE;			// set to TRUE if you have a config-dialog
	TSRCommander.ShowConfigDialog = FALSE;				// dont show config-dialog at startup
	for ( i=0 ; i < 64 ; i++ )					// deleting extension
	{
		TSRCommander.FutureExtensions[i] = 0;
	}

	// Call TAP's Main function.
	// Return value dictates whether TSR Commander shows a config menu item
	result = TSR_Main();
	TSRCommander.HasConfigDialog = (result & TAP_HASCONFIGMENU) != 0;
	return result > 0;
}


// Redefine normal TAP entry points become TSR_*
#define TAP_EventHandler TSR_EventHandler
#define TAP_Main TSR_Main


#endif
