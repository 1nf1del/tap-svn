#include <stdlib.h>

#ifndef __TOOLCHAIN__
  struct _reent impure_data/* = _REENT_INIT(impure_data)*/;             // macro is buggy
  struct _reent *__ATTRIBUTE_IMPURE_PTR__ _impure_ptr = &impure_data;
#endif
