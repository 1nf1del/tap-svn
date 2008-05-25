#ifndef FBLIB_FS_H
#define FBLIB_FS_H

#include "..\libFireBird.h"

#define MAX_SORT 256

bool FileSelectorInitialize(char *, dword);
bool FileSelectorHandle(dword *, char *);
void FileSelectorFill(void);
char *FileSelectorFilterGet(char *, int *);

#endif
