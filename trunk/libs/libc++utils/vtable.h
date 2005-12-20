#ifndef __VTABLE_H
#define __VTABLE_H


#if DEFAULT_VTABLE_THUNK==1
#error vtable-thunks is not currently supported
#else
struct vtableItem
{
   short offs1;
   short offs2;
   unsigned long fnPtr;
};


struct vtable
{
   vtableItem preamble;
   vtableItem items[1];
};
#endif

void FixupVTable( vtable& vtbl );
void FixupVTables();


#endif
