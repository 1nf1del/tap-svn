/***********************************************************
	tfdpack.c -- main file
***********************************************************/

static char *usage =
	"##--------TFDPACK----------###########################################\n"
	"#  -- File Packer/UnPacker for Topfield firmware--                   #\n"
	"#      based on AR002 archiver written by Haruhiko Okumura           #\n"
	"#      by David Wright  03/12/2005 : dww@berkswich.co.uk             #\n"
	"#                                                                    #\n"
	"#  Usage: tfdpack  <Command> <Archive> <File> <Model ID>             #\n"
	"#                                                                    #\n"
	"# Commands:                                                          #\n"
	"#   a: Compress firmware file to *.tfd archive for a specified model #\n"
	"#   x: Extract firmware from *.tfd  to a *.asm file                  #\n"
	"#                                                                    #\n"
	"#  Model ID = 456 ~TF5800PVR     406 ~ TF5000/5500  etc              #\n"
	"#  65535 = Graphic Display File ?                                    #\n"
	"#                                                                    #\n"
	"#  Default values coded in programme:                                #\n"
	"#  Blocktype = 0001 ~firmware     Blocksize = max (32762bytes)       #\n"
	"#                                                                    #\n"
	"# Example: tfdpack a tf001.tfd tfSept05.asm 456        <---          #\n"
	"# Example: tfdpack x tf001.tfd tfexpanded.asm          --->          #\n"
	"#                                                                    #\n"
	"######################################################################\n";

/*** You may copy, distribute, and rewrite this program freely***********

Compile this project as a DOS Compact model. 

File ar.c from AR002 has been modified to use a minimum header for each subfile
and to split a large file into blocks which are treated as if they were files
before packing into .tfd file.    CRC checking of the expanded file is not yet
implemented but CRC checking is used for the compression routine.

Structure of archive block
----- group header ---------------------------
	Size is 10 bytes (high order byte first)
2 Group header length -2 = 10 -2 = 0008
2 CRC16(System ID + Something + No of Blocks)
2 System ID ( 456 or TF5800) * Other codes for TF5*** series PVR
2 Something = 0001 (Unknown purpose)
2 No of Blocks
----- block header ---------------------------
	Size is 8 bytes (high order byte first)
 2	Datablock code ( = Compressed filesize +6 = true blocksize -2)
 2	CRC16(Block Type + Uncompressed size + Data)
 2	Block Type  ( 00 01 for Firmware, 00 02 for Flash, 00 03 EEPROM)
 2	Uncompressed size ( Normal max of 32762 bytes) datalength only 
----- compressed file ------------------------
***********************************************************/

/* Uncompressed file is expanded to mirror loading to a start address of
	0x80000000.  The TF code uses reference tables for procedures and
	strings. The TF loader is unknown but code software dump from the
	TF5800PVR corresponds to this expansion. The file structure information
	is based on a detailed analysis by Firebird */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "ar.h"

/* #define FNAME_MAX (255 - 25) *//* filename  for archivedTFD */
#define BUFFER_SIZE (32762) /* using precise size buffers for blocks */


FILE *sourcefile, *xxfile;
uint 	blockcount;		/* number of blocks*/
ulong length;  /*filelength*/

int unpackable;            /* global, set in io.c */
ulong compsize, origsize;  /* global */

static uint tembit;
static uint counter;
static char  *filename;
static uchar buffer[DICSIZ];
static uchar header[255], headerA[10];
static uchar headersize ;

static uint  file_crc, Header_crc;
static char  *temp_name;
static ulong  Pseudosize,PseudosizeA,blocktype;
static ulong Model_ID, Something;

static uint ratio(ulong a, ulong b)  /* [(1000a + [b/2]) / b] */
 {
	int i;

	for (i = 0; i < 3; i++)
		if (a <= ULONG_MAX / 10) a *= 10;  else b /= 10;
	if ((ulong)(a + (b >> 1)) < a) {  a >>= 1;  b >>= 1;  }
	if (b == 0) return 0;
	return (uint)((a + (b >> 1)) / b);
}

static void put_to_header(int i, int n, ulong x)
{  tembit = 0;
	while (--n >= 0) {
		header[(i + n)] = (uchar)((uint)x & 0xFF);  x >>= 8;
		 /* big endian  -- for mips processor*/ /* blockheader*/

	}
}

static ulong get_from_header(int i, int n)
{
	ulong s;

	s = 0;
	while (--n >= 0) s = (s << 8) + header[(i ++)];
	 /* big endian  -- for mips processor--  */ /* blockheader*/

	return s;

}
static void put_to_headerA(int i, int n, ulong x)
{  tembit = 0;
	while (--n >= 0) {
		headerA[(i + n)] = (uchar)((uint)x & 0xFF);  x >>= 8;
		 /* big endian  -- for mips processor*/  /* preheader*/

	}
}

static ulong get_from_headerA(int i, int n)
{
	ulong s;

	s = 0;
	while (--n >= 0) s = (s << 8) + headerA[(i ++)];
	 /* big endian  -- for mips processor--  */  /* preheader*/

	return s;

}


