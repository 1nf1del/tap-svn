#ifndef framework_skinoption_h
#define framework_skinoption_h
#include "option.h"

class SkinOption :
	public Option
{
public:
	SkinOption(Options* pContainer);
	virtual ~SkinOption(void);

	virtual void OnUpdate() const;
	virtual bool ImmediateUpdateNeeded() const; 
};
#endif