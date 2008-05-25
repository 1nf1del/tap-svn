MultiViewEPG

What is it?

A TAP that provides linear and grid style EPG listing modes, Timerlist, RSS News reader, Favourites manager for perlTGD and
provides a more user friendly alternative to the other existing guide TAPs. One of the key features is the ability to edit
conflicting timers when attempting to set a new timer. This enables you to adjust the timers to fit rather than the usual message
of can't set timer because it conflicts. Other key features relating to the guide is the ability to adjust an active recording to
cover the next event, ability to remove an event from an active recording if it hasn't started and many more features.


How do I install it?

Copy the file multiviewepg.tap to either the ProgramFiles or Auto Start directory on your PVR.

** Please run the TAP from program files first to ensure that it causes no problems with
your current setup.

How do I use it?

The default activation key is FAV. On its first load the TAP will need to gather the EPG data
from unit so it may take some time ( No more than 10 seconds ).

Once launched and data has been cached then you can navigate up and down selecting events.
To view the extended information press (i) and (exit) or (i) to close. In the linear EPG mode
all recordings will have a red circle next to them, orange for partial ( Configurable ) and an alternating
circle if it is currently recording. In Grid mode a red line will be under all shows that have timers or are
currently recording. When listing by channel press Forward and Rewind to move 
to the next and previous channel. When listing by name press FFW to navigate to next starting 
letter i.e. A will move first event starting with B etc... For Grid EPG see the keys list
below for there actions.

In the Linear EPG to perform a search press the fav key to activate the keyboard. Using the volume and channel
keys pressing Ok to select characters type the name of the show you are after. You will notice that
once you start typing and not press a key for a second ( configurable ) that it will search for 
the shows starting with the characters you have chosen. Currently there is a restriction on the
searches being case sensitive. See keys and there actions below for usage.

Can I reload the data?

Yes. Press (recall) when in either guide mode and the EPG will be scanned and sorted again. This TAP doesn't load events
as they are found or changed so in future version there will be an option to reload ever X hours to help with this.

How do I configure this TAP?

Use TSR Commander or TAPCommander to launch the configuration menu.

How much RAM does it Take up?

This TAP will use a maximum of around 4MB of RAM. This amount of RAM used will vary as the TAP only uses the 
memory that it needs to store the events so less events = less RAM etc..

RSS News - What is the filename and format?

Currently the name of the file is hard coded to rssnews.dat and the file must be in the ProgramFiles directory.

The format of this file is as follows

FeedName<TAB>FeedDate<TAB>FeedTitle<TAB>FeedDescription<NEWLINE>

FeedName - Maximum of 100 Characters
FeedDate - Format of YYYY-MM-DD
FeedTitle - Maximum of 100 Characters
FeedDecription = Maximum of 500 Characters

Is the source avaliable?

Currently the source is only avaliable on request until the TAP is deemed stable at which point the source will
be provided. At this stage the Source will be release once the Beta phase is complete.

Keys and there actions

Linear Guide

Channel Up/Down - Navigation up/down
Volume Up/Down - Navigation page up/down
OK - Change to events channel
Exit - Close guide
Record - Set timer/Edit timer
Recall - Reload data
Info - View extended information
White - Delete timer
Yellow - Search for next episode
Green - Search description
Red - Search title
Next - When keyboard is active i.e. searching press the Next key for the next search result
TvRadio - Display next timers when using PIG
Teletext - Edit timer
FAV - Add event to favourites list for perlTGD

Extended Information

Channel Up/Down - Next/Previous page of extended information
Info - Close extended information
Exit - Close extended information

Keyboard

Channel Up/Down - Navigation up/down
Volume Up/Down - Navigation sideways
Play - Change case
Record - Save and close
Exit - Cancel/Close
Forward - Space
Rewind - Delete character
White - Delete character

Conflict screen

OK - Edit selected timer
White - Delete selected timer
Exit - Cancel new timer

Menu

Channel Up/Down - Navigation up/down
Volume Up/Down - Change value
OK - Change value
Exit - Close menu
Record - Save settings

Timer Editing

Channel Up/Down - Navigation up/down
Volume Up/Down - Adjust values
OK - Edit timer name / Select OK or Cancel
Exit - Cancel editing of timer

Timerlist

Channel Up/Down - Navigation Up/Down
Volume Up/Down - Next day/Previous day
Ok - Edit selected timer
White - Delete selected timer
Exit - Close guide
TvRadio - Show all timers

RSS News

Channel Up/Down - Navigation Up/Down
Exit - Close screen
Recall - Reload rssnews.dat file

Grid EPG

Channel Up/Down - Navigation up/down
Volume Up/Down - Navigation page up/down
OK - Change to events channel
Exit - Close guide
Record - Set timer/Edit timer
Recall - Reload data
Info - View extended information
White - Delete timer
Red - -24 hrs
Blue - +24 hrs
Green - Prev page
Yellow - Next page
Teletext - Edit timer
FAV - Add event to favourites list for perlTGD
Number keys 1 - 5 - Change listing range to 1 = 1 Hour, 3 = 3 hour etc..


Files created and used by Jim's Guide

multiviewepg.ini in ProgramFiles/Settings/multiviewepg.ini

Known Limitations

Gathering of EPG data not friendly for over the air guides ( May look at a better way of caching eventually )
No skinning abilities ( see to do list )
Only in english at the moment ( see to do list )

Credits

Bellissimo for all his help with the description extender support and memory usage.
All the developers whose code I have used. Bellissimo, tonymy01, UK Project team members etc..
All developers that have released there source code to there TAPs.
Everyone associated with creating libFirebird, keep up the great work.

