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

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <string.h>
#include <tap.h>
#include <tapapifix.h>
#include "inifile.h"

const int maxIniLength = 2048;

IniFile::IniFile()
{
	// default to unix style line endings
	strcpy( eol, "\n" );
}

IniFile::IniFile( const char* filename )
{
	// default to unix style line endings
	strcpy( eol, "\n" );
	Load( filename );
}


IniFile::~IniFile()
{
}


// Load the file
bool IniFile::Load( const char* filename )
{
	TYPE_File* file = TAP_Hdd_Fopen( (char*)filename );
	if ( !file )
		return false;

	// Load the file in one chunk
	dword length = TAP_Hdd_Flen( file );
	length = min( length, maxIniLength );

	string buffer;
	TAP_Hdd_Fread( buffer.getbuffer(length), 1, length , file );
	buffer.releasebuffer();
	TAP_Hdd_Fclose( file );
	// work out the default line ending
	int i = buffer.findfirstof( "\r\n" );
	if ( i > -1 )
	{
		eol[0] = buffer[i];
		eol[1] = buffer[i+1] == '\n' ? '\n' : '\0';
		eol[2] = '\0';
	}
	else
		strcpy( eol, "\n" );

	buffer.split( eol, line );

	return true;
}


// Save the file
bool IniFile::Save( const char* filename ) const
{
	if ( !TAP_Hdd_Exist( (char*)filename ) )
		TAP_Hdd_Create( (char*)filename, ATTR_NORMAL );

	TYPE_File* file = TAP_Hdd_Fopen( (char*)filename );
	if ( !file )
		return false;

	// calculate the required length of buffer
	int length = 0;
	for ( unsigned int u = 0; u < line.size(); ++u )
		length += line[u].size() + 2;
	// pad to 512 bytes to prevent the TAP API writing garbage to the file
	length += 512-length%512;

	string buffer;
	buffer.resize( length );
	for ( unsigned int u = 0; u < line.size(); ++u )
		buffer += line[u] + eol;
	while ( buffer.size() < length-1 )
		buffer += " ";
	TAP_Hdd_Fwrite( (void*)(const char*)buffer, 1, buffer.size(), file );
	TAP_Hdd_Fclose( file );

	return true;
}


// Get an integer value
bool IniFile::GetValue( const char* key, int& value ) const
{
	string result;
	if ( !FindKey( key, result ) )
		return false;
	value = atoi(result);
	return true;
}


// Get a boolean value
bool IniFile::GetValue( const char* key, bool& value ) const
{
	string result;
	if ( !FindKey( key, result ) )
		return false;
	value = result == "1" ? 1 : 0;
	return true;
}


// Set a string value
void IniFile::SetValue( const char* key, const char* value )
{
	string entry;
	entry.format( "%s=%s", key, value );

	int index = FindKey( key );
	if ( index == -1 )
		line.push_back( entry );
	else
		line[index] = entry;
}


// Set an integer value
void IniFile::SetValue( const char* key, int value )
{
	char buffer[16];
	sprintf(buffer, "%d", value);
	SetValue( key, buffer );
}


// Set a boolean value
void IniFile::SetValue( const char* key, bool value )
{
	SetValue( key, value ? "1" : "0" );
}


// Get the key/comment/section at of given line
string IniFile::GetKey( int index ) const
{
	int pos = line[index].findfirstof( "=" );
    return line[index].substr( 0, pos );
}


// Get the value of a given line
string IniFile::GetValue( int index ) const
{
	int pos = line[index].findfirstof( "=" );
	return index ? line[index].substr( pos+1 ) : string();
}


// Find a key and return its index
int IniFile::FindKey( const char* key ) const
{
	size_t keyLength = strlen( key );
	for ( unsigned int u = 0; u < line.size(); ++u )
	{
		if ( strnicmp( line[u], key, keyLength ) == 0 )
		{
			char c = line[u][keyLength];
			if ( c == '=' || c == '\0' )
			{
				return (int)u;
			}
		}
	}
	return -1;
}


// Find a key and return its value
bool IniFile::FindKey( const char* key, string& value ) const
{
	size_t keyLength = strlen( key );
	for ( unsigned int u = 0; u < line.size(); ++u )
	{
		if ( strnicmp( line[u], key, keyLength ) == 0 )
			if ( line[u][keyLength] == '=' )
			{
				value = &line[u][keyLength+1];
				return true;
			}
	}
	return false;
}
