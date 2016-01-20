////////////////////////////////////////////////////////////////////////////////
//	profileDoc.c															  //
//																			  //
//	The code for dealing with the profile data, replaces much of printdialog.c//
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "profiledoc.h"
#include <stdio.h>
#include "colortran.h"
#include "monacoprint.h"

#include "csprofile.h"

#include "gammut_comp.h"
#include "calib_multi.h"
#include "add_black.h"
#include "find_blacklab.h"
#include "spline3.h"

#include "SLFunctions.h"

#include "linearize_data.h"

#include "buildtableindex.h"
#include "mpfileio.h"

#include "picelin3.h"

ProfileDoc::ProfileDoc(void)
{
	long 	i;
	long	max;
	unsigned char		*clip;

// unlock the document
	locked = 0;
	processing = 0;

// the patch data and a copy
	patchD = new RawData;
	if (patchD == 0L) goto bail;
	patchDcopy = new RawData;
	if (patchDcopy == 0L) goto bail;
	
	patchtype = No_Patches;
	
// the simulation ink patch data
	simupatchD = new RawData;
	if (simupatchD == 0L) goto bail;
	
	simupatchDcopy = new RawData;
	if (simupatchDcopy == 0L) goto bail;
			
//create connection space to device space table memory
	_con_to_dev_tableH = McoNewHandle(sizeof(double)*TABLE_ENTRIES*4);
	if(!_con_to_dev_tableH)
		goto bail;

//create device space to connection space table memory
	_dev_to_con_tableH = McoNewHandle(sizeof(unsigned char)*DEV_TO_CON_TABLE_ENTRIES*3);
	if(!_dev_to_con_tableH)
		goto bail;

//create device space to connection space table memory
	_dev_to_con_tableH2 = McoNewHandle(sizeof(unsigned char)*DEV_TO_CON_TABLE_ENTRIES*3);
	if(!_dev_to_con_tableH2)
		goto bail;


//create ICC out-of-gamut table memory
	_out_of_gamutH = McoNewHandle(sizeof(unsigned char)*TABLE_ENTRIES);
	if(!_out_of_gamutH)
		goto bail;

//create lab table memory, using photoshop format, for gamut compression
	_labtableH = McoNewHandle(sizeof(double)*TABLE_ENTRIES*3);
	if(!_labtableH)
		goto bail;
		
//create a copy of the lab table memory, 
	_labtableCopyH = McoNewHandle(sizeof(double)*TABLE_ENTRIES*3);
	if(!_labtableCopyH)
		goto bail;

//create blacktable memory, using photoshop format
	_blacktableH = McoNewHandle(sizeof(double)*101);
	if(!_blacktableH)
		goto bail;
		
	_blacktable = (double*)McoLockHandle(_blacktableH);
	for (i=0; i<101; i++) _blacktable[i] = (double)i;
	McoUnlockHandle(_blacktableH);
	

//added by James on 11/5, allocate the gammut surface buffer
	_gammutSurfaceH = McoNewHandle(sizeof(double)*4*256*360);
	if (!_gammutSurfaceH)
		goto bail;
		
//added by James on 2/20, allocate the gammut surface buffer for the CMYK inks
	_gammutSurfaceBlackH = McoNewHandle(sizeof(double)*4*256*360);
	if (!_gammutSurfaceBlackH)
		goto bail;
		
// allocate the gammut surface buffer for the CMYK inks
	_gammutSurfaceManyBlackH = McoNewHandle(sizeof(double)*50*72*GAM_DIM_K);
	if (!_gammutSurfaceManyBlackH)
		goto bail;
		
// allocate the gammut surface buffer for the CMYK inks
// (The L values)
	_gammutSurfaceManyBlackLH = McoNewHandle(sizeof(double)*50*72*GAM_DIM_K);
	if (!_gammutSurfaceManyBlackLH)
		goto bail;
		
	didGammutSurface = 0;
	
	didGammutCompression = 0;
				
// Added by James 11/11
//create cmyk table memory, using photoshop format
	_cmyktableH = McoNewHandle(sizeof(double)*CMYK_TABLE_ENTRIES*3);
	if(!_cmyktableH)
		goto bail;
		
	K_only_cmyk = 0L;	
		
		
	// create the tweaking object	
	
	tweak = new Tweak_Patch(Calibrate_Tweak);
	if (!tweak) goto bail;
	
	// create the gamut tweaking object	
	
	gtweak = new Tweak_Patch(GamutComp_Tweak);
	if (!gtweak) goto bail;

//clipping table
	clip = _clipt;
	for(i = 0; i < 5120; i++)
		clip[i] = 0;
	clip = &_clipt[5120];	
	for(i = 0; i < 256; i++)
		clip[i] = i;
	clip = &_clipt[5376];	
	for(i = 0; i < 5120; i++)
		clip[i] = 255;
	
	_initphotolab();
	
	_condata.tabletype = 2;	// 1 = photoshp, 2 = ICC, 3 = CRD, 4 = RGB, 5 = ICC CMYK, 6 = ICC TEK
	_condata.separation_type = 1;	//GCR == 0, UCR == 1
	_condata.black_type = 2; 		//None==1,Light==2,Medium==3,Heavy==4 or Maximum==5
	_condata.total_ink_limit = 400;	// from 0 to 400
	_condata.black_ink_limit = 100;	// from 0 to 100
	_condata.saturation = 100;		// from 0 to 100, 100 is max saturation
	_condata.col_balance = 0;	// 0 = neutral, 100 = paper color
	_condata.L_linear = 100;		// 0 = linear, 100 = compressed
	_condata.ink_linerize = 1;	// lineraize ink L
	_condata.ink_neutralize = 1; // neutralize ink sim for paper color
	_condata.ink_highlight = 0;  // lock the highlight point (L goes from minL to 100)
	_condata.rev_table_size = 1;	//reverse table size, large = 17, small = 9
	_condata.simulate = 0;	//simulate or not, if == 1, yes
	_condata.smooth = 1;		//amount of smoothing to apply
	_condata.smooth_sim = 1;	//amount of smoothing to apply to simulation
	_condata.col_bal_cut = 94; // defines where to cuttoff the color balnce function, 0== cuttoff at L = 0, 256 = cuttoff at L = 95
	_condata.K_only = 1;		// I

	
	// the buffer for the tiff file, do not allocate here
	targetBufferH = 0L;
	
	printerData = 0L;
	printData = 0L;
	calCurves = 0L;
	
	// set a couple of flags
	changedProfile = 0;
	monacoProfile = 1;
	monacoEdited = 0;
	changedLinearTone = 0;

	createdProfile = 1;
	
	
	linear_data = 0L;
	
	_staurationpreview = 0;	
	
	return;
	
bail:
	validData = FALSE;
	return;	
}

ProfileDoc::~ProfileDoc(void)
{
	if (patchD != 0L) delete patchD;
	if (patchDcopy != 0L) delete patchDcopy;
	if (simupatchD != 0L) delete simupatchD;
	if (simupatchDcopy != 0L) delete simupatchDcopy;

	if(_con_to_dev_tableH)
		McoDeleteHandle(_con_to_dev_tableH);
	if(_dev_to_con_tableH)
		McoDeleteHandle(_dev_to_con_tableH);
	if(_dev_to_con_tableH2)
		McoDeleteHandle(_dev_to_con_tableH2);
	if(_out_of_gamutH)
		McoDeleteHandle(_out_of_gamutH);
	if(_labtableH)
		McoDeleteHandle(_labtableH);
	if(_labtableCopyH)
		McoDeleteHandle(_labtableCopyH);
	if(_blacktableH)
		McoDeleteHandle(_blacktableH);	
		
	if (_gammutSurfaceH)
		McoDeleteHandle(_gammutSurfaceH);			
		
	if (_gammutSurfaceBlackH)
		McoDeleteHandle(_gammutSurfaceBlackH);
		
	if (_gammutSurfaceManyBlackH)
		McoDeleteHandle(_gammutSurfaceManyBlackH);		

	if (_gammutSurfaceManyBlackLH)
		McoDeleteHandle(_gammutSurfaceManyBlackLH);		
		
	if (_cmyktableH)
		McoDeleteHandle(_cmyktableH);		
		
	if (K_only_cmyk)
		delete K_only_cmyk;	
		
	if (tweak)
		delete tweak;
		
	if (gtweak)
		delete gtweak;
		
	if (targetBufferH)
		McoDeleteHandle(targetBufferH);	
		
	if (linear_data) delete linear_data;
	
	if (printerData) delete printerData;
		
#ifdef IN_POLAROID
	
	
	
	if (printData) 
		{
		Mpfileio mp(printData);
		mp.cleanup(2);
		delete printData;
		}
	if (calCurves) delete calCurves;
#endif		

}

McoStatus ProfileDoc::setData(ConData *data)
{

	_condata.tabletype = data->tabletype;
	_condata.separation_type = data->separation_type;	//GCR == 1, UCR == 2
	_condata.black_type = data->black_type; //None==1,Light==2,Medium==3,Heavy==4 or Maximum==5
	_condata.total_ink_limit = data->total_ink_limit;
	_condata.black_ink_limit = data->black_ink_limit;
	_condata.saturation = data->saturation;
	_condata.col_balance = data->col_balance;
	_condata.L_linear = data->L_linear;
	_condata.ink_linerize = data->ink_linerize;	
	_condata.ink_neutralize = data->ink_neutralize; 
	_condata.ink_highlight = data->ink_highlight; 
	_condata.rev_table_size = data->rev_table_size;	
	_condata.simulate = data->simulate;	
	_condata.smooth = data->smooth;		
	_condata.smooth_sim = data->smooth_sim;	
	_condata.col_bal_cut = data->col_bal_cut; 
	_condata.K_only = data->K_only;

return MCO_SUCCESS;
}

McoStatus ProfileDoc::setKCurve(BlackCurve *curve)
{
int i;

		
	_blacktable = (double*)McoLockHandle(_blacktableH);
	for (i=0; i<101; i++) _blacktable[i] = curve->curve[i];
	McoUnlockHandle(_blacktableH);	

return MCO_SUCCESS;
}


