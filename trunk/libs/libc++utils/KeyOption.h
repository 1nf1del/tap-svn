#ifndef libcpputils_keyoption_h
#define libcpputils_keyoption_h
#include "option.h"
#include "KeyDef.h"

class KeyOption :
	public Option
{
public:
	KeyOption(Options* pContainer, const string& key, const string& defaultValue, const string& name, const string& description,OptionUpdateNotifier* pNotifier);
	virtual ~KeyOption(void);

	virtual KeyDef ValueAsKeyDef() const;
private:
	static KeyDef GetKeyCodes(const string& sDesc);

};
#endif