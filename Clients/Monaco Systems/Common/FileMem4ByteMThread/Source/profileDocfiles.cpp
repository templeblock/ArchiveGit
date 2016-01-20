////////////////////////////////////////////////////////////////////////////////
//	profileDocFiles.c														  //
//																			  //
//	The code for dealing with the profile data, replaces much of printdialog.c//
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "profiledocfiles.h"
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

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))


ProfileDocFiles::ProfileDocFiles(void):ProfileDoc()
{	

	
	printerData = 0L;
	printData = 0L;
	calCurves = 0L;

	
	return;
	
bail:
	validData = FALSE;
	return;	
}



ProfileDocFiles::~ProfileDocFiles(void)
{

	
	if (printerData) delete printerData;
	
	if (calCurves) delete calCurves;
		
#ifdef IN_POLAROID
	
	
	
	if (printData) 
		{
		Mpfileio mp(printData);
		mp.cleanup(2);
		delete printData;
		}
	
#endif		

}





McoStatus ProfileDocFiles::initPrintData(void)
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
void ProfileDocFiles::clearToneLinear(void)
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

McoStatus ProfileDocFiles::copyIntoPrintData(void)
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
	
if (_condata.num_bits_icc == 16)
	printData->setting.flag = printData->setting.flag | ICC_16_BITS;
else
	printData->setting.flag = printData->setting.flag & (0xFFFFFFFF ^ ICC_16_BITS);
	
return status;
	
}


McoStatus	ProfileDocFiles::copyOutOfPrintData(void)
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
	
if (printData->setting.flag & ICC_16_BITS)	
	_condata.num_bits_icc = 16;
else
	_condata.num_bits_icc = 8;
	
return status;
}














// produce a lut for the inverse table that correct for linearization in the table

McoStatus ProfileDocFiles::buildInverseCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;
unsigned short	*slut;
double max;

if (CT->input_channels != patchD->format.numc) return MCO_FAILURE;


lut = new double[CT->input_entries];
if (!lut) return MCO_MEM_ALLOC_ERROR;

if (numbits_icc == 8)
	{
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
			if (numbits_icc == 8)
				clut[j] = (unsigned char)(lut[j]*2.55+0.5);
			else
				clut[j] = (unsigned short)(lut[j]*655.35+0.5);
			//clut[j] = (unsigned char)(255.0*(double)j/(CT->input_entries-1));
			}

		}
	}
else
	{
	for(i = 0; i <CT->input_channels; i++)
		{
		slut = (unsigned short*)CT->input_tables[i];
		for (j=0; j<CT->input_entries; j++)
			{
			lut[j] = slut[j]/655.35;
			}
		linear_data->CnvtLinear(lut,CT->input_entries,i);

		for (j=0; j<CT->input_entries; j++)
			{
			slut[j] = (unsigned short)(lut[j]*655.35+0.5);
			}

		}
	}

delete lut;
return MCO_SUCCESS;
}


// produce a lut for the inverse table that correct for linearization in the table

McoStatus ProfileDocFiles::buildOutputCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;
unsigned short	*slut;

if (CT->output_channels != patchD->format.numc) return MCO_FAILURE;


lut = new double[CT->output_entries];
if (!lut) return MCO_MEM_ALLOC_ERROR;

if (numbits_icc == 8)
	{
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
			}

		}
	}
else
	{
	for(i = 0; i <CT->output_channels; i++)
		{
		slut = (unsigned short*)CT->output_tables[i];
		for (j=0; j<CT->output_entries; j++)
			{
			lut[j] = slut[j]/655.35;
			}
		linear_data->CnvtLinearInv(lut,CT->output_entries,i);
		for (j=0; j<CT->output_entries; j++)
			{
			slut[j] = (unsigned short)(lut[j]*655.35+0.5);
			}

		}
	}

delete lut;
return MCO_SUCCESS;
} 


McoStatus ProfileDocFiles::buildPrelutCurve(Ctype *CT)
{
/*
double	*lut;
int		i,j;
unsigned char	*clut;
unsigned short	*slut;
long temp;

if (CT->output_channels != patchD->format.numc) return MCO_FAILURE;


lut = new double[CT->output_entries];
if (!lut) return MCO_MEM_ALLOC_ERROR;

if (numbits_icc == 8)
	{
	for(i = 1; i <CT->input_channels; i++)
		{
		clut = (unsigned char*)CT->input_tables[i];
		for (j=0; j<CT->input_entries; j++)
			{
			lut[j] = clut[j];
			_buildLabTableLut(lut[j]-128,&lut[j]);
			}
		
		for (j=0; j<CT->input_entries; j++)
			{
			clut[j] = (unsigned char)(lut[j]+0.5+128);
			}

		}
	}
else
	{
	for(i = 1; i <CT->input_channels; i++)
		{
		slut = (unsigned short*)CT->input_tables[i];
		for (j=0; j<CT->input_entries; j++)
			{
			lut[j] = slut[j]/256;
			_buildLabTableLut(lut[j]-128,&lut[j]);
			}
		
		for (j=0; j<CT->input_entries; j++)
			{
			temp = (unsigned short)((lut[j]+128)*256+0.5);
			temp = MaxVal(0,temp);
			temp = MinVal(65535,temp);
			slut[j] = temp;
			}

		}
	}

delete lut; */
return MCO_SUCCESS;
}

