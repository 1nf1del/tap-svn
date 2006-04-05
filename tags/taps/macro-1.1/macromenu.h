#ifndef __MACROMENU_H
#define __MACROMENU_H

void DisplayMenu( int row );
dword listactive_handler ( dword param1 );
void CreateMacro( void );
void DrawMacros( void );
void ListMacros( void );
void TestMacro( void );
dword menuactive_handler ( dword param1 );
dword recording_handler ( dword param1 );
dword recording_handler_end ( dword param1 );
dword listactive_handler ( dword param1 );

#endif
