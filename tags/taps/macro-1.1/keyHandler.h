#ifndef __KEYHANDLER_H
#define __KEYHANDLER_H

//max number of macro definitions
#define MAXMACROS 100
//max text length of a key's name
#define MAXKEYNAME	25
//max number of keys assignable to a macro
#define MAXMACROKEYS 50
//allowed delimiters between keynames for a macro
#define MACRODELIMITERS  ","

enum mType {
	keyType,
	menuType,
	delayType
};

typedef struct _macroKeyNode {
	int		macroKeyType;
	dword	macroVal;
} MACRONODE;

//KEYNODE - each macro key is described by one of these blocks
typedef struct _keyNode{
	char		keyName[MAXKEYNAME];
	dword		keyVal;
	int			keyCount;
	MACRONODE	keyMacroKeys[MAXMACROKEYS];
} KEYNODE;


void initSubStateTable();
dword getSubstateVal(char *menu);
void loadOptions(void);
int saveOptions(void);
int checkKey(dword key);
int ExecuteMenu();
void ExecuteMacro(bool initial);
void freeFunctionKeys();
void freeKeyList();
void dumpKeyList();
void addKey(dword val, int valType);
char *getFunctionState();
char *getLoggingLevel(char *str);
int updateFunctionKeys( dword mk );
void dumpFunctionKeys();


#endif
