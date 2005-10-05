#include <tap.h>
#include <string.h>
#include "ini.h"
#include "file.h"

static	char	temp[MAX_STR_LEN];

void getDir(char* dir, char* fname)
{
	//char* buffer;
	char* tmp;
	//buffer=(char*) malloc(MAX_STR_LEN);
	//*buffer='\0';
	tmp = strrchr(fname, '\\');
	if (tmp!=NULL)
		strncpy(dir, fname, strlen(fname) - (strlen(tmp)-1));
}

void getFile(char* f, char* fname)
{
	char buffer[MAX_STR_LEN];
	char* dirend;

	//buffer=(char*) malloc(MAX_STR_LEN);
	buffer[0] = '\0';
	strcpy(buffer, fname);
	dirend = strrchr(buffer, '\\');

	if (dirend==NULL){
		dirend = buffer;
	} else {
		dirend += 1;
	}
	strcpy(f, strtok(dirend,"."));
}

char* getExt(char* fname)
{
	char* buffer;
	buffer=(char*) malloc(MAX_STR_LEN);
	return strcpy(buffer, strrchr(fname, '.'));	
}

/*
Parameters

AppName 
[in] Pointer to a null-terminated string containing the name of the section to which the 
string will be copied. If the section does not exist, it is created. The name of the section 
is case-independent; the string can be any combination of uppercase and lowercase letters. 

KeyName 
[in] Pointer to the null-terminated string containing the name of the key to be associated 
with a string. If the key does not exist in the specified section, it is created. 

Todo: If this parameter is NULL, the entire section, including all entries within the section, is deleted. 

String 
[in] Pointer to a null-terminated string to be written to the file. 

Todo: If this parameter is NULL, the key pointed to by the KeyName parameter is deleted. 

FileName 
[in] Pointer to a null-terminated string that specifies the name of the initialization file. 

Return Values
If the function successfully copies the string to the initialization file, the return value 
is nonzero.

If the function fails, or if it flushes the cached version of the most recently accessed 
initialization file, the return value is zero. To get extended error information, call 
GetLastError.

If the file doesn't exist the file will be created.

*/
int WritePrivateProfileString (char* AppName, char* KeyName, char* String, char* FileName)
{
	UFILE* input;
	UFILE* tmpfile;
	int secflag = 0, keyflag = 0, flag=0, fileflag=0;
	char buffer[MAX_STR_LEN];

	char tmpstr_[MAX_STR_LEN];
	char* tmpstr;

	char tmpfstr_[MAX_STR_LEN];
	char* tmpfstr = &tmpfstr_[0];;

	logMessage( _INFO, "WritePrivateProfileString start" );

	if ((AppName==NULL) || (KeyName==NULL) || (String==NULL))
	{
		SaveError(-5, "Missing Parameters");
		return 0;
	}
	if ((input=fopen(FileName, "r"))==NULL)
		//?file doesn't exist - try creating it
		if ((input=fopen(FileName, "w"))==NULL)
		{
			SaveError(-1, "Error opening file for read or write");
			return 0;
		}
	
	//tmpfstr=(char*) malloc(MAX_STR_LEN);
	*tmpfstr = '\0';

		getDir(tmpfstr, FileName);

	strcat(tmpfstr, "~");
	tmpfstr = tmpfstr + strlen(tmpfstr);  //point to end of string
	getFile(tmpfstr, FileName);
	strcat(tmpfstr, ".tmp");
	
	//tmpstr=(char*) malloc(MAX_STR_LEN);
	tmpstr = &tmpstr_[0];
	*tmpstr = '\0';

	if ((tmpfile=fopen(tmpfstr, "w"))==NULL)
	{
		SaveError(-3, "Unable to create temporary file for write");
		return 0;
	}
	
	while (fgets(buffer, MAX_STR_LEN, input) != NULL)
	{
		if (buffer[0] == '['){
			ExtractSection(buffer, tmpstr);
			if (strcasecmp(tmpstr, AppName)==0)
			{
				secflag=1;
			}else{
				if ((secflag==1) && (keyflag==0))
				{
					fprintf4(tmpfile, "%s=%s\r\n", KeyName, String);
					flag=1;
				}
				secflag=0;
				keyflag=0;
			}
			if (fileflag==1){
				fputs("\r\n", tmpfile);
			} else {
				fileflag=1;
			}
			fputs(buffer, tmpfile);
		} else if (buffer[0] == '\n'){
			//We handle CRLF ourselves
		} else {
			strcpy(tmpstr, buffer);
			strtok(tmpstr, "=");
			if (tmpstr!=NULL)
			{
				if ((strcasecmp(trim(tmpstr), KeyName)==0) && (secflag==1))
				{
	//TAP_Print("Adding key: '%s', Val: '%s'\r\n", KeyName, String);
					fprintf4(tmpfile, "%s=%s\r\n", trim(KeyName), String);
					flag=1;
					keyflag = 1;
				} else {
					if (strchr(buffer, '\n')==NULL){
						fprintf3(tmpfile, "%s\n",buffer);
					}else{
						fprintf3(tmpfile, "%s",buffer);
					}

				}
			}
		}
		
	}
	
	//TAP_Print("Adding section to temp...\r\n");
	if ((flag == 0) && (secflag!=1)){
	//TAP_Print("Adding app: '%s'\r\n", AppName);
		fprintf3(tmpfile, "\r\n[%s]\r\n", AppName);
	}
	if (((secflag==1) && (keyflag==0)) || (flag==0)){
	//TAP_Print("Adding key: '%s'\r\n", KeyName);
	//TAP_Print("Adding val: '%s'\r\n", String);
		fprintf4(tmpfile, "%s=%s\r\n", KeyName, String);
	}
	fclose(tmpfile);
	fclose(input);

	
	if ((input=fopen(FileName, "w"))==NULL)
	{
		SaveError(-2, "Error opening file for write");
		return 0;
	}
	if ((tmpfile=fopen(tmpfstr, "r"))==NULL)
	{
		SaveError(-4, "Unable to create temporary file for read");
		return 0;
	}
	//TAP_Print("Copying from temp to ini...\r\n");
	while (fgets(buffer, MAX_STR_LEN, tmpfile) != NULL)
	{
		fputs(buffer, input);
	}
	
	fclose(tmpfile);
	fclose(input);
	//remove(tmpfstr);
	//free(tmpfstr);
	//free(tmpstr);
	return 1;
}

