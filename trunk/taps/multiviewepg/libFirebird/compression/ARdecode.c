#include                "FBLib_compression.h"
#include                "../libFireBird.h"

/*****************************************************************************************************************************/
/* Prototypes                                                                                                                */
/*****************************************************************************************************************************/
static word decode_c(ARDATA *ArData);
static word decode_p(ARDATA *ArData);
static void fillbuf(ARDATA *ArData, byte n);
static word getbits(ARDATA *ArData, byte n);
static void init_getbits(ARDATA *ArData);
static void make_table(ARDATA *ArData, short nchar, byte bitlen[], short tablebits, word table[]);
static void read_c_len(ARDATA *ArData);
static void read_pt_len(ARDATA *ArData, short nn, short nbit, short i_special);


/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
static word decode_c(ARDATA *ArData)
{
  word                  j, mask;

#ifdef ARCALLTRACE
  CallTraceEnter ("decode_c");
#endif

  if (ArData->blocksize == 0)
  {
    ArData->blocksize = getbits(ArData, 16);
    read_pt_len(ArData, NT, TBIT, 3);
    read_c_len(ArData);
    read_pt_len(ArData, ArData->np, ArData->pbit, -1);
  }
  ArData->blocksize--;
  j = ArData->c_table[peekbits(12)];
  if (j < NC)
    fillbuf(ArData, ArData->c_len[j]);
  else
  {
    fillbuf(ArData, 12);
    mask = 1 << (16 - 1);
    do
    {
      if (ArData->bitbuf & mask) j = ArData->right[j];
                            else j = ArData->left[j];
      mask >>= 1;
    } while (j >= NC);
    fillbuf(ArData, ArData->c_len[j] - 12);
  }
#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif

  return j;
}


/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
static word decode_p(ARDATA *ArData)
{
  word                  j, mask;

#ifdef ARCALLTRACE
  CallTraceEnter ("decode_p");
#endif

  j = ArData->pt_table[peekbits(8)];
  if (j < ArData->np)
    fillbuf(ArData, ArData->pt_len[j]);
  else
  {
    fillbuf(ArData, 8);
    mask = 1 << (16 - 1);
    do
    {
      if (ArData->bitbuf & mask) j = ArData->right[j];
                            else j = ArData->left[j];
      mask >>= 1;
    } while (j >= ArData->np);
    fillbuf(ArData, ArData->pt_len[j] - 8);
  }
  if (j != 0) j = (1 << (j - 1)) + getbits(ArData, j - 1);

#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif

  return j;
}


/* Shift ArData->bitbuf n bits ArData->left, read n bits */
static void fillbuf(ARDATA *ArData, byte n)
{
#ifdef ARCALLTRACE
  CallTraceEnter ("fillbuf");
#endif

  while (n > ArData->bitcount)
  {
    n -= ArData->bitcount;
    ArData->bitbuf = (ArData->bitbuf << ArData->bitcount) + (ArData->subbitbuf >> (CHAR_BIT - ArData->bitcount));
    if (ArData->CompSize != 0)
    {
      ArData->CompSize--;
      ArData->subbitbuf = (byte) *ArData->InFile; ArData->InFile++;
    }
    else ArData->subbitbuf = 0;
    ArData->bitcount = CHAR_BIT;
  }
  ArData->bitcount -= n;
  ArData->bitbuf = (ArData->bitbuf << n) + (ArData->subbitbuf >> (CHAR_BIT - n));
  ArData->subbitbuf <<= n;

#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif

}


/* ------------------------------------------------------------------------ */
static word getbits(ARDATA *ArData, byte n)
{
  word                  x;

#ifdef ARCALLTRACE
  CallTraceEnter ("getbits");
#endif

  x = ArData->bitbuf >> (2 * CHAR_BIT - n);
  fillbuf(ArData, n);

#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif

  return x;
}


/* ------------------------------------------------------------------------ */
static void init_getbits(ARDATA *ArData)
{
#ifdef ARCALLTRACE
  CallTraceEnter ("init_getbits");
#endif

  ArData->bitbuf = 0;
  ArData->subbitbuf = 0;
  ArData->bitcount = 0;
  fillbuf(ArData, 2 * CHAR_BIT);

#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif
}


