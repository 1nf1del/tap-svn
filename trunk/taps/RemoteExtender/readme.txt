Remote Extender is a TAP that, whilst running, patches the TF5800 firmware to make the EVT_KEY event send the internal remote keycodes in param2. RawKey.h contains definitions of these codes. If the value of param2 is 0 then RemoteExtender is either not running, or does not have an internal keycode available (usually due to the keypress being caused by TAP_GenerateEvent) and param1 should be processed instead.

Suggested uses for this are:
On the TF5800, for distinguishing between V+,V-, P+,P- and the arrow buttons and TV/Radio and List.
On all models, for providing extra functions for users of programmable remotes.


Simon Capewell
simon.capewell@gmail.com

