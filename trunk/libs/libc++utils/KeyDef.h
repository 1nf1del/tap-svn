#ifndef libcpputils_keydef_h
#define libcpputils_keydef_h
#include "tap.h"
class KeyDef
{
public:
	KeyDef(dword key = 0xFFFFFFFF, dword extKey = 0xFFFFFFFF);
	~KeyDef(void);

	dword key;
	dword extKey;
};
#endif