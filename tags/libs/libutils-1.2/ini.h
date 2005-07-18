#ifndef _UTILS_INI
#define _UTILS_INI

/*max ini key value length */
#define MAX_INI 256

char *strtok(char *, const char *);
int ExtractSection(char*, char*);
char* trim (char s[]);

int GetPrivateProfileString(char*, char*, char*, char*, int, char*);
int GetPrivateProfileSectionNames(char* buffer, int bufflen, char* path);
int GetLastErrorCode();
char* GetLastErrorString();
void initIni(int fl);

/*

int GetPrivateProfileString(
  char* lpAppName,
  char* lpKeyName,
  char* lpDefault,
  char* lpReturnedString,
  int nSize,
  char* lpFileName
);

Parameters
lpAppName 
[in] Pointer to a null-terminated string that specifies the name of the section containing 
the key name. If this parameter is NULL, the GetPrivateProfileString function copies all 
section names in the file to the supplied buffer.

lpKeyName 
[in] Pointer to the null-terminated string specifying the name of the key whose associated 
string is to be retrieved. If this parameter is NULL, all key names in the section specified 
by the lpAppName parameter are copied to the buffer specified by the lpReturnedString parameter. 

lpDefault 
[in] Pointer to a null-terminated default string. If the lpKeyName key cannot be found in the 
initialization file, GetPrivateProfileString copies the default string to the lpReturnedString
buffer. This parameter cannot be NULL. 
Avoid specifying a default string with trailing blank characters. The function inserts a null 
character in the lpReturnedString buffer to strip any trailing blanks.

lpReturnedString 
[out] Pointer to the buffer that receives the retrieved string. 

nSize 
[in] Size of the buffer pointed to by the lpReturnedString parameter, in TCHARs. 

lpFileName 
[in] Pointer to a null-terminated string that specifies the name of the initialization file. 
If this parameter does not contain a full path to the file, the system searches for the file 
in the Windows directory. 

Return Values
The return value is the number of characters copied to the buffer, not including the 
terminating null character.

If neither lpAppName nor lpKeyName is NULL and the supplied destination buffer is too small 
to hold the requested string, the string is truncated and followed by a null character, and 
the return value is equal to nSize minus one.

If either lpAppName or lpKeyName is NULL and the supplied destination buffer is too small to 
hold all the strings, the last string is truncated and followed by two null characters. In 
this case, the return value is equal to nSize minus two.


int GetPrivateProfileSection(
  [in] char* lpAppName,
  [out] char* lpReturnedString,
  [in] int nSize,
  [in] char* lpFileName
);

Parameters
lpAppName 
[in] Pointer to a null-terminated string specifying the name of the section in the 
initialization file. 

lpReturnedString 
[out] Pointer to a buffer that receives the key name and value pairs associated with the 
named section. The buffer is filled with one or more null-terminated strings; the last 
string is followed by a second null character. 

nSize 
[in] Size of the buffer pointed to by the lpReturnedString parameter, in TCHARs. 

lpFileName 
[in] Pointer to a null-terminated string that specifies the name of the initialization file. 
If this parameter does not contain a full path to the file, the system searches for the file 
in the Windows directory. 

Return Values
The return value specifies the number of characters copied to the buffer, not including the 
terminating null character. If the buffer is not large enough to contain all the key name and 
value pairs associated with the named section, the return value is equal to nSize minus two.


int GetPrivateProfileSectionNames(
  [out] char* lpszReturnBuffer,
  [in] int nSize,
  [in] char* lpFileName
);

Parameters

lpszReturnBuffer 
[out] Pointer to a buffer that receives the section names associated with the named file. 
The buffer is filled with one or more null-terminated strings; the last string is followed 
by a second null character. 

nSize 
[in] Size of the buffer pointed to by the lpszReturnBuffer parameter, in TCHARs. 

lpFileName 
[in] Pointer to a null-terminated string that specifies the name of the initialization file. 
If this parameter is NULL, the function searches the Win.ini file. If this parameter does not 
contain a full path to the file, the system searches for the file in the Windows directory. 

Return Values
The return value specifies the number of characters copied to the specified buffer, not 
including the terminating null character. If the buffer is not large enough to contain all 
the section names associated with the specified initialization file, the return value is 
equal to the size specified by nSize minus two.

*/

int WritePrivateProfileString (char*, char*, char*, char*);

/*
Parameters

lpAppName 
[in] Pointer to a null-terminated string containing the name of the section to which the 
string will be copied. If the section does not exist, it is created. The name of the section 
is case-independent; the string can be any combination of uppercase and lowercase letters. 

lpKeyName 
[in] Pointer to the null-terminated string containing the name of the key to be associated 
with a string. If the key does not exist in the specified section, it is created. If this 
parameter is NULL, the entire section, including all entries within the section, is deleted. 

lpString 
[in] Pointer to a null-terminated string to be written to the file. If this parameter is NULL,
the key pointed to by the lpKeyName parameter is deleted. 

lpFileName 
[in] Pointer to a null-terminated string that specifies the name of the initialization file. 

Return Values
If the function successfully copies the string to the initialization file, the return value 
is nonzero.

If the function fails, or if it flushes the cached version of the most recently accessed 
initialization file, the return value is zero. To get extended error information, call 
GetLastError.

int WritePrivateProfileSection(
  char* lpAppName,
  char* lpString,
  char* lpFileName
);

Parameters

lpAppName 
[in] Pointer to a null-terminated string specifying the name of the section in which data is 
written. This section name is typically the name of the calling application. 

lpString 
[in] Pointer to a buffer containing the new key names and associated values that are to be 
written to the named section. This string is limited to 65,535 bytes. 

lpFileName 
[in] Pointer to a null-terminated string containing the name of the initialization file. If 
this parameter does not contain a full path for the file, the function searches the Windows 
directory for the file. If the file does not exist and lpFileName does not contain a full 
path, the function creates the file in the Windows directory. The function does not create a 
file if lpFileName contains the full path and file name of a file that does not exist. 

Return Values
If the function succeeds, the return value is nonzero.

If the function fails, the return value is zero. To get extended error information, call 
GetLastError.

*/
#endif  // _UTILS_INI
