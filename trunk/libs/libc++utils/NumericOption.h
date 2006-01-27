#ifndef libcpputils_numericoption_h
#define libcpputils_numericoption_h
#include "option.h"

class NumericOption :
	public Option
{
public:
	NumericOption(Options* pContainer, const string& key,int minValue, int maxValue, int defaultValue, const string& name, const string& description, OptionUpdateNotifier* pNotifier = NULL);
	~NumericOption(void);

private:
	static string IntToString(int value);
};
#endif