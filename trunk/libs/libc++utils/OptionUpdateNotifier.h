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
#ifndef libcpputils_optionupdatenotifier_h
#define libcpputils_optionupdatenotifier_h
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
class Option;
class KeyDef;

class OptionUpdateNotifier
{
public:
	OptionUpdateNotifier(void);
	virtual ~OptionUpdateNotifier(void);

	virtual void OnUpdate(const Option* pOpt) const;

protected:
	void TransferValue(const Option* pOpt, int& value) const ;
	void TransferValue(const Option* pOpt, bool& value) const ;
	void TransferValue(const Option* pOpt, string& value) const;
	void TransferValue(const Option* pOpt, KeyDef& value) const ;

};

typedef void (* OptionUpdateNotificationFuncPtr)(const Option*, void*);

class OptionUpdateFunctionNotifier : public OptionUpdateNotifier
{
public:
	OptionUpdateFunctionNotifier(OptionUpdateNotificationFuncPtr pFunc = NULL, void* pObj = NULL);
	virtual void OnUpdate(const Option* pOpt) const;

private:
	OptionUpdateNotificationFuncPtr m_pFunc;
	void* m_pObj;

};

#define OPTIONUPDATEVALUECLASS(T) \
class OptionUpdateValueNotifier_##T : public OptionUpdateNotifier\
{\
public:\
	OptionUpdateValueNotifier_##T(T& value);\
	virtual void OnUpdate(const Option* pOpt) const;\
private:\
	T* m_pValue;\
};


OPTIONUPDATEVALUECLASS(bool);
OPTIONUPDATEVALUECLASS(string);
OPTIONUPDATEVALUECLASS(int);
OPTIONUPDATEVALUECLASS(KeyDef);
#endif