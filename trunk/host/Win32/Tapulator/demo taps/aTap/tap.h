#pragma once
#ifndef __cplusplus
typedef int bool;
#endif

#include <tap.h>

#ifdef _WIN32
#define __attribute__(x)
#define section(x)
#endif