static int read_header(void)
{
	fread_crc(header, 8, arcfile);   /* block header  - use  for TFD*/

	Pseudosize = get_from_header(0, 2);
	compsize = (Pseudosize -6);
	origsize = get_from_header(6, 2);

	file_crc = (uint)get_from_header(2, 2);
	blocktype = get_from_header(4,2);

	return 1;  /* success */
 }
static int read_headerA(void)
{

	 fread_crc(headerA, 10, arcfile);   /*  read preheader*/

	PseudosizeA = get_from_headerA(0, 2);
	Header_crc = (uint)get_from_headerA(2, 2);
	Model_ID = get_from_headerA(4,2);
	Something = get_from_headerA(6, 2);
	blockcount = (uint)get_from_headerA(8,2);
	printf("size %ld\n", PseudosizeA);
	return 1;  /* success */
 }


static void write_header(void)
{
	blocktype = 0x0001;
	Pseudosize = (compsize +  0x0006);
	put_to_header(0, 2, (ulong)Pseudosize);
	put_to_header(2, 2, (ulong)file_crc);
	put_to_header(4, 2, (ulong)blocktype);
	put_to_header(6, 2, (ulong)origsize);

	fwrite_crc(header, headersize, outfile);
}

 static void skip(void)
{
	fseek(arcfile, compsize, SEEK_CUR);
	printf(" file skipped ");
	}

static void store(void)
{
	uint n;

	origsize = 0;
	crc = INIT_CRC;
	while ((n = fread((char *)buffer, 1, DICSIZ, infile)) != 0) {
		fwrite_crc(buffer, n, outfile);  origsize += n;
	}
	compsize = origsize;
}

static int add(int replace_flag)
{
	uchar *crcbuffer;
	ulong headerpos, arcpos, filesizetransfer;
	uint r,i  ;

	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Can't open  %s\n", filename);
		return 0;  /* failure */
	}
	if (replace_flag) {
		printf("Replacing %s ", filename);  skip();
	} else
		printf("Adding");
		/*printf("Adding %s ", filename);  */
	/* need to set to end of existing file first*/

	headerpos = ftell(outfile);
	headersize = 8 ;
	for (i = 0; i < 8; i++ ) fputc(0 , outfile);
	/*write temporary header to provide space */
	/* identify end of header -start of block compressed data*/
	arcpos = ftell(outfile);

	origsize = compsize = 0;  unpackable = 0;
	crc = INIT_CRC;

	encode();            /* crc calculated in this loop   */
	if (unpackable) {
		/* block is copied without compression*/
		rewind(infile);
		fseek(outfile, arcpos, SEEK_SET);
		printf("unpackable flag set \n                         ");
		store();
	}

	fclose(infile);
	blocktype = 0x0001;
	Pseudosize = (compsize +  0x0006);
	put_to_header(0, 2, (ulong)Pseudosize);
	put_to_header(2, 2, (ulong)file_crc); /* orig file -rewritten below */
	put_to_header(4, 2, (ulong)blocktype);
	put_to_header(6, 2, (ulong)origsize);

	fseek(outfile, headerpos, SEEK_SET);
	/* finds start of header as declared above and overwrites*/
	write_header();  /* true header  - overwrites blank data */
	/*reset to declared file size in header*/
	filesizetransfer = origsize;

	fseek(outfile, headerpos + 4, SEEK_SET);
	/* determine crc of compressed file + 4 header characters*/
	crc = INIT_CRC;
	crcbuffer = (uchar*)malloc (32762);
	/* sized to be big enough for compressed block*/
	fread_crc(crcbuffer,(compsize + 4),outfile);
	/*there is a conversion from long to int above but data is compatible*/
	free(crcbuffer);
	file_crc = crc ^ INIT_CRC;
	origsize = filesizetransfer; /*reset original file size */
	put_to_header(2, 2, (ulong)file_crc);
	fseek(outfile, headerpos, SEEK_SET);
	write_header();  /* update file crc field in header*/
	fseek(outfile, 0L, SEEK_END);
	  /* need to go to end of file to ensure multiple files are appended*/
		/* and not overwritten !*/

	r = ratio(compsize, origsize);

	printf(" Offset: %04lX" , Pseudosize);
	printf(" CRC: %04X ", file_crc );
	printf(" %d.%d%%\n", r / 10, r % 10);
	/* printf(" %04X  \n" , origsize );   */

	return 1;  /* success */
 }

int get_line(char *s, int n)
{
	int i, c;

	i = 0;
	while ((c = getchar()) != EOF && c != '\n')
		if (i < n) s[i++] = (char)c;
	s[i] = '\0';
	return i;
}

