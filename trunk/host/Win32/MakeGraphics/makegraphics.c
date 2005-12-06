/*
Copyright (C) 2005 Simon Capewell

This file is part of the TAPs for Topfield PVRs project.
	http://tap.berlios.de/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <errno.h>
#include "FreeImage.h"
#include "tfdcompress.h"


typedef enum
{
	COMPRESSION_NONE,
	COMPRESSION_TFD
} Compression;

Compression compression = COMPRESSION_TFD;
char outputFilename[256];
FILE *outputFile;
FILE *headerFile;
int outputHeader = FALSE;
int verbose = TRUE;

// Returns a pointer to the beginning of the filename
char* GetFilename(char* path)
{
	char* c = path+strlen(path);
	while ( c != path && *c != '\\' && *c != '/' )
		--c;
	return c == path ? path : c+1;
}


// Returns a pointer to the beginning of the filename extension (excluding .)
const char* GetFilenameExtension(const char* path)
{
	const char* c = path + strlen(path);
	while ( c != path && *c != '.' && *c != '\\' && *c != '/' )
		--c;
	return *c == '.' ? c+1 : path+strlen(path);
}


// Strips the filename
void StripFilename(char* path)
{
	char *c = path + strlen(path);
	while ( c != path && *c != '\\' && *c != '/' )
		--c;
	if ( *c != '\\' && *c != '/' )
		*c = 0;
}


// Strips the filename extension
void StripFilenameExtension(char* path)
{
	char *c=path + strlen(path);
	while ( c != path && *c != '.' && *c != '\\' && *c != '/' )
		--c;
	if (*c == '.' )
		*c = 0;
}


void HexDump( BYTE* buffer, int bufferLength )
{
	int colCount = 0;
	fprintf( outputFile, "\t" );
	while ( bufferLength-- )
	{
		fprintf( outputFile, "0x%02x%s", *(buffer++), bufferLength ? "," : "" );
		if ( bufferLength && ++colCount == 0x10 )
		{
			colCount = 0;
			fprintf( outputFile, "\n\t" );
		}
	}
	fprintf( outputFile, "\n" );
}

unsigned short swap( unsigned short w );



int ProcessImage( char* filename )
{
	FREE_IMAGE_FORMAT fif;
	FIBITMAP *dib;
	char graphicName[100];
	int width, height;
	RGBQUAD rgb;
	int bufferSize, x,y;
	BYTE* buffer;
	WORD *p;

	if ( verbose )
		fprintf( stderr, "Processing %s\n", filename );

	// Check the image format can be processed
	fif = FreeImage_GetFileType( filename, 0 );
	if ( fif == FIF_UNKNOWN )
	{
		fprintf( stderr, "%s is not an image or cannot be processed\n", filename );
		return FALSE;
	}
	// Load the image into memory
	dib = FreeImage_Load( fif, filename, 0);
	if ( dib == 0 )
	{
		fprintf( stderr, "%s could not be loaded\n", filename );
		return FALSE;
	}

	strncpy( graphicName, GetFilename( filename ), 100 );
	graphicName[99] = 0;
	StripFilenameExtension( graphicName );
	strlwr( graphicName );

	width = FreeImage_GetWidth( dib );
	height = FreeImage_GetHeight( dib );

	// Convert RGB to ARGB555
	bufferSize = (width+1)*height*2;
	buffer = (BYTE*)malloc( bufferSize );
	p = (WORD*)buffer;
	for ( y = height-1; y >= 0; --y )
	{
		for ( x = 0; x < width; ++x )
		{
			FreeImage_GetPixelColor( dib, x,y, &rgb );
			rgb.rgbRed = (rgb.rgbRed+3) >> 3;
			rgb.rgbGreen = (rgb.rgbGreen+3) >> 3;
			rgb.rgbBlue = (rgb.rgbBlue+3) >> 3;
			*p++ = swap(0x8000 + (((min(31,rgb.rgbRed) << 5) + min(31,rgb.rgbGreen)) << 5) + min(31,rgb.rgbBlue));
		}
		// odd widths are padded with a black pixel on the right side of the image
		if ( width % 2 )
			*p++ = swap(0x8000);
	}

	// make new image width even
	width += width % 2;

	FreeImage_Unload( dib );

	if ( !outputHeader )
	{
		char filename[256];
		sprintf( filename, "%s.c", graphicName );
		outputFile = fopen( filename, "wb" );
		if ( outputFile == 0 )
		{
			fprintf( stderr, "Can't open output file %s\n", filename );
			return FALSE;
		}
		fprintf( outputFile, "#include <tap.h>\n\n" );
	}

	switch ( compression )
	{
	case COMPRESSION_NONE:
	{
		if ( headerFile )
		{
			fprintf( headerFile, "#define %s_Width %d;\n", graphicName, width );
			fprintf( headerFile, "#define %s_Height %d;\n", graphicName, height );
			fprintf( headerFile, "extern word _%sData[];\n", graphicName );
		}
		else
		{
			fprintf( outputFile, "#define %s_WIDTH %d;\n", graphicName, width );
			fprintf( outputFile, "#define %s_HEIGHT %d;\n", graphicName, height );
		}
		fprintf( outputFile, "\n" );
		fprintf( outputFile, "word %sData[] = \n", graphicName );
		fprintf( outputFile, "{\n" );
		HexDump( buffer, bufferSize );
		fprintf( outputFile, "};\n" );
		fprintf( outputFile, "\n" );
		break;
	}
	case COMPRESSION_TFD:
	{
		BYTE *output = (BYTE*)malloc( bufferSize );
		int compressed = tfdcompress( buffer, bufferSize, output );
		fprintf( outputFile, "byte _%sCpm[] = \n{\n", graphicName );
		HexDump( output, compressed );
		fprintf( outputFile, "};\n" );
		fprintf( outputFile, "TYPE_GrData _%sGd = { 1, 0, OSD_1555, COMPRESS_Tfp, _%sCpm, %d, %d, %d };\n\n", graphicName, graphicName, bufferSize, width, height );
		if ( headerFile )
		{
			fprintf( headerFile, "extern TYPE_GrData _%sGd;\n", graphicName );
		}
		free( output );
		break;
	}
	}

	if ( !outputHeader )
		fclose( outputFile );

	if ( buffer )
		free( buffer );

	return TRUE;
}


// Command line parameters
// makegraphics <filename(s)> <switches>
// -h generate header file
// -g generate gd file
// -p generate pixmap file
// -o:filename generate a single output file
// -v verbose

int Usage()
{
	printf("Make Graphics 1.0, a graphics to source converter for Topfield TAPs\n");
	printf("Usage: makegraphics <image filenames> [options]\n");
	printf("\t<image filenames> may contain wildcards\n");
	printf("\t-g\tGenerate compressed gd format files\n");
	printf("\t-p\tGenerate pixmap uncompressed format files\n");
	printf("\t-o\tSpecify a single output file and generate a matching .h\n");
	printf("\t-v\tVerbose output\n");
	return 0;
}

int main(int argc, char* argv[])
{
	int i;
	int hasFilename = FALSE;
	int fileCount;

	for ( i = 1; i < argc; ++i )
	{
		if ( argv[i][0] == '-' || argv[i][0] == '/' )
		{
			switch ( argv[i][1] | 0x20 )
			{
			case 'g':
				compression = COMPRESSION_TFD;
				break;
			case 'p':
				compression = COMPRESSION_NONE;
				break;
			case 'o':
				outputHeader = TRUE;
				if ( argv[i][2] == 0 )
					strcpy( outputFilename, "graphics.c" );
				else
					strncpy( outputFilename, argv[i]+2, sizeof(outputFilename)-1 );
				outputFilename[sizeof(outputFilename)-1] = 0;
				break;
			case 'v':
				verbose = TRUE;
				break;
			case '?':
			case 'h':
				return Usage();
			default:
				break;
			}
		}
		else
		{
			hasFilename = TRUE;
		}
	}
	if ( !hasFilename )
	{
		fprintf( stderr, "No filenames were specified\n" );
		return Usage();
	}

	FreeImage_Initialise( FALSE );

	if ( outputHeader )
	{
		// Create single output file
		outputFile = fopen( outputFilename, "wb" );
		if ( outputFile == 0 )
		{
			fprintf( stderr, "Can't open output file %s\n", outputFilename );
			return 1;
		}
		fprintf( outputFile, "#include <tap.h>\n\n" );

		// Create matching header file
		StripFilenameExtension( outputFilename );
		strcat( outputFilename, ".h" );
		headerFile = fopen( outputFilename, "wb" );
		if ( headerFile == 0 )
		{
			fprintf( stderr, "Can't open output file %s\n", outputFilename );
			return 1;
		}
		StripFilenameExtension( outputFilename );
		fprintf( headerFile, "#ifndef __%s_H\n#define __%s_H\n\n", outputFilename, outputFilename );
		fprintf( headerFile, "#include <tap.h>\n\n" );
	}

	fileCount = 0;
	for ( i = 1; i < argc; ++i )
	{
		if ( argv[i][0] != '-' && argv[i][0] != '/' )
		{
			struct _finddata_t fd;
			intptr_t fh = _findfirst( argv[i], &fd );
			if ( fh == -1 )
			{
				if ( errno == EINVAL )
					fprintf( stderr, "%s is not a valid wildcard\n", argv[i] );
				else if ( errno == ENOENT )
					fprintf( stderr, "No files found matching %s\n", argv[i] );
			}
			else
			{
				while ( fh > 0 )
				{
					if ( fd.attrib != _A_SUBDIR )
					{
						if ( ProcessImage( fd.name ) )
							++fileCount;
					}

					if ( _findnext( fh, &fd ) != 0 )
						break;
				}
				_findclose( fh );
			}
		}
	}

	if ( verbose )
		fprintf( stderr, "Processed %d files\n", fileCount );

	if ( outputFile )
		fclose( outputFile );
	if ( headerFile )
	{
		fprintf( headerFile, "\n#endif\n" );
		fclose( headerFile );
	}

	return 0;
}
