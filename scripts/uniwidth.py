#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

import re
import sys

eaw = open('EastAsianWidth.txt', 'r')
res = []
ent = [ 0, 0 ]

for line in eaw:
	if not re.search(r';\s*[WF]\s', line):
		continue

	seg = line.split()
	vl = seg[0].split('..')
	val = [ int(vl[0], 16), int(vl[1] if len(vl) == 2 else vl[0], 16) ]

	if val[0] > 0xFFFF:
		break

	if ent[1] + 1 == val[0]:
		ent[1] = val[1]
	else:
		res.append((hex(ent[0]), hex(ent[1])))
		ent[0] = val[0]
		ent[1] = val[1]

res.append((hex(ent[0]), hex(ent[1])))
res = res[1:]
eaw.close()

sys.stdout = open('include/uniwidth.h', 'w')

print(f'''\
/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_UNIWIDTH_H
#define NG39_UNIWIDTH_H

#include <stddef.h>

#include "attr.h"

#define uniwidth_size {len(res)}

extern const wchar_t uniwidth[][2];

int isfullwidth(wchar_t c) __leaf __pure;

#endif /* NG39_UNIWIDTH_H */\
''')

sys.stdout.close()
sys.stdout = open('platfree/uniwidth.c', 'w')

print('''\
// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * Generated by scripts/uniwidth.py
 */

#include "uniwidth.h"

#include "calc.h"

const wchar_t uniwidth[][2] = {\
''')

for val in res:
	print(f'\t{{ {val[0]}, {val[1]} }},')

print('''\
};

int isfullwidth(wchar_t c)
{
	size_t l = 0;
	size_t r = sizeof_array(uniwidth);

	while (l < r) {
		size_t m = (l + r) >> 1;

		if (c < uniwidth[m][0])
			r = m;
		else if (c > uniwidth[m][1])
			l = m + 1;
		else
			return 1;
	}

	return 0;
}\
''')
