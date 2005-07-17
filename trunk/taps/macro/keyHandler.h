#ifndef __KEYHANDLER_H
#define __KEYHANDLER_H

enum mType {
	keyType,
	menuType
};

void initSubStateTable();
dword getSubstateVal(char *menu);
void loadFunctionKeys();
int checkKey(dword key);
int ExecuteMenu();
void ExecuteMacro(bool initial);
void freeFunctionKeys();

#endif
