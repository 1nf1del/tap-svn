#include                <string.h>
#include                "../imem/imem.h"
#include                "FBLib_compression.h"
#include                "../libFireBird.h"


static byte *alloc_buf  (ARDATA *ArData);
static void  count_leaf  (ARDATA *ArData, int node, int nchar, word leaf_num[], int depth);
static void  count_t_freq(ARDATA *ArData);
static void  downheap    (int i, short *heap, size_t heapsize, word *freq);
static void  encode_alloc(ARDATA *ArData, int method);
static void  encode_c    (ARDATA *ArData, short c);
static void  encode_p    (ARDATA *ArData, word p);
static void  encode_start(ARDATA *ArData);
static void  encode_end  (ARDATA *ArData);
static int   fread_crc   (ARDATA *ArData, byte *p, dword n);
static void  free_memory (ARDATA *ArData);
static void  init_slide  (ARDATA *ArData);
static void  init_putbits(ARDATA *ArData);
static void  insert_hash (ARDATA *ArData, dword token, dword pos);
static void  make_code   (int nchar, byte *bitlen, word *code, word *leaf_num);
static void  make_len    (int nchar, byte *bitlen, word *sort, word *leaf_num);
static short make_tree   (ARDATA *ArData, int nchar, word *freq, byte *bitlen, word *code);
static void  next_token  (ARDATA *ArData, dword *token, dword *pos);
static void  output      (ARDATA *ArData, word c, word p);
static inline void  putbits     (ARDATA *ArData, byte n, word x);
static inline void  putcode     (ARDATA *ArData, byte n, word x);
static void  search_dict (ARDATA *ArData, dword token, dword pos, int min, MATCHDATA *m);
static void  search_dict_1(ARDATA *ArData, dword token, dword pos, dword off, dword max, MATCHDATA *m);
static void  send_block  (ARDATA *ArData);
static void  update_dict (ARDATA *ArData, dword *pos);
static void  write_c_len (ARDATA *ArData);
static void  write_pt_len(ARDATA *ArData, short n, short nbit, short i_special);
static void  encode      (ARDATA *ArData);


/* ------------------------------------------------------------------------ */
static byte *alloc_buf(ARDATA *ArData)
{
  ArData->bufsiz = 16 * 1024 *2; /* 65408U; */ /* t.okamoto */

  while (ArData->buf == NULL)
  {
    if (IMEM_isInitialized()) ArData->buf = (byte*) IMEM_Alloc(ArData->bufsiz);
                         else ArData->buf = (byte*) TAP_MemAlloc(ArData->bufsiz);

    if (!ArData->buf)
    {
      ArData->bufsiz = (ArData->bufsiz / 10) * 9;
      if (ArData->bufsiz < 4 * 1024)
      {
        TAP_Print ("alloc_buf: Not enough memory\n");
        return NULL;
      }
    }
  }
  return ArData->buf;
}


static void count_leaf(ARDATA *ArData, int node, int nchar, word leaf_num[], int depth) /* call with node = root */
{
  if (node < nchar)
    leaf_num[depth < 16 ? depth : 16]++;
  else
  {
    count_leaf(ArData, ArData->left[node], nchar, leaf_num, depth + 1);
    count_leaf(ArData, ArData->right[node], nchar, leaf_num, depth + 1);
  }
}


static void count_t_freq(ARDATA *ArData)
{
  short                 i, k, n, count;


  for (i = 0; i < NT; i++) ArData->t_freq[i] = 0;
  n = NC;
  while (n > 0 && ArData->c_len[n - 1] == 0)
    n--;
  i = 0;
  while (i < n)
  {
    k = ArData->c_len[i++];
    if (k == 0)
    {
      count = 1;
      while (i < n && ArData->c_len[i] == 0)
      {
        i++;
        count++;
      }
      if (count <= 2)
        ArData->t_freq[0] += count;
      else if (count <= 18)
        ArData->t_freq[1]++;
      else if (count == 19)
      {
        ArData->t_freq[0]++;
        ArData->t_freq[1]++;
      }
      else
        ArData->t_freq[2]++;
    } else
        ArData->t_freq[k + 2]++;
  }
}


/* priority queue; send i-th entry down heap */
static void downheap(int i, short *heap, size_t heapsize, word *freq)
{
  unsigned short        j, k;

  k = heap[i];
  while ((j = 2 * i) <= heapsize)
  {
    if (j < heapsize && freq[heap[j]] > freq[heap[j + 1]])
      j++;
    if (freq[k] <= freq[heap[j]])
      break;
    heap[i] = heap[j];
    i = j;
  }
  heap[i] = k;
}