/* ------------------------------------------------------------------------ */
static void make_table(ARDATA *ArData, short nchar, byte bitlen[], short tablebits, word table[])
{
  word                  count[17]; /* count of bitlen */
  word                  weight[17]; /* 0x10000ul >> bitlen */
  word                  start[17]; /* first code of bitlen */
  word                  total;
  int                   i, l, j, k, m, n, avail;
  word                  *p;

#ifdef ARCALLTRACE
  CallTraceEnter ("make_table");
#endif

  avail = nchar;

  /* initialize */
  for (i = 1; i <= 16; i++)
  {
    count[i] = 0;
    weight[i] = 1 << (16 - i);
  }

  /* count */
  for (i = 0; i < nchar; i++)
    count[bitlen[i]]++;

  /* calculate first code */
  total = 0;
  for (i = 1; i <= 16; i++)
  {
    start[i] = total;
    total += weight[i] * count[i];
  }
  if ((total & 0xffff) != 0) TAP_Print("make_table(): Bad table (5)\n");

  /* shift data for make table. */
  m = 16 - tablebits;
  for (i = 1; i <= tablebits; i++)
  {
    start[i] >>= m;
    weight[i] >>= m;
  }

  /* initialize */
  j = start[tablebits + 1] >> m;
  k = 1 << tablebits;
  if (j != 0)
    for (i = j; i < k; i++)
      table[i] = 0;

  /* create table and tree */
  for (j = 0; j < nchar; j++)
  {
    k = bitlen[j];
    if (k == 0)
      continue;
    l = start[k] + weight[k];
    if (k <= tablebits)
    {
      /* code in table */
      for (i = start[k]; i < l; i++)
        table[i] = j;
    }
    else
    {
      /* code not in table */
      p = &table[(i = start[k]) >> m];

      i <<= tablebits;
      n = k - tablebits;

      /* make tree (n length) */
      while (--n >= 0)
      {
        if (*p == 0)
        {
          ArData->right[avail] = ArData->left[avail] = 0;
          *p = avail++;
        }
        if (i & 0x8000)
          p = &ArData->right[*p];
        else
          p = &ArData->left[*p];
        i <<= 1;
      }
      *p = j;
    }
    start[k] = l;
  }
#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif
}


/* ------------------------------------------------------------------------ */
static void read_c_len(ARDATA *ArData)
{
  short                 i, c, n;

#ifdef ARCALLTRACE
  CallTraceEnter ("read_c_len");
#endif

  n = getbits(ArData, CBIT);
  if (n == 0)
  {
    c = getbits(ArData, CBIT);
    for (i = 0; i < NC; i++)
      ArData->c_len[i] = 0;
    for (i = 0; i < 4096; i++)
      ArData->c_table[i] = c;
  }
  else
  {
    i = 0;
    while (i < n)
    {
      c = ArData->pt_table[peekbits(8)];
      if (c >= NT)
      {
        word mask = 1 << (16 - 9);
        do
        {
          if (ArData->bitbuf & mask) c = ArData->right[c];
                                else c = ArData->left[c];
          mask >>= 1;
        } while (c >= NT);
      }
      fillbuf(ArData, ArData->pt_len[c]);
      if (c <= 2)
      {
        if (c == 0)
          c = 1;
        else if (c == 1)
          c = getbits(ArData, 4) + 3;
        else
          c = getbits(ArData, CBIT) + 20;
        while (--c >= 0)
          ArData->c_len[i++] = 0;
      }
      else
        ArData->c_len[i++] = c - 2;
    }
    while (i < NC)
      ArData->c_len[i++] = 0;
    make_table(ArData, NC, ArData->c_len, 12, ArData->c_table);
  }
#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif
}


