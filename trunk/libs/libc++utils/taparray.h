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
#include <limits.h>
#include "quicksort.h"

inline void* operator new (size_t s, void* _where)
{
	s;
	return _where;
}

#pragma warning (disable : 4291) // don't care about no matching operator delete - toppy has no exceptions

template <typename T> class array
{
	typedef SimpleComparitor<T> default_comparitor;
public:
	array(int iInitialSize = 0, int iInitialReserved = 0);

	array(const array<T>& other);

	array<T>& operator=(const array<T>& right);
	~array(void);
	T& operator[](unsigned int i);

	const T& operator[](unsigned int i) const;

	unsigned int size() const;

	void push_back(const T& newItem);


	T& back();

	const T& back() const;

	void pop();

	void erase(unsigned int iIndex);


	void resize(unsigned int iSize);

	void clear();

	bool empty() const;

	template <class Comparitor> void sort(unsigned int fromIndex=0, unsigned int length=INT_MAX)
	{
		if (fromIndex < m_iLen)
		{
			Comparitor comp;
			QuickSort(m_pArray+fromIndex, min(length, m_iLen-fromIndex), comp);
		}
	}

	void sort(unsigned int fromIndex=0, unsigned int length=INT_MAX);

	int find(const T& item) const;
	int findvalue(const T& pItem) const;

	bool contains(const T& item) const;
	bool containsvalue(const T& pItem) const;
	const T& nullObject() const;

private:

	void assign(unsigned int iOffs, unsigned int iCount, T* pSrcData);
	void copy(unsigned int iCount, T* pSrcData);


	void copyToSize(unsigned int iNewLen);

	void grow(unsigned int iNewSize);

	void shrink(unsigned int iNewSize);
	void destroy(int iFirstPos, int iLastPos);

	void construct(int iFirstPos, int iLastPos, const T& _val);

	T* m_pArray;
	T m_nullObject;
	unsigned int m_iLen;
	unsigned int m_iReserved;
};

template<typename T> array<T>::array(int iInitialSize = 0, int iInitialReserved = 0) : m_pArray(0), m_iLen(0), m_iReserved(0), m_nullObject(T())
{
	resize(iInitialSize);
	grow(iInitialReserved);
}

template<typename T> array<T>::array(const array<T>& other) : m_pArray(0), m_iLen(0), m_iReserved(0), m_nullObject(T())
{
	*this = other;
}

template<typename T> array<T>& array<T>::operator=(const array<T>& right)
{
	if (&right != this)
	{
		clear();
		grow(right.m_iLen);
		copy(right.m_iLen, right.m_pArray);
		m_iLen = right.m_iLen;
	}

	return *this;
}

template<typename T> array<T>::~array(void)
{
	destroy(0,m_iLen);
	delete [] (char*) m_pArray;
}

template<typename T> T& array<T>::operator[](unsigned int i)
{
	if (i>=m_iLen)
		return m_nullObject;

	return m_pArray[i];
}

template<typename T> const T& array<T>::operator[](unsigned int i) const
{
	if (i>=m_iLen)
		return m_nullObject;

	return m_pArray[i];
}

template<typename T> unsigned int array<T>::size() const
{
	return m_iLen;
}

template<typename T> void array<T>::push_back(const T& newItem)
{
	grow(m_iLen+1);
	construct(m_iLen, m_iLen+1, newItem);
	++m_iLen;
}

template<typename T> T& array<T>::back()
{
	if (m_iLen == 0)
		return m_nullObject;

	return m_pArray[m_iLen-1];
}

template<typename T> const T& array<T>::back() const
{
	if (m_iLen == 0)
		return m_nullObject;

	return m_pArray[m_iLen-1];
}

template<typename T> void array<T>::pop()
{
	if (m_iLen == 0)
		return;

	destroy(m_iLen-1, m_iLen);
	--m_iLen;
}

