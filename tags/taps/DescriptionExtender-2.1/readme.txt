Description Extender 2.1
------------------------
Description Extender extends the EPG programme description text that is visible to TAPs by returning a longer programme description (up to 256 characters) in the EPG extended data. No special support for Description Extender is required by other TAPs to see this information. Improbox, Surfer, Jag's EPG 3.0 and EPG Navigator all support Description Extender. 

There is now an options screen for configuring Description Extender. This can only be accessed using TAP Commander or TSR Commander. The 2 available options are:
Add a space between the programme description and extended information
Add the broadcast genre (on TF5800 firmware 12.88 or greater)

To configure or stop the TAP you must use TAP Commander or TSR Commander.

Description Extender currently supports the following Topfield models:
TF5000t, TF5000/5500, TF5010/5510, TF5100c, TF5100c MP, TF5100t, TF5100t MP, TF5200c, TF5800
and Procaster models:
PC5101c/5102c, PC5101t/5102t

If your firmware isn't supported, run the TAP and email me the DescriptionExtenderLog.txt file in ProgramFiles.

History
-------
2.1		Auto detect new firmware.
2.0		Support for loads of new firmware. Added an options screen.
1.6		Support for 5800 19/09/2006 (13.26) firmware. Fixes missing genre information.
1.5		Fixes a bug in extended information support for 5800 12.88 firmware.
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
