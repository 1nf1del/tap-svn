#ifndef __CONFIGPAGE_H
#define __CONFIGPAGE_H

#include "OptionsPage.h"


class ConfigPage : public ListPage
{
public:
	ConfigPage();
	~ConfigPage();

	virtual dword OnKey( dword key, dword extKey );

private:
	static dword ExitTapKeyPress(ListPage* page, ListItem* item, dword key, dword extKey);
	static dword RebootKeyPress(ListPage* page, ListItem* item, dword key, dword extKey);
	static dword ShutdownKeyPress(ListPage* page, ListItem* item, dword key, dword extKey);
};


#endif
