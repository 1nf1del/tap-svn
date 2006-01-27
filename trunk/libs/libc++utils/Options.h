#ifndef libcpputils_options_h
#define libcpputils_options_h
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "taparray.h"
class Option;
#include "OptionUpdateNotifier.h"
class IniFile;

class Options
{
public:
	Options(const string& sIniFile);
	~Options(void);

	const string& GetIniFile() const;
	bool Add(Option* pOption);
	// query by 'section:key'
	Option* GetOption(const string& name);
	array<Option*> GetAllOptions();
	void BroadcastDefaultValues();
	string ReadValue(const string& sKey);
	void WriteValue(const string& sKey, const string& sValue);
	void SaveValues();

private:

	string m_sIniFile;
	array<Option*> m_options;
	array<string> m_names;
	IniFile* m_pIniFile;
};
#endif