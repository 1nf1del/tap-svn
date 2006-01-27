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