# SPDX-License-Identifier: GPL-3.0-or-later or MIT

function(test_cc_builtin prog def)
  execute_process(COMMAND ${Python3_EXECUTABLE}
		  scripts/checkfeature.py "${prog}"
		  RESULT_VARIABLE err)

  if(NOT err)
    set(${def} 1 PARENT_SCOPE)
  endif()
endfunction()

test_cc_builtin("
int x = __builtin_align_down(8, 4);
" HAVE_BUILTIN_ALIGN_DOWN)

test_cc_builtin("
#define _GNU_SOURCE
#include <stdlib.h>

static void a(void)
{
	char *p = reallocarray(NULL, sizeof(*p), 5);
	if (p);
}
" HAVE_REALLOCARRAY)

test_cc_builtin("
#define _GNU_SOURCE
#include <sys/types.h>

uint a = 0;
" HAVE_UINT)

test_cc_builtin("
#define _GNU_SOURCE
#include <sys/types.h>

ssize_t a = 0;
" HAVE_SSIZE_T)

configure_file(include/features.h.in ../include/generated/features.h)