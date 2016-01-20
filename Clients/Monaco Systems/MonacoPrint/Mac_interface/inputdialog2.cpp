////////////////////////////////////////////////////////////////////////////////////////////////
//	The new input dialog																	  //
//  inputdialog2.c																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////


#include <math.h>
#include "inputdialog2.h"
#include "monacoprint.h"
#include "think_bugs.h"
#include "fullname.h"
#include "mcofile.h"
#include "pascal_string.h"
#include "colortran.h"
//#include "xritewarndialog.h"
#include "xritecom.h"
//#include "globalValues.h"
#include "recipes.h"
#include "scram_array.h"
#include "errordial.h"
#include "basedialog.h"

#define RGB_PATCHES_8_11 "\pMcoPatchesRGB8-11"
#define RGB_PATCHES_11_17 "\pMcoPatchesRGB11-17"
#define RGB_PATCHES_RGB "\pMcoPatchesRGB8-11-RGB"

#define OK	1
#define Cancel 3

#define Input_Dialog		(2006)
#define Ok_Box				(2)
#define Save_as				(4)
#define Strip_Rect			(5)
#define Message				(8)
#define Cur_sheet			(6)
#define Cur_strip			(7)
#define Patch_Rect			(10)
#define New_Patch_Rect		(15)
#define	Slider				(16)
#define Redo				(17)
#define ReadNext			(18)
#define VoiceSet			(19)
#define	UpdateEvent			(400)
#define	TimeEvent			(401)
#define Message_List_ID		(1000)

#define VoiceNums {101,102,-1,-1,-1,-1}

int32	_last_tick_time = 0;


InputDialog2::InputDialog2(ProfileDoc *dc,XyztoRgb *xtr,double g)
{
	short		iType;
	Handle		iHandle;
	Rect 		iRect, r,pRect,tRect;	
	UserItemUPP  box3D;
	
	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	RGBColor	c,oldbackcolor,oldforecolor;
	WindowPtr	oldP;
	OSErr			err;
	
	doc = dc;
	xyztorgb = xtr;
	gamma = g;
	
	Scramble = !(isPressed(0x3a));
	//Scramble = FALSE;
	

	
	_rgbpatchH = NULL;
	_labpatchrefH = NULL;

	WinType = StripDialog;
	WinNum = 0;

	setDialog(Input_Dialog);
	frame_button(Ok_Box);	

	chan = nil;
	if (Scramble)
		{
		err = SndNewChannel (&chan, 0, 0, nil);
		if (err != noErr) chan = nil;
		voice_set = 1;
		}	
	else
		{
		Disable(dp,VoiceSet);
		voice_set = 2;
		}
	//playSound(8193);
	

	_pagesize = doc->whichsize;
	
	// read in the RGB values of the patches

	switch(_pagesize){
		case (1): //US letter
			patchtype = CMYKPatches;
			patch_per_strip = PATCH_PER_STRIP_LETTER;
			strip_per_sheet = STRIPS_PER_SHEET;
			data_patches = NUM_TOTAL_PATCHES;
			if (NUM_TOTAL_PATCHES % (patch_per_strip-1) == 0)
				total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1)))*patch_per_strip;
			else total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1))+1)*patch_per_strip;
			break;
		case (3): //RGB US Letter
			patchtype = RGBPatches;
			patch_per_strip = PATCH_PER_STRIP_LETTER;
			strip_per_sheet = STRIPS_PER_SHEET;
			data_patches = NUM_TOTAL_RGB_PATCHES;
			if (NUM_TOTAL_RGB_PATCHES % (patch_per_strip-1) == 0)
				total_patches = ((NUM_TOTAL_RGB_PATCHES/(patch_per_strip-1)))*patch_per_strip;
			else total_patches = ((NUM_TOTAL_RGB_PATCHES/(patch_per_strip-1))+1)*patch_per_strip;
			break;
			
		case (2): //Tabloid
			patchtype = CMYKPatches;
			patch_per_strip = PATCH_PER_STRIP_TABLOID;
			strip_per_sheet = STRIPS_PER_SHEET_TAB;
			data_patches = NUM_TOTAL_PATCHES;
			if 	(NUM_TOTAL_PATCHES % (patch_per_strip-1) == 0)
				total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1)))*patch_per_strip;
			else total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1))+1)*patch_per_strip;
			break;
			
		default:
			patchtype = CMYKPatches;
			patch_per_strip = PATCH_PER_STRIP_LETTER;
			strip_per_sheet = STRIPS_PER_SHEET;
			data_patches = NUM_TOTAL_PATCHES;	
			if (NUM_TOTAL_PATCHES % (patch_per_strip-1) == 0)
				total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1)))*patch_per_strip;
			else total_patches = ((NUM_TOTAL_PATCHES/(patch_per_strip-1))+1)*patch_per_strip;
			break;
	}
	
	//patch_data = (double*)McoLockHandle(patchH);

	
	num_used_patches  = NUM_TOTAL_PATCHES;
	
	top_patch = 0;
						
