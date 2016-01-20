////////////////////////////////////////////////////////////////////////////////////////////
//  measure.cpp																			  //
//  An object abstracts the measuring process from any device or type of device			  //
//  Currently supports Xrite 408, Dtp 41, Techkon handheld and table, and Greytag		  //
//  handheld and table.																	  //
//  																					  //
// 																						  //
//	James Vogh																			  //
//  Nov 12, 1997																		  //
////////////////////////////////////////////////////////////////////////////////////////////

#include "errordial.h"
#include "measure.h"
#include "table.h"
#include "positiondial.h"
#include "tcr.h"
#include "spectrochart.h"
#include "rchoosedialog.h"
#include <math.h>

#define ClickNum 			(50)
#define EndColumnNum		(51)

measureDevice::measureDevice(ProfileDocInf *dc, RawData *pD,int szp)
{
	int32		i,n;
	OSErr			err;
	short		item,which;

	state = MCO_SUCCESS;

	doc = dc;

	patchD = pD;


	next_patch = 0L;
	chan = nil;
	skipZeroPer = szp;

	n = patchD->patches.strips;
	
	if (doc->defaults->input_type == DT_None)
		{
		state = _handle_read(&item,&which,1);
		if (item == 3) 
			{
			state = MCO_CANCEL;
			return;	
			}
		}
	
	// initialize comminications with the device
	if (patchD->patches.rows == 27) 	
		state = doc->openInputDevice(2,0,1);
	else  state = doc->openInputDevice(1,0,1);
	if (state != MCO_SUCCESS) goto fail;

	patch_per_strip = patchD->patches.rows+n;
	strip_per_sheet = patchD->patches.sheets;
	if (patchD->format.numLinear > 0)
		data_patches = patchD->format.total+patchD->format.numc*(patchD->format.numLinear-skipZeroPer);
	else 
		data_patches = patchD->format.total;

	if (data_patches % (patch_per_strip - n) == 0)
		total_patches = ((data_patches/(patch_per_strip - n)))*patch_per_strip;
	else total_patches = ((data_patches/(patch_per_strip - n))+1)*patch_per_strip;

	
	num_used_patches  = data_patches;
	
	next_patch = new int32[total_patches];
	if (!next_patch) goto fail;
	
	for (i=0; i<total_patches; i++)
 		{
		next_patch[i] = i+1;
		}
	next_patch[total_patches-1] = total_patches-1;
	
	current_patch = 0;	
	
	
	err = SndNewChannel (&chan, 0, 0, nil);
	if (err != noErr) chan = nil;
fail:	
	return;
}



measureDevice::~measureDevice(void)
{
OSErr			err;
	
if (doc) doc->closeInputDevice();
if (next_patch) delete next_patch;
if (chan != nil) err = SndDisposeChannel (chan,TRUE);
}

McoStatus measureDevice::_handle_read(short *item,short *which,int density)
{
	RchooseDialog *rchoosedialog;
	
	
	rchoosedialog = new RchooseDialog(doc,density);
	if(!rchoosedialog)
		return MCO_MEM_ALLOC_ERROR;
	
	*item = rchoosedialog->hit();
	*which = rchoosedialog->getchoice();
	delete rchoosedialog;

	
	return MCO_CANCEL;	
}

McoStatus measureDevice::playSound(short sn)
{
	Handle			Sound;
	
	OSErr			err;
	
	if (sn == -1) return MCO_SUCCESS;

	Sound = GetResource ('snd ', sn);
	if (ResError() != noErr || Sound == nil) return MCO_RSRC_ERROR;
	

	HLock(Sound);
	err = SndPlay (chan, (SndListResource**)Sound, TRUE);
	if (err != noErr) return MCO_RSRC_ERROR;

	
	HUnlock(Sound);
	ReleaseResource(Sound);
	return MCO_SUCCESS;
}

void	measureDevice::setCurrentPatch(int cp)
{
if (cp >= total_patches) return;
if (cp < 0) return;
current_patch = cp;
}

