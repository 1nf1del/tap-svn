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
#include "stringarchive.h"
#include <string.h>
#undef sprintf
#include <stdio.h>
#include "logger.h"
stringarchive::stringarchive(const string& data)
{
	m_sBackingData = data;
	m_iReadPos = 0;
	if (data.size())
		m_bReading = true;
}




stringarchive& stringarchive::operator <<(bool bValue)
{
	string sNew;
	sNew.format("b%d;", bValue ? 1:0);
	m_sBackingData += sNew;
	return *this;
}

stringarchive& stringarchive::operator <<(int iValue)
{
	string sNew;
	sNew.format("i%d;", iValue);
	m_sBackingData += sNew;
	return *this;
}

stringarchive& stringarchive::operator <<(word wValue)
{
	string sNew;
	sNew.format("w%d;", wValue);
	m_sBackingData += sNew;
	return *this;
}

stringarchive& stringarchive::operator <<(const string& sValue)
{
	string sNew;
	sNew.format("s%d:%s;", sValue.size(), sValue.c_str());
	m_sBackingData += sNew;
	return *this;
}

stringarchive& stringarchive::operator >>(bool& bValue)
{
	if (ValidToRead())
	{
		int iValue = 0;
		if (!ReadIntValue('b',';',iValue))
		{
			TRACE("Error reading boolean field from stringarchive\n");
		}
		bValue = iValue != 0;
	}
	return *this;
}

stringarchive& stringarchive::operator >>(int& iValue)
{
	if (ValidToRead())
	{
		iValue = 0;
		if (!ReadIntValue('i',';',iValue))
		{
			TRACE("Error reading int field from stringarchive\n");
		}
	}
	return *this;
}

stringarchive& stringarchive::operator >>(word& wValue)
{
	if (ValidToRead())
	{
		int iValue = 0;
		if (!ReadIntValue('w',';',iValue))
		{
			TRACE("Error reading word field from stringarchive\n");
		}
		wValue = (word) iValue;
	}
	return *this;
}

stringarchive& stringarchive::operator >>(string& sValue)
{
	if (ValidToRead())
	{
		int iValue = 0;
		if (!ReadIntValue('s',':',iValue))
		{
			TRACE("Error reading string field length from stringarchive\n");
		}

		memcpy(sValue.getbuffer(iValue), m_sBackingData.c_str() + m_iReadPos, iValue);
		sValue[iValue] = 0; // null terminate
		sValue.releasebuffer();
		m_iReadPos += (iValue+1);
	}
	return *this;
}

const string& stringarchive::toString() const
{
	return m_sBackingData;
}

void stringarchive::Advance()
{
	if (!ValidToRead())
		return;

	m_iReadPos = m_sBackingData.find(';',m_iReadPos);
	if (m_iReadPos>=0)
		m_iReadPos ++;
}

bool stringarchive::ValidToRead()
{
	if ((m_iReadPos == -1) || ! m_bReading)
		return false;

	return true;
}

bool stringarchive::ReadIntValue(char cPrefix, char cPostFix, int& value)
{
	value = 0;
	if (m_sBackingData[m_iReadPos]!=cPrefix)
		return false;
	m_iReadPos++;
	value = atoi(m_sBackingData.c_str() + m_iReadPos);
	m_iReadPos = m_sBackingData.find(cPostFix, m_iReadPos) + 1;
	if (m_iReadPos == 0)
		return false;
	return true;
}