//init the static members
	current_patch = 0;
	current_strip = 0;
	current_sheet = 0;
 		
 	_done = 0;
 	
	
	init();	
	SetSheetStripText();
	getpatchRGB();
	
	// Added by James, 3D the box
	threeD_box(Strip_Rect);
	
	// initialize comminications with the XRite
	xcom = NULL;
	xcom = new XriteCom(doc->whichport);
	if (xcom != NULL) if (xcom->state != MCO_SUCCESS)
		{
		McoErrorAlert(xcom->state);
		delete xcom;
		xcom = NULL;
		}
	if (xcom != NULL) 
		{
		xcom->InitializeXRite(_pagesize);
		_waiting = 1;
		}
	else _waiting = 0;
	
//	GetPort(&oldP);
//	SetPort(dp);
	
	// initialize the big gworld 
	GetDItem (dp, (short)(Strip_Rect), &iType, (Handle*)&iHandle, &iRect);
	tRect = iRect;
	OffsetRect(&tRect,-iRect.left,-iRect.top);
	error = NewGWorld( &BigGW, 32, &tRect, 0, 0, 0 );
	if (error != 0) 
		{
		delete this;
		return;
		} 
	GetGWorld(&oldGW,&oldGD);
	SetGWorld(BigGW,nil);	
	
	
//	GetBackColor(&oldbackcolor);
	GetForeColor(&oldforecolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);		
	EraseRect( &tRect );
	
	SetGWorld(oldGW,oldGD);	
//	RGBBackColor(&oldbackcolor);
	RGBForeColor(&oldforecolor);	

//	SetPort(oldP);
		
	GetDItem (dp, (short)(Patch_Rect), &iType, (Handle*)&iHandle, &pRect);
	num_display_patch = (iRect.bottom-iRect.top)/(pRect.bottom-pRect.top);
	top_patch = 0;
	
	Disable(dp,Redo);
	Disable(dp,ReadNext);
	//Disable(dp,Save_as);	
	checkFinished();
	
	GetDItem (dp, (short)(Slider), &iType, (Handle*)&iHandle, &pRect);
	SetCtlMin((ControlHandle)iHandle,0);
	SetCtlMax((ControlHandle)iHandle,total_patches-1);
	
	GetDItem(dp, VoiceSet, &iType, (Handle*)&iHandle, &pRect);
	SetCtlValue((ControlHandle)iHandle,1);
	//voice_set = 1;
	
	DrawWindow();
}	

InputDialog2::~InputDialog2(void)
{
OSErr			err;

	DisposeGWorld( BigGW );
	if (xcom != NULL) delete xcom;
	if (_rgbpatchH != NULL) McoDeleteHandle(_rgbpatchH);
	if (_labpatchrefH != NULL) McoDeleteHandle(_labpatchrefH);
	
	if (chan != nil) err = SndDisposeChannel (chan,TRUE);
}




McoStatus InputDialog2::StorePatches(void)
{
int i,c = 0;
double *p,*pc;

	// copy patch data  into _patchH
	p = (double *)McoLockHandle(doc->patchD->dataH);
	
	for (i=0; i<MAX_NUM_PATCHES; i++)
		{
		if ((i % patch_per_strip != 0) && (c < num_used_patches))
			{
			p[c*3] = patch_data[i*3];
			p[c*3+1] = patch_data[i*3+1];
			p[c*3+2] = patch_data[i*3+2];
			c++;
			}
		}
	if (Scramble) unscramble_array(p,data_patches,3);	
	McoUnlockHandle(doc->patchD->dataH);
	
	doc->patchDcopy->copyRawData(doc->patchD);	
		
	doc->didGammutSurface = FALSE;
	doc->_smoothedPatches = FALSE;	


	return MCO_SUCCESS;
}


