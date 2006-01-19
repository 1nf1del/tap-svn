Note that the array class tested properly only for pointer types so far - seems to have a problem in copy construction when applied to value types under some circumstances (string types have seemed to be ok). No bug obvious though.

Timer/Timers needs the following enhancements:

Rename a timer when you change its start point to include/excude a program
Ability to split a timer in two to unschedule a program that occurs in the middle of a timer that covers 3+ programs
Ability to merge two timers when scheduling a program that fills the gap between two programs on the same channel.
The ability to detect that an overlap problem is due to padding only and remove the padding to allow the program to be scheduled (possibly replace it if a cancellation allows it)