Description Extender 1.1
------------------------

Description Extender extends the EPG programme description text that is visible to TAPs by returning a longer programme description (up to 256 characters) in the EPG extended data which is normally blank in some countries. No special support for Description Extender is required by other TAPs to see this information.

This TAP currently only supports the following firmware
TF5000/5500:	13/09/2005
TF5010/5510:	05/10/2005
TF5100c:		15/03/2006, 12/09/2005, 04/05/2005
TF5100t:		15/03/2006, 12/09/2005
TF5200c:		13/09/2005
TF5800:			08/12/2005 (12.25), 14/10/2005 (12.09), 12.05, 12.04
PC5101c/5102c:	05/10/2005

If your firmware isn't supported, run the TAP and email me the text that's displayed on screen.

The TAP can be stopped using TAP Commander or TSR Commander. No configuration is necessary, just place the TAP in your Auto Start directory.

History
-------
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
