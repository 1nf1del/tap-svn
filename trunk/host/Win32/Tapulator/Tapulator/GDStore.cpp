//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#include "StdAfx.h"
#include ".\gdstore.h"
#include "utils.h"


class CRC16
{
public:
	CRC16()
	{
		m_crc = 0;
	}

	void Add(unsigned char c)
	{
		crc16(&m_crc, &c, 1);
	}

	void Add(unsigned short s)
	{
		crc16(&m_crc, (u8*)&s, 2);
	}

	void Add(unsigned long l)
	{
		crc16(&m_crc, (u8*)&l, 4);
	}

	void Add(BYTE* pData, int iCount)
	{
		crc16(&m_crc, pData, iCount);
	}

	unsigned short Result()
	{
		return m_crc;
	}

private:
	unsigned short m_crc;
};



GDStore::GDStore(void)
{
}

GDStore::~GDStore(void)
{
	for (std::map<TYPE_GrData*, BYTE*>::iterator it = m_GDs.begin(); it != m_GDs.end(); it ++)
	{
		free (it->second);
	}

}

BYTE* GDStore::UnpackData(TYPE_GrData* pGD)
{
	BYTE* pData = Unpack((BYTE*)pGD->data, pGD->dataSize);
	WORD* pDataW = (WORD*) pData;

	for (unsigned int i = 0; i<pGD->dataSize/2; i++)
	{
		*pDataW = SWAPWORD(*pDataW);
		pDataW++;
	}

	return pData;
}

BYTE* GDStore::GetBitmapDataForGD(TYPE_GrData* pGD)
{
	if (m_GDs.find(pGD) == m_GDs.end())
	{
		m_GDs[pGD] = UnpackData(pGD);


	}
	return m_GDs[pGD];
}


BYTE* GDStore::Unpack(BYTE* pTFD, int iOutLen)
{
	tfdHeader inHeader = *((tfdHeader*) pTFD);
	CRC16 inHeadercrc;
	inHeadercrc.Add(inHeader.type);
	inHeadercrc.Add(inHeader.something);
	inHeadercrc.Add(inHeader.blocks);
	WORD crc = SWAPWORD(inHeader.crc);
	_ASSERT(inHeadercrc.Result() == crc);
	WORD blocks = SWAPWORD(inHeader.blocks);

	BYTE* pOutData = (BYTE*) malloc(iOutLen);
	BYTE* pInPos = pTFD + sizeof(tfdHeader);

	DWORD addr = 0x0;
	for (int i=0; i<blocks; i++)
	{
		tfdBlockHeader inBlockH;
		memcpy(&inBlockH, pInPos, sizeof(tfdBlockHeader));
		pInPos += sizeof(tfdBlockHeader);
		CRC16 inCRC;
		inCRC.Add(inBlockH.type);
		inCRC.Add(inBlockH.length);
		int blocksize = SWAPWORD(inBlockH.size) - 6;
		
		WORD length = SWAPWORD(inBlockH.length);

		inCRC.Add(pInPos, blocksize);
		ASSERT(inCRC.Result() == SWAPWORD(inBlockH.crc));

		VERIFY(unlh5(pInPos, blocksize, pOutData + addr, length) == 0);

		pInPos += blocksize;
		addr += length;
	}


	return pOutData;
}