// produce a lut for the inverse table that corrects for linearization in the table

McoStatus ProfileDocFiles::buildInputCuvre(Ctype *CT)
{
double	*lut;
int		i,j;
unsigned char	*clut;
unsigned short	*slut;
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

	if (numbits_icc == 8)
		{
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
		}
	else
		{
		for(i = 0; i <CT->input_channels; i++)
			{
			slut = (unsigned short*)CT->input_tables[i];
			for (j=0; j<CT->input_entries; j++)
				{
				lut[j] = slut[j]/655.35;
				}
			slinear_data->CnvtLinear(lut,CT->input_entries,i);
			for (j=0; j<CT->input_entries; j++)
				{
				slut[j] = (unsigned short)(lut[j]*655.35+0.5);
				}
			}
		}	
		
	delete slinear_data;	
	delete lut;
	}

return MCO_SUCCESS;
}


McoStatus ProfileDocFiles::_handle_calibrate(void)
{
	McoStatus 	status;
	short		result;
	double		*patch, *patchcopy;
	long		i;
	long		last_changedLinearTone;
	long 		last_changedProfile;


	_SaturationAmount = _condata.saturation;
	_PaperWhite = _condata.col_balance;
	_col_bal_cutoff = _condata.col_bal_cut;
	numbits_icc = _condata.num_bits_icc;
	
	if(_condata.rev_table_size == 1)
		_rev_table_grid = 17;
	else
		_rev_table_grid = 9;
	if (patchD->format.numc == 4)
		_rev_table_size = _rev_table_grid*_rev_table_grid*_rev_table_grid*_rev_table_grid;
	else
		_rev_table_size = _rev_table_grid*_rev_table_grid*_rev_table_grid;

#ifdef IN_POLAROID

	last_changedLinearTone = changedLinearTone;
	last_changedProfile = changedProfile;
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
	//added by Peter
	if(printerData)
	tweak->Modify_Table(printerData->num_tweaks,printerData->tweaks);
	// tweak the patches using the tweaking table	
	tweak->eval(patchD,patchD,patchD->format.total);
	
	gtweak->Init_Table();
	// the gamut compresion tweak
	//added by Peter
	if(printerData)
	gtweak->Modify_Table(printerData->num_tweaks,printerData->tweaks);
	
	if (_condata.smooth == 4) patchD->median_patch();
	else for (i=0; i<_condata.smooth; i++) patchD->smooth_patch();
	
	
	// create inverse table
	SLDisable();				//HERE
	_create_CMYK_to_Lab(1);
	SLEnable();					//HERE



	// intiialize linearization data
	
	buildLinearData();
	
	switch(_condata.tabletype){
		case(1):
		numbits_icc = 8;
		_initicclab();
		//_initphotolab();
		//save to a photoshop table
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
#ifdef IN_MAC_OS_CODE
		status = savetoPhototable(0);
#endif
		//status = savetoPhototable2();
		break;
		
		case(2):
		_initicclab();
		_applyLabTableCurve();
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
#ifdef IN_MAC_OS_CODE
		status = savetoIcctable();
#endif
		break;
					
		case(3):
		numbits_icc = 8;
		_initicclab();
		status = _handle_cmyk_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
#ifdef IN_MAC_OS_CODE
		status = savetoCrdtable();
#endif
		break;
					
		case(4):
		_initicclab();
		_applyLabTableCurve();
		status = _handle_rgb_calibrate();
		if(status != MCO_SUCCESS)	goto bail;
#ifdef IN_MAC_OS_CODE
		status = savetoRGBtoRGB();
#endif
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
#ifdef IN_MAC_OS_CODE		
		status = savetoIccLinktable();
#endif
		break;

		case(6):
		numbits_icc = 8;
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
	
#ifdef IN_POLAROID
	if (status != MCO_SUCCESS)
		{
		last_changedLinearTone = changedLinearTone;
		last_changedProfile = changedProfile;
		}
	else saveSettings();
#endif	
	// unlock the documnet	
	locked = 0;	
	if (status == MCO_CANCEL) status = MCO_SUCCESS;		
	return 	status;
}









// determine if valid data is present
// source, destination, and linear must be present
Boolean ProfileDocFiles::dataPresent(void)
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
int ProfileDocFiles::validCal(void)
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


// load the linearization data from the raw data object int the printer settings
// do not load if no data is present or if linearization data has aready been loaded

McoStatus ProfileDocFiles::loadLinearData(void)
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


