#include <math.h>
#include "inputdialog.h"
#include "scram_array.h"
#include "pascal_string.h"
#include "think_bugs.h"
#include "fullname.h"

#define VoiceNums {101,102,-1,-1,-1,-1}

InputDialog::InputDialog(ProfileDoc *dc,RawData *pD,int strips)
{
	short		iType;
	Handle		iHandle;
	Handle		sHandle;
	Rect 		iRect, r,pRect,tRect;	
	UserItemUPP  box3D;
	
	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	RGBColor	c,oldbackcolor,oldforecolor;
	WindowPtr	oldP;
	OSErr			err;
	int32 wi,hi;
	double w,h;
	McoColorSync *mon,*lab,*printer,*list[2];
	int32		n;
	McoStatus state = MCO_SUCCESS;
	
	priority = 1;
	
	patchD = pD;
	
	if (pD == NULL) return;
	
	doc = (ProfileDocInf*)dc;
	
	dtype = DType_Unknown;
	
	//Scramble = !(isPressed(0x3a));
	Scramble = FALSE;
	

	
	_rgbpatchH = NULL;
	_labpatchrefH = NULL;

	chan = nil;
	
	err = SndNewChannel (&chan, 0, 0, nil);
	if (err != noErr) chan = nil;

	
	// see if black strips are present
	n =strips;

	// finda the total number of patches
	patchtype = patchD->type;
	oldpatchtype = doc->patchtype;
	patch_per_strip = patchD->patches.rows+n;
	strip_per_sheet = patchD->patches.sheets;
	if (patchD->format.numLinear > 0)
		data_patches = patchD->format.total+patchD->format.numc*(patchD->format.numLinear-1);
	else 
		data_patches = patchD->format.total;

	if (data_patches % (patch_per_strip - n) == 0)
		total_patches = ((data_patches/(patch_per_strip - n)))*patch_per_strip;
	else total_patches = ((data_patches/(patch_per_strip - n))+1)*patch_per_strip;

	patch_data = new double[total_patches*3];

	
	//patch_data = (double*)McoLockHandle(patchH);
	
	num_used_patches  = data_patches;


	
	
	_last_tick_time2 = 0;
	
	_first = 1;
	
	mon = new McoColorSync;
	mon->setType(Mco_ICC_System);

	lab = new McoColorSync;
	lab->GetProfile(LAB_PROFILE_NAME);
		
	lab_rgb_xform = new McoXformIccMac;
	list[0] = lab;
	list[1] = mon;
	state = lab_rgb_xform->buildCW(list,2);	
	if (state != MCO_SUCCESS) { delete lab_rgb_xform; lab_rgb_xform = 0L;}

	printer = new McoColorSync;
	printer->GetProfile(SWOP_PROFILE_NAME);

	cmyk_rgb_xform = new McoXformIccMac;
	list[0] = printer;
	list[1] = mon;
	state = cmyk_rgb_xform->buildCW(list,2);
	if (state != MCO_SUCCESS) { delete cmyk_rgb_xform; cmyk_rgb_xform = 0L;}

		
	delete mon;
	delete lab;
	delete printer;
}	

InputDialog::~InputDialog(void)
{
OSErr			err;

	DisposeGWorld( BigGW );
	if (_rgbpatchH != NULL) McoDeleteHandle(_rgbpatchH);
	if (_labpatchrefH != NULL) McoDeleteHandle(_labpatchrefH);
	
	if (chan != nil) err = SndDisposeChannel (chan,TRUE);
	if (lab_rgb_xform != NULL) delete lab_rgb_xform;
	if (cmyk_rgb_xform != NULL) delete cmyk_rgb_xform;
	if (patch_data) delete patch_data;
}

int32 InputDialog::findNextPatch(void)
{
int32 i;
for (i=0; i<total_patches; i++)
	{
	if (patch_data[i*3] < 0) return i;
	}
return -1;
}


