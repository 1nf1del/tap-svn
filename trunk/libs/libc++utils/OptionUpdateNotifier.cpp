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
#include "optionupdatenotifier.h"
#include "option.h"
#include "KeyDef.h"

OptionUpdateNotifier::OptionUpdateNotifier(void)
{
}

OptionUpdateNotifier::~OptionUpdateNotifier(void)
{
}

void OptionUpdateNotifier::OnUpdate(const Option* pOpt) const
{
	(pOpt);
}

OptionUpdateFunctionNotifier::OptionUpdateFunctionNotifier(OptionUpdateNotificationFuncPtr pFunc, void* pObj)
{
	m_pFunc = pFunc;
	m_pObj = pObj;
}

void OptionUpdateFunctionNotifier::OnUpdate(const Option* pOpt) const
{
	if (m_pFunc)
		(*m_pFunc)(pOpt, m_pObj);

}
void OptionUpdateNotifier::TransferValue(const Option* pOpt, int& value) const
{
	value = pOpt->ValueAsInteger();
}

void OptionUpdateNotifier::TransferValue(const Option* pOpt, bool& value) const
{
	value = pOpt->ValueAsBoolean();
}

void OptionUpdateNotifier::TransferValue(const Option* pOpt, string& value) const
{
	value = pOpt->GetValue();
}

void OptionUpdateNotifier::TransferValue(const Option* pOpt, KeyDef& value) const
{
	value = pOpt->ValueAsKeyDef();
}

#define OPTIONUPDATEVALUECLASSIMPL(T)\
OptionUpdateValueNotifier_##T::OptionUpdateValueNotifier_##T(T& value)\
{\
	m_pValue = &value;\
}\
void OptionUpdateValueNotifier_##T::OnUpdate(const Option* pOpt) const\
{\
	TransferValue(pOpt, *m_pValue);\
}

OPTIONUPDATEVALUECLASSIMPL(bool)
OPTIONUPDATEVALUECLASSIMPL(int)
OPTIONUPDATEVALUECLASSIMPL(string)
OPTIONUPDATEVALUECLASSIMPL(KeyDef)