McoStatus InputDialog2::DrawSinglePatch(int32 patch_num)
{
	char	st[255];
	double	new_rgb[3];
	RGBColor	c,oldbackcolor,oldforecolor;
	short	iType;
	Handle	iHandle;
	Rect 	iRect;
	Rect 	pRect, nRect,tRect,tRect2;
	double *rgb_data;
	WindowPtr	oldP;

	rgb_data = (double *)McoLockHandle(_rgbpatchH);

	GetDItem (dp, (short)(Strip_Rect), &iType, (Handle*)&iHandle, &iRect);
	GetDItem (dp, (short)(Patch_Rect), &iType, (Handle*)&iHandle, &pRect);
	GetDItem (dp, (short)(New_Patch_Rect), &iType, (Handle*)&iHandle, &nRect);
	
//	GetPort(&oldP);
//	SetPort(dp);	
	
	GetBackColor(&oldbackcolor);
	GetForeColor(&oldforecolor);

	c.red = ((int)rgb_data[3*patch_num])<<8;
	c.green = ((int)rgb_data[3*patch_num+1])<<8;
	c.blue = ((int)rgb_data[3*patch_num+2])<<8;

	RGBForeColor(&c);
	
	tRect2 = iRect;
	OffsetRect(&tRect2,-iRect.left,-iRect.top);
	tRect2.bottom = tRect2.bottom/num_display_patch;
	
	OffsetRect(&pRect,-iRect.left,-iRect.top);
	OffsetRect(&nRect,-iRect.left,-iRect.top);
	
	FillRect(&pRect,&qd.black);
	
	// draw the text

	if (patch_data[patch_num*3] > -1)
		{	
		//labtontscrgb(&patch_data[patch_num*3], new_rgb);
		ConvertToRGB(&patch_data[patch_num*3], new_rgb,1);
		c.red = ((int)new_rgb[0])<<8;
		c.green = ((int)new_rgb[1])<<8;
		c.blue = ((int)new_rgb[2])<<8;

		RGBForeColor(&c);
		
		FillRect(&nRect,&qd.black);
		
		c.red = 0;
		c.green = 0;
		c.blue = 0;
	
		TextSize(9);
		RGBForeColor(&c);
		MoveTo(nRect.right + 2,nRect.top+10);
		sprintf(st,"%5.1lf",patch_data[patch_num*3]);
		
		DrawString(ctopstr(st));
		//DrawString("\p  ");
		MoveTo(nRect.right + 42,nRect.top+10);
		sprintf(st,"%5.1lf",patch_data[patch_num*3+1]);
		DrawString(ctopstr(st));
		//DrawString("\p  ");
		MoveTo(nRect.right + 86,nRect.top+10);
		sprintf(st,"%5.1lf",patch_data[patch_num*3+2]);
		DrawString(ctopstr(st));

		} 
	if (current_patch == patch_num)
		{
		c.red = 0;
		c.green = 0;
		c.blue = 0;

		RGBForeColor(&c);
		PenNormal ();
		PenSize (3, 3);
		FrameRect (&tRect2);
		PenNormal ();
		}
RGBBackColor(&oldbackcolor);
RGBForeColor(&oldforecolor);
McoUnlockHandle(_rgbpatchH);
//SetPort(oldP);
return MCO_SUCCESS;
}

