////////////////////////////////////////////////////////////////////////////////////////////
//  measure.h																			  //
//  An object abstracts the measuring process from any device or type of device			  //
//  Currently supports Xrite 408, Dtp 41, Techkon handheld and table, and Greytag		  //
//  handheld and table.																	  //
//  																					  //
// 																						  //
//	James Vogh																			  //
//  Nov 12, 1997																		  //
////////////////////////////////////////////////////////////////////////////////////////////


#ifndef IN_MEASURE
#define IN_MEASURE

#include "profiledocinf.h"
#include	<Sound.h>

class measureDevice {
private:
protected:
ProfileDocInf *doc;

int32	patch_per_strip,strip_per_sheet,data_patches,total_patches,num_used_patches,current_patch;

int32	*next_patch;

int		skipZeroPer;

RawData	*patchD;

int 	_waiting;

int 	trueDensity;



// The sound channel used for alerting the user to errors
SndChannelPtr	chan;

public:

McoStatus 	state;

measureDevice(ProfileDocInf *dc, RawData *pD,int szp);

~measureDevice(void);

McoStatus _handle_read(short *item,short *which,int density);

McoStatus playSound(short sn);

McoStatus DoStart(void);

void	setCurrentPatch(int cp);

McoStatus ReadPatches(int *changed,int *finished,int *patch);
};

#endif