int ExtractSection(char* string, char* output)
{
int	i, j, l, ptr1;

	l = (int)strlen(string);
	i = 0;
	while ( i < l && string[i] != '[' )
		i++;
	if ( i >= l )
		return 0;
	ptr1 = ++i;
	j = 0;
	while ( i < l && string[i] != ']' )
	{
		output[j++] = string[i];
		i++;
	}
	if ( i >= l )
		return 0;
	output[j] = '\0';
	return j;
}

int	GetSectionNames(char* buffer, UFILE* input)
{
	long	length=0, j=0;

	logMessage( _INFO, "GetSectionNames start" );

	while ( fgets(temp, sizeof(temp), input) != NULL )
	{
		if ( temp[0] == '[' )
		{
			if ( (length = ExtractSection(temp, &buffer[j])) != 0 )
			{
				j += length + 1;
				buffer[j] = '\0';
			}
		}
	}
	return length;
}

int GetPrivateProfileSectionNames(char* buffer, int bufflen, char* path)
{
UFILE	*input;
int		length;

	logMessage( _INFO, "GetPrivateProfileSectionNames start" );

	if ( ( input = fopen(path, "r")) == NULL )
	{
		SaveError(-1, "Error opening file for read");
		return 0;
	}
	length = GetSectionNames(buffer, input);
	fclose(input);
	return length;
}

