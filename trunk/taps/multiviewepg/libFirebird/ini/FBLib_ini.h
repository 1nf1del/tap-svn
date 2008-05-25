#ifndef FBLIB_INI_H
#define FBLIB_INI_H

#include "tap.h"

extern dword            LangNrStrings;
extern dword            *LangStringPtr;
extern char             *LangStrings;
extern char *INIBuffer, *INICommentBuffer;

extern dword BufferSize;

char *stricstr(const char *, const char *);
void INIFindStartEnd(char *, char **, char **, dword);

#endif
