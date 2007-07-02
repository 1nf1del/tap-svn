#ifndef _LOCALTAP_H_
#define _LOCALTAP_H_

#ifdef _WIN32
#ifndef __cplusplus
typedef int bool;
#endif
#endif

#include <tap.h>
#undef malloc
#undef free
#undef sprintf


#ifdef _WIN32
#define __attribute__(x)
#define section(x)
#endif

#endif /*_LOCALTAP_H_*/