////////////////////////////////////////////////////////////////////////////////
//	printdialog.c															  //
//																			  //
//	The main dialog for MonacoPRINT											  //
//	James Vogh																  //
// June 4, 1996																  //
////////////////////////////////////////////////////////////////////////////////

#include "basedialog.h"
#include "printdialog.h"

ThermDialog::ThermDialog(ProfileDoc *d,int winnum)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	int32 	i;
	long	max;
	unsigned char		*clip;
	int		ids_t[] = Pri_IDS;
	
	for (i=0; i<NUM_Pri_IDS; i++) ids[i] = ids_t[i];
	
	setDialog(ids[Print_Dialog]);
	//frame_button(ids[Pri_OK_BOX]);
	
	WinType = PrintWindow;
	WinNum = winnum;
	
	doc = d;
	updateWindowData();
	
	Disable(dp,ids[PrintIcon]);
	
	gammut_winNum = -1;
}	


McoStatus ThermDialog::updateWindowData(void)
{
short	iType;
Handle	iHandle;
Rect	iRect;
WindowPtr	oldp;
	
GetPort(&oldp);
SetPort(dp);	
	
if (doc->patchD->type == No_Patches) 
	{
	if (doc->inputStatus() == Getting_Input)
		{
		GetDItem (dp, ids[ReadIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,255);
		}
	else 
		{
		GetDItem (dp, ids[ReadIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		}
	GetDItem (dp, ids[CalibrateIcon], &iType, &iHandle, &iRect);
	HiliteControl((ControlHandle)iHandle,255);
	GetDItem (dp, ids[GamutIcon], &iType, &iHandle, &iRect);
	HiliteControl((ControlHandle)iHandle,255);
	}
else
	{
	if (doc->inputStatus() == No_Input)
		{
		GetDItem (dp, ids[ReadIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		GetDItem (dp, ids[CalibrateIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		GetDItem (dp, ids[GamutIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		}
	else if ((doc->inputStatus() == Processing_Input1) || (doc->inputStatus() == Processing_Input2))
		{
		GetDItem (dp, ids[ReadIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,255);
		GetDItem (dp, ids[CalibrateIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		GetDItem (dp, ids[GamutIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,0);
		}
	else if (doc->inputStatus() == Getting_Input)
		{
		GetDItem (dp, ids[ReadIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,255);
		GetDItem (dp, ids[CalibrateIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,255);
		GetDItem (dp, ids[GamutIcon], &iType, &iHandle, &iRect);
		HiliteControl((ControlHandle)iHandle,255);
		}
	}

// set the window title
if (doc->patchFName != NULL) SetWTitle( dp, doc->patchFName);
SetPort(oldp);
return MCO_SUCCESS;
}



McoStatus	ThermDialog::DoEvents(short item, Point clickPoint, Window_Events &code, MS_WindowTypes &wintype, int32 &winnum, void **data,Boolean &changed)
{
	McoStatus	status;
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	

	wintype = WinType;
	winnum = WinNum;
	code = WE_Do_Nothing;

		if (item == ids[PrintIcon])
			{
			code = WE_Doc_Print;
			}
		
		else if (item == ids[ReadIcon])
			{
			code = WE_Doc_Read;
			}

		else if (item == ids[CalibrateIcon])
			{
			code = WE_PrDoc_Calibrate;
			}	
		
		else if (item == ids[GamutIcon])
			{
			code = WE_PrDoc_Gammut;
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