int GetPrivateProfileString(char* section, char* key, char* def, char* buffer, int bufflen, char* path)
{
UFILE	*input;
char	*outptr, *stmp, *send;
int	l, i=0, length = 0, found = 0;
char	fullsection[53];
char	msg[MAX_MESSAGE_SIZE];

	logMessage( _INFO, "GetPrivateProfileString: starting" );

	sprintf(msg, "GetPrivateProfileString: section: %s, key: %s, path: %s", section, key, path);
	logMessage( _INFO, msg );

	outptr = buffer;
	if (def != NULL)
	{
		strcpy(buffer, def);
	}else{
		*buffer = '\0';
	}
	if (path == NULL)
	{
		logMessage( _INFO, "GetPrivateProfileString: Missing parameters" );
		SaveError(-5, "Missing Parameters");
		return 0;
	}

	if ( ( input = fopen(path, "r")) == NULL )
	{
		logMessage( _INFO, "GetPrivateProfileString: Error opening file for read" );
		SaveError(-1, "Error opening file for read");
		return 0;
	}

	if ( section == NULL )
	{
		logMessage( _INFO, "GetPrivateProfileString: Section is NULL" );
		length = GetSectionNames(outptr, input);
		fclose(input);
		return length;
	}
	fullsection[0]='\0';
	strcat(fullsection, "[");
	strcat(fullsection, section);
	strcat(fullsection, "]");

	if ( key == NULL )
	{
		sprintf(msg, "GetPrivateProfileString: key = NULL, looking for section: /%s/", fullsection);
		logMessage( _INFO, msg );

		while ( fgets(temp, sizeof(temp), input) != NULL )
		{
			sprintf(msg, "GetPrivateProfileString: Got next line: /%s/", temp);
			logMessage( _INFO, msg );

			if ((stmp=strchr(temp,'[')) == NULL)
				continue;
			if ((send=strchr(stmp,']')) == NULL)
				continue;

			send[1] = '\0';

			sprintf(msg, "GetPrivateProfileString: Comparing: /%s/", stmp);
			logMessage( _INFO, msg );

			if (strcasecmp(stmp, fullsection)==0)
			{
				logMessage( _INFO, "GetPrivateProfileString: stmp matches fullsection" );

				while ( fgets(temp, sizeof(temp), input) != NULL )
				{
					if ( temp[0] == '[' )
						break;
					if ( temp[0] ==  ';')
						continue;
					temp[strlen(temp) - 2] = '\0';   //\r\n
					if ( strlen(temp) == 0 )
						continue;
					strtok(temp, "=");
					l = (int)(strlen(temp) + 1);
					memcpy(outptr, temp, l);
					outptr += l;
					length += l;
					memset(outptr, '\0', 1);
				}
				fclose(input);
				return length;
			}
		}
		fclose(input);
		SaveError(-6, "Item not found");
		return 0;
	}

	logMessage( _INFO, "GetPrivateProfileString: Key not null" );
	while ( fgets(temp, sizeof(temp), input) != NULL )
	{
		sprintf(msg, "GetPrivateProfileString: Got line: /%s/", temp);
		logMessage( _INFO, msg );

		if ((stmp=strchr(temp,'[')) == NULL)
			continue;
		if ((send=strchr(stmp,']')) == NULL)
			continue;

		send[1] = '\0';

		sprintf(msg, "GetPrivateProfileString: Comparing: /%s/", stmp);
		logMessage( _INFO, msg );

		//maybe get rid of newline? stmp[strlen(stmp) - 1] = '\0';
		//stmp[strlen(stmp) - 1] = '\0

		if ( strcasecmp(stmp, fullsection)==0)
		{
			while ( fgets(temp, sizeof(temp), input) != NULL )
			{
				temp[strlen(temp) - 2] = '\0';   //\r\n
				if ( strlen(temp) == 0 )
					continue;
				if ( temp[0] == '[' )
					break;
				if ( temp[0] ==  ';')
					continue;
				if ( (outptr = (char *)strchr(temp, '=')) == (char *)NULL )
				{ 
					break;
				}
				*outptr = '\0';
				if ( !strcasecmp(trim(temp), key) )
				{
					strcpy(buffer, ++outptr);
					length = (int)strlen(buffer);
					break;
				}
			}
			fclose(input);
			return length;
		}
	}
	fclose(input);
	return 0;
}

char* trim (char s[MAX_STR_LEN])
{
	int n;
	static char tmpstr[MAX_STR_LEN];

	for (n=0; n<=((int) strlen(s)-1); n++)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	strcpy(&tmpstr[0], &s[n]);

	for (n=(int)(strlen(tmpstr)-1); n>=0; n--)
		if (tmpstr[n] != ' ' && tmpstr[n] != '\t' && tmpstr[n] != '\n')
			break;
	tmpstr[n+1] = '\0';
	return &tmpstr[0];
	
}

char* strtok(char *s, const char *delim)
{
    const char *spanp;
    int c, sc;
    char *tok;
    static char *last;

    if (s == NULL && (s = last) == NULL)
	return (NULL);

    // Skip (span) leading delimiters (s += strspn(s, delim), sort of).

cont:
    c = *s++;
    for (spanp = delim; (sc = *spanp++) != 0;) {
	if (c == sc)
	    goto cont;
    }

    if (c == 0) {			//* no non-delimiter characters
	last = NULL;
	return (NULL);
    }
    tok = s - 1;

    // * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
    // * Note that delim must have one NUL; we stop if we see that, too.

    for (;;) {
	c = *s++;
	spanp = delim;
	do {
	    if ((sc = *spanp++) == c) {
		if (c == 0)
		    s = NULL;
		else
		    s[-1] = 0;
		last = s;
		return (tok);
	    }
	} while (sc != 0);
    }
    // NOTREACHED
}

void initIni(int fl)
{
	initFile(fl);
}

