//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _MAP_H_
#define _MAP_H_

typedef struct _maps
	{
	MAP	MasterMap;	// affects all channels
	MAP	RedMap; 	// sometimes CYAN
	MAP	GreenMap;	// sometimes MAGENTA
	MAP	BlueMap;	// sometimes YELLOW
	MAP	BlackMap;
	MAP	HueMap;
	MAP	SatMap;
	} MAPS, FAR *LPMAPS;

BOOL ContBritImage(LPIMAGE lpImage, LPCONTBRIT_PARMS lpParms);
BOOL BalanceImage(LPIMAGE lpImage, LPBALANCE_PARMS lpParms);
BOOL MapImage(LPIMAGE lpImage, LPMODIFYMAP_PARMS lpParms);
BOOL ToneImage(LPIMAGE lpImage, LPTONE_PARMS lpParms);
BOOL PosterizeImage(LPIMAGE lpImage, LPPOSTERIZE_PARMS lpParms);
BOOL HueImage(LPIMAGE lpImage, LPHUE_PARMS lpParms);
BOOL ApplyCalImage(LPIMAGE lpImage, LPAPPLYCAL_PARMS lpParms);
STATUS_CODE ApplyColorMap(LPIMAGE lpImage, LPCOLORMAP lpColorMap, BOOL fReMap,
							DITHER_TYPE DitherType, ITEMID idDirty);

#endif // _MAP_H_
