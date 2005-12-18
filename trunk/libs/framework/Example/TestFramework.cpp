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

#include <tap.h>
#include <Tapplication.h>
#include "YesNoBox.h"
#include "BitmapScreen.h"
#include "graphics.c"


class TestTap : public Tapplication
{
public:
	TestTap()
	{
		TAP_Print("TestTap\n");
	}

	~TestTap()
	{
		TAP_Print("~TestTap\n");
	}

	bool Start();
	dword OnKey( dword key, dword extKey );
	void Test();

	void YesNoResult( dword result );
};


bool TestTap::Start()
{
	TAP_Print("TestTap::Start\n");
	
	TAP_Osd_FillBox( screenRgn, 0,0, 720,576, RGB(31,31,31) );
	TAP_Osd_PutGd( screenRgn, (720-_bigkeyblueGd.width)/2 , (576-_bigkeyblueGd.height)/2, &_bigkeyblueGd, TRUE );

	return true;
}


dword TestTap::OnKey( dword key, dword extKey )
{
	if ( Tapplication::OnKey( key, extKey ) == 0 )
		return 0;

	if ( pageCount == 0 )
	{
		if ( key == RKEY_1 )
		{
			YesNoBox* p = new YesNoBox("Test Title", "This is line 1", "Do you want to do this", "Yes", "No", 1, MakeDelegate( this, &TestTap::YesNoResult ) );
			p->Open();
		}
		else if ( key == RKEY_2 )
		{
			BitmapScreen* p = new BitmapScreen( &_popup360x180Gd );
			p->Open();
		}
		else
		{
			TAP_EnterNormal();
			return Close();
		}
	}

	if ( key == RKEY_Power )
	{
		TAP_EnterNormal();
		return Close();
	}

	return key;
}

void TestTap::YesNoResult( dword result )
{
	TAP_Print("TestTap::YesNoResult\n");
}

DEFINE_APPLICATION(TestTap)


#ifndef WIN32

extern "C"
{
TAP_ID(0x8A004423);
TAP_PROGRAM_NAME("Test Framework");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("");
TAP_ETCINFO(__DATE__);
}

#endif

void * __builtin_new(size_t size)
{
	return TAP_MemAlloc(size);
}

void __builtin_delete(void * mem)
{
	if (mem)
		TAP_MemFree(mem);
}
