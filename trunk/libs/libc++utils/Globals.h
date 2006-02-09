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
#ifndef cpputils_globals_h
#define cpputils_globals_h

class Timers;
class EPGdata;
class Channels;
class ProgressNotification;
class Archive;
#include "EPGdata.h"

class Globals
{
public:
	Globals(void);
	~Globals(void);

	static void Cleanup();
	static Timers* GetTimers();
	static EPGdata* GetEPGdata();
	static Channels* GetChannels();
	static bool LoadEPGData(DataSources dataSource, ProgressNotification* pProgress = 0, dword dwFlags = 0);
	static Archive* GetArchive();

private:

	static Timers* m_pTheTimers;
	static EPGdata* m_pEPGdata;
	static Channels* m_pChannels;
	static Archive* m_pArchive;
};
#endif