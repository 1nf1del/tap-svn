/*
	Copyright (C) 2006 Simon Capewell

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

#ifndef __FOOTERLISTITEM_H
#define __FOOTERLISTITEM_H

#include "ListPage.h"
#include "SimpleTextListItem.h"
#include "ActionListItem.h"


// List item that sets text in the footer
class FooterTextListItem : public SimpleTextListItem
{
public:
	FooterTextListItem(ListPage* pParentList, const char* footerText, dword dwFlags, const char* pszCol1, const char* pszCol2 = 0) :
		SimpleTextListItem(pParentList, dwFlags, pszCol1, pszCol2),
		m_footerText( footerText )
	{
	}

	virtual string GetFooterText();

private:
	string m_footerText;
};


// List item that sets text in the footer and calls a static function when a key is pressed
class FooterActionListItem : public FooterTextListItem
{
public:
	FooterActionListItem(ListPage* pParentList, KeyHandlerFnPtr action, const char* footerText, dword dwFlags, const char* pszCol1, const char* pszCol2 = 0) :
		FooterTextListItem(pParentList, footerText, dwFlags, pszCol1, pszCol2),
		m_action( action )
	{
	}

	virtual dword OnKey( dword key, dword extKey );

private:
	KeyHandlerFnPtr m_action;
};


#endif