McoStatus ProfileDoc::initPrintData(void)
{
int i,j;
char *dev_con,*con_dev;
Mpfileio *tempmpf;

	if (printData) 
		{
		tempmpf = new Mpfileio(printData);
		tempmpf->cleanup(2); // cleanup everything
		delete tempmpf;
		delete printData;
		}
		
	printData = new PrintData;
	if (!printData) return MCO_MEM_ALLOC_ERROR;

	printData->numlinear = 4;	
	printData->sizelinear = 256;
	
	for(i = 0; i < 6; i++){
		printData->linear[i] = new double[printData->sizelinear];
		for(j = 0; j < 256; j++)
			printData->linear[i][j] = (double)j;
	}
	
	printData->numtone = 4;
	printData->sizetone = 256;

	for(i = 0; i < 6; i++){
		printData->tone[i] = new double[printData->sizetone];
		for(j = 0; j < 256; j++)
			printData->tone[i][j] = (double)j;
	}
	
	printData->A2B0.input_channels = 4;
	printData->A2B0.output_channels = 4;
	printData->A2B0.grid_points	= 2;
	printData->A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes
	printData->A2B0.input_entries = 256;
	printData->A2B0.output_entries = 256;
	printData->A2B0.offset = 0;

	for (i=0; i< 6; i++) 
		{
		printData->A2B0.input_tables[i] = 0L;
		printData->A2B0.output_tables[i] = 0L;
		}
	
	for(i = 0; i < printData->A2B0.input_channels; i++)
		{
		dev_con = new char[256];
		if (!dev_con) return MCO_MEM_ALLOC_ERROR;
		for(j = 0; j < 256; j++){
			dev_con[j] = j;
			}	
		printData->A2B0.input_tables[i] = dev_con;	
		}
		
	for(i = 0; i < printData->A2B0.output_channels; i++)
		{
		con_dev = new char[256];
		if (!con_dev) return MCO_MEM_ALLOC_ERROR;
		for(j = 0; j < 256; j++){
			con_dev[j] = j;
			}	
		printData->A2B0.output_tables[i] = con_dev;	
		}
		
	printData->icctype = MONACO_CREATED_ICC;
	
	for (i=0; i<6; i++) printData->setting.numLinearHand[i] = 0;
	for (i=0; i<6; i++) printData->setting.numToneHand[i] = 0;
		
	printData->srcpatch.patchType = 0;
	printData->srcpatch.numComp = 0;
	printData->srcpatch.numCube = 0;
	printData->srcpatch.numLinear = 0;
	printData->srcpatch.steps = 0L;
	printData->srcpatch.blacks = 0L;
	printData->srcpatch.cmy = 0L;
	printData->srcpatch.linear = 0L;
	printData->srcpatch.data = 0L;
	printData->srcpatch.ldata = 0L;
	printData->srcpatch.name[0] = 0;
	
	printData->destpatch.patchType = 0;
	printData->destpatch.numComp = 0;
	printData->destpatch.numCube = 0;
	printData->destpatch.numLinear = 0;
	printData->destpatch.steps = 0L;
	printData->destpatch.blacks = 0L;
	printData->destpatch.cmy = 0L;
	printData->destpatch.linear = 0L;
	printData->destpatch.data = 0L;
	printData->destpatch.ldata = 0L;
	printData->destpatch.name[0] = 0;
	
	printData->setting.linearDesc[0] = 0;
	printData->setting.toneDesc[0] = 0;
	
	printData->bitsize = 8;
		
	printData->setting.flag = 0;	
		
	for (i=0; i<6; i++) 
		{
		printData->setting.numLinearHand[i] = 0;
		for (j=0; j<MAX_LINEAR_HAND; j++)
			{ 
			printData->setting.linearX[i*MAX_LINEAR_HAND+j] = 0;
			printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 0;
			}
		printData->setting.linearX[i*MAX_LINEAR_HAND] = 0;
		printData->setting.linearX[i*MAX_LINEAR_HAND+1] = 0;
		printData->setting.linearY[i*MAX_LINEAR_HAND] = 0;
		printData->setting.linearY[i*MAX_LINEAR_HAND+1] = 0;
		
		printData->setting.numToneHand[i] = 2;
		for (j=0; j<MAX_TONE_HAND; j++) 
			{
			printData->setting.toneX[i*MAX_TONE_HAND+j] = 0;
			printData->setting.toneY[i*MAX_TONE_HAND+j] = 0;
			}
		printData->setting.toneX[i*MAX_TONE_HAND] = 0;
		printData->setting.toneX[i*MAX_TONE_HAND+1] = 100;
		printData->setting.toneY[i*MAX_TONE_HAND] = 0;
		printData->setting.toneY[i*MAX_TONE_HAND+1] = 100;
		}
		
printData->tableH = 0L;	
printData->tweakele = 0L;
printData->numTweaks = 0;
		
printData->A2B0.grid_points	= _rev_table_grid;		
		
// copy the tone and linearization curves
if (calCurves) delete calCurves;

calCurves = new CalCurves(printData);

copyIntoPrintData();
		
return MCO_SUCCESS;
}

// clear the tone and linear data for a revert
void ProfileDoc::clearToneLinear(void)
{

	int i,j;

	for (i=0; i<6; i++) 
		{
		printData->setting.numLinearHand[i] = 0;
		for (j=0; j<MAX_LINEAR_HAND; j++)
			{ 
			printData->setting.linearX[i*MAX_LINEAR_HAND+j] = 0;
			printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 0;
			}
		printData->setting.linearX[i*MAX_LINEAR_HAND] = 0;
		printData->setting.linearX[i*MAX_LINEAR_HAND+1] = 0;
		printData->setting.linearY[i*MAX_LINEAR_HAND] = 0;
		printData->setting.linearY[i*MAX_LINEAR_HAND+1] = 0;
		
		printData->setting.numToneHand[i] = 2;
		for (j=0; j<MAX_TONE_HAND; j++) 
			{
			printData->setting.toneX[i*MAX_TONE_HAND+j] = 0;
			printData->setting.toneY[i*MAX_TONE_HAND+j] = 0;
			}
		printData->setting.toneX[i*MAX_TONE_HAND] = 0;
		printData->setting.toneX[i*MAX_TONE_HAND+1] = 100;
		printData->setting.toneY[i*MAX_TONE_HAND] = 0;
		printData->setting.toneY[i*MAX_TONE_HAND+1] = 100;
		}
}

McoStatus ProfileDoc::copyIntoPrintData(void)
{

int i,j;
McoStatus status = MCO_SUCCESS;

if (printData == 0L) return MCO_OBJECT_NOT_INITIALIZED;

// copy the patch data
if (!simupatchD) return MCO_OBJECT_NOT_INITIALIZED;
status = simupatchD->copyIntoPatchData(&printData->srcpatch);
if (status != MCO_SUCCESS) return status;

status = patchD->copyIntoPatchData(&printData->destpatch);
if (status != MCO_SUCCESS) return status;


// copy the tweak data
if (!printerData) return MCO_OBJECT_NOT_INITIALIZED;
printData->numTweaks = printerData->num_tweaks;

if (printData->tweakele) delete printData->tweakele;
if (printerData->num_tweaks > 0) 
	{
	printData->tweakele = new TweakElement[printerData->num_tweaks];
	if (!printData->tweakele) return MCO_MEM_ALLOC_ERROR;
	}
for (i=0; i<printData->numTweaks; i++)
	{
	printData->tweakele[i].Lr = printerData->tweaks[i]->Lr;
	printData->tweakele[i].Cr = printerData->tweaks[i]->Cr;
	printData->tweakele[i].type = printerData->tweaks[i]->type;
	for (j=0; j<3; j++)
		{
		printData->tweakele[i].lab_d[j] = printerData->tweaks[i]->lab_d[j];
		printData->tweakele[i].lab_g[j] = printerData->tweaks[i]->lab_g[j];
		printData->tweakele[i].lab_p[j] = printerData->tweaks[i]->lab_p[j];
		}
	for (j=0; j<40; j++)
		{
		printData->tweakele[i].name[j] = printerData->tweaks[i]->name[j];
		}
	}
	
// copy the black curves
for (i=0; i<31; i++)
	{
	printData->setting.blackX[i] = 0;
	printData->setting.blackY[i] = 0;
	}
printData->setting.numBlackHand = printerData->num_hands;
for (i=0; i<printerData->num_hands; i++)
	{
	printData->setting.blackX[i] = printerData->x_hands[i];
	printData->setting.blackY[i] = printerData->y_hands[i];
	}
	
//setKCurve(printData->setting.blackCurve);
_blacktable = (double*)McoLockHandle(_blacktableH);
for (i=0; i<101; i++) printData->setting.blackCurve[i] = _blacktable[i];
McoUnlockHandle(_blacktableH);	

if (calCurves) delete calCurves;
calCurves = new CalCurves(printData);

// copy the settings	
printData->setting.tableType = _condata.tabletype;			
printData->setting.separationType = _condata.separation_type;
printData->setting.blackType = _condata.black_type;		
printData->setting.totalInkLimit = _condata.total_ink_limit;	
printData->setting.blackInkLimit = _condata.black_ink_limit;	
printData->setting.saturation = _condata.saturation;			
printData->setting.colorBalance = _condata.col_balance;		
printData->setting.colorBalanceCut = _condata.col_bal_cut; 		
printData->setting.linear = _condata.L_linear;			
printData->setting.inkLinearize = _condata.ink_linerize;		
printData->setting.inkNeutralize = _condata.ink_neutralize; 	
printData->setting.inkHighlight = _condata.ink_highlight;	
printData->setting.simulate = _condata.simulate;			
printData->setting.smooth = _condata.smooth;				
printData->setting.revTableSize = _condata.rev_table_size;		
printData->setting.smoothSimu = _condata.smooth_sim;			
printData->setting.kOnly = _condata.K_only;	

printData->A2B0.grid_points	= _rev_table_grid;
	
return status;
	
}


McoStatus	ProfileDoc::copyOutOfPrintData(void)
{
int i,j;
McoStatus status = MCO_SUCCESS;

if (printData == 0L) return MCO_OBJECT_NOT_INITIALIZED;

// set up backup data if necessary


if ((printData->numlinear != printData->A2B0.output_channels) || 
	(printData->sizelinear != printData->A2B0.output_entries))	// set up linear
	{
	printData->numlinear = printData->A2B0.output_channels;
	printData->sizelinear =  printData->A2B0.output_entries;
	for (i=0; i<printData->numlinear; i++) 
		{
		if (printData->linear[i]) delete printData->linear[i];
		printData->linear[i] = new double[printData->sizelinear];
		}
	}

if ((printData->numtone != printData->A2B0.input_channels) || 
	(printData->sizetone != printData->A2B0.input_entries))// set up tone
	{
	printData->numtone = printData->A2B0.input_channels;
	printData->sizetone =  printData->A2B0.input_entries;
	for (i=0; i<printData->numtone; i++) 
		{
		if (printData->tone[i]) delete printData->tone[i];
		printData->tone[i] = new double[printData->sizetone];
		}
	}

// copy the patch data
if (!simupatchD) return MCO_OBJECT_NOT_INITIALIZED;
status = simupatchD->copyOutOfPatchData(&printData->srcpatch);
if (status != MCO_SUCCESS) return status;

status = simupatchDcopy->copyRawData(simupatchD);
if (status != MCO_SUCCESS) return status;

status = patchD->copyOutOfPatchData(&printData->destpatch);
if (status != MCO_SUCCESS) return status;

patchtype = patchD->type;

status = patchDcopy->copyRawData(patchD);
if (status != MCO_SUCCESS) return status;

// copy the tweak data
if (!printerData) return MCO_OBJECT_NOT_INITIALIZED;
printerData->num_tweaks = printData->numTweaks;

for (i=0; i<printData->numTweaks; i++)
	{
	printerData->tweaks[i] = new Tweak_Element(printData->tweakele[i].name);
	if (!printerData->tweaks[i]) return MCO_MEM_ALLOC_ERROR;
	printerData->tweaks[i]->Lr = printData->tweakele[i].Lr;
	printerData->tweaks[i]->Cr = printData->tweakele[i].Cr;
	printerData->tweaks[i]->type = (Tweak_Types)printData->tweakele[i].type;
	for (j=0; j<3; j++)
		{
		printerData->tweaks[i]->lab_d[j] = printData->tweakele[i].lab_d[j];
		printerData->tweaks[i]->lab_g[j] = printData->tweakele[i].lab_g[j];
		printerData->tweaks[i]->lab_p[j] = printData->tweakele[i].lab_p[j];
		}
	}
	
// copy the black curves
printerData->num_hands = printData->setting.numBlackHand;
for (i=0; i<31; i++)
	{
	printerData->x_hands[i] = printData->setting.blackX[i];
	printerData->y_hands[i] = printData->setting.blackY[i];
	}
	
		
_blacktable = (double*)McoLockHandle(_blacktableH);
for (i=0; i<101; i++) _blacktable[i] = printData->setting.blackCurve[i];
McoUnlockHandle(_blacktableH);		

	

// copy the tone and linearization curves
if (calCurves) delete calCurves;

calCurves = new CalCurves(printData);
if (!calCurves) return MCO_MEM_ALLOC_ERROR;
// copy the settings	
_condata.tabletype 			= printData->setting.tableType;			
_condata.separation_type	= printData->setting.separationType;
_condata.black_type			= printData->setting.blackType;		
_condata.total_ink_limit	= printData->setting.totalInkLimit;	
_condata.black_ink_limit	= printData->setting.blackInkLimit;	
_condata.saturation			= printData->setting.saturation;			
_condata.col_balance		= printData->setting.colorBalance;		
_condata.col_bal_cut 		= printData->setting.colorBalanceCut; 		
_condata.L_linear 			= printData->setting.linear;			
_condata.ink_linerize 		= printData->setting.inkLinearize;		
_condata.ink_neutralize 	= printData->setting.inkNeutralize; 	
_condata.ink_highlight 		= printData->setting.inkHighlight;	
_condata.simulate 			= printData->setting.simulate;			
_condata.smooth 			= printData->setting.smooth;				
_condata.rev_table_size 	= printData->setting.revTableSize;		
_condata.smooth_sim 		= printData->setting.smoothSimu;			
_condata.K_only 			= printData->setting.kOnly;	
	
return status;
}



