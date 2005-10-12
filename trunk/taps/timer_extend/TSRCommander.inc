#define	COLOR_CustomYellow	RGB(28,28,7)
typedef struct
{
	char	landingzone[256];
	char	signature[20];
	char	*TAPName;
	bool	ExitTAP;
	bool	HasConfigDialog;
	bool	ShowConfigDialog;
	dword	FutureExtensions[64];
} TYPE_TSRCommander;

static TYPE_TSRCommander TSRCommander;

void TSRCommanderConfigDialog (void);
bool TSRCommanderExitTAP (void);

void TSRCommanderInit (int Delay, bool TAPhasConfigDialog)
{
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
	TSRCommander.HasConfigDialog = TAPhasConfigDialog;		// set to TRUE if you have a config-dialog
	TSRCommander.ShowConfigDialog = FALSE;				// dont show config-dialog at startup
	for ( i=0 ; i < 64 ; i++ )					// deleting extension
	{
		TSRCommander.FutureExtensions[i] = 0;
	}
	if (Delay > 0)
	{
		char str[80];						// stores startup-message
		int rgn;						// stores rgn-handle for osd
		dword w;						// stores width of message-text

		strcpy( str, (char*) __tap_program_name__ );		// build startup-message
		strcat( str, " has started" );
		rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );	// create rgn-handle
		w = TAP_Osd_GetW( str, 0, FNT_Size_1926 ) + 10;		// calculate width of message
		if (w > 720) w = 720;					// if message is to long
		TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 36,	// draw background-box for border
				COLOR_User1 );
		TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, str,	// show message
			     COLOR_User1, COLOR_CustomYellow, 0,
			     FNT_Size_1926, FALSE, ALIGN_CENTER);
		TAP_Delay(Delay);					// show it for 1.5 secounds
		TAP_Osd_Delete(rgn);					// release rgn-handle
	}
}

void TSRCommanderErase (void)
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

void TSRCommanderWork (void)
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