McoStatus InputDialog2::DisplayPatches(int32 top_patch,int32 bottom_patch)
{
	int i;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	GWorldPtr	SmallGW;
	short	iType;
	Handle	iHandle;
	Rect 	iRect;
	Rect 	pRect, nRect,tRect,tRect2,tRect3;
	QDErr		error;
	PixMapHandle	smallPixMap,bigPixMap;
	WindowPtr	oldGraf;
	RGBColor	c,oldbackcolor,oldforecolor;
	
	GetPort(&oldGraf);
	SetPort(dp);
		
	GetBackColor(&oldbackcolor);
	GetForeColor(&oldforecolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);
	
	GetDItem (dp, (short)(Strip_Rect), &iType, (Handle*)&iHandle, &iRect);

	tRect = iRect;
	OffsetRect(&tRect,-tRect.left,-tRect.top);
	tRect3 = tRect;
	tRect.bottom = tRect.bottom/num_display_patch;
	
	error = NewGWorld( &SmallGW, 32, &tRect, 0, 0, 0 );
	if (error == 0)
		{
		GetGWorld(&oldGW,&oldGD);
		SetGWorld(SmallGW,nil);

		//EraseRect( &tRect );		// clear the GWorld 
		
		smallPixMap = GetGWorldPixMap ( SmallGW );
		bigPixMap = GetGWorldPixMap ( BigGW );
		if( (LockPixels ( bigPixMap )) && (LockPixels ( smallPixMap )) )
			{
			for (i=top_patch; i<=bottom_patch; i++)
				{
				EraseRect( &tRect );
				DrawSinglePatch(i);
				tRect2 = tRect;
				OffsetRect(&tRect2,0,(i-top_patch)*(tRect.bottom-tRect2.top));
				CopyBits( (BitMap*)*smallPixMap, (BitMap*)*bigPixMap, &tRect, &tRect2, srcCopy, 0 );
				}
			SetGWorld(oldGW,oldGD);	
			CopyBits( (BitMap*)*bigPixMap, (BitMap*)&dp->portBits, &tRect3, &iRect, srcCopy + ditherCopy, 0 );
			}
		DisposeGWorld( SmallGW );
		}
RGBBackColor(&oldbackcolor);
RGBForeColor(&oldforecolor);
SetPort(oldGraf);
return MCO_SUCCESS;
}




McoStatus InputDialog2::ShowPatch(int32	patch_num)
{
int32	top,bottom;


/*if ((patch_num-top_patch < -1) || ((patch_num >= top_patch) && (patch_num <= top_patch+num_display_patch)) ||
	(patch_num > top_patch+num_display_patch+1))
	{ */
	top = patch_num - 2*num_display_patch/3;
	bottom = top + num_display_patch - 1;
	if (top < 0) 
		{
		top = 0;
		bottom = top + num_display_patch - 1;
		}
	if (bottom >= total_patches-1)
		{
		bottom = total_patches - 1;
		top = bottom - num_display_patch + 1;
		}
	
/*	if (total_patches - patch_num < num_display_patch) 
		{
		top = total_patches - num_display_patch + 1;
		bottom = total_patches;
		}
	else 
		{
		top = patch_num;
		bottom = patch_num+num_display_patch - 1;
		} */
	DisplayPatches(top,bottom);
	top_patch = top;
/*	}
else if (patch_num-top_patch < -1)
	{
	ScrollDisplay(patch_num,-1);
	top_patch = patch_num;
	}
else 
	{
	ScrollDisplay(patch_num,-1);
	top_patch++;
	} */
return MCO_SUCCESS;
}


McoStatus InputDialog2::playSound(short sn)
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
	return MCO_SUCCESS;
}