void	ProfileDoc::_initphotolab(void)
{
	long 	i, j, k, m,n;
	double	stepL;
	double 	L, a, b;
	double  *lab;

	
	lab = (double*)McoLockHandle(_labtableH);
	stepL = 100.0/32.0;
	n = 0;
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = -16; j < 16; j++){				
			a = j*8;
			for(m = -16; m < 16; m++){
				b = m*8;
				//save to a structure
				lab[0] = L;
				lab[1] = a;
				lab[2] = b;
				lab += 3;
				n+=3;
			}
			
			lab[0] = L;
			lab[1] = a;
			lab[2] = 127.0;
			lab += 3;
			n+=3;
		}

		for(m = -16; m < 16; m++){
			b = m*8;
			//save to a structure
			lab[0] = L;
			lab[1] = 127.0;
			lab[2] = b;
			lab += 3;
			n+=3;
		}
		
		lab[0] = L;
		lab[1] = 127.0;
		lab[2] = 127.0;
		lab += 3;
		n+=3;		
	}

	McoUnlockHandle(_labtableH);
}

void	ProfileDoc::_initicclab(void)
{
	long 	i, j, k, m;
	double	stepL, stepA;
	double 	L, a, b;
	double  *lab;
	
	lab = (double*)McoLockHandle(_labtableH);
	stepL = 100.0/32.0;
	stepA = 255.0/32.0;
	
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = 0; j < 33; j++){				
			a = j*stepA - 128.0;
			for(m = 0; m < 33; m++){
				b = m*stepA - 128.0;
				//save to a structure
				lab[0] = L;
				lab[1] = a;
				lab[2] = b;
				lab += 3;
			}
		}
	}

	McoUnlockHandle(_labtableH);
}



McoStatus ProfileDoc::_initGammutC(void)
{
didGammutSurface = FALSE;
return MCO_SUCCESS;
}


McoStatus ProfileDoc::_initAll(void)
{
_initphotolab();
_initGammutC();

patchtype = No_Patches;

_smoothedPatches = FALSE;



return MCO_SUCCESS;
}





McoStatus ProfileDoc::aftercropCancel(void)
{
	McoUnlockHandle(targetBufferH);
	McoDeleteHandle(targetBufferH);	
	targetBufferH = 0L;
	return MCO_SUCCESS;
}



