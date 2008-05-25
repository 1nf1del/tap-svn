#ifndef FBLIB_COMPRESSION_H
#define FBLIB_COMPRESSION_H

#include                <limits.h>
#include                "type.h"

//Uncomment or pass compiler option to get a call history on the serial console
//#define ARCALLTRACE

/* variables for hash */
typedef struct
{
    dword pos;
    int too_flag;               /* if 1, matching candidate is too many */
} HASH;

typedef struct
{
    int len;
    dword off;
} MATCHDATA;


#define LOAD_WORD(x)      (((x)[0] << 8) | (x)[1])
#define LOAD_WORDLE(x)    (((x)[1] << 8) | (x)[0])
#define STORE_WORD(x,y)   (x)[0] = (y) >> 8;(x)[1] = (y) & 0xff
#define STORE_WORDLE(x,y) (x)[1] = (y) >> 8;(x)[0] = (y) & 0xff

#define LZHUFF5_METHOD_NUM      5
#define LZHUFF6_METHOD_NUM      6
#define LZHUFF7_METHOD_NUM      7

#define LZHUFF4_DICBIT  12      /* 2^12 =  4KB sliding dictionary */
#define LZHUFF5_DICBIT  13      /* 2^13 =  8KB sliding dictionary */
#define LZHUFF6_DICBIT  15      /* 2^15 = 32KB sliding dictionary */
#define LZHUFF7_DICBIT  16      /* 2^16 = 64KB sliding dictionary */

#define NIL        0

/* hash function: it represents 3 letters from `pos' on `text' */
#define INIT_HASH(pos) \
        ((( (ArData->text[(pos)] << 5) \
           ^ ArData->text[(pos) + 1]  ) << 5) \
           ^ ArData->text[(pos) + 2]         ) & (unsigned)(HSHSIZ - 1);
#define NEXT_HASH(hash,pos) \
        (((hash) << 5) \
           ^ ArData->text[(pos) + 2]         ) & (unsigned)(HSHSIZ - 1);


#define peekbits(n)     (ArData->bitbuf >> (sizeof(ArData->bitbuf)*8 - (n)))
#define NC              (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define MAXMATCH        256     /* formerly F (not more than UCHAR_MAX + 1) */
#define THRESHOLD       3       /* choose optimal value */
#define MAX_DICBIT      LZHUFF7_DICBIT      /* lh7 use 16bits */
#define MAX_DICSIZ      (1L << MAX_DICBIT)
#define USHRT_BIT       16      /* (CHAR_BIT * sizeof(ushort)) */
#define NP              (MAX_DICBIT + 1)
#define NT              (USHRT_BIT + 3)
#define NPT             0x80                /* #if NT > NP #define NPT NT #else #define NPT NP #endif  */
#define TBIT            5                   /* smallest integer such that (1 << TBIT) > * NT */
#define CBIT            9                   /* smallest integer such that (1 << CBIT) > * NC */
#define HSHSIZ          (((dword)1) <<15)
#define TXTSIZ          (MAX_DICSIZ * 2L + MAXMATCH)
#define LIMIT           0x100              /* limit of hash chain */

typedef struct
{
  byte                  *InFile, *OutFile;        //provided by the main program
  dword                 OrigSize;
  dword                 CompSize;

  dword                 InFileRemaining;          //io
  int                   unpackable;
  word                  bitbuf;
  byte                  bitcount, subbitbuf;
  int                   dicbit;
  int                   pbit, np;

  word                  pt_code [NPT];            //huf
  word                  c_code [NC];

  word                  c_freq [2 * NC - 1];
  word                  p_freq [2 * NP - 1];
  word                  t_freq [2 * NT - 1];

  word                  left[2 * NC - 1];
  word                  right[2 * NC - 1];
  word                  output_pos;
  word                  output_mask;
  byte                 *buf;
  byte                  c_len[NC];
  byte                  pt_len [NPT];
  dword                 bufsiz;

  HASH                 *hash;                     //maketree
  word                  depth;

  byte                 *text;                     //encode
  dword                *prev;
  word                  cpos;
  word                  maxmatch;

  dword                 txtsiz;
  dword                 dicsiz;
  int                   remainder;

  word                  blocksize;                //decode
  word                  c_table[4096];
  word                  pt_table[256];

  dword                 Magic;

} ARDATA;

#endif
