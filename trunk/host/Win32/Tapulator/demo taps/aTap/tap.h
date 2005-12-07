#pragma once
#ifdef _WIN32
#ifndef __cplusplus
typedef int bool;
#endif
#endif

#include <tap.h>

#ifdef _WIN32
#define __attribute__(x)
#define section(x)
#endif