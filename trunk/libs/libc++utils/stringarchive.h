/*
	Copyright (C) 2006 Robin Glover

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
#ifndef _cpputils_stringarchive_h
#define _cpputils_stringarchive_h
#include "tapstring.h"
#include <stdlib.h>
#include "tap.h"

class stringarchive
{
public:
	stringarchive(const string& data = "");

	stringarchive& operator << (bool bValue);
	stringarchive& operator << (int iValue);
	stringarchive& operator << (word wValue);
	stringarchive& operator << (const string& sValue);

	stringarchive& operator >> (bool& bValue);
	stringarchive& operator >> (int& iValue);
	stringarchive& operator >> (word& wValue);
	stringarchive& operator >> (string& sValue);

	const string& toString() const;

private:
	bool ReadIntValue(char cPrefix, char cPostFix, int& value);
	bool ValidToRead();
	void Advance();

	string m_sBackingData;
	bool m_bReading;
	int m_iReadPos;
};

#endif