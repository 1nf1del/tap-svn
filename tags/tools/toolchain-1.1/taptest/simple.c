#include <tap.h>

#define ID_TAP	0x80100213
#define TAP_NAME	"simple v0.1"

TAP_ID(ID_TAP);
TAP_PROGRAM_NAME(TAP_NAME);
TAP_AUTHOR_NAME("Steve Bennett");
TAP_DESCRIPTION("Simple Test");
TAP_ETCINFO(__DATE__);

dword	TAP_EventHandler ( word wEvent, dword dwKey, dword dwParam2 )
{
	return dwKey; 
}

int	TAP_Main (void)
{
	return 0;
}
