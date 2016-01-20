#include "fscdefs.h"
#include "fontmath.h"
#include "fnt.h"
#include "sc.h"
#include "effects.h"

Outline LightPen =
{
	OUTLINE_rectangle,	// type

	1,							// X size
	1,							// Y size

	0,							// X offset
	0							// Y offset
};

Outline MediumPen =
{
	OUTLINE_rectangle,	// type

	2,							// X size
	2,							// Y size

	0,							// X offset
	0							// Y offset
};

Outline HeavyPen =
{
	OUTLINE_region,		// type

	5,							// X size
	5,							// Y size

	2,							// X offset
	2,							// Y offset

	1,3,						// data
	0,4,
	0,4,
	0,4,
	1,3
};

Outline CalligraphyPen =
{
	OUTLINE_diagonal,		// type

	9,							// X size
	-10,						// Y size

	4,							// X offset
	-4							// Y offset
};

// Pens
//
//		Table of static pens indexed by MLS outline number.

Outline* Pens[] =
{
	(Outline*)NO_OUTLINE,
	&LightPen,
	&MediumPen,
	&HeavyPen,
	&CalligraphyPen
};
