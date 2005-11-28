Logs timestamps to track when incorrect data is received

Details

This tap compares the current time/date with the time/date from the previous iteration. If the difference
is 10 minutes or more the following data is stored in the 'timemon.log' file:

'Previous time/date, Signal Level, Signal Quality, total number of timers' 'Current time/date, Signal Level, Signal Quality, total number of timers'

The size of the 'timemon.log' file is 5k and it can store up to 62 events.

Pressing 'Exit' will show how many events have been stored and pressing the 'Blue' function key will exit the tap.

Warning! - This tap reads/writes to the HDD so it is advisable that all recordings are backed up before using this tap.