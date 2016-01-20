////////////////////////////////////////////////////////////////////////////////
//	printdialog.c															  //
//																			  //
//	The main dialog for MonacoPRINT											  //
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////

#include "basedialog.h"
#include "printdialog.h"
#include "pascal_string.h"

PrintDialog::PrintDialog(ProfileDoc *d,int winnum)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	int32 	i;
	long	max;
	unsigned char		*clip;
	int		ids_t[] = Pri_IDS;
	
	priority = 1;
	
	for (i=0; i<NUM_Pri_IDS; i++) ids[i] = ids_t[i];
	
	setDialog(ids[Print_Dialog],0,120);
	//frame_button(ids[Pri_OK_BOX]);
	
	WinType = PrintWindow;
	WinNum = winnum;
	
	doc = d;
	updateWindowData(0);
	
	Disable(dp,ids[PrintIcon]);
	
	gammut_winNum = -1;
}	


McoStatus PrintDialog::updateWindowData(int changed)
{
short	iType;
Handle	iHandle;
Rect	iRect;
WindowPtr	oldp;
Str255	wintitle;
	
GetPort(&oldp);
SetPort(dp);	
	
if (doc->locked)
	{
	Disable(dp,ids[ReadIcon]);
	Disable(dp,ids[CalibrateIcon]);
	Disable(dp,ids[GamutIcon]);
	}
else if (doc->patchD->type == No_Patches) 
	{
	if (doc->inputStatus() & Getting_Input)
		{
		Disable(dp,ids[ReadIcon]);
		}
	else 
		{
		Enable(dp,ids[ReadIcon]);
		}
	Disable(dp,ids[CalibrateIcon]);
	Disable(dp,ids[GamutIcon]);
	}
else
	{
	if (doc->inputStatus() == No_Input)
		{
		Enable(dp,ids[ReadIcon]);
		Enable(dp,ids[CalibrateIcon]);
		Enable(dp,ids[GamutIcon]);
		}
	else if ((doc->inputStatus() & Processing_Input1) || 
			 (doc->inputStatus() & Processing_Input2) ||
			 (doc->inputStatus() & Tweaking_Input))
		{
		Disable(dp,ids[ReadIcon]);
		Enable(dp,ids[CalibrateIcon]);
		Enable(dp,ids[GamutIcon]);
		}
	else if (doc->inputStatus() & Getting_Input)
		{
		Disable(dp,ids[ReadIcon]);
		Disable(dp,ids[CalibrateIcon]);
		Disable(dp,ids[GamutIcon]);
		}
	}

// set the window title
if (doc->patchFName != 0L) 
	{
	GetWTitle(dp,wintitle);
	if (!cmp_str(wintitle,doc->patchFName)) SetWTitle( dp, doc->patchFName);
	}
SetPort(oldp);
return MCO_SUCCESS;
}


// The event handler for the main menu
McoStatus	PrintDialog::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
	McoStatus	status;
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	
	// no codes passed back
	*numwc = 0;

	if (item == ids[PrintIcon])		// print patches
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_Doc_Print;
		wc[0].doc = doc;
		}
	
	else if (item == ids[ReadIcon])	// read patches
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_Doc_Read;
		wc[0].doc = doc;
		}

	else if (item == ids[CalibrateIcon]) // build a table
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_PrDoc_Calibrate;
		wc[0].doc = doc;
		}	
	
	else if (item == ids[GamutIcon]) // display the gamut
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_PrDoc_Gammut;
		wc[0].doc = doc;
		//winnum = gammut_winNum;
		}				
return MCO_SUCCESS;
}




// debugging
/*
void PrintDialog::showNum(int32 num) 
{
short	iType;
Handle	iHandle;
Rect	iRect;
Str255	st;

	RGBColor	c;

	SetPort(dp);
	
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);		

	GetDItem (dp, 15, &iType, &iHandle, &iRect);
	NumToString(num,st);
	SetIText(iHandle,st);

} */




