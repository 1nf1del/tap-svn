#include <tap.h>
#include "vtable.h"


void FixupVTable( vtable& vtbl )
{
#ifdef _TAP
	// preamble has always been zero (so far), so if it doesn't look right, we give up
	if (vtbl.preamble.offs1 != 0)
	{
		TAP_Print("Not fixing: preamble.offs1 %08X\n" , vtbl.preamble.offs1 );
		return;
	}
	if (vtbl.preamble.offs2 != 0)
	{
		TAP_Print("Not fixing: preamble.offs2 %08X\n" , vtbl.preamble.offs2 );
		return;
	}
	if (vtbl.preamble.fnPtr != 0)
	{
		TAP_Print("Not fixing: preamble.fnPtr %08X\n" , vtbl.preamble.fnPtr );
		return;
	}

	for (int i=0; i<100; i++) // can increase if you have > 100 virtual functions!
	{
		vtableItem& item = vtbl.items[i];
		if (item.fnPtr > 0x200000 ) // a 2mb tap? no, surely not. This pointer probably already adjusted
			continue;

		if (item.fnPtr == 0) // the compiler inserts 8 bytes of 0's at end of vtable - when we hit that we can stop
			break; // that's all folks

		item.fnPtr += _tap_startAddr;
	}
#endif
}


void * __builtin_new(size_t size)
{
	return TAP_MemAlloc(size);
}
void __builtin_delete(void * mem)
{
	if (mem)
		TAP_MemFree(mem);
}
