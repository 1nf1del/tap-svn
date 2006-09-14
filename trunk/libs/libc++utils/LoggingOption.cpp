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
#include ".\LoggingOption.h"
#include "Logger.h"

LoggingOption::LoggingOption(Options* pContainer, const string& key, const string& name, const string& description, OptionUpdateNotifier* pNotifier) :
Option(pContainer, key, "None", name, description, pNotifier)
{
	m_choices.push_back("None");
	m_choices.push_back("Serial");
	m_choices.push_back("Screen");
	m_choices.push_back("Both");
}

LoggingOption::~LoggingOption()
{

}


int LoggingOption::ValueAsInteger() const
{
	const string& value = GetValue();

	if (value == "None")
		return Logger::None;

	if (value == "Serial")
		return Logger::Serial;

	if (value == "Screen")
		return Logger::Screen;

	if (value == "Both")
		return Logger::Serial+Logger::Screen;

	return Logger::Serial;


}

bool LoggingOption::ImmediateUpdateNeeded() const
{
	return true;
}