McoStatus InputDialog2::loadSwop(double *lab_data)
{
Str255 dirname;
McoFile		*SwopF;
FSSpec	myspec;
McoStatus state;
FILE 	*fs;
int i,n,p;
double *lab,*lab2;
double *labtemp;
double	L,A,B;

	SwopF = new McoFile;

	copy_str(dirname,MONACO_FOLDER);
	cat_str(dirname,"\p:");
	cat_str(dirname,MONACO_INK_SIM);

	state = SwopF->getFssFileParam( "\pSwop",dirname,&myspec);
	delete SwopF;

	copy_str(dirname,"\pSwop");
	PathNameFromDirID(myspec.parID, myspec.vRefNum, dirname);
	ptocstr(dirname);
	fs = fopen( (char*)dirname, "r" );
	if (fs == NULL) return MCO_FILE_OPEN_ERROR;

	labtemp = new double[NUM_TOTAL_PATCHES*3];
	
	n = 0;
	lab = labtemp;
	for(i= 0; i < NUM_TOTAL_PATCHES; i++){
		fscanf( fs, "%lf %lf %lf", &L, &A, &B);
		*lab++ = L;
		*lab++ = A;
		*lab++ = B;
		n++;
	}
	
	if (Scramble) scramble_array(labtemp,data_patches,3);
	
	n = 0;
	p = 0;
	lab = lab_data;
	lab2 = labtemp;
	for(i= 0; i < NUM_TOTAL_PATCHES; i++){
		if ((n % patch_per_strip == 0) || (p > data_patches))
			{
			*lab++ = 18.62;
			*lab++ = 0.43;
			*lab++ = 1.05;
			n++;
			}
		*lab++ = *lab2++;
		*lab++ = *lab2++;
		*lab++ = *lab2++;
		p++;
		n++;
	}	
	
delete labtemp;	
return MCO_SUCCESS;
}
void InputDialog2::makeRGB(double *rgb_data)
{
/*
long 		i, j, k, m,n,p;
double		one_d_rgb_patch_table[] = ONE_D_RGB_PATCH_TABLE;
double		*rgbtemp;

	n = 0;
	m = 0;
	
	rgbtemp = new double[ONE_D_PATCHES*ONE_D_PATCHES*ONE_D_PATCHES*3];
	
	for(i = 0; i < ONE_D_PATCHES; i++){
		for(j = 0; j < ONE_D_PATCHES; j++){
			//m = (i*TWO_D_PATCHES + j*ONE_D_PATCHES)*3;
			for(k = 0; k < ONE_D_PATCHES; k++){
				rgbtemp[m++] = one_d_rgb_patch_table[k];
				rgbtemp[m++] = one_d_rgb_patch_table[j];
				rgbtemp[m++] = one_d_rgb_patch_table[i];
			}	
		}	
	}

	for(i = ONE_D_PATCHES*ONE_D_PATCHES*ONE_D_PATCHES; i < total_patches; i++){
		rgbtemp[m++] = 0;
		rgbtemp[m++] = 0;
		rgbtemp[m++] = 0;
		} 

	if (Scramble) scramble_array(rgbtemp,data_patches,3);

	n = 0;
	m = 0;
	p = 0;
	for(i = 0; i < data_patches; i++)
		{
		if (n % patch_per_strip == 0)
			{
			rgb_data[m++] = 0;
			rgb_data[m++] = 0;
			rgb_data[m++] = 0;
			n++;
			}
		rgb_data[m++] = rgbtemp[p++];
		rgb_data[m++] = rgbtemp[p++];
		rgb_data[m++] = rgbtemp[p++];
		n ++;	
		}	
	
	delete rgbtemp;	
	*/
}


#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

void InputDialog2::ConvertToRGB(double *lab,double *rgb,int n)
{
McoStatus state;
double	big_rgb;
double  *rgbp;
int i;

rgbp = rgb;
for (i=0; i<n; i++)
	{
	rgbp[0] = lab[0];
	rgbp[1] = lab[1];
	rgbp[2] = lab[2];
	rgbp += 3;
	lab += 3;
	}

labtonxyzinplace(rgb,n);
state = xyztorgb->compute(rgb, gamma, n);

rgbp = rgb;
for (i=0; i<n; i++)
	{
	big_rgb = bigger(rgbp[0],bigger(rgbp[1],rgbp[2]));
	if (big_rgb>255)
		{
		rgbp[0] = rgbp[0]*255/big_rgb;
		rgbp[1] = rgbp[1]*255/big_rgb;
		rgbp[2] = rgbp[2]*255/big_rgb;
		}
	if (rgbp[0] <0) rgbp[0] = 0;
	if (rgbp[1] <0) rgbp[1] = 0;
	if (rgbp[2] <0) rgbp[2] = 0;
	rgbp +=3;
	}
}

void InputDialog2::ConvertToLAB(double *rgb,double *lab, int n)
{
McoStatus state;
int i;
double	*labp;

labp = lab;
for (i=0; i<n; i++)
	{
	labp[0] = rgb[0];
	labp[1] = rgb[1];
	labp[2] = rgb[2];
	labp +=3;
	rgb += 3;
	}

state = xyztorgb->computeInv(lab, gamma, n);
nxyztoLabinplace(lab,n);
}

