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

#ifndef __array_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#define __array_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#include "tap.h"

#ifndef _TAP
#include <memory.h>
#endif

inline void* operator new (size_t s, void* _where)
{
	s;
	return _where;
}

#pragma warning (disable : 4291) // don't care about no matching operator delete - toppy has no exceptions

template <typename T> class array
{
public:
	array(int iInitialSize = 0) : m_pArray(0), m_iLen(0), m_iReserved(0), m_nullObject(T())
	{
		resize(iInitialSize);
	}

	array(const array<T>& other) : m_pArray(0), m_iLen(0), m_iReserved(0), m_nullObject(T())
	{
		*this = other;	
	}

	array<T>& operator=(const array<T>& right)
	{
		if (&right != this)
		{
			clear();
			resize(right.m_iLen);
			assign(0, right.m_iLen, right.m_pArray);
			m_iLen = right.m_iLen;
		}

		return *this;
	}

	~array(void)
	{
		destroy(0,m_iLen);
		delete [] (char*) m_pArray;
	}

	T& operator[](unsigned int i)
	{
		if (i>=m_iLen)
			return m_nullObject;

		return m_pArray[i];
	}

	const T& operator[](unsigned int i) const
	{
		if (i>=m_iLen)
			return m_nullObject;

		return m_pArray[i];
	}

	unsigned int size() const
	{
		return m_iLen;
	}

	void push_back(const T& newItem)
	{
		grow(m_iLen+1);
		construct(m_iLen, m_iLen+1, newItem);
		++m_iLen;
	}

	T& back()
	{
		if (m_iLen == 0)
			return m_nullObject;

		return m_pArray[m_iLen-1];
	}

	const T& back() const
	{
		if (m_iLen == 0)
			return m_nullObject;

		return m_pArray[m_iLen-1];
	}

	void pop()
	{
		if (m_iLen == 0)
			return;

		destroy(m_iLen-1, m_iLen);
		--m_iLen;
	}

	void resize(unsigned int iSize)
	{
		if (iSize>m_iReserved)
			grow(iSize);

		if (iSize>m_iLen)
		{
			construct(m_iLen, iSize-m_iLen, T());
			m_iLen = iSize;
		}
		else if (iSize<m_iLen)
		{
			destroy(iSize, m_iLen-iSize);
			m_iLen = iSize;

			shrink(iSize);
		}
	}

	void clear()
	{
		resize(0);
	}

	bool empty() const
	{
		return size() == 0;
	}

private:

	void assign(unsigned int iOffs, unsigned int iCount, T* pSrcData)
	{
		if (iCount > 0)
			memcpy(m_pArray + iOffs, pSrcData, iCount * sizeof(T));
	}

	void copyToSize(unsigned int iNewLen)
	{
		T* pOldData = m_pArray;
		m_pArray = (T*) new char[iNewLen*sizeof(T)];
		assign(0, min(m_iReserved, iNewLen), pOldData);
		delete [] (char*)pOldData; 
		m_iReserved = iNewLen;
	}

	void grow(unsigned int iNewSize)
	{
		if (iNewSize <= m_iReserved)
			return;

		iNewSize = max(iNewSize, m_iReserved + m_iReserved/2);
		if (iNewSize<8)
			iNewSize = 8;

		copyToSize(iNewSize);
	}

	void shrink(unsigned int iNewSize)
	{
		if (iNewSize*2 >= m_iReserved)
			return;

		if (m_iReserved <= 8)
			return;

		copyToSize(max(m_iLen, iNewSize));
	}

	void destroy(int iFirstPos, int iLastPos)
	{
		T* pItem = m_pArray + iFirstPos;
		int iCount = iLastPos - iFirstPos;
		for (int i=0; i<iCount; i++)
		{
			pItem->~T();
			pItem++;
		}
	}

	void construct(int iFirstPos, int iLastPos, const T& _val)
	{
		T* pItem = m_pArray + iFirstPos;
		int iCount = iLastPos - iFirstPos;
		for (int i=0; i<iCount; i++)
		{
			new (pItem) T(_val);
			pItem++;
		}
	}

	T* m_pArray;
	T m_nullObject;
	unsigned int m_iLen;
	unsigned int m_iReserved;
};

#endif