static void encode_alloc(ARDATA *ArData, int method)
{
  switch (method)
  {
    case LZHUFF5_METHOD_NUM:
      ArData->maxmatch = MAXMATCH;
      ArData->dicbit = LZHUFF5_DICBIT;  /* 13 bits */
      break;

    case LZHUFF6_METHOD_NUM:
      ArData->maxmatch = MAXMATCH;
      ArData->dicbit = LZHUFF6_DICBIT;  /* 15 bits */
      break;

    case LZHUFF7_METHOD_NUM:
      ArData->maxmatch = MAXMATCH;
      ArData->dicbit = LZHUFF7_DICBIT;  /* 16 bits */
      break;

    default:
      TAP_Print ("encode_alloc: unknown method %d. Using lh5\n", method);
      ArData->maxmatch = MAXMATCH;
      ArData->dicbit = LZHUFF5_DICBIT;  /* 13 bits */
  }

  ArData->dicsiz = (((dword)1) << ArData->dicbit);
  ArData->txtsiz = ArData->dicsiz * 2 + ArData->maxmatch;

  if (!ArData->hash)
  {
    alloc_buf(ArData);

    if (IMEM_isInitialized())
    {
      ArData->hash = (HASH*)IMEM_Alloc(HSHSIZ * sizeof(HASH));
      ArData->prev = (dword*)IMEM_Alloc(MAX_DICSIZ * sizeof(dword));
      ArData->text = (byte*)IMEM_Alloc(TXTSIZ);
    }
    else
    {
      ArData->hash = (HASH*) TAP_MemAlloc(HSHSIZ * sizeof(HASH));
      ArData->prev = (dword*)TAP_MemAlloc(MAX_DICSIZ * sizeof(dword));
      ArData->text = (byte*) TAP_MemAlloc(TXTSIZ);
    }
  }
}


/* ------------------------------------------------------------------------ */
static void encode_c(ARDATA *ArData, short c)
{
  putcode(ArData, ArData->c_len[c], ArData->c_code[c]);
}


/* ------------------------------------------------------------------------ */
static void encode_p(ARDATA *ArData, word p)
{
  word                  c, q;

  c = 0;
  q = p;
  while (q)
  {
    q >>= 1;
    c++;
  }
  putcode(ArData, ArData->pt_len[c], ArData->pt_code[c]);
  if (c > 1) putbits(ArData, c - 1, p);
}


/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
static void encode_start(ARDATA *ArData)
{
  int                   i;

  switch (ArData->dicbit)
  {
    case LZHUFF4_DICBIT:
    case LZHUFF5_DICBIT: ArData->pbit = 4; ArData->np = LZHUFF5_DICBIT + 1; break;
    case LZHUFF6_DICBIT: ArData->pbit = 5; ArData->np = LZHUFF6_DICBIT + 1; break;
    case LZHUFF7_DICBIT: ArData->pbit = 5; ArData->np = LZHUFF7_DICBIT + 1; break;
    default:
      TAP_Print("Cannot use %d bytes dictionary. Defaulting to lh5\n", 1 << ArData->dicbit);
      ArData->pbit = 4; ArData->np = LZHUFF5_DICBIT + 1;
  }

  for (i = 0; i < NC; i++)         ArData->c_freq[i] = 0;
  for (i = 0; i < ArData->np; i++) ArData->p_freq[i] = 0;
  ArData->output_pos = ArData->output_mask = 0;
  init_putbits(ArData);
  ArData->buf[0] = 0;
}


/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
static void encode_end(ARDATA *ArData)
{

  if (!ArData->unpackable)
  {
    send_block(ArData);
    putbits(ArData, CHAR_BIT - 1, 0);  /* flush remaining bits */
  }
}


static int fread_crc(ARDATA *ArData, byte *p, dword n)
{
  if (n > ArData->InFileRemaining) n = ArData->InFileRemaining;
  memcpy (p, ArData->InFile, n);
  ArData->InFile += n;
  ArData->InFileRemaining -= n;

  return n;
}


