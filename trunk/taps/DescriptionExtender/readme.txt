DescriptionExtender 0.3 Beta 

The usual beta caution applies: don't try this whilst you are recording something important. It may crash your Toppy. I've only tested on firmware version 12.09.

This TAP extends the EPG programme description text that is visible to TAPs by returning a longer programme description (up to 256 characters) in the EPG extended data which is normally blank in the UK. No special coding is required by other TAPs to support this TAP, other than the use of the standard TAP_EPG_GetExtInfo API call.

The TAP can be stopped using TSRCommander.

The design of the TAP architecture prevents easy lengthening of the description field from its 128 limit without breaking all existing TAPs that request data from the EPG, which is probably the reason Topfield have not addressed the issue already.

Brief overview of the implementation
The TAP replaces the default TAP_EPG_GetExtInfo function in the firmware with a call to a custom function. This new function temporarily hacks the TAP_GetEvent function to fill the eventName and description with a 256 character description. It then searches for the relevant TYPE_TapEvent in the list of results and returns a string allocated on the heap as you would normally expect from TAP_EPG_GetExtInfo.

Simon Capewell
simon.capewell@gmail.com
