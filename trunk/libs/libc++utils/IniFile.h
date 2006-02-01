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

#ifndef __INIFILE_H
#define __INIFILE_H


#include "taparray.h"
#include "tapstring.h"


class IniFile
{
public:
	IniFile();
	IniFile( const char* filename );
	virtual ~IniFile();

	bool Load( const char* filename );
	bool Save( const char* filename ) const;

	// Get a key returning success
	bool GetValue( const char* key, string& value ) const				{ return FindKey( key, value ); }
	bool GetValue( const char* key, int& value ) const;
	bool GetValue( const char* key, short& value ) const;
	bool GetValue( const char* key, bool& value ) const;

	void SetValue( const char* key, const char* value );
	void SetValue( const char* key, int value );
	void SetValue( const char* key, bool value );

	string GetKey( int index ) const;
	string GetValue( int index ) const;

	int FindKey( const char* key ) const;
	int GetKeyCount() const; 

private:
	bool FindKey( const char* key, string& value ) const;

	array<string> line;
};


#endif