McoStatus InputDialog::StorePatches(void)
{
int i,c = 0,d=0,e=0;;
double *p,*p2,*pc;

	// copy patch data  into _patchH
	p = (double *)McoLockHandle(patchD->dataH);
	if (patchD->linearDataH) p2 = (double *)McoLockHandle(patchD->linearDataH);
	
	for (i=0; i<total_patches; i++)
		{
		if  ((c < num_used_patches) && (!BlackPatch(i)) && (c<patchD->format.total))
			{
			p[c*3] = patch_data[i*3];
			p[c*3+1] = patch_data[i*3+1];
			p[c*3+2] = patch_data[i*3+2];
			c++;
			}
		else if ((c < num_used_patches) && (!BlackPatch(i)) && (patchD->linearDataH))
			{
			p2[e*3] = patch_data[i*3];
			p2[e*3+1] = patch_data[i*3+1];
			p2[e*3+2] = patch_data[i*3+2];
			c++;
			e++;
			}
		}

	if (Scramble) unscramble_array(p,data_patches,3);	
	McoUnlockHandle(patchD->dataH);

	if (patchD->linearDataH) McoUnlockHandle(patchD->linearDataH);	
		
	doc->didGammutSurface = FALSE;
	doc->_smoothedPatches = FALSE;
	doc->changedProfile = 1;	

	return MCO_SUCCESS;
}

McoStatus InputDialog::StoreDataToTemp(void)
{
	FileFormat filef;
	FSSpec fspec;
	McoStatus  state;
	Str255  def = MONACO_TEMP_PATCHES;
	Str255  fold = MONACO_FOLDER;
	Str255  outname = "\p";
	FILE 				*ofsb;

	StorePatches();

	ptocstr(def);
	ptocstr(fold);

	state = filef.createFileinPref(&fspec,(char*)def,(char*)fold);
	if (state != MCO_SUCCESS) return state;
	filef.closeFile();
	memcpy(outname, fspec.name, *(fspec.name)+1);
	PathNameFromDirID(fspec.parID, fspec.vRefNum, outname);
	ptocstr(outname);

	ofsb = fopen( (char*)outname, "w");
	if(!ofsb)	return MCO_FILE_OPEN_ERROR;

	state = patchD->saveData(ofsb);
	fclose(ofsb);
	
	filef.setInfo(&fspec,MONACO_SIG,'TEXT');
	return state;
}

