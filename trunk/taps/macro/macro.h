#ifndef __MACRO_H
#define __MACRO_H

#define PROGRAMNAME "macro"
#define VERSION "v1.11"

//define reasonable start time in minutes
#define REASONABLE_START_TIME  2
//time between function key press and the macro key (in 10msecs)
//#define FUNCTIONKEY_TIME  2*100
//name of the keymap file
#define KEYMAPFILE "macro.ini"

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
	_recordingMacro,
	_recordingMacroEnd,
	_menuActive,
	_listActive,
};


//menu options - these should match the menu lines in DisplayMenu
enum menuOptions {
	_optCreateMacro = 1,
	_optListMacros,
	_optTestMacro,
	_optFunctionKey,
	_optFunctionKeyTime,
	_optFunctionState,
	_optLoggingLevel,
	_optRecordDelays,
	_optSaveOptions,
	_optExitTAP,
	_optPassThrough
};

//logging levels - how much to log
enum loggingLevels {
	_logNone,
	_logSome,
	_logMore,
	_logAll
};

#endif
