// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

static LPTR qBase;
static int qSize;
static LPROC qCompare;

#define COMPARE(i,j) ((*qCompare)(qBase + (long)i * qSize, \
				  qBase + (long)j * qSize))

/************************************************************************/
void qsort(base, n, size, compare)
/************************************************************************/
LPTR base;
long n;
int size;
LPROC compare;
{
qBase = base;
qSize = size; /* qSize = (size + 1) & ~1; */
qCompare = compare;
qsort1( 0L, n );
}


/************************************************************************/
static void swap(i, j)
/************************************************************************/
long i, j;
{
LPTR p, q;
int count;
BYTE b;

p = qBase + i * qSize;
q = qBase + j * qSize;

count = qSize;
while ( --count >= 0 )
	{
	b = *p;
	*p++ = *q;
	*q++ = b;
	}
}


/*
 *  sort elements "first" through "last"-1
 */

/************************************************************************/
static void qsort1(first, last)
/************************************************************************/
long first, last;
{
static long i;		/*  "static" to save stack space  */
long j;

while (last - first > 1)
	{
	i = first;
	j = last;
	while (1)
		{
		while (++i < last && COMPARE(i, first) < 0)
			;
		while (--j > first && COMPARE(j, first) > 0)
			;
		if (i >= j)
 			break;
		swap(i, j);
		}
	if (j == first)
		{
		++first;
		continue;
		}
	swap(first, j);
	if (j - first < last - (j + 1))
		{
		qsort1(first, j);
		first = j + 1;		/*  qsort1(j + 1, last);  */
		}
	else	{
		qsort1(j + 1, last);
		last = j;		/*  qsort1(first, j);  */
		}
	}
}
