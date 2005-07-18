#ifndef __MACRO_H
#define __MACRO_H

#define PROGRAMNAME "macro"
#define VERSION "v1.0"

//define reasonable start time in minutes
#define REASONABLE_START_TIME  2
//time between function key press and the macro key (in 10msecs)
#define FUNCTIONKEY_TIME  2*100
//max number of macro definitions
#define MAXMACROS 100
//max text length of a key's name
#define MAXKEYNAME	25
//max number of keys assignable to a macro
#define MAXMACROKEYS 50
//name of the keymap file
#define KEYMAPFILE "macro.ini"
//allowed delimiters between keynames for a macro
#define MACRODELIMITERS  ","

//FunctionState:  one of AlwaysOn, UseKeyOnce, UseKeyEveryTime - V1 only AlwaysOn is supported
//   AlwaysOn: no need to press function key to activate macros
//   UseKeyOnce: press the function key once and macros are enabled from then on
//   UseKeyEveryTime: function key must be pressed every time a macro is required

enum _eFState {
	_AlwaysOn = 1, 
	_UseKeyOnce, 
	_UseKeyEveryTime
};


//phases used in EventHandler
enum inputPhases {
	_initialState,
	_functionKeyPressed,
	_executingMacro,
	_executingMacroMenu,
	_menuActive
};

#endif
