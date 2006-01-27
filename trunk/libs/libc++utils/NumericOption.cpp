#include ".\numericoption.h"

NumericOption::NumericOption(Options* pContainer, const string& key,
							 int minValue, int maxValue, int defaultValue, const string& name, 
							 const string& description, OptionUpdateNotifier* pNotifier) :
Option(pContainer, key, IntToString(defaultValue), name, description, pNotifier)
{
	for (int i=minValue; i<=maxValue; i++)
	{
		m_choices.push_back(IntToString(i));
	}
}

NumericOption::~NumericOption(void)
{
}

string NumericOption::IntToString(int value)
{
	string result;
	result.format("%d", value);
	return result;
}