// Do the RGB to RGB conversion
McoStatus ProfileDoc::_handle_rgb_calibrate(void)
{

	McoStatus 	status = MCO_SUCCESS;
	short		result;
	//calib_multi	*calib_intern[BT_DIM];
	calib_inter3	*calib = 0L;
	short		end;
	double 		rv_l, rv_a, rv_b;
	long 		c,i, j, k, l, m;
	GammutComp	*gammutcomp = 0L;
	double		Params[20];
	double		*labtable;
	double		ucrgcr;
	add_black	*addblack = 0L;
	Boolean		didGS;
	double		minlab[3];
	double		l_linear;
//	double		one_d_cmyk_patch_table[] =ONE_D_CMYK_PATCH_TABLE;
	double		gammutExpand;
	RawData		*patchTempD = 0L;
	double		*patchTemp;
	McoHandle	patchrefH = 0L;
	double		*patchref;
	double		*patchrefBT;
	McoHandle 	cmyktolabBigH;
	double		*cmyktolabBig;
	double		*vals;
	double 		*temp;
	double		minGam,maxGam;
	double		*K_only_lab;
	unsigned char	*cmyk_2;
	double		*labt,*rgbt;
	double		dist;
	int32 size = (BT_DIM-BT_DIM/2);
	double		*labp,*laborig,*lab;
	
	minlab[0] = 0;
	
	laborig = new double[TABLE_ENTRIES*3];
	if (!laborig) return MCO_MEM_ALLOC_ERROR;
	labp = laborig;
	lab = (double*)McoLockHandle(_labtableH);
	
	for( i = 0; i < TABLE_ENTRIES; i++){
		*labp++ = *lab++;
		*labp++ = *lab++;
		*labp++ = *lab++;
	}
	McoUnlockHandle(_labtableH); 
	
	
	//for (i=0; i<BT_DIM; i++) calib_intern[i] = 0L;	
	
	gammutExpand = (1-_SaturationAmount/100)+1.2*(_SaturationAmount/100);


	patchrefH = McoNewHandle(patchD->format.total*3*sizeof(double));
	if (patchrefH == 0L) goto bail;

	patchref = (double*)McoLockHandle(patchrefH);

// Create the refrence data
	patchD->makeRef(patchref);	
		

	if((_condata.simulate) && (_condata.tabletype != 5) && (_condata.tabletype != 6)){
	
		for (i=0; i<_condata.smooth_sim; i++) simupatchD->smooth_patch();
		
		//apply the gamut compression to simulated ink
		//!!!Apply gamut compression

		double *lab, mingrayL, maxgrayL;		
		
		l_linear = (double)_condata.L_linear/100.0;
		
		didGS = FALSE;
		gammutcomp = new GammutComp(simupatchD,_labtableH,_labtableCopyH,_out_of_gamutH,
				_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,0L,
					_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear, 100,0,100, 0,0,0L,thermd);
		if (gammutcomp == 0L) 
			{
			status = MCO_MEM_ALLOC_ERROR;
			goto bail;
			}
		status = gammutcomp->apply_gamut_comp2(1.0);
		if (status != MCO_SUCCESS) goto bail;

		didGammutSurface = 1;
		didGammutCompression = 1;

		gammutcomp->getMinMaxL(mingrayL, maxgrayL);	
		delete gammutcomp;
		gammutcomp = 0L;  
		
		_simumaxL = maxgrayL;
		_simuminL = mingrayL;

		//find the mingrayL and maxgrayL from the compressed _labtableH
		lab = (double*)McoLockHandle(_labtableH);
		maxgrayL = mingrayL = *lab;
		for( i = 0; i < TABLE_ENTRIES; i++ ){
			if(maxgrayL < *lab)	maxgrayL = *lab;
			if(mingrayL > *lab) mingrayL = *lab;
			lab += 3;
		}
		McoUnlockHandle(_labtableH);
		
		_simumaxL = maxgrayL;
		_simuminL = mingrayL;
	
		
	}

	_patch = (double*)McoLockHandle(patchD->dataH);
	patchref = (double*)McoLockHandle(patchrefH);

	// create the internal calibration
	// first create a copy of the patch data
	patchTempD = new RawData;
	if (patchTempD == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	status = patchTempD->copyRawData(patchD);
	if (status != MCO_SUCCESS) goto bail;
	patchTemp = (double*)McoLockHandle(patchTempD->dataH);
	
	// allocate the VERY BIG table
	cmyktolabBigH = McoNewHandle(BT_DIM*BT_DIM*BT_DIM*3*sizeof(double));
	if (cmyktolabBigH == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
		
	patchrefBT = new double [BT_DIM*BT_DIM*BT_DIM*3];
	if (patchrefBT == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	
	// do the interpolation
	
	vals = new double[BT_DIM];
	for (i=0; i<BT_DIM; i++) 
		{
		vals[i] = 100*(double)i/(BT_DIM-1);
		}
	
	for (i=0; i<size; i++) 
		for (j=0; j<size; j++) 
			for (k=0; k<size; k++) 
				{
				patchrefBT[k*3+j*size*3+i*size*size*3] = 100*(double)k/(size-1);
				patchrefBT[k*3+j*size*3+i*size*size*3+1] = 100*(double)j/(size-1);
				patchrefBT[k*3+j*size*3+i*size*size*3+2] = 100*(double)i/(size-1);
				}
	
	
	
	cmyktolabBig = (double*)McoLockHandle(cmyktolabBigH);
	status = patchD->doThreeDInterp(cmyktolabBig,BT_DIM,vals);
	delete vals;
	if (status != MCO_SUCCESS) return status;
	

	
	
	// create the actuall calibration 
	calib = new calib_inter3((calib_base**)0L,0,100.0,_condata.total_ink_limit,thermd);
	if (calib == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}
	calib->set_up(_patch,patchref,20,patchD->format.total,_cmyktableH,cmyktolabBigH);



	delete patchrefBT;
	
	McoUnlockHandle(patchrefH);
	McoUnlockHandle(patchD->dataH);

	_apply_color_balance();
	

	l_linear = (double)_condata.L_linear/100.0;
	
	
	//apply the gamut compression to it
	//!!!Apply gamut compression
	didGS = didGammutSurface;
	if(_condata.simulate)
		gammutcomp = new GammutComp(patchD,_labtableH,_labtableCopyH,_out_of_gamutH,
		_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,cmyktolabBigH,
		_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear,
		_maxgrayL,_mingrayL,_simumaxL,_simuminL,minlab[0],calib,thermd);
	else
		gammutcomp = new GammutComp(patchD,_labtableH,_labtableCopyH,_out_of_gamutH,
		_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,cmyktolabBigH,
		_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear,
		_maxgrayL,_mingrayL,100,0,minlab[0],calib,thermd);
	
		
		
	if (gammutcomp == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}
	status = gammutcomp->apply_gamut_comp2(gammutExpand);
	if (status != MCO_SUCCESS) goto bail;	
	didGammutSurface = 1;
	didGammutCompression = 1;
	
	// apply the gamut compresion tweak now
	labt = (double*)McoLockHandle(_labtableH);
	status = gtweak->eval(laborig,labt,labt,33*33*33);
	McoUnlockHandle(_labtableH);

	if (!didGS) gammutcomp->getMinMaxL(_mingrayL,_maxgrayL);
	gammutcomp->getMinMaxG(minGam,maxGam);
	delete gammutcomp;
	gammutcomp = 0L; 
	
	// calibrate
	
	calib->start();
	
	labt = (double*)McoLockHandle(_labtableH);
	rgbt = (double*)McoLockHandle(_con_to_dev_tableH);
	status = calib->eval(labt,rgbt,33*33*33,&dist);
	
	calib->end();
		
	McoUnlockHandle(_labtableH);	
		
	double	temp1,temp2;	
		
	// create an inverse table and apply color balance
	if ((_condata.tabletype != 1) && (_condata.tabletype != 5) && (_condata.tabletype != 6))
		{
		status = _build_small_invers_3(cmyktolabBigH,_dev_to_con_tableH,_dev_to_con_tableH2,&temp1,&temp2);
		_apply_color_balance(patchD,100.0*(double)_condata.ink_neutralize,_dev_to_con_tableH2,
			_rev_table_grid*_rev_table_grid*_rev_table_grid);
		}
	
	if (status != MCO_SUCCESS) goto bail;	
		
	delete calib;	

	
	if (patchrefH != 0L) McoDeleteHandle(patchrefH);
	
	if (patchTempD) delete patchTempD;
	
	
	McoUnlockHandle(patchD->dataH);
	McoDeleteHandle(cmyktolabBigH);
	
	if (laborig) delete laborig;

	return 	MCO_SUCCESS;
	
bail:
	
	if (calib) delete calib;	
	if (gammutcomp) delete gammutcomp;
	//for (i=0; i<BT_DIM; i++) if (calib_intern[i]) delete calib_intern[i];	

	if (patchrefH != 0L) McoDeleteHandle(patchrefH);
	
	if (patchTempD) delete patchTempD;
	
	McoUnlockHandle(patchD->dataH);
	McoDeleteHandle(cmyktolabBigH);

	if (laborig) delete laborig;

	return status; 
}



McoStatus ProfileDoc::_handle_cmyk_calibrate(void)
{
	McoStatus 	status = MCO_SUCCESS;
	short		result;
	calib_inter4	*calib = 0L;
	short		end;
	double 		rv_l, rv_a, rv_b;
	long 		c,i, j, k, l, m;
	GammutComp	*gammutcomp = 0L;
	double		Params[20];
	double		*labtable;
	double		ucrgcr;
	add_black	*addblack = 0L;
	Boolean		didGS;
	double		minlab[3];
	double		l_linear;
//	double		one_d_cmyk_patch_table[] =ONE_D_CMYK_PATCH_TABLE;
	double		gammutExpand;
	RawData		*patchTempD = 0L;
	double		*patchTemp;
	McoHandle	patchrefH = 0L;
	double		*patchref;
	double		*patchrefBT;
	McoHandle 	cmyktolabBigH;
	double		*cmyktolabBig;
	double		*vals;
	double 		*temp;
	double		minGam,maxGam;
	double		*K_only_lab;
	unsigned char	*cmyk_2;
	int32 size = (BT_DIM-BT_DIM/2);
	BuildTableIndex	*TableIndex;
	double		*labt;
	double		*labp,*laborig,*lab;
	
	laborig = new double[TABLE_ENTRIES*3];
	if (!laborig) return MCO_MEM_ALLOC_ERROR;
	labp = laborig;
	lab = (double*)McoLockHandle(_labtableH);
	
	for( i = 0; i < TABLE_ENTRIES; i++){
		*labp++ = *lab++;
		*labp++ = *lab++;
		*labp++ = *lab++;
	}
	McoUnlockHandle(_labtableH); 

	
	gammutExpand = (1-_SaturationAmount/100)+1.2*(_SaturationAmount/100);


	patchrefH = McoNewHandle(patchD->format.total*4*sizeof(double));
	if (patchrefH == 0L) goto bail;

	patchref = (double*)McoLockHandle(patchrefH);

// Create the refrence data
	patchD->makeRef(patchref);	
		

	if((_condata.simulate) && (_condata.tabletype != 5) && (_condata.tabletype != 6)){
	
		for (i=0; i<_condata.smooth_sim; i++) simupatchD->smooth_patch();
		
		//apply the gamut compression to simulated ink
		//!!!Apply gamut compression

		double *lab, mingrayL, maxgrayL;		
		
		l_linear = (double)_condata.L_linear/100.0;
		
		didGS = FALSE;
		gammutcomp = new GammutComp(simupatchD,_labtableH,_labtableCopyH,_out_of_gamutH,
				_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,0L,_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear, 100,0,100, 0,0,0L,thermd);
		if (gammutcomp == 0L) 
			{
			status = MCO_MEM_ALLOC_ERROR;
			goto bail;
			}
		status = gammutcomp->apply_gamut_comp2(1.0);
		if (status != MCO_SUCCESS) goto bail;
		didGammutSurface = 1;
		didGammutCompression = 1;

		gammutcomp->getMinMaxL(mingrayL, maxgrayL);	
		delete gammutcomp;
		gammutcomp = 0L;  
		
		_simumaxL = maxgrayL;
		_simuminL = mingrayL;

		//find the mingrayL and maxgrayL from the compressed _labtableH
		lab = (double*)McoLockHandle(_labtableH);
		maxgrayL = mingrayL = *lab;
		for( i = 0; i < TABLE_ENTRIES; i++ ){
			if(maxgrayL < *lab)	maxgrayL = *lab;
			if(mingrayL > *lab) mingrayL = *lab;
			lab += 3;
		}
		McoUnlockHandle(_labtableH);
		
		_simumaxL = maxgrayL;
		_simuminL = mingrayL;
	
		
	}

	_patch = (double*)McoLockHandle(patchD->dataH);
	patchref = (double*)McoLockHandle(patchrefH);

	// create the internal calibration
	// first create a copy of the patch data
	patchTempD = new RawData;
	if (patchTempD == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	status = patchTempD->copyRawData(patchD);
	if (status != MCO_SUCCESS) goto bail;
	patchTemp = (double*)McoLockHandle(patchTempD->dataH);
	
	// allocate the VERY BIG table
	cmyktolabBigH = McoNewHandle(BT_DIM*BT_DIM*BT_DIM*BT_DIM*3*sizeof(double));
	if (cmyktolabBigH == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
		
	patchrefBT = new double [BT_DIM*BT_DIM*BT_DIM*3];
	if (patchrefBT == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	
	// do the interpolation
	
	vals = new double[BT_DIM];
	if (!vals) 
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	for (i=0; i<BT_DIM; i++) 
		{
		vals[i] = 100*(double)i/(BT_DIM-1);
		}
	
	for (i=0; i<size; i++) 
		for (j=0; j<size; j++) 
			for (k=0; k<size; k++) 
				{
				patchrefBT[k*3+j*size*3+i*size*size*3] = 100*(double)k/(size-1);
				patchrefBT[k*3+j*size*3+i*size*size*3+1] = 100*(double)j/(size-1);
				patchrefBT[k*3+j*size*3+i*size*size*3+2] = 100*(double)i/(size-1);
				}
	
	
	
	cmyktolabBig = (double*)McoLockHandle(cmyktolabBigH);
	SLDisable();
	status = patchD->doFourDInterp(cmyktolabBig,BT_DIM,vals,1);
	SLEnable();
	delete vals;
	if (status != MCO_SUCCESS) return status;
	
	
	// create the table index
	
	TableIndex = new BuildTableIndex(33,BT_DIM,thermd);
	status = TableIndex->SetInverseTable(cmyktolabBig,BT_DIM);
	if (status != MCO_SUCCESS) goto bail;
	status = TableIndex->Build();
	if (status != MCO_SUCCESS) goto bail;
	
	// create the actual calibration 
	calib = new calib_inter4(0L,0,100.0,_condata.total_ink_limit,TableIndex,linear_data);
	if (calib == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}
	calib->set_up(_patch,patchref,20,patchD->format.total,_cmyktableH,cmyktolabBigH);



	delete patchrefBT;
	
	McoUnlockHandle(patchrefH);
	McoUnlockHandle(patchD->dataH);

	_apply_color_balance();
	
	addblack = new add_black(this,calib); 
//	addblack = new add_black(calib,_labtableH, patchD, _con_to_dev_tableH, 
//		_blacktableH, _gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,_out_of_gamutH,_mingrayL,
//		_condata.total_ink_limit,_condata.separation_type, _dev_to_con_tableH, _rev_table_grid);

	if (addblack == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}


	addblack->findMinLMB(minlab); 

	l_linear = (double)_condata.L_linear/100.0;
	
	// Do the K only calculations if this is a link 
	
	if (((_condata.tabletype == 5) || (_condata.tabletype == 6)) && (_condata.K_only))
		{
		K_only_lab = new double[_rev_table_grid*3];
		if (!K_only_lab) 
			{
			status = MCO_MEM_ALLOC_ERROR;
			goto bail;
			}	
		
		// get the lab values from the inverse tabletable
		
		cmyk_2 = (unsigned char*)McoLockHandle(_dev_to_con_tableH2);
		
		for (i=0; i<_rev_table_grid; i++)
			{
			K_only_lab[i*3] = ((double)cmyk_2[i*3])/2.55;
			K_only_lab[i*3+1] = (double)cmyk_2[i*3+1] - 128;
			K_only_lab[i*3+2] = (double)cmyk_2[i*3+2]- 128;
			}
		
		addblack->eval_K_Only(K_only_lab,K_only_cmyk,_rev_table_grid);
		}
	

	//apply the gamut compression to it
	//!!!Apply gamut compression
	didGS = didGammutSurface;
	if(_condata.simulate)
		gammutcomp = new GammutComp(patchD,_labtableH,_labtableCopyH,_out_of_gamutH,
		_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,cmyktolabBigH,
		_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear,
		_maxgrayL,_mingrayL,_simumaxL,_simuminL,minlab[0],calib,thermd);
	else
		gammutcomp = new GammutComp(patchD,_labtableH,_labtableCopyH,_out_of_gamutH,
		_gammutSurfaceH,_gammutSurfaceBlackH,_gammutSurfaceManyBlackH,_gammutSurfaceManyBlackLH,cmyktolabBigH,
		_SaturationAmount,_PaperWhite,_col_bal_cutoff,l_linear,
		_maxgrayL,_mingrayL,100,0,minlab[0],calib,thermd);
	
		
		
	if (gammutcomp == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}
	status = gammutcomp->apply_gamut_comp2(gammutExpand);
	if (status != MCO_SUCCESS) goto bail;
	didGammutSurface = 1;
	didGammutCompression = 1;
	
	// apply the gamut compresion tweak now
	labt = (double*)McoLockHandle(_labtableH);
	status = gtweak->eval(laborig,labt,labt,33*33*33);
	McoUnlockHandle(_labtableH);
	
	if (!didGS) gammutcomp->getMinMaxL(_mingrayL,_maxgrayL);
	gammutcomp->getMinMaxG(minGam,maxGam);
	delete gammutcomp;
	gammutcomp = 0L; 
	
	// add the black and calibrate

	addblack->setMinL(_mingrayL);	
	status = addblack->evalMB(minGam,maxGam);
	if (status != MCO_SUCCESS) goto bail;

	double temp1,temp2;	
		
	// create an inverse table and apply color balance
	if ((_condata.tabletype != 1) && (_condata.tabletype != 5) && (_condata.tabletype != 6))
		{
		status = _build_small_invers(cmyktolabBigH,_dev_to_con_tableH,_dev_to_con_tableH2,&temp1,&temp2);
		_apply_color_balance(patchD,100.0*(double)_condata.ink_neutralize,_dev_to_con_tableH2,
			_rev_table_grid*_rev_table_grid*_rev_table_grid*_rev_table_grid);
		}
	
	if (status != MCO_SUCCESS) goto bail;	
	
	delete addblack;	
	delete calib;
	if (TableIndex) delete TableIndex;


	//apply the black to it
	//_apply_black();
	
	if (patchrefH != 0L) McoDeleteHandle(patchrefH);
	
	if (patchTempD) delete patchTempD;
	
	McoUnlockHandle(patchD->dataH);
	McoDeleteHandle(cmyktolabBigH);
	
	if (laborig) delete laborig;

	return 	MCO_SUCCESS;
	
bail:

	if (addblack) delete addblack;	
	if (calib) delete calib;
	if (TableIndex) delete TableIndex;	
	if (gammutcomp) delete gammutcomp;	

	if (patchrefH != 0L) McoDeleteHandle(patchrefH);
	
	if (patchTempD) delete patchTempD;
	
	if (patchD->dataH) McoUnlockHandle(patchD->dataH);
	if (cmyktolabBigH) McoDeleteHandle(cmyktolabBigH);
	
	if (laborig) delete laborig;

	return status; 
}


McoStatus ProfileDoc::_createLinktable(McoHandle linkH)
{
	McoHandle			tableH;
	unsigned char		*src, *desth, *table, *tableh, *dest;
	long				i;
	unsigned	char*	clip;

	//new con_to_dev table (data is unsigned char in stead of double)
	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	{
		return MCO_MEM_ALLOC_ERROR;
	}
	
	table = (unsigned char*)McoLockHandle(tableH);
	tableh = table;
		
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
		table[0] = clip[(short)(_table[0]*2.55+0.5)];
		table[1] = clip[(short)(_table[1]*2.55+0.5)];
		table[2] = clip[(short)(_table[2]*2.55+0.5)];
		table[3] = clip[(short)(_table[3]*2.55+0.5)];
		table += 4;
		_table += 4;		
	}


	
	
	dest = (unsigned char*)McoLockHandle(linkH);
	desth = (unsigned char*)dest;
	src = (unsigned char*)McoLockHandle(_dev_to_con_tableH2);

	table = tableh;

	//get cmyk from destination table
	linearinterp3to4(table, 33, src, _rev_table_size, dest);	
	
	
	// add in the K only data
	int32	grid_3;
	grid_3 = _rev_table_grid*_rev_table_grid*_rev_table_grid;
	if (_condata.K_only)
		{
		for (i=0; i<_rev_table_grid; i++)
			{
			dest[i*4]   = clip[(short)(K_only_cmyk[i*4]*2.55+0.5)];
			dest[i*4+1] = clip[(short)(K_only_cmyk[i*4+1]*2.55+0.5)];
			dest[i*4+2] = clip[(short)(K_only_cmyk[i*4+2]*2.55+0.5)];
			dest[i*4+3] = clip[(short)(K_only_cmyk[i*4+3]*2.55+0.5)];
			}
		}
		
	if (_condata.tabletype != 5) for(i = 0; i < TABLE_ENTRIES; i++){
		dest[i*4] = clip[(short)(linear_data->CnvtLinearInv((double)dest[i*4],0)*255+0.5)];
		dest[i*4+1] = clip[(short)(linear_data->CnvtLinearInv((double)dest[i*4+1],0)*255+0.5)];
		dest[i*4+2] = clip[(short)(linear_data->CnvtLinearInv((double)dest[i*4+2],0)*255+0.5)];
		dest[i*4+3] = clip[(short)(linear_data->CnvtLinearInv((double)dest[i*4+3],0)*255+0.5)];	
	}	
	
	//if(_condata.paper_as_white == 1){
	if (_PaperWhite == 100)
		{
		dest[0] = 0;
		dest[1] = 0;
		dest[2] = 0;
		dest[3] = 0;
		}
	else
		{
		dest[0] = table[141568];
		dest[1] = table[141569];
		dest[2] = table[141570];
		dest[3] = table[141571];
		}

/*
	// The following code is A BUG, it was put here for testing and should not be here
	// anymore !!!!!!!!!!!!!!!!!!!!
	
	int j,k,l,c=0;
	
	for (i=0; i<_rev_table_grid; i++)
		for (j=0; j<_rev_table_grid; j++)
			for (k=0; k<_rev_table_grid; k++)
				for (l=0; l<_rev_table_grid; l++)
					{
					dest[c++] = (unsigned char)(255.0*(double)i/(_rev_table_grid-1));
					dest[c++] = (unsigned char)(255.0*(double)j/(_rev_table_grid-1));
					dest[c++] = (unsigned char)(255.0*(double)k/(_rev_table_grid-1));
					dest[c++] = (unsigned char)(255.0*(double)l/(_rev_table_grid-1));
					}
					
	// end of the BUG				
*/

	McoUnlockHandle(_con_to_dev_tableH);

		
	McoUnlockHandle(tableH);	
	McoDeleteHandle(tableH);	
		
	McoUnlockHandle(linkH);
	McoUnlockHandle(_dev_to_con_tableH2);
	
	return MCO_SUCCESS;
}


// Do the ICC cmyk to CMYK link

McoStatus ProfileDoc::_create_simul_inverse(void)
{
	McoStatus 	status = MCO_SUCCESS;
	long 		c,i, j, k, m,l;
	double		l_linear;
	McoHandle 	cmyktolabBigH = 0L;
	double		*cmyktolabBig;
	int32 size = (BT_DIM-BT_DIM/2);	
	double		*vals = 0L;
	add_black	*addblack = 0L;
	int			useLuts;
	
	// allocate the VERY BIG table
	cmyktolabBigH = McoNewHandle(BT_DIM*BT_DIM*BT_DIM*BT_DIM*3*sizeof(double));
	if (cmyktolabBigH == 0L)
		{
		status = MCO_MEM_ALLOC_ERROR;
		goto bail;
		}	
	
	
		
	l_linear = (double)_condata.L_linear/100.0;	
	//simulated is the source ICC file
	
	
	for (i=0; i<_condata.smooth_sim; i++) simupatchD->smooth_patch();
	
// do the interpolation for the simulated ink
	
	vals = new double[BT_DIM];
	for (i=0; i<BT_DIM; i++) 
		{
		vals[i] = 100*(double)i/(BT_DIM-1);
		}
	
	
	cmyktolabBig = (double*)McoLockHandle(cmyktolabBigH);
//	SLDisable();
	if (_condata.tabletype == 5) useLuts = 1;
	else useLuts = 0;

	status = simupatchD->doFourDInterp(cmyktolabBig,BT_DIM,vals,useLuts);
//	SLEnable();
	delete vals;
	
	_build_small_invers(cmyktolabBigH,_dev_to_con_tableH,_dev_to_con_tableH2,&_simumaxL,&_simuminL);
	
	_apply_color_balance(simupatchD,100.0*(double)_condata.ink_neutralize,_dev_to_con_tableH2,
		_rev_table_grid*_rev_table_grid*_rev_table_grid*_rev_table_grid);


bail:
		
	if (cmyktolabBigH) McoDeleteHandle(cmyktolabBigH);
	return status;
}


McoStatus ProfileDoc::buildLinearData(void)
{
double paper[3];
double *patch;

if (linear_data) delete linear_data;
linear_data = new LinearData;
if (!linear_data) return MCO_MEM_ALLOC_ERROR;


patchD->getPaperLab(paper);

if (patchD->linearDataH != 0L) patch = (double*)McoLockHandle(patchD->linearDataH);
if (patchD->format.numLinear > 0) linear_data->setUp(patch,patchD->format.linearValues,paper,
													 patchD->format.numc,patchD->format.numLinear);
if (patchD->linearDataH != 0L) McoUnlockHandle(patchD->linearDataH);
/*
#else

linear_data->setUp(printData->setting.linearX,printData->setting.linearY,
				   MAX_LINEAR_HAND,4,printData->setting.numLinearHand);


#endif */

return MCO_SUCCESS;
}

// produce a lut for the inverse table that correct for linearization in the table

McoStatus ProfileDoc::buildInverseCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;
double max;

if (CT->input_channels != patchD->format.numc) return MCO_FAILURE;


lut = new double[CT->input_entries];
if (!lut) return MCO_MEM_ALLOC_ERROR;

for(i = 0; i <CT->input_channels; i++)
	{
	clut = (unsigned char*)CT->input_tables[i];
	for (j=0; j<CT->input_entries; j++)
		{
		lut[j] = clut[j]/2.55;
		}
	linear_data->CnvtLinear(lut,CT->input_entries,i);

	for (j=0; j<CT->input_entries; j++)
		{
		clut[j] = (unsigned char)(lut[j]*2.55+0.5);
		//clut[j] = (unsigned char)(255.0*(double)j/(CT->input_entries-1));
		}

	}
	

delete lut;
return MCO_SUCCESS;
}


// produce a lut for the inverse table that correct for linearization in the table

McoStatus ProfileDoc::buildOutputCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;

if (CT->output_channels != patchD->format.numc) return MCO_FAILURE;


lut = new double[CT->output_entries];
if (!lut) return MCO_MEM_ALLOC_ERROR;

for(i = 0; i <CT->output_channels; i++)
	{
	clut = (unsigned char*)CT->output_tables[i];
	for (j=0; j<CT->output_entries; j++)
		{
		lut[j] = clut[j]/2.55;
		}
	linear_data->CnvtLinearInv(lut,CT->output_entries,i);
	for (j=0; j<CT->output_entries; j++)
		{
		clut[j] = (unsigned char)(lut[j]*2.55+0.5);
		//clut[j] = (unsigned char)(255.0*(double)j/(CT->input_entries-1));
		}

	}
	

delete lut;
return MCO_SUCCESS;
}

// produce a lut for the inverse table that corrects for linearization in the table

McoStatus ProfileDoc::buildInputCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;
LinearData	*slinear_data;
double	paper[3];
double	*patch;

if (!simupatchD->type) return MCO_FAILURE;


simupatchD->getPaperLab(paper);

if ((simupatchD->linearDataH != 0L) && (simupatchD->format.numLinear > 0))
	{
	slinear_data = new LinearData;
	if (!slinear_data) return MCO_MEM_ALLOC_ERROR;
	
	
	patch = (double*)McoLockHandle(simupatchD->linearDataH);
	slinear_data->setUp(patch,simupatchD->format.linearValues,paper,
													 simupatchD->format.numc,simupatchD->format.numLinear);
	McoUnlockHandle(simupatchD->linearDataH);


	if (CT->input_channels != simupatchD->format.numc) return MCO_FAILURE;


	lut = new double[CT->input_entries];
	if (!lut) return MCO_MEM_ALLOC_ERROR;

	for(i = 0; i <CT->input_channels; i++)
		{
		clut = (unsigned char*)CT->input_tables[i];
		for (j=0; j<CT->input_entries; j++)
			{
			lut[j] = clut[j]/2.55;
			}
		slinear_data->CnvtLinear(lut,CT->input_entries,i);
		for (j=0; j<CT->input_entries; j++)
			{
			clut[j] = (unsigned char)(lut[j]*2.55+0.5);
			}
		}
		
	delete slinear_data;	
	delete lut;
	}

return MCO_SUCCESS;
}


McoStatus ProfileDoc::_handle_calibrate(void)
{
	McoStatus 	status;
	short		result;
	double		*patch, *patchcopy;
	long		i;


	_SaturationAmount = _condata.saturation;
	_PaperWhite = _condata.col_balance;
	_col_bal_cutoff = _condata.col_bal_cut;
	
	if(_condata.rev_table_size == 1)
		_rev_table_grid = 17;
	else
		_rev_table_grid = 9;
	if (patchD->format.numc == 4)
		_rev_table_size = _rev_table_grid*_rev_table_grid*_rev_table_grid*_rev_table_grid;
	else
		_rev_table_size = _rev_table_grid*_rev_table_grid*_rev_table_grid;

#ifdef IN_POLAROID

	changedLinearTone = 0;
	status = copyIntoPrintData();
	if (status != MCO_SUCCESS) goto bail;
	if (!calCurves) return MCO_OBJECT_NOT_INITIALIZED;
	if ((printData->icctype == NOT_MONACO_ICC) || (printData->icctype == MONACOP_CREATED_ICC)) calCurves->backupLuts();
	
 	if (!changedProfile) 
 		{
 		calCurves->buildToneCurve();
		if (printData->icctype == MONACO_CREATED_ICC) calCurves->buildLinearCurve(1);
		else calCurves->buildLinearCurve(0);
		status = saveToneAndLinear();
		if (printData->icctype == NOT_MONACO_ICC) 
			{
			printData->icctype = MONACO_EDITED_ICC;
			monacoProfile = 1;
			monacoEdited = 1;
			}
		if (printData->icctype == MONACOP_CREATED_ICC) 
			{
			printData->icctype = MONACOP_EDITED_ICC;
			monacoProfile = 1;
			monacoEdited = 1;
			}
		return status;
 		}
 	else _condata.tabletype = 5;
 	changedProfile = 0;
 	_condata.simulate = 1;

#endif


// lock down the document because processing is starting

	locked = 1;	
	

//reread the patches data again from their copy

	//return MCO_SUCCESS;
	patchD->copyRawData(patchDcopy);	

//reinit the lab table
	_initphotolab();
	
// reinint the gammut compression
	_initGammutC();
	
	//	Check EVE dongle here
	status = _checkEve();
	if (status != MCO_SUCCESS) goto bail;
	
	
	tweak->Init_Table();
	tweak->Modify_Table(printerData->num_tweaks,printerData->tweaks);
	// tweak the patches using the tweaking table	
	tweak->eval(patchD,patchD,patchD->format.total);
	
	gtweak->Init_Table();
	// the gamut compresion tweak
	gtweak->Modify_Table(printerData->num_tweaks,printerData->tweaks);
	
	for (i=0; i<_condata.smooth; i++) patchD->smooth_patch();
	
	
	// create inverse table
	SLDisable();				//HERE
	_create_CMYK_to_Lab(1);
	SLEnable();					//HERE



	// intiialize linearization data
	
	buildLinearData();
	
	switch(_condata.tabletype){
		case(1):
		
		_initphotolab();
		//save to a photoshop table
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoPhototable(0);
		//status = savetoPhototable2();
		break;
		
		case(2):
		_initicclab();
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoIcctable();
		break;
					
		case(3):
		_initicclab();
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoCrdtable();
		break;
					
		case(4):
		_initicclab();
		status = _handle_rgb_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoRGBtoRGB();
		break;

		case(5):
		_initicclab();
		K_only_cmyk = new double[_rev_table_grid*4];
		status = _create_simul_inverse();   
		if(status != MCO_SUCCESS)	goto bail;
		status = _handle_cmyk_calibrate();  
		if(status != MCO_SUCCESS)	goto bail;
	//	status = savetoPhototable(0);
	#ifdef IN_POLAROID
		buildInputCuvre(&printData->A2B0);
		calCurves->createBackupLuts();
		calCurves->buildToneCurve();
		calCurves->buildLinearCurve(1);
	#endif	
		status = savetoIccLinktable();
		break;

		case(6):
		_initicclab();
		K_only_cmyk = new double[_rev_table_grid*4];
		status = _create_simul_inverse();
		if(status != MCO_SUCCESS)	goto bail;
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoTekCmyktable();
		break;
		
		case(7):
	#ifdef IN_POLAROID
		calCurves->buildToneCurve();
		if (printData->icctype == MONACO_CREATED_ICC) calCurves->buildLinearCurve(1);
		else calCurves->buildLinearCurve(0);
	#endif	
		status = saveToneAndLinear();
		break;
		
		case(8): // icc and photoshop
		_initicclab();
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
		status = savetoPhototable(0);
		status = savetoIcctable();
		break;		
		
		default:
		status = MCO_UNSUPPORTED_MODE;
		break;
	}
	
	// reinitiialize the lab table	
	_initphotolab();
bail:
	
	// unlock the documnet	
	locked = 0;			
	return 	status;
}





// compensate for the color of paper by shifting colors towards the paper color
McoStatus ProfileDoc::_apply_color_balance()
{
int i,j,k,l;
double	*lab,*labh;
double	paperlab[3];
double  L,A,B;
double  cut,fl;


	cut = (double)(_condata.col_bal_cut)/50.0;
	cut = 10*cut*cut;

	patchD->getPaperLab(paperlab);

	A = paperlab[1];
	B = paperlab[2];
		
	A = A * _PaperWhite/100.0;
	B = B * _PaperWhite/100.0;
		
	lab = (double*)McoLockHandle(_labtableH);
	labh = lab;	

	for( i = 0; i < 33; i++){
		for( j = 0; j < 1089; j++){
				
			L = 1-(*labh++)/100.0;
			fl = exp(-cut*L);
			*labh = *labh + fl*A;
			labh++;
			*labh = *labh + fl*B;
			labh++;
			}
		}
	
	McoUnlockHandle(_labtableH);
	return MCO_SUCCESS;
}

// use this for an inverse table
// compensate for the color of paper by shifting colors away the paper color
McoStatus ProfileDoc::_apply_color_balance(RawData *pD,double paperwhite,McoHandle charData,int32 num)
{
int i,j,k,l;
unsigned char	*lab,*labh;
short A,B;
unsigned char		*clip;
double	paperLab[3];

	clip = &_clipt[5120];

	
	pD->getPaperLab(paperLab);
		
	A = paperLab[1] * paperwhite/100.0;
	B = paperLab[2] * paperwhite/100.0;
		
	lab = (unsigned char*)McoLockHandle(charData);
	labh = lab;	

	for( i = 0; i < num; i++){		
			labh++;
			*labh = clip[(short)(*labh - A)];
			labh++;
			*labh = clip[(short)(*labh - B)];
			labh++;
		}
	
	McoUnlockHandle(charData);
	return MCO_SUCCESS;
}

McoStatus ProfileDoc::_build_small_invers(McoHandle BigH,McoHandle Table1H,McoHandle Table2H,double *maxTL,double *minTL)
{
	int i,j,k,l;
	double 		tempTL;	//max and min L in table
	double      L,a,b;
	long count, mycount;
	count = 0;
	double		minDestL, maxDestL;
	double		*lighth, *light;
	McoHandle	lightnessH;
	int32		step;
	unsigned char	*cmyktolab,*cmyktolabh;
	unsigned char	*cmyktolab2,*cmyktolab2h;
	double		*cmyktolabBig;
	int32		index;
	double		lab2[3];
	double		sat_mult;
	
	*maxTL = 0.0;
	*minTL = 100.0;

	step = _rev_table_grid;

	cmyktolab = (unsigned char*)McoLockHandle(Table1H);
	cmyktolabh = cmyktolab;
	cmyktolab2 = (unsigned char*)McoLockHandle(Table2H);
	cmyktolab2h = cmyktolab2;
	
	cmyktolabBig = (double*)McoLockHandle(BigH);
	

	lightnessH = McoNewHandle(_rev_table_size*sizeof(double));
	if(!lightnessH)	return MCO_MEM_ALLOC_ERROR;
	lighth = (double*)McoLockHandle(lightnessH);
	light = lighth;

	
	for( i = 0; i < step; i++){	//C
		for( j = 0; j < step; j++){	//M
			for( k = 0; k < step; k++){	//Y
				for( l = 0; l < step; l++){	//K
				//save to cmyktolab table
					index = 3*(l*step*step*step + k*step*step + j*step + i);
					lab2[0] = cmyktolabBig[index];
					lab2[1] = cmyktolabBig[index+1];
					lab2[2] = cmyktolabBig[index+2];
					*cmyktolab++ = (unsigned char)(lab2[0]*2.55 + 0.5);
					*cmyktolab++ = (unsigned char)(lab2[1]+128.0 + 0.5);
					*cmyktolab++ = (unsigned char)(lab2[2]+128.0 + 0.5);
				
					tempTL = lab2[0];
					if(tempTL > *maxTL)
						*maxTL = tempTL;
					if(tempTL < *minTL){
						*minTL = tempTL;	
						mycount = count;
					}	
					count++;	
					
					//*minTL = lab2[0];
					*light++ = lab2[0];	
				//stretch the lightness 
					*cmyktolab2++ = (unsigned char)(lab2[0]*2.55 + 0.5);
					*cmyktolab2++ = (unsigned char)(lab2[1]+128.0 + 0.5);
					*cmyktolab2++ = (unsigned char)(lab2[2]+128.0 + 0.5);
					
						

				}
			}
		}
	}	

	minDestL = 0.0;
	maxDestL = 100.0;

	light = lighth;
//	*maxTL = light[0];	//cmyk = 0 should always be the lightest point

	if (_condata.ink_highlight) minDestL = *minTL*0.5;
	
	sat_mult = 1.0+(0.05*(double)_staurationpreview/50.0);
	
	cmyktolab2 = cmyktolab2h;
	if ((_condata.ink_linerize) || (_condata.ink_highlight)) for( i = 0; i < _rev_table_size; i++){	//C
		L = minDestL + (*light++ - *minTL)*(maxDestL - minDestL)/(*maxTL - *minTL);
		if(L < 0)	L = 0;
		if(L > 100) L = 100.0;
			
		*cmyktolab2++ = (unsigned char)(L*2.55);
		if (_condata.ink_highlight)
			{
			a = *cmyktolab2;
			a = (double)a*sat_mult; 
			*cmyktolab2++ = a;
			b = *cmyktolab2;
			b = (double)b*sat_mult; 
			*cmyktolab2++ = b;
			}
		else cmyktolab2 += 2;
					

	}				

	if (_condata.ink_linerize)
		{
		*maxTL = 100.0;
		*minTL = 0.0;
		}
		
		
	McoDeleteHandle(lightnessH);
		
	McoUnlockHandle(Table1H);
	McoUnlockHandle(Table2H);
	McoUnlockHandle(BigH);

	return MCO_SUCCESS;
}

// the rgb version of the inverse builder
McoStatus ProfileDoc::_build_small_invers_3(McoHandle BigH,McoHandle Table1H,McoHandle Table2H,double *maxTL,double *minTL)
{
	int i,j,k,l;
	double 		tempTL;	//max and min L in table
	double      L,a,b;
	long count, mycount;
	count = 0;
	double		minDestL, maxDestL;
	double		*lighth, *light;
	McoHandle	lightnessH;
	int32		step;
	unsigned char	*cmyktolab,*cmyktolabh;
	unsigned char	*cmyktolab2,*cmyktolab2h;
	double		*cmyktolabBig;
	int32		index;
	double		lab2[3];
	double		sat_mult;
	
	*maxTL = 0.0;
	*minTL = 100.0;

	step = _rev_table_grid;

	cmyktolab = (unsigned char*)McoLockHandle(Table1H);
	cmyktolabh = cmyktolab;
	cmyktolab2 = (unsigned char*)McoLockHandle(Table2H);
	cmyktolab2h = cmyktolab2;
	
	cmyktolabBig = (double*)McoLockHandle(BigH);
	

	lightnessH = McoNewHandle(_rev_table_size*sizeof(double));
	if(!lightnessH)	return MCO_MEM_ALLOC_ERROR;
	lighth = (double*)McoLockHandle(lightnessH);
	light = lighth;

	
	for( i = 0; i < step; i++){	//R
		for( j = 0; j < step; j++){	//G
			for( k = 0; k < step; k++){	//B
				//save to cmyktolab table
				index = 3*(k*step*step + j*step + i);
				lab2[0] = cmyktolabBig[index];
				lab2[1] = cmyktolabBig[index+1];
				lab2[2] = cmyktolabBig[index+2];
				*cmyktolab++ = (unsigned char)(lab2[0]*2.55 + 0.5);
				*cmyktolab++ = (unsigned char)(lab2[1]+128.0 + 0.5);
				*cmyktolab++ = (unsigned char)(lab2[2]+128.0 + 0.5);
			
				tempTL = lab2[0];
				if(tempTL > *maxTL)
					*maxTL = tempTL;
				if(tempTL < *minTL){
					*minTL = tempTL;	
					mycount = count;
				}	
				count++;	
				
				//*minTL = lab2[0];
				*light++ = lab2[0];	
			//stretch the lightness 
				*cmyktolab2++ = (unsigned char)(lab2[0]*2.55 + 0.5);
				*cmyktolab2++ = (unsigned char)(lab2[1]+128.0 + 0.5);
				*cmyktolab2++ = (unsigned char)(lab2[2]+128.0 + 0.5);

			}
		}
	}	

	minDestL = 0.0;
	maxDestL = 100.0;

	light = lighth;
//	*maxTL = light[0];	//cmyk = 0 should always be the lightest point

	if (_condata.ink_highlight) minDestL = *minTL*0.5;
	
	sat_mult = 1.0+(0.05*(double)_staurationpreview/50.0);
	
	cmyktolab2 = cmyktolab2h;
	if ((_condata.ink_linerize) || (_condata.ink_highlight)) for( i = 0; i < _rev_table_size; i++){	//C
		L = minDestL + (*light++ - *minTL)*(maxDestL - minDestL)/(*maxTL - *minTL);
		if(L < 0)	L = 0;
		if(L > 100) L = 100.0;
			
		*cmyktolab2++ = (unsigned char)(L*2.55);
		if (_condata.ink_highlight)
			{
			a = *cmyktolab2;
			a = (double)a*sat_mult; 
			*cmyktolab2++ = a;
			b = *cmyktolab2;
			b = (double)b*sat_mult; 
			*cmyktolab2++ = b;
			}
		else cmyktolab2 += 2;
					

	}				

	if (_condata.ink_linerize)
		{
		*maxTL = 100.0;
		*minTL = 0.0;
		}
		
		
	McoDeleteHandle(lightnessH);
		
	McoUnlockHandle(Table1H);
	McoUnlockHandle(Table2H);
	McoUnlockHandle(BigH);

	return MCO_SUCCESS;
}


// create a photoshop cmyk to lab table, the black is not very good
McoStatus ProfileDoc::_create_CMYK_to_Lab(int32 BlackWhiteFlag)
{
int32	i,j,n,k;
int32	dim;
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double *cmyktable;
double maxgray=0,mingray=100;

double *bL,*bK,*at,*bt,*bT;
double K;
int32  numB;
double		black_step[] = BLACK_PATCHES;
FindBlackLab *findblacklab;
double	cmyk_temp[4];
double  minKL,maxKL;
double	*y2a_L,*y2a_a,*y2a_b;
double	y_L[33],y_a[33],y_b[33];
double	*patch_L,*patch_a,*patch_b;
double	x1[33],y1[33],x23[2];
double	*C_vals,*M_vals,*Y_vals;
LinearData	*linear_data;
double	paper[3];

linear_data = new LinearData;


y2a_L = new double[patchD->format.total];
y2a_a = new double[patchD->format.total];
y2a_b = new double[patchD->format.total];

patch_L = new double[patchD->format.total];
patch_a = new double[patchD->format.total];
patch_b = new double[patchD->format.total];


cmyktable = (double*)McoLockHandle(_cmyktableH);

C_vals = new double[patchD->format.cubedim[0]];
M_vals = new double[patchD->format.cubedim[0]];
Y_vals = new double[patchD->format.cubedim[0]];

for (i=0; i<patchD->format.cubedim[0]; i++) 
	{
	C_vals[i] = patchD->getcmpValues(0,i);
	M_vals[i] = patchD->getcmpValues(0,i);
	Y_vals[i] = patchD->getcmpValues(0,i);
	}
	
patchD->getPaperLab(paper);
	
if (patchD->linearDataH) _patch = (double*)McoLockHandle(patchD->linearDataH);
if (patchD->format.numLinear > 0) linear_data->setUp(_patch,patchD->format.linearValues,paper,patchD->format.numc,patchD->format.numLinear);
if (patchD->linearDataH) McoUnlockHandle(patchD->linearDataH);

linear_data->CnvtLinear(C_vals,patchD->format.cubedim[0],0);
linear_data->CnvtLinear(M_vals,patchD->format.cubedim[0],1);
linear_data->CnvtLinear(Y_vals,patchD->format.cubedim[0],2);

_patch = (double*)McoLockHandle(patchD->dataH);

dim = patchD->format.cubedim[0];

for (i=0; i<patchD->format.total; i++)
	{
	patch_L[i]=_patch[i*3];
	patch_a[i]=_patch[i*3+1];
	patch_b[i]=_patch[i*3+2];
	}

for (i = 0; i< 33; i++)
	{
	x1[i] = 100*(1-((double)i*8)/256);  // yellow, slowest change
	}
	
//linear_data->CnvtLinear(x1,33,2);


spline3(Y_vals,C_vals,M_vals,patch_L,dim,dim,dim, y2a_L);
spline3(Y_vals,C_vals,M_vals,patch_a,dim,dim,dim, y2a_a);
spline3(Y_vals,C_vals,M_vals,patch_b,dim,dim,dim, y2a_b);

k = 0;
for (i = 0; i< 33; i++) 		// cyan
	for (j = 0; j < 33; j++)	// magenta, fastest change
		{
		x23[0] = 100*(1-((double)i*8)/256); //linear_data->CnvtLinear(100*(1-((double)i*8)/256),0);
		
		x23[1] = 100*(1-((double)j*8)/256); //linear_data->CnvtLinear(100*(1-((double)j*8)/256),1);
		
		splint3(Y_vals,C_vals,M_vals,patch_L,y2a_L,dim,dim,dim,x23,x1,33,y_L);
		splint3(Y_vals,C_vals,M_vals,patch_a,y2a_a,dim,dim,dim,x23,x1,33,y_a);
		splint3(Y_vals,C_vals,M_vals,patch_b,y2a_b,dim,dim,dim,x23,x1,33,y_b);
		//piceLin3(Y_vals,C_vals,M_vals,patch_L,y2a_L,dim,dim,dim,x23,x1,33,y_L);
		//piceLin3(Y_vals,C_vals,M_vals,patch_a,y2a_a,dim,dim,dim,x23,x1,33,y_a);
		//piceLin3(Y_vals,C_vals,M_vals,patch_b,y2a_b,dim,dim,dim,x23,x1,33,y_b);

		for (n = 0; n < 33; n++)
			{
			cmyktable[i*33*33*3+j*33*3+n*3] = y_L[n];		
			
			if (cmyktable[i*33*33*3+j*33*3+n*3] < mingray) mingray = cmyktable[i*33*33*3+j*33*3+n*3];
			if (cmyktable[i*33*33*3+j*33*3+n*3] > maxgray) maxgray = cmyktable[i*33*33*3+j*33*3+n*3];

			cmyktable[i*33*33*3+j*33*3+n*3+1] = y_a[n];
			cmyktable[i*33*33*3+j*33*3+n*3+2] = y_b[n];			
			}
		}
 k = 107811;		
// now for the black			

	bL = new double[patchD->format.numslow];
	bK = new double[patchD->format.numslow];
	at = new double[patchD->format.numslow];
	bt = new double[patchD->format.numslow];
	
	int32	st;
	
	for (i=0; i<patchD->format.numslow; i++)
		{
		
		st = patchD->getDataIndex(i);
		bL[i] = _patch[st];
		bK[i] = patchD->format.slowValues[i];
		at[i] = _patch[st+1];
		bt[i] = _patch[st+2];
		}
	
	
	numB = patchD->format.numslow;
	
	findblacklab = new FindBlackLab(bL,bK, at, bt, numB);					
			
	minKL = _patch[0];
	// find the L value for K == 100		
	findblacklab->eval(100.0,cmyk_temp);
	maxKL = cmyk_temp[0];
			
for (i=0; i<256; i++)
	{
	K = ((double)255-i)/2.550;
	findblacklab->eval(K,&cmyktable[k]);
	// cheat to make Photoshop display "better"
	//cmyktable[k] = 100*(cmyktable[k] - maxKL)/(minKL - maxKL);
	cmyktable[k] = (100-maxKL)*(cmyktable[k] - maxKL)/(minKL - maxKL)+maxKL;	
	k += 3;
	}

delete findblacklab;
delete bL;
delete bK;
delete at;
delete bt;

delete y2a_L;
delete y2a_a;
delete y2a_b;

delete patch_L;
delete patch_a;
delete patch_b;

delete C_vals;
delete M_vals;
delete Y_vals;

delete linear_data;

//saveCMYKLABAsTiff(cmyktable);

McoUnlockHandle(patchD->dataH);
McoUnlockHandle(_cmyktableH);  
return MCO_SUCCESS;	
}


// create a photoshop cmyk to lab table, the black is not very good
McoStatus ProfileDoc::_create_PS_CMYK_to_Lab(int32 BlackWhiteFlag)
{
int32	i,j,n,k;
int32	dim;
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double *cmyktable;
double maxgray=0,mingray=100;

double *bL,*bK,*at,*bt,*bT;
double K;
int32  numB;
double		black_step[] = BLACK_PATCHES;
FindBlackLab *findblacklab;
double	cmyk_temp[4];
double  minKL,maxKL;
double	*y2a_L,*y2a_a,*y2a_b;
double	y_L[33],y_a[33],y_b[33];
double	*patch_L,*patch_a,*patch_b;
double	x1[33],y1[33],x23[2];
double	*C_vals,*M_vals,*Y_vals;
LinearData	*linear_data;
double	paper[3];

linear_data = new LinearData;


y2a_L = new double[patchD->format.total];
y2a_a = new double[patchD->format.total];
y2a_b = new double[patchD->format.total];

patch_L = new double[patchD->format.total];
patch_a = new double[patchD->format.total];
patch_b = new double[patchD->format.total];


cmyktable = (double*)McoLockHandle(_cmyktableH);

C_vals = new double[patchD->format.cubedim[0]];
M_vals = new double[patchD->format.cubedim[0]];
Y_vals = new double[patchD->format.cubedim[0]];

for (i=0; i<patchD->format.cubedim[0]; i++) 
	{
	C_vals[i] = patchD->getcmpValues(0,i);
	M_vals[i] = patchD->getcmpValues(0,i);
	Y_vals[i] = patchD->getcmpValues(0,i);
	}
	
patchD->getPaperLab(paper);
	
if (patchD->linearDataH) _patch = (double*)McoLockHandle(patchD->linearDataH);
if (patchD->format.numLinear > 0) linear_data->setUp(_patch,patchD->format.linearValues,paper,patchD->format.numc,patchD->format.numLinear);
if (patchD->linearDataH) McoUnlockHandle(patchD->linearDataH);

linear_data->CnvtLinear(C_vals,patchD->format.cubedim[0],0);
linear_data->CnvtLinear(M_vals,patchD->format.cubedim[0],1);
linear_data->CnvtLinear(Y_vals,patchD->format.cubedim[0],2);

_patch = (double*)McoLockHandle(patchD->dataH);

dim = patchD->format.cubedim[0];

for (i=0; i<patchD->format.total; i++)
	{
	patch_L[i]=_patch[i*3];
	patch_a[i]=_patch[i*3+1];
	patch_b[i]=_patch[i*3+2];
	}

for (i = 0; i< 33; i++)
	{
	x1[i] = 100*(1-((double)i*8)/256);  // yellow, slowest change
	}
	
linear_data->CnvtLinear(x1,33,2);


spline3(Y_vals,C_vals,M_vals,patch_L,dim,dim,dim, y2a_L);
spline3(Y_vals,C_vals,M_vals,patch_a,dim,dim,dim, y2a_a);
spline3(Y_vals,C_vals,M_vals,patch_b,dim,dim,dim, y2a_b);

k = 0;
for (i = 0; i< 33; i++) 		// cyan
	for (j = 0; j < 33; j++)	// magenta, fastest change
		{
		x23[0] = linear_data->CnvtLinear(100*(1-((double)i*8)/256),0);
		
		x23[1] = linear_data->CnvtLinear(100*(1-((double)j*8)/256),1);
		
		splint3(Y_vals,C_vals,M_vals,patch_L,y2a_L,dim,dim,dim,x23,x1,33,y_L);
		splint3(Y_vals,C_vals,M_vals,patch_a,y2a_a,dim,dim,dim,x23,x1,33,y_a);
		splint3(Y_vals,C_vals,M_vals,patch_b,y2a_b,dim,dim,dim,x23,x1,33,y_b);
		//piceLin3(Y_vals,C_vals,M_vals,patch_L,y2a_L,dim,dim,dim,x23,x1,33,y_L);
		//piceLin3(Y_vals,C_vals,M_vals,patch_a,y2a_a,dim,dim,dim,x23,x1,33,y_a);
		//piceLin3(Y_vals,C_vals,M_vals,patch_b,y2a_b,dim,dim,dim,x23,x1,33,y_b);

		for (n = 0; n < 33; n++)
			{
			cmyktable[i*33*33*3+j*33*3+n*3] = y_L[n];		
			
			if (cmyktable[i*33*33*3+j*33*3+n*3] < mingray) mingray = cmyktable[i*33*33*3+j*33*3+n*3];
			if (cmyktable[i*33*33*3+j*33*3+n*3] > maxgray) maxgray = cmyktable[i*33*33*3+j*33*3+n*3];

			cmyktable[i*33*33*3+j*33*3+n*3+1] = y_a[n];
			cmyktable[i*33*33*3+j*33*3+n*3+2] = y_b[n];			
			}
		}
 k = 107811;		
// now for the black			

	bL = new double[patchD->format.numslow];
	bK = new double[patchD->format.numslow];
	at = new double[patchD->format.numslow];
	bt = new double[patchD->format.numslow];
	
	int32	st;
	
	for (i=0; i<patchD->format.numslow; i++)
		{
		
		st = patchD->getDataIndex(i);
		bL[i] = _patch[st];
		bK[i] = patchD->format.slowValues[i];
		at[i] = _patch[st+1];
		bt[i] = _patch[st+2];
		}
	
	
	numB = patchD->format.numslow;
	
	findblacklab = new FindBlackLab(bL,bK, at, bt, numB);					
			
	minKL = _patch[0];
	// find the L value for K == 100		
	findblacklab->eval(100.0,cmyk_temp);
	maxKL = cmyk_temp[0];
			
for (i=0; i<256; i++)
	{
	K = ((double)255-i)/2.550;
	findblacklab->eval(K,&cmyktable[k]);
	// cheat to make Photoshop display "better"
	//cmyktable[k] = 100*(cmyktable[k] - maxKL)/(minKL - maxKL);
	cmyktable[k] = (100-maxKL)*(cmyktable[k] - maxKL)/(minKL - maxKL)+maxKL;	
	k += 3;
	}

delete findblacklab;
delete bL;
delete bK;
delete at;
delete bt;

delete y2a_L;
delete y2a_a;
delete y2a_b;

delete patch_L;
delete patch_a;
delete patch_b;

delete C_vals;
delete M_vals;
delete Y_vals;

delete linear_data;

//saveCMYKLABAsTiff(cmyktable);

McoUnlockHandle(patchD->dataH);
McoUnlockHandle(_cmyktableH);  
return MCO_SUCCESS;	
}



// Normalize the L values so Photoshop displays correctly
McoStatus ProfileDoc::_normalizeInvTable(int32 BlackWhiteFlag)
{
int32	i,j,n,k;
double *cmyktable;
double maxL=0,minL=100;

cmyktable = (double*)McoLockHandle(_cmyktableH);

if (!BlackWhiteFlag) return MCO_SUCCESS;

k = 0;
for (i = 0; i< 33; i++) 
	for (j = 0; j < 33; j++)
		for (n = 0; n < 33; n++)
			{
			if (maxL < cmyktable[k]) maxL = cmyktable[k];
			if (minL > cmyktable[k]) minL = cmyktable[k];
			k += 3;
			}	

k = 0;
for (i = 0; i< 33; i++) 
	for (j = 0; j < 33; j++)
		for (n = 0; n < 33; n++)
			{
			cmyktable[k] = (100-minL*0.75)*(cmyktable[k] - minL)/(maxL-minL)+minL*0.75;
			if (cmyktable[k] < 0) cmyktable[k] = 0;
			if (cmyktable[k] > 100) cmyktable[k] = 100;
			k += 3;
			}		
				
McoUnlockHandle(_cmyktableH);
return MCO_SUCCESS;	
}


// load the linearization data from the raw data object int the printer settings
// do not load if no data is present or if linearization data has aready been loaded

McoStatus ProfileDoc::loadLinearData(void)
{
#ifdef IN_POLAROID

int i,j,s;
double	*data;
int		LAB = 0;
double	paper[3];
double	t,t2,t3;
double  max[6];

if (!patchD) return MCO_OBJECT_NOT_INITIALIZED;
if (!printData) return MCO_OBJECT_NOT_INITIALIZED;

// see if different linearization data is present (either loaded or measured)
if (printData->setting.flag & DIFFERENT_LINEAR) return MCO_SUCCESS;

// see if patch data has linearization data
if (patchD->format.numLinear == 0) return MCO_SUCCESS;

data = (double*)McoLockHandle(patchD->linearDataH);
if (!data) return MCO_FAILURE;

s = patchD->format.numLinear;

changedLinearTone = 1;

//  see if data is in lab or density format
for (i=0; i<patchD->format.numc; i++)
	{
	for (j=0; j<patchD->format.numLinear-1; j++)
		{
		if (fabs(data[i*(s-1)*3+j*3+1]) > 0.01) LAB = 1;
		if (fabs(data[i*(s-1)*3+j*3+2]) > 0.01) LAB = 1;
		}
	}

if (LAB) patchD->getPaperLab(paper);

// now load the data
for (i=0; i<patchD->format.numc; i++)
	{
	printData->setting.numLinearHand[i] = patchD->format.numLinear;
	printData->setting.linearX[0] = 0;
	printData->setting.linearY[0] = 0;
	max[i] = 2.0;	
	for (j=0; j<s; j++)
		{
		if (j == 0) printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 0.0;
		else if (LAB)
			{
			t = data[i*(s-1)*3+(j-1)*3]-paper[0];	
			t2 = t*t;
			t = data[i*(s-1)*3+(j-1)*3+1]-paper[1];	
			t2 += t*t;
			t = data[i*(s-1)*3+(j-1)*3+2]-paper[2];	
			t2 += t*t;
			t3 = sqrt(t2)/65.0;
			if (t3 > max[i]) max[i] = t3;
			printData->setting.linearY[i*MAX_LINEAR_HAND+j] = t3;
			}
		else printData->setting.linearY[i*MAX_LINEAR_HAND+j] = data[i*(s-1)*3+(j-1)*3];
		printData->setting.linearX[i*MAX_LINEAR_HAND+j] = patchD->format.linearValues[j];
		}
	for (j=0; j<s; j++)
		{
		printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 2.0*printData->setting.linearY[i*MAX_LINEAR_HAND+j]/max[i];
		}		
	}
return MCO_SUCCESS;
#else
return MCO_SUCCESS;
#endif

}


// determine if valid data is present
// source, destination, and linear must be present
Boolean ProfileDoc::dataPresent(void)
{
#ifndef IN_POLAROID
//if (patchtype != No_Patches) return TRUE;
if ((!patchD)) return FALSE;
if ((patchD->type)) return TRUE;
return FALSE;
#else
if ((!patchD) || (!simupatchD)) return FALSE;
if (!printData) return FALSE;
if (printData->setting.numLinearHand[3] == 0) return FALSE;
if (printData->setting.linearX[3*MAX_LINEAR_HAND+printData->setting.numLinearHand[3]-1] != 100) return FALSE;
if ((patchD->type) && (simupatchD->type)) return TRUE;
return FALSE;
#endif
}

// return true if the current calibration is valid
int ProfileDoc::validCal(void)
{
int i;

if (!printData) return 0;

for (i=0; i<4; i++) 
	{
	if (!printData->setting.numLinearHand[i]) return 0;
	if (printData->setting.linearX[(printData->setting.numLinearHand[i]-1)+i*MAX_LINEAR_HAND] != 100) return 0;
	}
return 1;
}