// get the rgb values for all of the patches (JWV)
void InputDialog2::getpatchRGB(void)
{
McoStatus state;
Str255	ts;
Str255	fn;
McoFile		*JobF;
int32	hs;
double *rgbpatches;
double *labpatchesref;

_rgbpatchH = NULL;
JobF = new McoFile;

_rgbpatchH = McoNewHandle(sizeof(double)*total_patches*3);
_labpatchrefH = McoNewHandle(sizeof(double)*total_patches*3);

//modified by Peter 9/20
switch(_pagesize){
	case (1): //US letter
	default:
	labpatchesref = (double*)McoLockHandle(_labpatchrefH);
	rgbpatches = (double *)McoLockHandle(_rgbpatchH);
	loadSwop(labpatchesref);
	// convert to rgb
	ConvertToRGB(labpatchesref,rgbpatches,total_patches);
	break;
		
	case (3): //RGB US Letter
	rgbpatches = (double *)McoLockHandle(_rgbpatchH);
	labpatchesref = (double*)McoLockHandle(_labpatchrefH);
	makeRGB(rgbpatches);
	// convert to lab
	ConvertToLAB(rgbpatches,labpatchesref,total_patches);
	break;	

	}
McoUnlockHandle(_rgbpatchH);
McoUnlockHandle(_labpatchrefH);
	
}

McoStatus InputDialog2::_save_target(void)
{
	StandardFileReply 	soutReply;
	Str255				prompt = "\pSave the Current File As";
	Str255				outname = "\p";
	FILE 				*ofsb;
	long 				i, j, c;
	double 				*dataptr;

	StandardPutFile(prompt, outname, &soutReply);
	if(!soutReply.sfGood) return MCO_CANCEL;
	memcpy(outname, soutReply.sfFile.name, *(soutReply.sfFile.name)+1);	
	PathNameFromDirID(soutReply.sfFile.parID, soutReply.sfFile.vRefNum, outname);
	ptocstr(outname);

	ofsb = fopen( (char*)outname, "w");
	if(!ofsb)	return MCO_FILE_OPEN_ERROR;

	dataptr = (double *)McoLockHandle(doc->patchD->dataH);
	
	
	c = 0;
	for(i = 0; i < total_patches; i++)
		{
		if ((i % patch_per_strip != 0) && (c < num_used_patches))
			{
			j = i*3;
			fprintf( ofsb, "%lf \t %lf \t %lf\n", patch_data[j],patch_data[j+1],patch_data[j+2]);
			c++;
			}
		}
	fclose(ofsb);
	McoUnlockHandle(doc->patchD->dataH);

	return MCO_SUCCESS;
}

void InputDialog2::checkFinished(void)
{
int i,c = 0,good_patches = 0;

for (i=0; i<MAX_NUM_PATCHES; i++)
	{
	if ((i % patch_per_strip != 0) && (c < num_used_patches) && (c<data_patches))
		{
		if ( patch_data[i*3] > -1) good_patches++;
		c++;
		}
	}
if (good_patches == c)
	{
	Enable(dp,OK);
	}
else Disable(dp,OK);
}


void InputDialog2::init(void)
{
int32 i,c = 0;
double *p,*p2;
int		flag;

	// copy patch data  into _patchH
	p = (double *)McoLockHandle(doc->patchD->dataH);
	p2 = new double[MAX_NUM_PATCHES*3];

	for (i=0; i<MAX_NUM_PATCHES*3; i++)
		{
		p2[i] = p[i];
		}
		
	if (Scramble) scramble_array(p2,data_patches,3);	
	
	for (i=0; i<MAX_NUM_PATCHES; i++)
		{
		if ((i % patch_per_strip != 0) && (c < num_used_patches) && (c<data_patches))
			{
			patch_data[i*3] = p2[c*3];
			patch_data[i*3+1] = p2[c*3+1];
			patch_data[i*3+2] = p2[c*3+2];
			c++;
			}
		else //if (p[c*3] >=0)
			{
			patch_data[i*3] = 0;
			patch_data[i*3+1] = 0;
			patch_data[i*3+2] = 0;
			}
/*		else 
			{
			patch_data[i*3] = p[c*3];
			patch_data[i*3+1] = p[c*3+1];
			patch_data[i*3+2] = p[c*3+2];
			c++;
			} */
		}
		
	delete p2;
	McoUnlockHandle(doc->patchD->dataH);
}

