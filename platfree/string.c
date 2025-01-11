// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "string.h"

#include "mbctype.h"

xchar *__strchrnul(const xchar *s, xchar c)
{
	while(*s && *s != c)
		s++;

	return (xchar *)s;
}

size_t __mbslen(const char *s)
{
	size_t len = 0;

	while (*s) {
		s += __mbctype(*s);
		len++;
	}

	return len;
}

wchar_t __mbtowc(const char *seq)
{
	uint shift = 6;
	uint mask = 0x1F;
	wchar_t res = 0;
	uint len = __mbctype(*seq);

	switch (len) {
	case _9D:
		res |= seq[3] & 0x3F;
		shift += 6;
		mask >>= 1;
	case _9C:
		res |= (seq[2] & 0x3F) << (shift - 6);
		shift += 6;
		mask >>= 1;
	case _9B:
		res |= ((seq[0] & mask) << shift) |
		       ((seq[1] & 0x3F) << (shift - 6));
		break;
	default:
		res = seq[0];
	}

	return res;
}
