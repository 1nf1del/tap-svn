#include ".\optionupdatenotifier.h"
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