McoStatus InputDialog2::checkError(int32 start_patch)
{
int32 i;
int32 end_patch;
double max_ref,min_ref;
double max_read,min_read;
double	*labref;
double  read,ref;
double	diff;
short   voice_nums[] = VoiceNums;

end_patch = start_patch+patch_per_strip;
if (end_patch > total_patches) end_patch = total_patches;

labref = (double*)McoLockHandle(_labpatchrefH);

max_ref = -1;
min_ref = 500;
max_read = -1;
min_read = 500;
for (i=start_patch; i<end_patch; i++)
	{
	if (patch_data[i*3] > max_read) max_read = patch_data[i*3];
	if (patch_data[i*3] < min_read) min_read = patch_data[i*3];
	if (labref[i*3] > max_ref) max_ref = labref[i*3];
	if (labref[i*3] < min_ref) min_ref = labref[i*3];
	}
	
diff = 0;	
for (i=start_patch; i<end_patch; i++)
	{
	read = (patch_data[i*3] - min_read)/(max_read-min_read);
	ref = (labref[i*3] - min_ref)/(max_ref-min_ref);
	diff += (read - ref)*(read-ref);
	}
diff = sqrt(diff);

if (diff > 0.90) playSound(voice_nums[(voice_set-1)*2]);
else playSound(voice_nums[(voice_set-1)*2+1]);

McoUnlockHandle(_labpatchrefH);
}


McoStatus InputDialog2::readInPatches(void)
{
int i,count;
short		item;	
short		itemtype;
Handle		cn1,sliderControl;
Rect		r;
McoStatus state;
int32		start_patch;

current_patch = current_patch/patch_per_strip;
current_patch *= patch_per_strip;
start_patch = current_patch;

for (i=0; i<patch_per_strip; i++)
	{
	count = 0;
	do {
		if (xcom != NULL) state = xcom->getNextPatch(&patch_data[current_patch*3]);
		count ++;
		} while ((state != MCO_SUCCESS) && (count < 5));
	if (state != MCO_SUCCESS) 
		{
		//xcom->TurnOnStripRead();
		return state;
		}
	if (doc->patchtype == No_Patches) doc->patchtype = patchtype;
	GetDItem ( dp, Slider, &itemtype, (Handle*)&sliderControl, &r );
	SetCtlValue ( (ControlHandle)sliderControl,current_patch);
	ShowPatch(current_patch);
	current_patch++;
	}
// check for error
checkError(start_patch);
if (current_patch >= total_patches) current_patch = total_patches -1;
ShowPatch(current_patch);
current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;
SetSheetStripText();
//xcom->TurnOnStripRead();
return MCO_SUCCESS;
}



McoStatus InputDialog2::SetSheetStripText(void)
{
	Rect r;
	short itemType;
	Handle cn1;
	Str255 theString;
	
	
GetDItem ( dp,Cur_sheet, &itemType, (Handle*)&cn1, &r );
NumToString(current_sheet+1,theString);
SetIText(cn1,theString);
GetDItem ( dp,Cur_strip, &itemType, (Handle*)&cn1, &r );
NumToString(current_strip+1,theString);
SetIText(cn1,theString);
return MCO_SUCCESS;
}

int32 InputDialog2::findNextPatch(void)
{
int32 i;
for (i=0; i<total_patches; i++)
	{
	if (patch_data[i*3] < 0) return i;
	}
return -1;
}

pascal Boolean myFilter(DialogPtr dlg, EventRecord *event, short *itemHit);

Boolean InputDialog2::TimerEventPresent(void)
{
if( TickCount() - _last_tick_time > 60 ) 
	{
	return TRUE;
	}
return FALSE;
}

				


McoStatus InputDialog2::DrawWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldP;
	
	GetPort(&oldP);

	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	ShowPatch(current_patch);
	SetPort(oldP);
	return MCO_SUCCESS;
}	

McoStatus InputDialog2::UpdateWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldP;
	
	GetPort(&oldP);
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	ShowPatch(current_patch);
	DrawDialog( dp );
	EndUpdate( dp );
	
	SetPort(oldP);
	return MCO_SUCCESS;
}			

