#include <string.h>
#include <tap.h>
#include "file.h"

#define printf TAP_Print

static void dumpBuffer(); /* for debugging */

UFILE _iob[OPEN_MAX];

/*
returns a pointer to the open file. A null pointer value indicates an error
The character string mode specifies the type of access requested for the file, as follows: 
"r" 
Opens for reading. If the file does not exist or cannot be found, the fopen call fails. 
"w" 
Opens an empty file for writing. If the given file exists, its contents are destroyed. 
*/
UFILE* fopen(char* name, char* mode)
{
	TYPE_File *fd;
	UFILE *fp;

	//printf("Opening file %s for %c\n", name, *mode);
	//dumpBuffer();

	if (*mode != 'r' && *mode != 'w' && *mode != 'a') {
		return NULL;
	}
	for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
		if ((fp->flag & (_READ | _WRITE)) == 0)
			break;		/* found free slot */
	if (fp >= _iob + OPEN_MAX) {	/* no free slots */
		return NULL;
	}

	if (*mode == 'w')
		TAP_Hdd_Create(name, ATTR_NORMAL);   //TAP_Hdd_Delete(name);
	else if (*mode == 'a')
		if (!TAP_Hdd_Exist(name))
			TAP_Hdd_Create(name, ATTR_NORMAL);

	fd = TAP_Hdd_Fopen( name );
	if (fd == 0) {	/* couldn't access name */
		return NULL;
	}
	if (*mode == 'a')
		TAP_Hdd_Fseek(fd, 0L, 2L);
	fp->fd = fd;
	fp->cnt = 0;
	fp->size = 0;
	fp->base = NULL;
	fp->flag = (*mode == 'r') ? _READ : _WRITE;
	fp->size = TAP_Hdd_Flen(fd);
	//TAP_Print("file size: %d\r\n", fp->size);
	//fp->flag |= _UNBUF;	//
	//dumpBuffer();
	return fp;
}

/* fflush */
static int fflush(UFILE *f)
{
    int retval;
    int i;

	retval = 0; 
	if (f == NULL) { 
		/* flush all output streams */ 
		for (i = 0; i < OPEN_MAX; i++) { 
			if ((_iob[i].flag & _WRITE) && (fflush(&_iob[i]) == -1)) 
				retval = -1; 
		} 
	} else { 
		if ((f->flag & _WRITE) == 0) 
			return -1; 
		_flushbuf(EOF, f); 
		if (f->flag & _ERR) 
			retval = -1; 
	} 
	return retval; 
}

/*
fclose returns 0 if the stream is successfully closed. return EOF to indicate an error.
*/
int fclose(UFILE *f)
{
    TYPE_File *fd;
    
    if (f == NULL)
        return -1;
    fd = f->fd;
    fflush(f);
    f->cnt = 0;
    f->ptr = NULL;
    if (f->base != NULL)
        free(f->base);
    f->base = NULL;
    f->flag = 0;
	//printf("file size: %d\n", (int)f->size);
	f->fd->size = f->size;
	TAP_Hdd_Fclose(fd);
    f->fd = NULL;
    return 0;
}


/*
The fgets function reads a string from the input stream argument and stores it in string. 
fgets reads characters from the current stream position to and including the first newline 
character, to the end of the stream, or until the number of characters read is equal to n – 1, 
whichever comes first. The result stored in string is appended with a null character. 
The newline character, if read, is included in the string. 
*/
char* fgets(char *s, int n, UFILE *iop)
{
	//return NULL at EOF, otherwise line terminated with \0

	register int c;
	register char *cs;

	cs = s;
	while (--n > 0 && (c = getc(iop)) != EOF)
		if ((*cs++ = c) == '\n')
			break;
	*cs = '\0';
	return (c == EOF && cs == s) ? NULL : s;
}

/* fputs:  put string s on file f */
int fputs(char *s, UFILE *iop)
{

	int c;
#ifdef DEBUG
	TAP_Print("fputs: Adding string '%s'\r\n", s);
#endif
	while (c = *s++)
		putc(c, iop);

	return ferror(iop) ? EOF : 0;

}

/* _fillbuf: allocate and fill input buffer */
int _fillbuf(UFILE *fp)
{
	int bufsize, fpos;

	if ((fp->flag&(_READ|_EOF|_ERR)) != _READ)
		return EOF;
	bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;
	if (fp->base == NULL)	/* no buffer yet */
	{
		if ((fp->base = (char *) malloc(bufsize)) == NULL)
			return EOF;		/* can't get buffer */
		memset(fp->base, 0, bufsize);
	}
	fp->ptr = fp->base;

	fpos = TAP_Hdd_Ftell(fp->fd);

	//TAP_Print("fillbuf: about to read bytes at offset %d\r\n", fpos );

	if (fp->size - fpos > 0)  //more bytes to read
	{
		if (fp->size - fpos > bufsize)
		{
			//TAP_Print("fillbuf:  reading %d bytes\r\n", bufsize);
			//read bufsize bytes
			fp->cnt = read( fp->fd, fp->ptr, bufsize );
			if (fp->cnt > 0) fp->cnt = bufsize;
		}
		else
		{
			//TAP_Print("fillbuf:  reading %d bytes\r\n", fp->size - fpos);
			//read fp->size - fpos bytes
			fp->cnt = read( fp->fd, fp->ptr, fp->size - fpos );
			if (fp->cnt > 0) fp->cnt = fp->size - fpos;
		}
	}
	else
		fp->cnt = 0;
	

	if (--fp->cnt < 0) {
		if (fp->cnt == -1)
			fp->flag |= _EOF;
		else
			fp->flag |= _ERR;
		fp->cnt = 0;
		return EOF;
	}
	return *fp->ptr++;  //change in case EOF is found
}