static void free_memory(ARDATA *ArData)
{
  if (IMEM_isInitialized())
  {
    if (ArData->buf)  IMEM_Free (ArData->buf) ; ArData->buf = NULL;
    if (ArData->hash) IMEM_Free (ArData->hash); ArData->hash = NULL;
    if (ArData->prev) IMEM_Free (ArData->prev); ArData->prev = NULL;
    if (ArData->text) IMEM_Free (ArData->text); ArData->text = NULL;
  }
  else
  {
    if (ArData->buf)  TAP_MemFree (ArData->buf) ; ArData->buf = NULL;
    if (ArData->hash) TAP_MemFree (ArData->hash); ArData->hash = NULL;
    if (ArData->prev) TAP_MemFree (ArData->prev); ArData->prev = NULL;
    if (ArData->text) TAP_MemFree (ArData->text); ArData->text = NULL;
  }
}


static void init_slide(ARDATA *ArData)
{
  dword                 i;

  for (i = 0; i < HSHSIZ; i++)
  {
    ArData->hash[i].pos = NIL;
    ArData->hash[i].too_flag = 0;
  }
}


/* associate position with token */
static void insert_hash(ARDATA *ArData, dword token, dword pos)
{
  ArData->prev[pos & (ArData->dicsiz - 1)] = ArData->hash[token].pos; /* chain the previous pos. */
  ArData->hash[token].pos = pos;
}


static void init_putbits(ARDATA *ArData)
{
  ArData->bitcount = CHAR_BIT;
  ArData->subbitbuf = 0;
}


static void make_code(int nchar, byte *bitlen, word *code, word *leaf_num)
{
  word                  weight[17]; /* 0x10000ul >> bitlen */
  word                  start[17]; /* start code */
  word                  total;
  int                   i;
  int                   c;

  total = 0;
  for (i = 1; i <= 16; i++)
  {
    start[i] = total;
    weight[i] = 1 << (16 - i);
    total += weight[i] * leaf_num[i];
  }
  for (c = 0; c < nchar; c++)
  {
    i = bitlen[c];
    code[c] = start[i];
    start[i] += weight[i];
  }
}


static void make_len(int nchar, byte *bitlen, word *sort, word *leaf_num)
{
  int                   i, k;
  dword                 cum;

  (void) nchar;

  cum = 0;
  for (i = 16; i > 0; i--)
  {
    cum += leaf_num[i] << (16 - i);
  }
#if (UINT_MAX != 0xffff)
  cum &= 0xffff;
#endif
  /* adjust len */
  if (cum)
  {
    leaf_num[16] -= cum; /* always leaf_num[16] > cum */
    do
    {
      for (i = 15; i > 0; i--)
      {
        if (leaf_num[i]) {
          leaf_num[i]--;
          leaf_num[i + 1] += 2;
          break;
        }
      }
    } while (--cum);
  }
  /* make len */
  for (i = 16; i > 0; i--)
  {
    k = leaf_num[i];
    while (k > 0)
    {
      bitlen[*sort++] = i;
      k--;
    }
  }
}


/* make tree, calculate bitlen[], return root */
static short make_tree(ARDATA *ArData, int nchar, word *freq, byte *bitlen, word *code)
{
  short                 i, j, avail, root;
  word                  *sort;

  short                 heap[NC + 1];    /* NC >= nchar */
  size_t                heapsize;

  avail = nchar;
  heapsize = 0;
  heap[1] = 0;
  for (i = 0; i < nchar; i++)
  {
    bitlen[i] = 0;
    if (freq[i]) heap[++heapsize] = i;
  }
  if (heapsize < 2)
  {
    code[heap[1]] = 0;
    return heap[1];
  }

  /* make priority queue */
  for (i = heapsize / 2; i >= 1; i--)
    downheap(i, heap, heapsize, freq);

  /* make huffman tree */
  sort = code;
  do              /* while queue has at least two entries */
  {
    i = heap[1];  /* take out least-freq entry */
    if (i < nchar) *sort++ = i;
    heap[1] = heap[heapsize--];
    downheap(1, heap, heapsize, freq);
    j = heap[1];  /* next least-freq entry */
    if (j < nchar) *sort++ = j;
    root = avail++;  /* generate new node */
    freq[root] = freq[i] + freq[j];
    heap[1] = root;
    downheap(1, heap, heapsize, freq);  /* put into queue */
    ArData->left[root] = i;
    ArData->right[root] = j;
  } while (heapsize > 1);

  {
    word      leaf_num[17];

    /* make leaf_num */
    memset(leaf_num, 0, sizeof(leaf_num));
    count_leaf(ArData, root, nchar, leaf_num, 0);

    /* make bitlen */
    make_len(nchar, bitlen, code, leaf_num);

    /* make code table */
    make_code(nchar, bitlen, code, leaf_num);
  }

  return root;
}


