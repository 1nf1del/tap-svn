/*
	Copyright (C) 2005 Robin Glover

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


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "taparray.h"
#include "tap.h"
#include ".\tapstring.h"

string::string(void) 
{
	init();
}

string::~string(void)
{
	clear();
}

string::string(const char* pData, int iLen)
{
	init();
	assign(pData, iLen);
}

string::string(const string& other)
{
	init();
	assign(other);
}

string::string(char c)
{
	init();
	assign(c);
}

bool string::operator==(const string& right) const
{
	return (compare(right) == 0);
}

bool string::operator==(const char* pRight) const
{
	return (compare(pRight) == 0);
}

bool string::operator!=(const string& right) const
{
	return !(*this == right);
}

bool string::operator!=(const char* pRight) const
{
	return !(*this == pRight);
}


string& string::operator=(const string& right)
{
	if (this != &right)
		assign(right);
	return *this;
}

string& string::operator=(const char* pData)
{
	assign(pData);
	return *this;
}

string& string::operator=(char c)
{
	assign(c);
	return *this;
}

string& string::operator+=(const string& right)
{
	if (this == &right)
	{
		string copy = right;
		append(copy, copy.size());
	}
	else
	{
		append(right, right.size());
	}
	return *this;
}

string& string::operator+=(const char* pData)
{
	append(pData, strlen(pData));
	return *this;
}

string& string::operator+=(char c)
{
	append(&c, 1);
	return *this;
}

bool string::operator<(const string& right) const
{
	return strcmp(getstr(), right.getstr()) < 0;
}


string::operator const char*() const
{
	return getstr();
}
	
const char& string::operator[](int i) const
{
	return getstr()[i];
}

char& string::operator[](int i)
{
	return getstr()[i];
}

const char& string::operator[](unsigned int i) const
{
	return getstr()[i];
}

char& string::operator[](unsigned int i)
{
	return getstr()[i];
}


const char* string::c_str() const
{
	return getstr();
}

char* string::getbuffer( int size )
{
	resize( size );
	return getstr();
}

int string::releasebuffer()
{
	getstr()[m_iReserved-1] = 0; // null terminate the data at the maximum length requested
	m_iLen = strlen( getstr() );
	return m_iLen;
}

string string::operator+(const string& right) const
{
	string result = *this;
	result += right;
	return result;
}

string string::operator+(const char* pRight) const
{
	string result = *this;
	result += pRight;
	return result;
}

string string::operator+(char c) const
{
	string result = *this;
	result += c;
	return result;
}

string string::substr(int offs , int count ) const
{
	return string(getstr() + offs, min(count, size() - offs));
}

int string::compare(const char* pOther) const
{
	return strncmp(getstr(), pOther, max(m_iLen, strlen(pOther)));
}

int string::compareNoCase(const char* pOther) const
{
	return strnicmp(getstr(), pOther, max(m_iLen, strlen(pOther)));
}

int string::compare(const string& other) const
{
	return strncmp(getstr(), other, max(m_iLen, other.m_iLen));
}

int string::compareNoCase(const string& other) const
{
	return strnicmp(getstr(), other, max(m_iLen, other.m_iLen));
}


int string::find(const string& toFind, int iStartAfterChar) const
{
	return indexof(toFind, iStartAfterChar);
}

int string::findfirstof(const string& toFind, int iStartAfterChar) const
{
	return findfirstof(toFind.c_str(), iStartAfterChar);
}

int string::findfirstof(const char* pToFind, int iStartAfterChar) const
{
	int result = strcspn(getstr() + iStartAfterChar + 1, pToFind) + iStartAfterChar + 1;
	return result >= size() ? -1 : result;
}
	
int string::find(const char* pToFind, int iStartAfterChar) const
{
	return indexof(pToFind, iStartAfterChar);
}

int string::find(char c, int iStartAfterChar) const
{
	return indexof(c, iStartAfterChar);
}

bool string::empty() const
{
	return m_iLen == 0;
}

string operator+(const char* pLeft, const string& right)
{
	string result(pLeft);
	result += right;
	return result;
}

string operator+(char left, const string& right)
{
	string result(left);
	result += right;
	return result;
}


int string::size() const
{
	return m_iLen;
}

void string::resize(unsigned int newSize)
{
	if (newSize>m_iLen)
	{
		grow(newSize);
	}
	else
	{
		shrink(newSize);
	}
}

int string::replace(char replace, char with)
{
	int iCount = 0;
	int iPos = -1;
	while ((iPos = indexof(replace, iPos))!=-1)
	{
		getstr()[iPos]=with;
		iCount++;
	}

	return iCount;
}

string string::trim() const
{
	int iCut1 = 0;
	int iCut2 = size();
	const char* buf = getstr();

	while (iCut1<iCut2 && isspace(buf[iCut1]))
		iCut1++;

	while (iCut2>iCut1 && isspace(buf[iCut2-1]))
		iCut2--;

	return string(buf+iCut1, iCut2-iCut1);

}

string string::tolower() const
{
	string result = *this;
	strlwr(result.getstr());
	return result;
}

string string::toupper() const
{
	string result = *this;
	strupr(result.getstr());
	return result;
}

void string::assign(const char* pData, int iLen)
{
	if (pData == 0 || iLen == 0)
	{
		shrink(0);
		return;
	}

	if (iLen == -1)
		iLen = strlen(pData);
	grow(iLen);
	memcpy(getstr(), pData, iLen);
	m_iLen = iLen;
	getstr()[m_iLen] = 0;
}

void string::assign(const string& other)
{
	assign(other, other.size());
}

void string::assign(char c)
{
	assign(&c, 1);
}

void string::append(const char* pData, int iLen)
{
	grow(size() + iLen);
	strncat(getstr(), pData, iLen);
	m_iLen += iLen;
	getstr()[m_iLen] = 0;
}

int string::indexof(const char* pToFind, int iStartAfterChar) const
{
	char* result = strstr(getstr() + iStartAfterChar + 1, pToFind);
	if (result == 0)
		return -1;

	return result - getstr();
}

int string::indexof(char c, int iStartAfterChar) const
{
	char* result = strchr(getstr() + iStartAfterChar + 1, c);
	if (result == 0)
		return -1;

	return result - getstr();
}

const char* string::getstr() const
{
	return usingPtr() ? m_data.ptr : m_data.buf ;
}

bool string::usingPtr() const
{
	return (m_iReserved > _bufferSize);
}

char* string::getstr()
{
	return usingPtr() ? m_data.ptr : m_data.buf ;
}

void string::copyToPtr(unsigned int iNewSize)
{
	char* pNewData = new char[iNewSize];

	memcpy(pNewData, getstr(), m_iLen+1);
	if (usingPtr())
		delete [](m_data.ptr);

	m_data.ptr = pNewData;
	m_iReserved = iNewSize;
}

void string::grow(unsigned int iNewSize)
{
	if (iNewSize < m_iReserved)
		return;

	if (iNewSize < _bufferSize)
		return;

	iNewSize = max (iNewSize+1, m_iReserved * 2);
	copyToPtr(iNewSize);
}

void string::shrink(unsigned int iNewSize)
{
	getstr()[iNewSize] = 0;
	if (usingPtr())
	{
		// might be worth shrinking
		if (iNewSize < _bufferSize)
		{
			char* pSrc = getstr();
			memcpy(m_data.buf, pSrc, _bufferSize);
			m_iReserved = _bufferSize;
			delete [] pSrc ;
		}
		else if (iNewSize < m_iReserved / 2)
		{
			copyToPtr(iNewSize+1);
		}
	}

	m_iLen = iNewSize;
}

void string::clear()
{
	if (usingPtr())
		delete [] m_data.ptr;
	m_iLen = 0;
	m_data.ptr = 0;
	m_iReserved = _bufferSize;
}

void string::init()
{
	memset(&m_data, 0, sizeof(dataT));
	m_iLen = 0;
	m_iReserved = _bufferSize; 
}

void string::format(const char* format, ...)
{
	char buf[4096]; // 
	va_list argList;
	va_start(argList, format);
	vsprintf(buf, format, argList);
	va_end(argList);
	assign(buf);
}

array<string> string::split(const char* delimiter) const
{
	array<string> result;
	split(delimiter, result);
	return result;
}

int string::split( const char* delimiter, array<string>& result ) const
{
	int start = 0;
	int delim = find(delimiter);
	int delimLen = strlen(delimiter);
	int elemCnt = 1; // the ACTUAL number of items, there'll be at least 1

	// counting the elements, setting the size and then filling the array
	// is much faster than doing .Add for each new element
	while (delim > -1)
	{
		elemCnt++;
		start = delim + delimLen;
		delim = find(delimiter, start);
	}

	// manually going through and finding each delimiter again is faster than
	// keeping track of the positions from the last loop because doing .Add over and over
	// to the position array would be such a bottleneck
	start = 0;
	delim = find(delimiter);
	result.resize(elemCnt); // now we don't have to use .Add, saving tons of cpu cycles
	elemCnt = -1;

	while (delim > -1)
	{
		elemCnt++;
		result[elemCnt] = substr(start, delim-start);
		start = delim + delimLen;
		delim = find(delimiter, start);
	}

	if (start < size())
		result[elemCnt+1] = substr(start);
	else
		result[elemCnt+1] = "";

	return elemCnt+1;
}