McoStatus InputDialog::DrawSinglePatch(int32 patch_num)
{
	int 	i;
	char	st[255];
	double	new_rgb[3];
	RGBColor	c,oldbackcolor,oldforecolor;
	short	iType;
	Handle	iHandle;
	Rect 	iRect;
	Rect 	pRect, nRect,tRect,tRect2;
	double *rgb_data;
	WindowPtr	oldP;
	int density;

	rgb_data = (double *)McoLockHandle(_rgbpatchH);

	GetDItem (dp, ids[Strip_Rect], &iType, (Handle*)&iHandle, &iRect);
	GetDItem (dp, ids[Patch_Rect], &iType, (Handle*)&iHandle, &pRect);
	GetDItem (dp, ids[New_Patch_Rect], &iType, (Handle*)&iHandle, &nRect);
	
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
		density = 0;
		if ((patch_num >= patchD->format.total) && (patchD->format.numLinear > 0) && 
			(patch_data[patch_num*3+1] == 0) && (patch_data[patch_num*3+2] == 0) && 
			(dtype != DType_Lab))
			{
			i = (patch_num-patchD->format.total)/(patchD->format.numLinear-1);
			ConvertDensityToRGB(patch_data[patch_num*3],i, new_rgb);
			density = 1;
			}
		else ConvertToRGB(&patch_data[patch_num*3], new_rgb,1);
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
		sprintf(st,"%5.2f",patch_data[patch_num*3]);
		
		if (strlen(st) <10) DrawString(ctopstr(st));
		//DrawString("\p  ");
		MoveTo(nRect.right + 42,nRect.top+10);
		sprintf(st,"%5.1f",patch_data[patch_num*3+1]);
		if ((strlen(st) <10) && (!density)) DrawString(ctopstr(st));
		//DrawString("\p  ");
		MoveTo(nRect.right + 86,nRect.top+10);
		sprintf(st,"%5.1f",patch_data[patch_num*3+2]);
		if ((strlen(st) <10) && (!density)) DrawString(ctopstr(st));

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

McoStatus InputDialog::DisplayPatches(int32 top_patch,int32 bottom_patch)
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
	
	GetDItem (dp, ids[Strip_Rect], &iType, (Handle*)&iHandle, &iRect);

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




McoStatus InputDialog::ShowPatch(int32	patch_num)
{
int32	top,bottom;

	if ((patch_num < current_top) || (patch_num > current_bottom))
		{
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
			
		current_top = top;
		current_bottom = bottom;
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
	DisplayPatches(current_top,current_bottom);
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

McoStatus InputDialog::ShowPatchScroll(int32	patch_num)
{
int32	top,bottom;

/*if ((patch_num-top_patch < -1) || ((patch_num >= top_patch) && (patch_num <= top_patch+num_display_patch)) ||
	(patch_num > top_patch+num_display_patch+1))
	{ */
	top = patch_num - current_disp;
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
		
	current_top = top;
	current_bottom = bottom;
	
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

// find the number of the selected 
// -1 indicates that no patch has been found
int32	InputDialog::whichPatch(Point thePoint)
{

	short	iType;
	Handle	iHandle;
	Rect 	iRect;
	Rect 	pRect;
	Point	tPoint;
	int32	PatchHeight;
	
	GetDItem (dp, ids[Strip_Rect], &iType, (Handle*)&iHandle, &iRect);
	GetDItem (dp, ids[Patch_Rect], &iType, (Handle*)&iHandle, &pRect);
	
	// be sure that it is within the correct box
	if (PtInRect(thePoint,&iRect)) 
		{
		tPoint = thePoint;
		tPoint.v -= iRect.top;
		
		PatchHeight = pRect.bottom-pRect.top;
		if (PatchHeight == 0) return -1;
		return current_top+(tPoint.v/PatchHeight);	
		}
	return -1;
}

McoStatus InputDialog::playSound(short sn)
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

// load in the swop icc, build the color world, and translate the colors
McoStatus InputDialog::loadSwop(double *rgb_data,double *lab_data)
{
int i,j,k,l,m=0,n=0;;
McoColorSync *printer = NULL,*lab= NULL,*list[2];
McoXformIccMac  *xform = NULL;
McoStatus state = MCO_SUCCESS;
unsigned char	*cmyk_d = NULL;
unsigned char	*rgb_d = NULL;

for (i=0; i<total_patches; i++) 
	{
	rgb_data[i*3] = 255;
	rgb_data[i*3+1] = 255;
	rgb_data[i*3+1] = 255;
	lab_data[i*3] = 100;
	lab_data[i*3+1] = 0;
	lab_data[i*3+1] = 0;
	}


cmyk_d = new unsigned char[(total_patches+1)*4];
rgb_d = new unsigned char[(total_patches+1)*4];

if (patchD->ordering == Order_MCYK)
	{
	for (l=0; l<patchD->format.numslow; l++)
		{
		for (i=0; i<patchD->format.cubedim[l]; i++) 
			{
			for (j=0; j<patchD->format.cubedim[l]; j++)
				{
				for (k=0; k<patchD->format.cubedim[l]; k++)
					{
					cmyk_d[m++] = (unsigned char)(2.55*patchD->getcmpValues(l,j));
					cmyk_d[m++] = (unsigned char)(2.55*patchD->getcmpValues(l,k));
					cmyk_d[m++] = (unsigned char)(2.55*patchD->getcmpValues(l,i));
					cmyk_d[m++] = (unsigned char)(2.55*patchD->format.slowValues[l]);
					n++;
					}
				}
			}
		}	
	}

for (i=0; i<patchD->format.numc; i++)
	{
	for (j=1; j<patchD->format.numLinear; j++)
		{
		for (k=0; k<patchD->format.numc; k++) cmyk_d[m+k] = 0;
		cmyk_d[m+i] = (unsigned char)(2.55*patchD->format.linearValues[j]);
		m+=patchD->format.numc;
		n++;
		}
	}
	
for (i=n; i<total_patches; i++)
	{
	cmyk_d[m++] = 0;
	cmyk_d[m++] = 0;
	cmyk_d[m++] = 0;
	cmyk_d[m++] = 0;
	}
	
if (BlackPatch(0))
	{
	cmyk_d[m++] = 255;
	cmyk_d[m++] = 255;
	cmyk_d[m++] = 255;
	cmyk_d[m++] = 255;
	}

cmyk_rgb_xform->evalColors(cmyk_d,rgb_d,total_patches+1);

if (Scramble) scramble_array(rgb_d,data_patches,3);	

m=0;
for (i=0; i<total_patches; i++) 
	{
	if (BlackPatch(i))
		{
		rgb_data[i*3] = rgb_d[total_patches*3];
		rgb_data[i*3+1] = rgb_d[total_patches*3+1];
		rgb_data[i*3+2] = rgb_d[total_patches*3+2];
		}
	else
		{
		rgb_data[i*3] = rgb_d[m*3];
		rgb_data[i*3+1] = rgb_d[m*3+1];
		rgb_data[i*3+2] = rgb_d[m*3+2];
		m++;
		}
	}


lab = new McoColorSync;
lab->GetProfile(LAB_PROFILE_NAME);

printer = new McoColorSync;
printer->GetProfile(SWOP_PROFILE_NAME);

xform = new McoXformIccMac;
list[0] = printer;
list[1] = lab;
state = xform->buildCW(list,2);
if (state != MCO_SUCCESS) goto bail;

xform->evalColors(cmyk_d,rgb_d,total_patches+1);
if (Scramble) scramble_array(rgb_d,data_patches,3);	

m =0;
for (i=0; i<total_patches; i++)
	{
	if (BlackPatch(i))
		{
		lab_data[i*3] = (double)rgb_d[total_patches*3]/2.55;
		lab_data[i*3+1] = (double)rgb_d[total_patches*3+1]-128; 
		lab_data[i*3+2] = (double)rgb_d[total_patches*3+2]-128; 
		}
	else
		{	
		lab_data[i*3] = (double)rgb_d[m*3]/2.55;
		lab_data[i*3+1] = (double)rgb_d[m*3+1]-128; 
		lab_data[i*3+2] = (double)rgb_d[m*3+2]-128; 
		m++;
		}
	}


bail:	
if (xform) delete xform;
if (lab) delete lab;
if (printer) delete printer;
if (cmyk_d) delete cmyk_d;
if (rgb_d) delete rgb_d;

return state;
}


McoStatus InputDialog::makeRGB(double *rgb_data, double *lab_data)
{
McoColorSync *mon,*lab,*list[2];
McoXformIccMac  *xform;
long 		i, j, k, m=0;
unsigned char *rgb_d;
McoStatus state = MCO_SUCCESS;

rgb_d = new unsigned char[total_patches*3];	
	
for (i=0; i<patchD->format.cubedim[0]; i++) 
	{
	for (j=0; j<patchD->format.cubedim[0]; j++)
		{
		for (k=0; k<patchD->format.cubedim[0]; k++)
			{
			rgb_d[m++] = patchD->getcmpValues(0,k)*2.55;
			rgb_d[m++] = patchD->getcmpValues(0,j)*2.55;
			rgb_d[m++] = patchD->getcmpValues(0,i)*2.55;
			}
		}
	}	

for (i=0; i<patchD->format.numc; i++)
	{
	for (j=1; j<patchD->format.numLinear; j++)
		{
		for (k=0; k<patchD->format.numc; k++) rgb_d[m+k] = 0;
		rgb_d[m+i] = patchD->format.linearValues[j];
		m+=3;
		}
	}
	
if (Scramble) scramble_array(rgb_d,data_patches,3);	

m = 0;
for (i=0; i<total_patches; i++)
	{
	if (BlackPatch(i))
		{
		rgb_data[i*3] = (double)rgb_d[0];
		rgb_data[i*3+1] = (double)rgb_d[1];
		rgb_data[i*3+2] = (double)rgb_d[2];
		}
	else
		{
		rgb_data[i*3] = (double)rgb_d[m*3];
		rgb_data[i*3+1] = (double)rgb_d[m*3+1];
		rgb_data[i*3+2] = (double)rgb_d[m*3+2];
		m++;
		}
	}

mon = new McoColorSync;
mon->setType(Mco_ICC_System);

lab = new McoColorSync;
lab->GetProfile(LAB_PROFILE_NAME);
	
xform = new McoXformIccMac;
list[0] = mon;
list[1] = lab;
state = xform->buildCW(list,2);	
	
xform->evalColors(rgb_d,rgb_d,total_patches);

m = 0;
for (i=0; i<total_patches; i++)
	{
	if (BlackPatch(i))
		{
		lab_data[i*3] = (double)rgb_d[0]/2.55;
		lab_data[i*3+1] = (double)rgb_d[1]-128; 
		lab_data[i*3+2] = (double)rgb_d[2]-128; 
		}
	else
		{	
		lab_data[i*3] = (double)rgb_d[m*3]/2.55;
		lab_data[i*3+1] = (double)rgb_d[m*3+1]-128; 
		lab_data[i*3+2] = (double)rgb_d[m*3+2]-128; 
		m++;
		}
	}
	
if (Scramble) scramble_array(lab_data,data_patches,3);	
	
bail:	
if (xform) delete xform;
if (mon) delete mon;
if (lab) delete lab;
if (rgb_d) delete rgb_d;

return MCO_SUCCESS;		
}


#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

void InputDialog::ConvertToRGB(double *lab,double *rgb,int n)
{
unsigned char *temp;
int i;

temp = new unsigned char[n*3];

for (i=0; i<n; i++) 
	{
	temp[i*3] = 2.55*lab[i*3];
	temp[i*3+1] = lab[i*3+1]+128;
	temp[i*3+2] = lab[i*3+2]+128;
	}
	
if (lab_rgb_xform != NULL) lab_rgb_xform->evalColors(temp,temp,n);
	
for (i=0; i<n*3; i++) rgb[i] = temp[i];
delete temp;
	
}


void InputDialog::ConvertDensityToRGB(double d,int c,double *rgb)
{
unsigned char temp[4];
int i;
double	ink;

for (i=0; i<4; i++) temp[i] = 0;

ink = 100*d/1.5;

if (ink > 100) ink = 100;
if (ink < 0) ink = 0;

temp[c] = (unsigned char)(2.55*ink);
	
if (lab_rgb_xform != NULL) cmyk_rgb_xform->evalColors(temp,temp,1);
	
for (i=0; i<3; i++) rgb[i] = temp[i];

	
}


// get the rgb values for all of the patches (JWV)
void InputDialog::getpatchRGB(void)
{
McoStatus state;
Str255	ts;
Str255	fn;
int32	hs;
double *rgbpatches;
double *labpatchesref;

_rgbpatchH = 0L;

_rgbpatchH = McoNewHandle(sizeof(double)*total_patches*3);
_labpatchrefH = McoNewHandle(sizeof(double)*total_patches*3);

//modified by Peter 9/20
switch(patchtype){
	case (CMYKPatches): //US letter
	default:
	labpatchesref = (double*)McoLockHandle(_labpatchrefH);
	rgbpatches = (double *)McoLockHandle(_rgbpatchH);
	loadSwop(rgbpatches,labpatchesref);

	break;
		
	case (RGBPatches): //RGB US Letter
	rgbpatches = (double *)McoLockHandle(_rgbpatchH);
	labpatchesref = (double*)McoLockHandle(_labpatchrefH);
	makeRGB(rgbpatches,labpatchesref);

	break;	

	}
McoUnlockHandle(_rgbpatchH);
McoUnlockHandle(_labpatchrefH);
	
	
}



Boolean InputDialog::checkFinished(void)
{
int i,c = 0,good_patches = 0;

for (i=0; i<MAX_NUM_PATCHES; i++)
	{
	if (((i % patch_per_strip != 0) || (!BlackPatch(i))) && (c < num_used_patches) && (c<data_patches))
		{
		if ( patch_data[i*3] > -1) good_patches++;
		c++;
		}
	}
if (good_patches == c)
	{
	if (current_patch >= data_patches) StoreDataToTemp();
	Enable(dp,OK);
	return TRUE;
	}
else Disable(dp,OK);
return FALSE;
}


void InputDialog::init(void)
{
int32 i,c = 0, d=0, n=0,l=0;
double *p,*p2,*pl;
int		flag;
int		lab = 0;

	// copy patch data  into _patchH
	p = (double *)McoLockHandle(patchD->dataH);
	if (patchD->linearDataH) pl = (double *)McoLockHandle(patchD->linearDataH);
	p2 = new double[data_patches*3];
	
	if (!p2) return;

	for (i=0; i<data_patches*3; i++)
		{
		p2[i] = p[i];
		}
		
	if (Scramble) scramble_array(p2,data_patches,3);	
	
	for (i=0; i<total_patches; i++)
		{
		if ((c < num_used_patches) && (c<data_patches) && (c<patchD->format.total) && (!BlackPatch(i)))
			{
			patch_data[i*3] = p2[c*3];
			patch_data[i*3+1] = p2[c*3+1];
			patch_data[i*3+2] = p2[c*3+2];
			c++;
			n++;
			}
		else if ((c < num_used_patches) && (c<data_patches)  && (!BlackPatch(i)) && (patchD->linearDataH))
			{
			patch_data[i*3] = pl[l*3];
			patch_data[i*3+1] = pl[l*3+1];
			patch_data[i*3+2] = pl[l*3+2];
			if ((pl[l*3+1] != 0) && (pl[l*3+2] != 0) && (pl[l*3+1] > -500)) lab = 1;
			l++;
			c++;
			} 
		else 
			{
			patch_data[i*3] = 0;
			patch_data[i*3+1] = 0;
			patch_data[i*3+2] = 0;
			n++;
			} 
		}
		
	if ((!lab) && (patchD->linearDataH))
		{
		if ((pl[0] > -500) && (pl[0] > pl[3*(patchD->format.numLinear-2)]) && (pl[patchD->format.numLinear-2] > -500))
			lab = 1;
		}
		
	if (lab) dtype = DType_Lab;
		
	if (findNextPatch() == -1) 
		{
		NotAlreadFinished = FALSE;
		}
	else NotAlreadFinished = TRUE;	
		
	delete p2;
	McoUnlockHandle(patchD->dataH);
	if (patchD->linearDataH) McoUnlockHandle(patchD->linearDataH);
	
}

McoStatus InputDialog::checkError(int32 start_patch)
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

Boolean InputDialog::TimerEventPresent(void)
{
if( TickCount() - _last_tick_time2 > 1 ) 
	{
	return TRUE;
	}
return FALSE;
}


McoStatus InputDialog::DrawWindow(void)	
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

McoStatus InputDialog::UpdateWindow(void)	
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

Boolean InputDialog::isMyObject(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;
if (titem == ids[Slider])
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


Boolean InputDialog::KeyFilter(EventRecord *theEvent,short *itemHit )
{

//	if (( theEvent->what != keyDown) && (theEvent->what != autoKey))	// just looking for keystrokes
//		return(FALSE);

	switch ( (theEvent->message) & charCodeMask ) {
		case 0x0d:	// Return pressed or ...
		case 0x03:	// ... Enter pressed
			*itemHit = OK;
			return( TRUE );	// Note: pascal-style TRUE
		case 0x1b:
			*itemHit = Cancel;	// Esc pressed
			return( TRUE );
		case 0x1E:
			*itemHit = Position_Up;
			return( TRUE );
		case 0x1F:
			*itemHit = Position_Down;
			return( TRUE );
		case 0x1C:
			*itemHit = Position_Left;
			return( TRUE );
		case 0x1D:
			*itemHit = Position_Right;
			return( TRUE );
		} 
	
		
return( FALSE );	// all others
} 


