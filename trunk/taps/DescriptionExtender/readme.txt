Description Extender 1.3
------------------------
Description Extender extends the EPG programme description text that is visible to TAPs by returning a longer programme description (up to 256 characters) in the EPG extended data. No special support for Description Extender is required by other TAPs to see this information.

There are 2 versions of Description Extender, the only difference being whether a space is placed between the programme description and extended information. Choose the version most appropriate to your EPG data:
Standard:	Adds a space.
CT:			Doesn't add a space. Programme description and extended information are contiguous.

This TAP currently only supports the following firmware
TF5000/5500:	13/09/2005
TF5010/5510:	05/10/2005
TF5100c:		15/03/2006, 12/09/2005, 04/05/2005, 19/04/2006
TF5100c MP:		15/03/2006, 05/10/2005, 24/04/2006
TF5100t:		15/03/2006, 12/09/2005, 19/04/2006
TF5100t MP:		15/03/2006, 04/10/2005, 24/04/2006
TF5200c:		13/09/2005
TF5800:			14/07/2006 (12.88), 08/12/2005 (12.25), 14/10/2005 (12.09), 12.05, 12.04

Procaster PVRs:
PC5101c/5102c:	15/03/2006*, 05/10/2005
PC5101t/5102t:	15/03/2006*, 05/10/2005
*These versions are crossflashed TF5100c and TF5100t firmwares as 05/10/2005 was the last Procaster firmware Topfield will provide.

If your firmware isn't supported, run the TAP and email me the text that's displayed on screen.

The TAP can be stopped using TAP Commander or TSR Commander. No configuration is necessary, just place your chosen version in the Auto Start directory.

Improbox, UKOZSurfer and Jag's EPG 3.0 and EPG Navigator all support Description Extender. 


History
-------
1.4		Support for 5800 14/07/2006 (12.88) firmware.
1.3		Support for latest 5100 firmware.
1.2		Support for TF5100 Masterpiece and Procaster models.
		Additional version for EPGs where programme description flows into the extended information.
1.1		Support for additional models and firmware versions.
1.0		Initial release.


For TAP Developers
------------------
A brief overview of the implementation:
The design of the TAP architecture prevents easy lengthening of the description field from its 128 limit without breaking all existing TAPs that request data from the EPG, which is probably the reason Topfield have not addressed the issue already.

The TAP replaces the default TAP_EPG_GetExtInfo function in the firmware with a call to a custom function. This function looks directly at the EPG tables to retrieve the full description plus the original extended information and returns a string allocated on the heap as you would normally expect from TAP_EPG_GetExtInfo.

Supporting Description Extender in your TAP
If Description Extender support isn't added to your TAP, users will see the first 127 characters of long descriptions duplicated when you show description and extended information. The following example source code will overcome this problem.


// Get the current event.
TYPE_TapEvent* event = TAP_GetCurrentEvent( chType, chNum );
// Get the extended info.
byte* extInfo = TAP_EPG_GetExtInfo( event );

// Test for the circumstances where Description Extender is running:
// We have extended information AND the normal description matches the 
// first 127 extended info characters.
if ( extInfo && strncmp(event->description, extInfo, 127) == 0 )
{
	// only process extended info
	TAP_Print( extInfo );
}
else
{
	// process both description and extended info
	TAP_Print( event->description );
	if ( extInfo )
		TAP_Print( extInfo );
}
// We must free the memory returned by TAP_EPG_GetExtInfo
if ( extInfo )
	TAP_MemFree( extInfo );


Simon Capewell
simon.capewell@gmail.com
