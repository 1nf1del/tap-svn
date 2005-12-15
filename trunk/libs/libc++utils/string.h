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

#ifndef __string_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#define __string_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h

class string
{
public:
	string(void);
	~string(void);

	string(const char* pData, int iLen = -1);
	string(const string& other);
	string(char c);

	bool operator==(const string& right) const ;
	bool operator==(const char* pRight) const ;
	string& operator=(const string& right);
	string& operator=(const char* pData);
	string& operator=(char c);
	string& operator+=(const string& right);
	string& operator+=(const char* pData);
	string& operator+=(char c);

	operator const char*() const;

	string operator+(const string& right);
	string operator+(const char* pRight);
	string operator+(char c);

	string substr(int offs = 0, int count = -1) const;
	int compare(const char* pOther) const;
	int compareNoCase(const char* pOther) const;
	int compare(const string& other) const;
	int compareNoCase(const string& other) const;

	int find(const string& toFind) const;
	int find(const char* pToFind) const;
	int find(char c) const;

	bool empty() const;
	void clear();
	int size() const;
	void resize(unsigned int newSize);
	void format(const char* format, ...);

private:

	void assign(const char* pData, int iLen = -1);
	void assign(const string& other);
	void assign(char c);
	void grow(unsigned int iNewSize);
	void shrink(unsigned int iNewSize);
	void append(const char* pdata, int iLen);
	int indexof(const char* pToFind) const;
	int indexof(char c) const;
	const char* getstr() const;
	char* getstr();
	void init();
	bool usingPtr() const;
	void copyToPtr(unsigned int iNewSize);

	static const unsigned int _bufferSize = 16;

	union dataT
	{
		char* ptr;
		char buf[_bufferSize];
	};

	dataT m_data;
	unsigned int m_iLen;
	unsigned int m_iReserved;
};

string operator+(const char* pLeft, const string& right);
string operator+(char left, const string& right);

#endif