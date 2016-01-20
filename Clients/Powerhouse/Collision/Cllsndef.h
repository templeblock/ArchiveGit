#ifndef _cllsndef_h_
#define _cllsndef_h

// INI file stuff
#define INI_FILE			"CLLSN.INI"
#define OPTIONS_SECTION		"Options"

													// Shot Flag Bits
// NOTE: If you changed these make sure the shot flag masks are also changed.
//
#define F_TYPE_START    1		// First type flag
#define F_SPINE_HIT		1
#define F_SPINE_DEAD	2
#define F_NEWCELL		3
#define F_YOU_LOSE		4
#define F_YOU_WIN		5
#define F_EXIT			6
#define F_HIT			7
#define F_AUTOREVERSE	8
#define F_LIVE_DIE		9
#define F_TYPE_LAST		9		// Last type flag

#define F_HX			10
#define F_UJ			11

#define F_POS_FIRST		12		// First f_pos flag 
#define F_POS_BEG		12
#define F_POS_MID		13
#define F_POS_END		14
#define F_POS_FULL		15
#define F_POS_FULLC		16
#define F_POS_WALL		17
#define F_POS_VEER		18
#define F_POS_BEG_3QTR  19
#define F_POS_LAST		19		// Last f_pos flag

// Shot flag masks and other aids
#define FSTART_SHOTTYPES		F_SPINE_HIT
#define FSTART_CELLTYPES		F_HX
#define FSTART_CELLPOSITIONS	F_POS_BEG
#define FMASK_SHOTTYPES			0x0001FF
#define FMASK_CELLTYPES			0x000600
#define FMASK_CELLPOSITIONS		0x07F800

#endif
