#ifndef framework_optionspage_h
#define framework_optionspage_h
#include "ListPage.h"
class Options;

class OptionsPage :
	public ListPage
{
public:
	OptionsPage(const string& title, Options* pOptions);
	virtual ~OptionsPage(void);

	virtual dword OnKey( dword key, dword extKey );

private:
	void OnOK();

	Options* m_pOptions;
};
#endif