/* slide dictionary */
static void next_token(ARDATA *ArData, dword *token, dword *pos)
{
  ArData->remainder--;
  if (++*pos >= ArData->txtsiz - ArData->maxmatch)
  {
    update_dict(ArData, pos);
  }
  *token = NEXT_HASH(*token, *pos);
}


/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
static void output (ARDATA *ArData, word c, word p)
{
  ArData->output_mask >>= 1;
  if (ArData->output_mask == 0)
  {
    ArData->output_mask = 1 << (CHAR_BIT - 1);
    if (ArData->output_pos >= ArData->bufsiz - 3 * CHAR_BIT)
    {
      send_block(ArData);
      if (ArData->unpackable) return;
      ArData->output_pos = 0;
    }
    ArData->cpos = ArData->output_pos++;
    ArData->buf[ArData->cpos] = 0;
  }
  ArData->buf[ArData->output_pos++] = (byte) c;
  ArData->c_freq[c]++;
  if (c >= (1 << CHAR_BIT))
  {
    ArData->buf[ArData->cpos] |= ArData->output_mask;
    ArData->buf[ArData->output_pos++] = (byte) (p >> CHAR_BIT);
    ArData->buf[ArData->output_pos++] = (byte) p;
    c = 0;
    while (p)
    {
      p >>= 1;
      c++;
    }
    ArData->p_freq[c]++;
  }
}


/* Write rightmost n bits of x */
static inline void putbits(ARDATA *ArData, byte n, word x)
{
  x <<= USHRT_BIT - n;
  putcode(ArData, n, x);
}


/* Write leftmost n bits of x */
static inline void putcode(ARDATA *ArData, byte n, word x)
{
  while (n >= ArData->bitcount)
  {
    n -= ArData->bitcount;
    ArData->subbitbuf += x >> (USHRT_BIT - ArData->bitcount);
    x <<= ArData->bitcount;
    if (ArData->CompSize < ArData->OrigSize)
    {
      if (ArData->OutFile)
      {
        *ArData->OutFile = ArData->subbitbuf;
        ArData->OutFile++;
        //if (fwrite(&ArData->subbitbuf, 1, 1, ArData->OutFile) == 0)
        //{
        //  TAP_Print ("putcode: Write error in bitio.c(putcode)\n");
        //}
      }
      ArData->CompSize++;
    }
    else ArData->unpackable = 1;
    ArData->subbitbuf = 0;
    ArData->bitcount = CHAR_BIT;
  }
  ArData->subbitbuf += x >> (USHRT_BIT - ArData->bitcount);
  ArData->bitcount -= n;
}


/* search the longest token matching to current token */
static void search_dict(ARDATA *ArData, dword token, dword pos, int min, MATCHDATA *m)
{
  dword                 off, tok, max;

  if (min < THRESHOLD - 1) min = THRESHOLD - 1;

  max = ArData->maxmatch;
  m->off = 0;
  m->len = min;

  off = 0;
  for (tok = token; ArData->hash[tok].too_flag && off < (unsigned)ArData->maxmatch - THRESHOLD; )
  {
    /* If matching position is too many, The search key is
      changed into following token from `off' (for speed). */
    ++off;
    tok = NEXT_HASH(tok, pos+off);
  }
  if (off == (unsigned)ArData->maxmatch - THRESHOLD)
  {
    off = 0;
    tok = token;
  }

  search_dict_1(ArData, tok, pos, off, max, m);

  if (off > 0 && (unsigned)m->len < off + 3)
    /* re-search */
    search_dict_1(ArData, token, pos, 0, off+2, m);

  if (m->len > ArData->remainder) m->len = ArData->remainder;
}


