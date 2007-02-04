/*
Copyright (C) 2006-2007 Simon Capewell

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


char outputFilename[256];
char outputDirectory[256];
FILE *outputFile;
FILE *headerFile;
int outputHeader = FALSE;
int verbose = FALSE;
int extract = FALSE;

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


unsigned short swap( unsigned short w )
{
	return ((w & 0xff) << 8) | (w >> 8);
}


const int CHANNEL_NAME_SIZE = 23;

// logo width and height
const int LOGO_WIDTH = 60;
const int LOGO_HEIGHT = 38;
#define LOGO_DATA_SIZE  60 * (38+1) * 2

int ProcessImage( char* filename )
{
	FREE_IMAGE_FORMAT fif;
	FIBITMAP *dib, *dibAlpha;
	char channelName[100];
	int width, height;
	int bufferSize, i;
	BYTE *buffer, *bufferAlpha;
	WORD *p;
	BYTE *a;

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

	width = FreeImage_GetWidth( dib );
	height = FreeImage_GetHeight( dib );

	if ( width != LOGO_WIDTH || height != LOGO_HEIGHT )
	{
		fprintf( stderr, "ignoring %s. Image is the wrong size\n", filename );
		return 1;
	}

	// Get the raw bits in RGBA format
	dibAlpha = FreeImage_ConvertTo32Bits( dib );
	bufferSize = width*height*4;
	bufferAlpha = (BYTE*)malloc( bufferSize );
	memset( bufferAlpha, 0, bufferSize );
	FreeImage_ConvertToRawBits( bufferAlpha, dibAlpha, 4*width, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE );
	FreeImage_Unload( dibAlpha );

	// Get the raw bits in RGB555 format
	bufferSize = width*height*2;
	buffer = (BYTE*)malloc( bufferSize );
	// ensure that any padding bytes are set to zero
	memset( buffer, 0, bufferSize );
	FreeImage_ConvertToRawBits( buffer, dib, 2*width, 16, FI16_555_RED_MASK,FI16_555_GREEN_MASK,FI16_555_BLUE_MASK, FALSE );
	FreeImage_Unload( dib );

	// Swap words for MIPs big endian format
	p = (WORD*)buffer;
	a = (BYTE*)bufferAlpha;
	for ( i = 0; i < bufferSize; i+=2 )
	{
		if ( a[FI_RGBA_ALPHA] != 0 )
			*p = 0x8000 | *p;
		*p = swap(*p);
		++p;
		a+=4;
	}

	strncpy( channelName, GetFilename( filename ), 100 );
	channelName[99] = 0;
	StripFilenameExtension( channelName );
	fwrite( channelName, CHANNEL_NAME_SIZE, 1, outputFile );		// Write the channel name
	fwrite( buffer, bufferSize, 1, outputFile );					// Write the bitmap
	// Pad to 39 lines
	bufferSize = LOGO_WIDTH * 2;
	memset(buffer, 0x8000, bufferSize);
	fwrite( buffer, bufferSize, 1, outputFile );

	if ( buffer )
		free( buffer );
	if ( bufferAlpha )
		free( bufferAlpha );

	return TRUE;
}


int ProcessDat( char* filename )
{
	FILE *datFile;
	char path[1024];
	char* channelName;
	BYTE buffer[LOGO_DATA_SIZE];
	WORD *p;

	FIBITMAP *dib, *png;
	int i, x,y;

	if ( verbose )
		fprintf( stderr, "Processing %s\n", filename );

	strcpy( path, outputDirectory );
	if ( strlen(path) != 0 )
		strcat( path, "/" );
	channelName = path + strlen(path);
	datFile = fopen( filename, "rb" );
	if ( datFile == 0 )
	{
		fprintf( stderr, "%s could not be opened\n", filename );
		return FALSE;
	}

	while ( !feof( datFile ) )
	{
		if (fread( channelName, CHANNEL_NAME_SIZE, 1, datFile ) == 0)		// skip the channel name
			break;
		channelName[CHANNEL_NAME_SIZE] = '\0';
		if ( verbose )
			fprintf( stderr, "%s\n", channelName );
		fread( buffer, LOGO_DATA_SIZE, 1, datFile );		// read logos one block at a time
		p = (WORD*)buffer;
		for ( i = 0; i < LOGO_DATA_SIZE; i+=2 )
		{
			*p = swap(*p);
			++p;
		}

		dib = FreeImage_ConvertFromRawBits( buffer, LOGO_WIDTH, LOGO_HEIGHT, 2*LOGO_WIDTH, 16, FI16_555_RED_MASK,FI16_555_GREEN_MASK,FI16_555_BLUE_MASK, FALSE );
		strcat( channelName, ".png" );

		png = FreeImage_ConvertTo32Bits( dib );
		FreeImage_Unload( dib );

		p = (WORD*)buffer;
		for ( y = LOGO_HEIGHT-1; y >= 0; --y )
			for ( x = 0; x < LOGO_WIDTH; ++x )
			{
				RGBQUAD rgb;
				FreeImage_GetPixelColor( png, x,y, &rgb );
				rgb.rgbReserved = (*p & 0x8000) ? 255 : 0;
				FreeImage_SetPixelColor( png, x,y, &rgb );
				++p;
			}

		if ( !FreeImage_Save( FIF_PNG, png, path, 0 ) )
			fprintf( stderr, "%s cannot be saved\n", channelName );
		FreeImage_Unload( png );
	}

	fclose( datFile );

	return TRUE;
}


int ProcessFile( char* filename )
{
	if (extract)
		return ProcessDat( filename );
	return ProcessImage( filename );
}


// Display command line parameters
int Usage()
{
	printf("MakeLogos 1.2, a logo archive builder/extractor for Topfield TAPs\n");
	printf("Usage: makelogos <image filenames> [options]\n");
	printf("\t<image filenames> may contain wildcards\n");
	printf("\t-o\tSpecify an output filename for build mode (default: logo.dat)\n");
	printf("\t-d\tSpecify an output directory\n");
	printf("\t-v\tVerbose output\n");
	printf("\t-x\tExtract graphics\n");
	return 0;
}

int main(int argc, char* argv[])
{
	int i;
	int hasFilename = FALSE;
	int fileCount;

	strcpy( outputFilename, "logo.dat" );

	for ( i = 1; i < argc; ++i )
	{
		if ( argv[i][0] == '-' || argv[i][0] == '/' )
		{
			switch ( argv[i][1] | 0x20 )
			{
			case 'd':
				if ( argv[i][2] != 0 )
					strncpy( outputDirectory, argv[i]+2, sizeof(outputDirectory)-1 );
				outputDirectory[sizeof(outputDirectory)-1] = 0;
				break;
			case 'o':
				if ( argv[i][2] != 0 )
					strncpy( outputFilename, argv[i]+2, sizeof(outputFilename)-1 );
				outputFilename[sizeof(outputFilename)-1] = 0;
				break;
			case 'v':
				verbose = TRUE;
				break;
			case 'x':
				extract = TRUE;
				break;
			case '?':
			case 'h':
				return Usage();
			default:
				return Usage();
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

	if (!extract)
	{
		char path[1024];
		strcpy( path, outputDirectory );
		if ( strlen(path) != 0 )
			strcat( path, "/" );
		strcat( path, outputFilename );

		// Create output file
		outputFile = fopen( path, "wb" );
		if ( outputFile == 0 )
		{
			fprintf( stderr, "Can't open output file %s\n", outputFilename );
			return 1;
		}
		if ( verbose )
			fprintf( stderr, "Created %s\n", outputFilename );
	}

	fileCount = 0;
	for ( i = 1; i < argc; ++i )
	{
		if ( argv[i][0] != '-' && argv[i][0] != '/' )
		{
			#ifdef WIN32
			struct _finddata_t fd;
			intptr_t fh = _findfirst( argv[i], &fd );
			if ( fh == -1 )
			{
				if ( errno == EINVAL )
					fprintf( stderr, "%s is not a valid wildcard\n", argv[i] );
				else if ( errno == ENOENT && verbose )
					fprintf( stderr, "No files found matching %s\n", argv[i] );
			}
			else
			{
				while ( fh > 0 )
				{
					if ( fd.attrib != _A_SUBDIR )
					{
						if ( ProcessFile( fd.name ) )
							++fileCount;
					}

					if ( _findnext( fh, &fd ) != 0 )
						break;
				}
				_findclose( fh );
			}
			#else
			if (ProcessFile(argv[i]))
				++fileCount;
			#endif
		}
	}

	fprintf( stderr, "Processed %d files\n", fileCount );

	if ( !extract )
	{
		if ( outputFile )
			fclose( outputFile );
	}

	return 0;
}
