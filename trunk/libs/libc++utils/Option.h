#ifndef libcpputils_option_h
#define libcpputils_option_h
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "taparray.h"
class Option;
class Options;
class OptionUpdateNotifier;
class KeyDef;

class Option
{
public:
	Option(Options* pContainer, const string& key, const string& defaultvalue, const string& name, const string& description, OptionUpdateNotifier* pNotifier = NULL);
	virtual ~Option(void);

	const string& GetValue() const;
	const string& GetName() const;
	const string& GetDescription() const;
	bool SetValue(const string& value);
	virtual void OnUpdate() const;
	int ValueAsInteger() const;
	bool ValueAsBoolean() const;
	virtual KeyDef ValueAsKeyDef() const;
	const array<string>& GetChoices() const;
	string GetUniqueId() const;
	virtual bool ImmediateUpdateNeeded() const;

protected:
	array<string> m_choices;

private:
	void ReadValue() const;
	void WriteValue() const;

	OptionUpdateNotifier* m_pNotifier;
	string m_key;
	mutable string m_value;
	string m_defaultvalue;
	string m_name;
	string m_description;
	Options* m_pContainer;
};

#endif