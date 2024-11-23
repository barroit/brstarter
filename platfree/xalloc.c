// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "xalloc.h"
#include "termas.h"
#include "tercol.h"

#define ALLOC_MAX ((size_t)512 * SZ_1K)

static inline void __warn_on_exceeds(const char *file, int line,
				     const char *func, size_t size)
{
	if (likely(size <= ALLOC_MAX))
		return;

	__termas(file, line, func, TM_WARN, NULL, TM_FLLN | TM_FUNC,
		 _("allocation of %zu exceeds limit of %zu"), size, ALLOC_MAX);
}

#ifdef CONFIG_WARN_HUGE_ALLOC
# define warn_on_exceeds __warn_on_exceeds
#else
# define warn_on_exceeds NOOP
#endif

static inline void assert_allocated(const char *file, int line,
				    const char *func, void *ptr, size_t size)
{
	if (likely(ptr))
		return;

	__termas(file, line, func, TM_FATAL, NULL, TM_FLLN | TM_FUNC,
		 _("out of memory (tried to allocate %zu bytes)"), size);
}

void *__xmalloc(const char *file,
		int line, const char *func, size_t size)
{
	void *ptr;

	warn_on_exceeds(file, line, func, size);
	ptr = malloc(size);

	assert_allocated(file, line, func, ptr, size);
	return ptr;
}

void *__xcalloc(const char *file,
		int line, const char *func, size_t nmemb, size_t size)
{
	void *ptr;
	size_t n = st_mult(nmemb, size);

	warn_on_exceeds(file, line, func, n);
	ptr = calloc(nmemb, size);

	assert_allocated(file, line, func, ptr, n);
	return ptr;
}

void *__xrealloc(const char *file,
		 int line, const char *func, void *ptr, size_t size)
{
	warn_on_exceeds(file, line, func, size);
	ptr = realloc(ptr, size);

	assert_allocated(file, line, func, ptr, size);
	return ptr;
}

void *__xreallocarray(const char *file, int line,
		      const char *func, void *ptr, size_t nmemb, size_t size)
{
	size_t n = st_mult(nmemb, size);

	warn_on_exceeds(file, line, func, n);
	ptr = realloc(ptr, n);

	assert_allocated(file, line, func, ptr, n);
	return ptr;
}

char *__xstrdup(const char *file,
		int line, const char *func, const char *s)
{
	char *ptr = strdup(s);

	if (likely(ptr))
		return ptr;

	__termas(file, line, func, TM_FATAL,
		 strerror(errno), TM_FLLN | TM_FUNC,
		 _("failed to duplicate string '%.10s...'"), s);
	unreachable();
}

wchar_t *__xwcsdup(const char *file,
		   int line, const char *func, const wchar_t *s)
{
	wchar_t *ptr = wcsdup(s);

	if (likely(ptr))
		return ptr;

	__termas(file, line, func, TM_FATAL, strerror(errno),
		 TM_FLLN | TM_FUNC, "failed to duplicate wide string");
	unreachable();
}