template<typename T> void array<T>::erase(unsigned int iIndex)
{
	if (iIndex>=m_iLen)
		return;

	destroy(iIndex, iIndex+1);
	assign(iIndex, m_iLen - iIndex - 1, m_pArray + iIndex + 1);
	resize(m_iLen - 1);
}


template<typename T> void array<T>::resize(unsigned int iSize)
{
	if (iSize>m_iReserved)
		grow(iSize);

	if (iSize>m_iLen)
	{
		construct(m_iLen, iSize, T());
		m_iLen = iSize;
	}
	else if (iSize<m_iLen)
	{
		destroy(iSize, m_iLen-iSize);
		m_iLen = iSize;

		shrink(iSize);
	}
}

template<typename T> void array<T>::clear()
{
	resize(0);
}

template<typename T> bool array<T>::empty() const
{
	return size() == 0;
}


template<typename T> void array<T>::sort(unsigned int fromIndex, unsigned int length)
{
	if (fromIndex < m_iLen)
	{
		default_comparitor comp;
		QuickSort(m_pArray+fromIndex, min(length, m_iLen-fromIndex), comp);
	}
}

template<typename T> int array<T>::find(const T& item) const
{
	for (unsigned int i=0; i<m_iLen; i++)
	{
		if (m_pArray[i]==item)
			return i;
	}
	return -1;
}

template<typename T> int array<T>::findvalue(const T& pItem) const
{
	for (unsigned int i=0; i<m_iLen; i++)
	{
		if (*m_pArray[i]==*pItem)
			return i;
	}
	return -1;
}


template<typename T> bool array<T>::contains(const T& item) const
{
	return find(item)!=-1;
}

template<typename T> bool array<T>::containsvalue(const T& pItem) const
{
	return findvalue(pItem)!=-1;
}

template<typename T> const T& array<T>::nullObject() const
{
	return m_nullObject;
}


template<typename T> void array<T>::assign(unsigned int iOffs, unsigned int iCount, T* pSrcData)
{
	if (iCount > 0)
		memcpy(m_pArray + iOffs, pSrcData, iCount * sizeof(T));
}

template<typename T> void array<T>::copy(unsigned int iCount, T* pSrcData)
{
	if (iCount > 0)
	{
		for (unsigned int i=0; i<iCount; i++)
		{
			construct(i, i+1, pSrcData[i]);
		}
	}
}


template<typename T> void array<T>::copyToSize(unsigned int iNewLen)
{
	T* pOldData = m_pArray;
	m_pArray = (T*) new char[iNewLen*sizeof(T)];
	assign(0, min(m_iReserved, iNewLen), pOldData);
	delete [] (char*)pOldData;
	m_iReserved = iNewLen;
}

template<typename T> void array<T>::grow(unsigned int iNewSize)
{
	if (iNewSize <= m_iReserved)
		return;

	iNewSize = max(iNewSize, m_iReserved + m_iReserved/2);
	if (iNewSize<8)
		iNewSize = 8;

	copyToSize(iNewSize);
}

template<typename T> void array<T>::shrink(unsigned int iNewSize)
{
	if (iNewSize*2 >= m_iReserved)
		return;

	if (m_iReserved <= 8)
		return;

	copyToSize(max(m_iLen, iNewSize));
}

template<typename T> void array<T>::destroy(int iFirstPos, int iLastPos)
{
	T* pItem = m_pArray + iFirstPos;
	int iCount = iLastPos - iFirstPos;
	for (int i=0; i<iCount; i++)
	{
		pItem->~T();
		pItem++;
	}
}

template<typename T> void array<T>::construct(int iFirstPos, int iLastPos, const T& _val)
{
	T* pItem = m_pArray + iFirstPos;
	int iCount = iLastPos - iFirstPos;
	for (int i=0; i<iCount; i++)
	{
		new (pItem) T(_val);
		pItem++;
	}
}


#endif