static void search_dict_1(ARDATA *ArData, dword token, dword pos, dword off, dword max, MATCHDATA *m)
{
  dword                 chain = 0;
  dword                 scan_pos = ArData->hash[token].pos;
  int                   scan_beg = scan_pos - off;
  int                   scan_end = pos - ArData->dicsiz;
  dword                 len;

  while (scan_beg > scan_end)
  {
    chain++;

    if (ArData->text[scan_beg + m->len] == ArData->text[pos + m->len])
    {
      {
        /* collate token */
        byte *a = &ArData->text[scan_beg];
        byte *b = &ArData->text[pos];

        for (len = 0; len < max && *a++ == *b++; len++);
      }

      if (len > (unsigned)m->len)
      {
        m->off = pos - scan_beg;
        m->len = len;
        if ((unsigned)m->len == max)
          break;
      }
    }
    scan_pos = ArData->prev[scan_pos & (ArData->dicsiz - 1)];
    scan_beg = scan_pos - off;
  }

  if (chain >= LIMIT) ArData->hash[token].too_flag = 1;
}


/* ------------------------------------------------------------------------ */
static void send_block(ARDATA *ArData)
{
  byte                  flags = 0;
  word                  i, k, root, pos, size;

  root = make_tree(ArData, NC, ArData->c_freq, ArData->c_len, ArData->c_code);
  size = ArData->c_freq[root];
  putbits(ArData, 16, size);
  if (root >= NC)
  {
    count_t_freq(ArData);
    root = make_tree(ArData, NT, ArData->t_freq, ArData->pt_len, ArData->pt_code);
    if (root >= NT)
    {
      write_pt_len(ArData, NT, TBIT, 3);
    }
    else
    {
      putbits(ArData, TBIT, 0);
      putbits(ArData, TBIT, root);
    }
    write_c_len(ArData);
  }
  else
  {
    putbits(ArData, TBIT, 0);
    putbits(ArData, TBIT, 0);
    putbits(ArData, CBIT, 0);
    putbits(ArData, CBIT, root);
  }
  root = make_tree(ArData, ArData->np, ArData->p_freq, ArData->pt_len, ArData->pt_code);
  if (root >= ArData->np)
  {
    write_pt_len(ArData, ArData->np, ArData->pbit, -1);
  }
  else
  {
    putbits(ArData, ArData->pbit, 0);
    putbits(ArData, ArData->pbit, root);
  }
  pos = 0;
  for (i = 0; i < size; i++)
  {
    if (i % CHAR_BIT == 0)
      flags = ArData->buf[pos++];
    else
      flags <<= 1;
    if (flags & (1 << (CHAR_BIT - 1)))
    {
      encode_c(ArData, ArData->buf[pos++] + (1 << CHAR_BIT));
      k = ArData->buf[pos++] << CHAR_BIT;
      k += ArData->buf[pos++];
      encode_p(ArData, k);
    }
    else
      encode_c(ArData, ArData->buf[pos++]);
    if (ArData->unpackable) return;
  }
  for (i = 0; i < NC; i++) ArData->c_freq[i] = 0;
  for (i = 0; i < ArData->np; i++) ArData->p_freq[i] = 0;
}


/* update dictionary */
static void update_dict(ARDATA *ArData, dword *pos)
{
  dword                 i, j;
  long                  n;

  memmove(&ArData->text[0], &ArData->text[ArData->dicsiz], ArData->txtsiz - ArData->dicsiz);

  n = fread_crc(ArData, &ArData->text[ArData->txtsiz - ArData->dicsiz], ArData->dicsiz);

  ArData->remainder += n;

  *pos -= ArData->dicsiz;
  for (i = 0; i < HSHSIZ; i++)
  {
    j = ArData->hash[i].pos;
    ArData->hash[i].pos = (j > ArData->dicsiz) ? j - ArData->dicsiz : NIL;
    ArData->hash[i].too_flag = 0;
  }
  for (i = 0; i < ArData->dicsiz; i++)
  {
    j = ArData->prev[i];
    ArData->prev[i] = (j > ArData->dicsiz) ? j - ArData->dicsiz : NIL;
  }
}


/* ------------------------------------------------------------------------ */
static void write_c_len(ARDATA *ArData)
{
  short                 i, k, n, count;

  n = NC;
  while (n > 0 && ArData->c_len[n - 1] == 0)
    n--;
  putbits(ArData, CBIT, n);
  i = 0;
  while (i < n)
  {
    k = ArData->c_len[i++];
    if (k == 0)
    {
      count = 1;
      while (i < n && ArData->c_len[i] == 0)
      {
        i++;
        count++;
      }
      if (count <= 2)
      {
        for (k = 0; k < count; k++)
          putcode(ArData, ArData->pt_len[0], ArData->pt_code[0]);
      }
      else if (count <= 18)
      {
        putcode(ArData, ArData->pt_len[1], ArData->pt_code[1]);
        putbits(ArData, 4, count - 3);
      }
      else if (count == 19)
      {
        putcode(ArData, ArData->pt_len[0], ArData->pt_code[0]);
        putcode(ArData, ArData->pt_len[1], ArData->pt_code[1]);
        putbits(ArData, 4, 15);
      }
      else
      {
        putcode(ArData, ArData->pt_len[2], ArData->pt_code[2]);
        putbits(ArData, CBIT, count - 20);
      }
    }
    else
      putcode(ArData, ArData->pt_len[k + 2], ArData->pt_code[k + 2]);
  }
}


