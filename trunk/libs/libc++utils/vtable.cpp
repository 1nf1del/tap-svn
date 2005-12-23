#include <tap.h>
#include "vtable.h"
#include "Logger.h"
#include "dlmalloc.h"

void FixupVTable( vtable& vtbl )
{
#ifdef _TAP
	// offset1 is zero for single inheritance, -ve for multiple
	if (vtbl.preamble.offs1 > 0)
	{
		TRACE1("Not fixing: preamble.offs1 %08X\n" , vtbl.preamble.offs1 );
		return;
	}
	// offset2 allways seen as zero so far
	if (vtbl.preamble.offs2 != 0)
	{
		TRACE1("Not fixing: preamble.offs2 %08X\n" , vtbl.preamble.offs2 );
		return;
	}
	if (vtbl.preamble.fnPtr != 0)
	{
		TRACE1("Not fixing: preamble.fnPtr %08X\n" , vtbl.preamble.fnPtr );
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

	TRACE1("Fixed vtable at address 0x%08X\n", &vtbl);
#else
	(vtbl);
#endif

}

#ifdef WIN32

	void FixupVTables()
	{

	}
#endif

void * operator new(size_t size)
{
	return dlmalloc( (dword) size);
}
void operator delete(void * mem)
{
	if (mem)
		dlfree(mem);

	dlmalloc_trim(0);
}

void* operator new[](size_t size)
{
	return dlmalloc(size);
}

void operator delete[](void* pMem)
{
	dlfree(pMem);

	dlmalloc_trim(0);
}
