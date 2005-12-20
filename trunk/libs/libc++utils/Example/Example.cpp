#include <tap.h>
#include <inifile.h>


extern "C"
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
    if( event == EVT_KEY )
    {
		TAP_EnterNormal();
        TAP_Exit();
	    return 0;
    }
    return event;
}


extern "C"
int TAP_Main(void)
{
	TAP_Print("Test TAP\n" );

	IniFile file( "test.ini" );
	string s;
	TAP_Print("Record %s\n" , file.GetValue( "Record", "new value" ).c_str() );
	file.SetValue( "Record", "new value" );
	file.Save( "test2.ini" );

	TAP_Print("Test TAP ended\n" );

    return 0;
}

