#ifndef _UTILS_FILE
#define _UTILS_FILE

#include <tap.h>

#ifdef  __cplusplus
extern "C" {
#endif

//block read size
#define BUFSIZ		2048
#define EOF		(-1)
#define OPEN_MAX	20	//max files open at once

typedef struct _iobuf {
	int	cnt;		/* characters left */
	int	size;		/* size of file in bytes */
	char	*ptr;		/* next character position */
	char	*base;		/* location of buffer */
	int	flag;		/* mode of file access */
	TYPE_File *fd;		/* TAP file descriptor */
} UFILE;

#define write(a, b, c)   TAP_Hdd_Fwrite(b, c, 1, a)
#define read(a, b, c)    TAP_Hdd_Fread(b, c, 1, a)
int remove(char *name);

#include "Utils.h"

extern UFILE _iob[OPEN_MAX];

UFILE* fopen(char*, char*);
int fclose(UFILE*);
char* fgets(char*, int, UFILE*);
int fputs(char*, UFILE*);
static int fflush(UFILE *f);
int fclose(UFILE *f);
int appendToFile(char* name, char* msg);
void initFile(int fl);

int _fillbuf(UFILE *);
int _flushbuf(int, UFILE *);

enum _flags {
	_READ	= 01,
	_WRITE	= 02,
	_UNBUF	= 04,
	_EOF	= 010,
	_ERR	= 020
};

#define getc(p)	(--(p)->cnt >= 0 ? *(p)->ptr++ : _fillbuf(p))
#define putc(x,p)	(--(p)->cnt >= 0 ? *(p)->ptr++ =(x) : _flushbuf((x),p))
#define ferror(p)	(((p)->flag & _ERR) != 0)

#define fprintf4(a, b, c, d) { char t[BUFSIZ]; sprintf(t, b, c, d); fputs(t, a); }

#define fprintf3(a, b, c) { char t[BUFSIZ]; sprintf(t, b, c); fputs(t, a); }

#ifdef  __cplusplus
}
#endif

#endif  // _UTILS_FILE

