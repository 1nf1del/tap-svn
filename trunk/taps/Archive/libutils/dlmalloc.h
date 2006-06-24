/* dlmalloc.h - interface to use Doug Lea's malloc/free
Added to libutils by Robin Glover

TAP_MemAlloc/TAP_MemFree are slow - one alloc/free seems to take >0.5ms. 
These routines build a general purpose heap on top of blocks allocated using
the underlying TAP_ routines, giving better performance, especially for c++
code which makes frequent use of new/delete

*/

#ifndef _utils_DLMALLOC_H
#define _utils_DLMALLOC_H
#ifdef  __cplusplus
extern "C" {
#endif

void* dlmalloc(size_t);
void dlfree(void*);
void* dlrealloc(void*, size_t);

// this function will try to free memory back to the system if some
// blocks are not in use
int  dlmalloc_trim(size_t);
// Added for TAP version - after this call *all* blocks become invalid, but 
// memory has been released back via TAP_Free. Call in TAP_Exit
int  dlmalloc_exit();

#ifdef  __cplusplus
}
#endif
#endif
