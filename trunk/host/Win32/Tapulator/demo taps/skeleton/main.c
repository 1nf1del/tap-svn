#include <tap.h>


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if (event != EVT_KEY)
		return param1;

	TAP_Exit();
	TAP_Print("Exiting Skeleton tap\n" );
	return 0;
}


int TAP_Main(void)
{
	TAP_Print("Starting Skeleton tap\n" );

	return 1;
}