McoStatus measureDevice::DoStart(void)
{
McoStatus status;
double	w,h;
int32	papernum;
double	patchPoints[6];
int32		tablePoints[6];
TCR		*tcr;
SpectroChart *spch;


if (!doc) return MCO_FAILURE;
if (!doc->tcom) return MCO_FAILURE;

switch (doc->tcom->device_num) {
	case DT_DTP51:			// strip readres
	case DT_XriteDTP41:
		break;
	case DT_TechkonCP300:  	// handheld devices
	case DT_TechkonSP820:
	case DT_GretagHand:
	case DT_Xrite408:
		status = doc->tcom->Calibrate();
		if (skipZeroPer)
			{
			McoMessAlert(MCO_MEAS_PAPER_DENSITY,0L);
			doc->tcom->SetDensityOutput();
			status = doc->tcom->GetPaperDensity();
			if (doc->tcom->device_num == DT_Xrite408) playSound(ClickNum);
			doc->tcom->SetLabOutput();
			}
		break;
	case DT_TechkonTCRCP300: // tables
	case DT_TechkonTCRSP820:

		tcr = (TCR*)doc->tcom;
		w = patchD->patches.width;
		h = patchD->patches.height;

		if (patchD->type == CMYKPatches) papernum = 0;
		else if (patchD->type == RGBPatches) papernum = patchD->format.total-1;

				
		if (tcr != 0L) status = tcr->setUpPatches(patchD->patches.columns,patchD->patches.rows,w,h,patchD->patches.starting,patchD->patches.direction,papernum);

		tcr->getPatchPoint(0,&patchPoints[0]);
		tcr->getPatchPoint(patchD->patches.rows-1,&patchPoints[2]);
		tcr->getPatchPoint(patchD->patches.rows*(patchD->patches.columns-1),&patchPoints[4]);

		//tcom->setCurrentPatch(0);
		tcr->moveToPatch(0,1);
		status = PositionTable((Table*)(tcr),&tablePoints[0],1,1);
		if (status != MCO_SUCCESS) return status;
		tcr->setPoints_1(tablePoints,patchPoints);
		tcr->moveToPatch(patchD->patches.rows-1,1);
		status = PositionTable((Table*)(tcr),&tablePoints[2],2,1);
		if (status != MCO_SUCCESS) return status;
		tcr->setPoints_1(tablePoints,patchPoints);
		tcr->moveToPatch(patchD->patches.rows*(patchD->patches.columns-1),1);
		status = PositionTable((Table*)(tcr),&tablePoints[4],3,1);
		if (status != MCO_SUCCESS) return status;
		tcr->setPoints_3(tablePoints,patchPoints);
		status = doc->tcom->Calibrate();
		if (status != MCO_SUCCESS) return status;
		status = tcr->moveToPatch(current_patch,0);
		if (status != MCO_SUCCESS) return status;
		if (skipZeroPer)
			{
			doc->tcom->SetDensityOutput();
			doc->tcom->GetPaperDensity();
			doc->tcom->SetLabOutput();
			}

		break;
		
	case DT_GretagTable:
	
		spch = (SpectroChart*)doc->tcom;
		w = patchD->patches.width;
		h = patchD->patches.height;

		if (patchD->type == CMYKPatches) papernum = 0;
		else if (patchD->type == RGBPatches) papernum = patchD->format.total-1;

				
		if (spch != 0L) status = spch->setUpPatches(patchD->patches.columns,patchD->patches.rows,w,h,patchD->patches.starting,patchD->patches.direction,papernum);

		spch->getPatchPoint(0,&patchPoints[0]);
		spch->getPatchPoint(patchD->patches.rows-1,&patchPoints[2]);
		spch->getPatchPoint(patchD->patches.rows*(patchD->patches.columns-1),&patchPoints[4]);

		//tcom->setCurrentPatch(0);
		spch->moveToPatch(0,1);
		status = PositionTable((Table*)(spch),&tablePoints[0],1,1);
		if (status != MCO_SUCCESS) return status;
		spch->setPoints_1(tablePoints,patchPoints);
		spch->moveToPatch(patchD->patches.rows-1,1);
		status = PositionTable((Table*)(spch),&tablePoints[2],2,1);
		if (status != MCO_SUCCESS) return status;
		spch->setPoints_1(tablePoints,patchPoints);
		spch->moveToPatch(patchD->patches.rows*(patchD->patches.columns-1),1);
		status = PositionTable((Table*)(spch),&tablePoints[4],3,1);
		if (status != MCO_SUCCESS) return status;
		spch->setPoints_3(tablePoints,patchPoints);
		status = spch->Calibrate();
		if (status != MCO_SUCCESS) return status;
		status = spch->moveToPatch(current_patch,0);
		if (status != MCO_SUCCESS) return status;
		if (skipZeroPer)
			{
			spch->SetDensityOutput();
			spch->GetPaperDensity();
			spch->SetLabOutput();
			}

		break;
		
	}


switch (doc->tcom->device_num) {
	case DT_DTP51:			// strip readres
	case DT_TechkonCP300:  	// handheld devices
	case DT_TechkonSP820:
	case DT_TechkonTCRCP300: // tables
	case DT_TechkonTCRSP820:
		trueDensity = 0;
		break;
	case DT_XriteDTP41:
	case DT_Xrite408:
	case DT_GretagTable:
	case DT_GretagHand:
		trueDensity = 1;
		break;
	}
	
_waiting = TRUE;
return status;	
}

