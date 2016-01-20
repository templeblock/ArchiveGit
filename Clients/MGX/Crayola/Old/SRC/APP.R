//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "mrc\types.r"
#include "mrc\balloons.r"
#include "ftab.r"
#include "id.h"

/* Crayola application bundle - just the application's icon and
   the version string */

resource 'BNDL' (128)
{
	'WGEN',	/* signature */
	0,		/* version id */
	{
		'ICN#',
		{
			0, IDC_ABOUTAPP;
		};
		'FREF',
		{
			0, 128;
		};
	};
};

type 'RAYW' as 'STR ';
resource 'RAYW' (0)
{
	"Crayola Art Studio - Copyright 1994-1995 Micrografx, Inc."
};

resource 'FREF' (128)
{
	'APPL', 0, "";
};


/* Balloon help resources */

resource 'hfdr' (-5696)
{
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
		HMSTRResItem { 512 }
	}
};

resource 'STR ' (512)
{
	"Crayola Art Studio by Micrografx."
};



/*	SIZE resource */

resource 'SIZE' (-1)
{
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreAppDiedEvents,
	is32BitCompatible,
	isHighLevelEventAware,
	localAndRemoteHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	4000 * 1024,
	3000 * 1024	
};