Boolean InputDialog2::isMyObject(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;
if (titem == Slider)
	{
	if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
		{
		*item = titem;
		SetPort(oldP);
		return TRUE;
		}
	}
SetPort(oldP);
return FALSE;
}


McoStatus InputDialog2::DoEvents(short item, Point clickPoint, Window_Events &code, MS_WindowTypes &wintype, int32 &winnum, void **data, Boolean &changed)
{
UserItemUPP myfilter;
Str255		theString;
short		itemtype;
Handle		cn1,sliderControl;
Rect		r;
short		part;
Point		mpoint;
int32		next_patch;
McoStatus	status;
WindowPtr	oldp;

	GetPort(&oldp);
	SetPort(dp);

	Changed = FALSE;
	code = WE_Do_Nothing;
	wintype = No_Dialog;
	winnum = -1;
	status = MCO_SUCCESS;

	switch(item){
		case (TIMER_ITEM ):   // a timer event has occured
			_last_tick_time = TickCount();
			if (_waiting)
				{
				if (!xcom->IsBufferEmpty())	// has a strip been read in
					{
					changed = TRUE; 
					// check to see if a strip has been read in 
					if (xcom->IsStripPresent()) 
						{
						doc->needsSave = TRUE;
						GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
						GetIndString(theString,Message_List_ID,4);
						SetIText(cn1,theString);
						readInPatches();
						next_patch = findNextPatch();
						if (next_patch == -1)
							{
							GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
							GetIndString(theString,Message_List_ID,5);
							SetIText(cn1,theString);
							_done == TRUE;
							_waiting = FALSE;
							Disable(dp,Redo);
							Disable(dp,ReadNext);
							Disable(dp,Cancel);
							Enable(dp,Save_as);	
							Enable(dp,OK);			
							}
						else
							{
							GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
							GetIndString(theString,Message_List_ID,2);
							SetIText(cn1,theString);
							}
						}
					}
				checkFinished();
				}
			break;
		case (Slider):   // the slider has been adjusted
			GetMouse(&mpoint);
			do {
				current_patch = GetSlider(mpoint,Slider);
				ShowPatch(current_patch);
				SetSheetStripText();
				current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
				current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;	
				GetMouse(&mpoint);
				} while ( StillDown ( ) );
			checkError((current_patch/patch_per_strip)*patch_per_strip);
			_waiting = 0;
			Enable(dp,Redo);
			Enable(dp,ReadNext);	   
			//ShowPatch(current_patch); // display the window
			//current_strip = current_patch/patch_per_strip;
			//current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
			GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
			GetIndString(theString,Message_List_ID,1);
			SetIText(cn1,theString);
			SetSheetStripText();
			break;
		case (ReadNext): // get the next strip
			_waiting = 1;	
			current_patch = findNextPatch();  // reset the display, current patch, sheet, strip, and text
			ShowPatch(current_patch); // display the window
			current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
			current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;
			Disable(dp,Redo);
			Disable(dp,ReadNext);	 
			GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
			GetIndString(theString,Message_List_ID,2);
			SetIText(cn1,theString);
			SetSheetStripText();
			break;
		case (Redo): // Reread a strip
			_waiting = 1;		   // reset the display, current patch, sheet, strip, and text
			Disable(dp,Redo);
			Disable(dp,ReadNext);	 
			GetDItem ( dp,Message, &itemtype, (Handle*)&cn1, &r );
			GetIndString(theString,Message_List_ID,3);
			SetIText(cn1,theString);
			break;
		case (Save_as):
			StorePatches();
			doc->handle_save_datafile(doc->patchDcopy);
			ShowPatch(current_patch);
			break;
		case (VoiceSet):
			GetDItem(dp, VoiceSet, &itemtype, (Handle*)&cn1, &r );
			voice_set = GetCtlValue((ControlHandle)cn1);
			break;
		case (OK):
			StorePatches();
			doc->patchtype = patchtype;
			//doc->needsSave = TRUE;
			code = WE_Close_Window;
			wintype = WinType;
			winnum = WinNum;
			break;
		case (Cancel):
			code = WE_Close_Window;
			wintype = WinType;
			winnum = WinNum;
			break;
    }	
	SetPort(oldp);
	return status;	    	
}