McoStatus measureDevice::ReadPatches(int *changed,int *finished, int *patch)
{
int i,j,n,count;
McoStatus state = MCO_SUCCESS;
double *data,*data1 = 0L,*data2 = 0L;

*changed = 0;

if (!doc) return MCO_FAILURE;
if (!doc->tcom) return MCO_FAILURE;

if (!_waiting) return MCO_SUCCESS;

if ((current_patch < patchD->format.total) || (patchD->format.numLinear == 0)) doc->tcom->SetLabOutput();
else doc->tcom->SetDensityOutput();

if (patchD->dataH) data1 = (double*)McoLockHandle(patchD->dataH);
if (patchD->linearDataH) data2 = (double*)McoLockHandle(patchD->linearDataH);	
		
switch (doc->tcom->device_num) {
	case DT_DTP51:			// strip readres
	case DT_XriteDTP41:
		
		if (doc->tcom->IsBufferEmpty()) goto finish;
		if (!doc->tcom->IsDataPresent()) goto finish;
	
		data = new double[patch_per_strip*4];
	
		current_patch = current_patch/patch_per_strip;
		current_patch *= patch_per_strip;

		for (i=0; i<patch_per_strip; i++)
			{
			count = 0;
			do {
				if (doc->tcom != NULL) state = doc->tcom->getNextPatch(data+i*3);
				count ++;
				} while ((state != MCO_SUCCESS) && (count < 5));
			if (state != MCO_SUCCESS) return state;
			}
		for (i=0; i<patch_per_strip; i++)
			{
			if ((current_patch > patchD->format.total) && (data1)) 
				{
				data1[current_patch*3] = data[i*3];
				data1[current_patch*3+1] = data[i*3+1];
				data1[current_patch*3+2] = data[i*3+2];
				}
			else if (data2)
				{
				if ((current_patch == patchD->format.total) && (!skipZeroPer)) doc->tcom->SetPaperDensity(&data[i*4]);
				if (trueDensity)
					{
					j = (current_patch-patchD->format.total)/(patchD->format.numLinear-skipZeroPer);
					data2[(current_patch - patchD->format.total)*3] = data[i*4+j];
					data2[(current_patch - patchD->format.total)*3+1] = 0;
					data2[(current_patch - patchD->format.total)*3+2] = 0;
					}
				else 
					{
					data2[(current_patch - patchD->format.total)*3] = data[i*3];
					data2[(current_patch - patchD->format.total)*3+1] = data[i*3+1];
					data2[(current_patch - patchD->format.total)*3+2] = data[i*3+2];
					}
				}
			current_patch++;
			}
		if (current_patch >= total_patches) 
			{
			current_patch = total_patches -1;
			_waiting = FALSE;
			}
		*changed = 1;
		delete data;	
		
		break;
		
	case DT_TechkonCP300:  	// handheld devices
	case DT_TechkonSP820:
	case DT_GretagHand:
	case DT_Xrite408:
	
		if (doc->tcom->IsBufferEmpty()) goto finish;
		if (!doc->tcom->IsDataPresent()) goto finish;
	
		data = new double[4];
		count = 0;
		do {
			state = doc->tcom->getNextPatch(data);
			if (state == MCO_SUCCESS) 
				{
				 playSound(ClickNum);
				}
			count ++;
			} while ((state != MCO_SUCCESS) && (count < 5));
		if (state != MCO_SUCCESS) return state;

		if ((current_patch > patchD->format.total) && (data1)) 
			{
			data1[current_patch*3] = data[0];
			data1[current_patch*3+1] = data[1];
			data1[current_patch*3+2] = data[2];
			}
		else if (data2)
			{
			if ((current_patch == patchD->format.total) && (!skipZeroPer)) doc->tcom->SetPaperDensity(data);
			if (trueDensity)
				{
				j = (current_patch-patchD->format.total)/(patchD->format.numLinear-skipZeroPer);
				data2[(current_patch - patchD->format.total)*3] = data[j];
				data2[(current_patch - patchD->format.total)*3+1] = 0;
				data2[(current_patch - patchD->format.total)*3+2] = 0;
				}
			else 
				{
				data2[(current_patch - patchD->format.total)*3] = data[0];
				data2[(current_patch - patchD->format.total)*3+1] = data[1];
				data2[(current_patch - patchD->format.total)*3+2] = data[2];
				}
			}
		current_patch++;
		*changed = 1;	
		delete data;
		break;
	case DT_TechkonTCRCP300: // tables
	case DT_TechkonTCRSP820:
	case DT_GretagTable:	
	
		data = new double[4];
		count = 0;
		do {
			state = doc->tcom->getNextPatch(current_patch,data);
			if (state == MCO_SUCCESS) 
				{
				 playSound(ClickNum);
				}
			count ++;
			} while ((state != MCO_SUCCESS) && (count < 5));
		if (state != MCO_SUCCESS) return state;

		if ((current_patch > patchD->format.total) && (data1)) 
			{
			data1[current_patch*3] = data[0];
			data1[current_patch*3+1] = data[1];
			data1[current_patch*3+2] = data[2];
			}
		else if (data2)
			{
			if ((current_patch == patchD->format.total) && (!skipZeroPer)) doc->tcom->SetPaperDensity(data);
			if (trueDensity)
				{
				j = (current_patch-patchD->format.total)/(patchD->format.numLinear-skipZeroPer);
				data2[(current_patch - patchD->format.total)*3] = data[j];
				data2[(current_patch - patchD->format.total)*3+1] = 0;
				data2[(current_patch - patchD->format.total)*3+2] = 0;
				}
			else 
				{
				data2[(current_patch - patchD->format.total)*3] = data[0];
				data2[(current_patch - patchD->format.total)*3+1] = data[1];
				data2[(current_patch - patchD->format.total)*3+2] = data[2];
				}
			}
		current_patch++;
		*changed = 1;	
		delete data;
		break;
	}
	
finish:	


if (current_patch == total_patches) *finished = TRUE;
else *finished = FALSE;

*patch = current_patch;

if (patchD->dataH) McoUnlockHandle(patchD->dataH);
if (patchD->linearDataH) McoUnlockHandle(patchD->linearDataH);
	
return MCO_SUCCESS;
}	
	
	