/* ------------------------------------------------------------------------ */
static void read_pt_len(ARDATA *ArData, short nn, short nbit, short i_special)
{
  int      i, c, n;

#ifdef ARCALLTRACE
  CallTraceEnter ("read_pt_len");
#endif

  n = getbits(ArData, nbit);
  if (n == 0)
  {
    c = getbits(ArData, nbit);
    for (i = 0; i < nn; i++)
      ArData->pt_len[i] = 0;
    for (i = 0; i < 256; i++)
      ArData->pt_table[i] = c;
  }
  else
  {
    i = 0;
    while (i < n)
    {
      c = peekbits(3);
      if (c != 7)
        fillbuf(ArData, 3);
      else
      {
        word mask = 1 << (16 - 4);
        while (mask & ArData->bitbuf)
        {
          mask >>= 1;
          c++;
        }
        fillbuf(ArData, c - 3);
      }

      ArData->pt_len[i++] = c;
      if (i == i_special)
      {
        c = getbits(ArData, 2);
        while (--c >= 0)
          ArData->pt_len[i++] = 0;
      }
    }
    while (i < nn)
      ArData->pt_len[i++] = 0;
    make_table(ArData, nn, ArData->pt_len, 8, ArData->pt_table);
  }
#ifdef ARCALLTRACE
  CallTraceExit (&ArData->Magic);
#endif
}

// UncompressBlock() can now decompress blocks with up to 32 kB of uncompressed size.
// The original code has been slightly modified to fit the purpose.
word UncompressBlock (byte *pInput, word compCount, byte *pOutput, word bufSize)
  /* The calling function must keep the number of
     bytes to be processed.  This function decodes
     either 'count' bytes or 'DICSIZ' bytes, whichever
     is smaller, into the array 'buffer[]' of size
     'DICSIZ' or more.  */
{
  word                  i;
  word                  r = 0, c;
  int                   j;        // remaining bytes to copy
  ARDATA                ARData;
  byte                  *OrigInFile;

#ifdef ARCALLTRACE
  CallTraceEnter ("UncompressBlock");
#endif

  if (compCount == bufSize)
  {
    if (bufSize) memcpy (pOutput, pInput, bufSize);
  }
  else
  {
    ARData.Magic = 0xFB1BB100;

    // initialize data for a new block
    ARData.InFile = pInput;
    ARData.OutFile = pOutput;
    ARData.OrigSize = bufSize;
    ARData.CompSize = compCount;

    ARData.bufsiz           = 0;
    ARData.InFileRemaining  = ARData.OrigSize;
    ARData.unpackable       = 0;
    ARData.cpos             = 0;
    ARData.depth            = 0;
    OrigInFile              = ARData.InFile;

    if (bufSize >0x7ffa) ARData.dicbit = LZHUFF7_DICBIT;
                    else ARData.dicbit = LZHUFF5_DICBIT;

    switch (ARData.dicbit)
    {
      case LZHUFF4_DICBIT:
      case LZHUFF5_DICBIT: ARData.pbit = 4; ARData.np = LZHUFF5_DICBIT + 1; break;
      case LZHUFF6_DICBIT: ARData.pbit = 5; ARData.np = LZHUFF6_DICBIT + 1; break;
      case LZHUFF7_DICBIT: ARData.pbit = 5; ARData.np = LZHUFF7_DICBIT + 1; break;
      default:
        TAP_Print("UncompressBlock: Cannot use %d bytes dictionary. Defaulting to lh4", 1 << ARData.dicbit);
        ARData.pbit = 4; ARData.np = LZHUFF5_DICBIT + 1;
    }

    init_getbits(&ARData);
    ARData.blocksize = 0;

    j = 0;
    r = 0;
    for ( ; ; )
    {
      c = decode_c(&ARData);
      if (c <= UCHAR_MAX)
      {
        if (ARData.OutFile) ARData.OutFile[r] = c;
        if (++r == bufSize)
        {
          ARData.InFile = OrigInFile;

  #ifdef ARCALLTRACE
          CallTraceExit (&ARData.Magic);
  #endif
          return r;
        }
      }
      else
      {
        j = c - (UCHAR_MAX + 1 - THRESHOLD);
        i = (r - decode_p(&ARData) - 1) & (MAX_DICSIZ - 1);
        while (--j >= 0)
        {
          if (ARData.OutFile) ARData.OutFile[r] = ARData.OutFile[i];
          i = (i + 1) & (MAX_DICSIZ - 1);
          if (++r == bufSize)
          {
            ARData.InFile = OrigInFile;

  #ifdef ARCALLTRACE
            CallTraceExit (&ARData.Magic);
  #endif
            return r;
          }
        }
      }
    }
  }

#ifdef ARCALLTRACE
  CallTraceExit (&ARData.Magic);
#endif

  return r;
}