static void extract(int to_file)
{
	int n, method ;

				if (to_file) {
		while ((outfile = fopen(filename, "a+b")) == NULL) {
			fprintf(stderr, "Can't open %s\n New filename: ", filename);


		}
		printf("Unpacking block %d", counter);
	} else {
		outfile = stdout;
		printf("===== %s =====\n", filename);
	}
	crc = INIT_CRC;
	method = 5;      /* huffman- LZ percolating & slide algorithm */



		crc = INIT_CRC;
		printf(" origsize %04lx", origsize);
		printf(" compsize %04lx", compsize);
		printf(" type %04lx", blocktype);

			  if (compsize >= origsize) method = 0;  /* block not compressed*/
				if (method == 5)   decode_start();
				while (origsize != 0) {
					n = (uint)((origsize > DICSIZ) ? DICSIZ : origsize);
									if (method == 5) decode(n, buffer);
					else  {
					 if (fread((char *)buffer, 1, n, arcfile) != n)
					error("Can't read");
									 }
					fwrite_crc(buffer, n, outfile);
					if (outfile != stdout) putc('.', stderr);
					origsize -= n;

			}

	if (method == 0) printf("\n Block %d was not compressed ", counter);
	if (to_file) fclose(outfile);  else outfile = NULL;
	printf("\n");

/*	if ((crc ^ INIT_CRC) != file_crc)
		fprintf(stderr, "CRC error\n");    */
	  /* this is not the right CRC - fix  much later*/
}

static void exitfunc(void)
{
	fclose(outfile);  remove(temp_name);
}


int main(int argc, char *argv[])
{
	int i, cmd;
	uint	readfsize;		/* number of bytes on last read */
  uchar	*parsebuffer;	/* buffer for data */


	/* Check command line arguments. */
	if (argc < 3
	 || argv[1][1] != '\0'
	 || ! strchr("AX", cmd = toupper(argv[1][0]))
	 || (argc == 3 && strchr("A", cmd)))
		error(usage);


	/* Open temporary file. */
	if (strchr("A", cmd))
		{
			temp_name = tmpnam(NULL);
			outfile = fopen(temp_name, "w+b");/* file is reread to get crc data*/
			if (outfile == NULL) error("Can't open temporary file");
			for (i = 0; i < 10; i++ ) fputc(0 , outfile);
			/*write temporary preheader to provide space */

			atexit(exitfunc);
		}
		else temp_name = NULL;

	  /*========================================*/
			if (cmd =='X'){
          	/* Open archive. */
	arcfile = fopen(argv[2], "rb");
	if (arcfile == NULL && cmd != 'X')
		error("Can't open archive '%s'", argv[2]);


				filename = argv[3];
				read_headerA();

				for (i = 1; i < blockcount + 1; i++)
				{
				read_header();
				counter = i;
				extract(cmd =='X');
				}
				  printf("\n");
				  printf("Model ID : %ld ",Model_ID);
				  printf(" Total:  %d Blocks processed \n ",counter);
				}
		/*==========================================*/
			make_crctable();
	if (cmd == 'A')
		{
					filename = "$$toppy$.$$$";
					blockcount =1;
               	if ((sourcefile = fopen(argv[3], "rb")) == NULL)
						{
							fprintf(stderr, "Can't open  %s\n", argv[3]);
							return 0;  /* failure */
						}

			while(1)
				{
					xxfile = fopen("$$toppy$.$$$", "wb");
				if (xxfile == NULL) error(" Error: unable to open %s\n",argv[2]);

						parsebuffer = (uchar*) malloc (32762);
						readfsize = fread(parsebuffer,1,32762,sourcefile);
						if (readfsize == 0) 	break;
						fwrite(parsebuffer,1,readfsize,xxfile);
						free (parsebuffer);

						printf("Block %d ",blockcount);
						blockcount = blockcount + 1;
						fseek(xxfile,0L,SEEK_END);
						length = ftell(xxfile);

						printf("Original length: %ld bytes ", length);

						fclose(xxfile);

						add(0);

					}
							(void) fclose(sourcefile);


				if (temp_name != NULL )
				/* ====Preheader ==========*/
				{ 	Something = 0x0001;     /* Unknown marker */
				  /*	Model_ID = 0x01C8; */
					Model_ID = atol(argv[4]);
					put_to_headerA(4, 2, (ulong)Model_ID);
					put_to_headerA(6, 2, (ulong)Something);
					put_to_headerA(8, 2, (ulong)blockcount -1);
					crc = INIT_CRC;
					fseek(outfile, 0L ,SEEK_SET);
					fwrite_crc(headerA, 10, outfile);
					/* calc on last 6 bytes for TFD but first 4 bytes are null*/
					Header_crc = crc ^ INIT_CRC;

					PseudosizeA = 0x0008; /* preheader size -2 */


					put_to_headerA(0, 2, (ulong)PseudosizeA);
					put_to_headerA(2, 2, (ulong)Header_crc);
					fseek(outfile, 0L ,SEEK_SET);
					fwrite(headerA,1,10,outfile);
					/*update preheader with actual data*/

				/* end of archive */
				if (ferror(outfile) || fclose(outfile) == EOF)
				error("Can't write");
				remove(argv[2]);  rename(temp_name, argv[2]);
				printf("Archived file name : %s ", argv[2]);
				printf("    Model ID : %ld ", Model_ID);
				printf("  %d Total blocks added\n", (blockcount - 1));

				}


		}

	  if (cmd == 'A') {remove(filename); }
	return EXIT_SUCCESS;
}