/* ------------------------------------------------------------------------ */
static void write_pt_len(ARDATA *ArData, short n, short nbit, short i_special)
{
  short                 i, k;

  while (n > 0 && ArData->pt_len[n - 1] == 0)
    n--;
  putbits(ArData, nbit, n);
  i = 0;
  while (i < n)
  {
    k = ArData->pt_len[i++];
    if (k <= 6)
      putbits(ArData, 3, k);
    else
      /* k=7 -> 1110 k=8 -> 11110 k=9 -> 111110 ... */
      putbits(ArData, k - 3, USHRT_MAX - 1);
    if (i == i_special)
    {
      while (i < 6 && ArData->pt_len[i] == 0)
        i++;
      putbits(ArData, 2, i - 3);
    }
  }
}


static void encode(ARDATA *ArData)
{
  MATCHDATA             match, last;
  dword                 token, pos;
  size_t                count;
  byte                  *OrigInFile, *OrigOutFile;

  ArData->bufsiz          = 0;
  ArData->InFileRemaining = ArData->OrigSize;
  ArData->CompSize        = 0;
  ArData->unpackable      = 0;
  OrigInFile              = ArData->InFile;
  OrigOutFile             = ArData->OutFile;
  ArData->dicbit          = LZHUFF5_DICBIT;

  count = 0L;
  ArData->hash = NULL;
  encode_alloc(ArData, LZHUFF5_METHOD_NUM);
  encode_start(ArData);
  init_slide(ArData);
  memset (ArData->text, ' ', TXTSIZ);

  ArData->remainder = fread_crc(ArData, &ArData->text[ArData->dicsiz], ArData->txtsiz - ArData->dicsiz);
  match.len = THRESHOLD - 1;
  match.off = 0;
  if (match.len > ArData->remainder) match.len = ArData->remainder;

  pos = ArData->dicsiz;
  token = INIT_HASH(pos);
  insert_hash(ArData, token, pos);   /* associate token and pos */

  while (ArData->remainder > 0 && ! ArData->unpackable)
  {
    last = match;

    next_token(ArData, &token, &pos);
    search_dict(ArData, token, pos, last.len-1, &match);
    insert_hash(ArData, token, pos);

    if (match.len > last.len || last.len < THRESHOLD)
    {
      /* output a letter */
      output(ArData, ArData->text[pos - 1], 0);
      count++;
    }
    else
    {
      /* output length and offset */
      output(ArData, last.len + (256 - THRESHOLD), (last.off-1) & (ArData->dicsiz-1) );
      count += last.len;

      --last.len;
      while (--last.len > 0)
      {
        next_token(ArData, &token, &pos);
        insert_hash(ArData, token, pos);
      }
      next_token(ArData, &token, &pos);
      search_dict(ArData, token, pos, THRESHOLD - 1, &match);
      insert_hash(ArData, token, pos);
    }
  }
  encode_end (ArData);

  ArData->InFile = OrigInFile;
  ArData->OutFile = OrigOutFile;
  if (OrigOutFile && ArData->unpackable) memcpy (OrigOutFile, OrigInFile, ArData->OrigSize);

  free_memory(ArData);
}


word CompressBlock (byte *pInput, word InBufferSize, byte *pOutput)
{
  ARDATA                ARData;

  memset(&ARData, 0, sizeof(ARDATA));
  ARData.Magic = 0xFB1BB100;

  if (!pInput || InBufferSize > 0x7ffa) return 0;

  ARData.InFile       = pInput;
  ARData.OrigSize     = InBufferSize;
  ARData.OutFile      = pOutput;

  encode(&ARData);
  if (ARData.unpackable && pOutput) memcpy (pOutput, pInput, InBufferSize);

  return ARData.CompSize;
}