/* _flushbuf - flush a buffer
 * According to the code on p. 176, _flushbuf
 * is what putc calls when the buffer is full.
 * EOF as the character causes everything to
 * be written -- I don't tack on the EOF.
 */
int _flushbuf(int c, UFILE *f)
{
    int num_written, bufsize;
    unsigned char uc = c;    

    if ((f->flag & (_WRITE|_EOF|_ERR)) != _WRITE)
        return EOF;
    if (f->base == NULL && ((f->flag & _UNBUF) == 0)) {
        /* no buffer yet */
        if ((f->base = malloc(BUFSIZ)) == NULL) 
            /* couldn't allocate a buffer, so try unbuffered */
            f->flag |= _UNBUF;
        else {
			memset(f->base, 0, BUFSIZ);
            f->ptr = f->base;
            f->cnt = BUFSIZ - 1;
        }
    }
    if (f->flag & _UNBUF) {
        /* unbuffered write */
        f->ptr = f->base = NULL;
        f->cnt = 0;
        if (c == EOF)
            return EOF;
        num_written = write(f->fd, &uc, 1);
        bufsize = 1;
    } else {
        /* buffered write */
        if (c != EOF)
            *f->ptr++ = uc;
		// write eof marker
		else
			*f->ptr++ = EOF;
        bufsize = (int)(f->ptr - f->base);
        num_written = write(f->fd, f->base, bufsize);
        f->ptr = f->base;
        f->cnt = BUFSIZ - 1;
    }
	if (num_written == 1)
	{
		f->size += bufsize;
        return c;
	} else {       
        f->flag |= _ERR;
        return EOF;
    }
}


int appendToFile(char* name, char* msg)
{
	UFILE *fp;

	if (fp = fopen(name, "a"))
	{
		fputs(msg, fp);
		fclose(fp);
		return 1;
	} else
		return 0;
}


/*
int _flushbuf(int c, UFILE *f)
{
   int bufsize;

    if ((fp->flag & (_WRITE|_ERR)) != _WRITE) 
	return EOF;

    bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;

    if (fp->base == NULL) {
	if ((fp->base = (char *) malloc(bufsize)) == NULL)
	    return EOF;
    }
    else write(fp->fd, fp->base, bufsize);

    fp->ptr = fp->base;
    *(fp->ptr++) = c;
    fp->cnt = bufsize - 1;
    
    return c;
}


*/

int remove(char *name)
{
	TAP_Hdd_Delete(name);
	return 0;
}

void initFile(int fl)
{
	UFILE *fp;

	initUtils(fl);	//need the error stuff from Utils

	//int i;
	//init file structure - do we need this?
	//for( i=0; i < OPEN_MAX; i++ )
	//{
	//	_iob[i].ptr = (char *)((dword)(_iob[i].ptr) + _tap_startAddr);
	//	_iob[i].base = (char *)((dword)(_iob[i].base) + _tap_startAddr);
	//	_iob[i].fd = (TYPE_File *)((dword)(_iob[i].fd) + _tap_startAddr);
	//}

	for (fp = _iob; fp < _iob + OPEN_MAX; fp++) {
		//fp->base = NULL;  seems to screw stdout??
		fp->cnt = 0;
		fp->flag = 0;
	}
}

static void dumpBuffer()
{
	int i = 0;
	//dump _iobuf in readable format to stdout
	//UFILE _iob[OPEN_MAX];
	UFILE *fp;
	char line[100];

	for (fp = _iob; fp < _iob + OPEN_MAX; fp++, i++) {
		if ((fp->flag & (_READ | _WRITE)) == 0) {
			printf ("Slot %i not in use\n", i);
			break;		/* found free slot */
		}
		sprintf (line, "Slot %i open for ", i);
		if (fp->flag & _READ) strcat(line, "READ ");
		if (fp->flag & _WRITE) strcat(line, "WRITE ");
		strcat(line, "\n");
		printf(line);
	}
}

/*
#include <stdarg.h>
extern int vsprintf(char *, const char *, va_list);

int fprintf(UFILE *fp, char *format, ...)
{
	va_list args;
	char fstr[BUFSIZ];
	int cnt;

	va_start(args, format);
	cnt = vsprintf(fstr, format, args);
	fputs(fstr, fp);
	va_end(args);
	return cnt;
}
*/

