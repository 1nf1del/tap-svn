#ifndef libcpputils_directoryutils
#define libcpputils_directoryutils
#include <string.h>
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "file.h"


string GetCurrentDirectory();
bool ChangeDirectory(const string& newDirectory);
UFILE* OpenFile(char* szFileName, char* szMode);


#endif