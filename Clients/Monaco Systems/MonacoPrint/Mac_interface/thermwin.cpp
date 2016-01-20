/*      Thermometer.c
		
		Draw a dialog with a thermometer
		
		(c) Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/

#include "thermwin.h"
#include "monacoprint.h"

#define THERM_IDs	{16003, 1, 2, 3,4}
#define THERM_MESS 17008

#define Therm_PICT	(133)

#ifdef __POWERPC
//QDGlobals	qd;
#endif

//extern QDGlobals			*qdPtr;	//we'll use this to stuff the apps A5 into

void gray_d_item(DialogPtr dp, short item)
{
	Rect r;
	Handle h;
	short itemType;
	RGBColor  c,old;

	GetDItem (dp, item, &itemType, &h, &r);

	PenNormal ();
	PenSize (2, 2);
	
	InsetRect (&r, -2, -2);
	FrameRect (&r);
	GetForeColor(&old);
	c.red = 32000;
	c.green = 32000;
	c.blue = 32000;
	RGBForeColor(&c);
	InsetRect (&r, 2, 2);
	PaintRect(&r);
	RGBForeColor(&old);

	PenNormal ();

}


ThermWin::ThermWin(Document *dc,ProcessingStates ps,long max,MS_WindowTypes wint,int winn)
{

		Rect 		r;
		Handle 		h;
		short 		itemType;
		short 		wi,hi;
		short		IDS[] = THERM_IDs;
		int 		i;
		Str255		theString;
		short 		text_ids[] = PROCESS_STATE_IDS;
		
	doc = dc;	
		
	priority = 25;

	WinType = wint;
	WinNum = winn;

	last_k = 0;
	current_k = 0;
	max_k = max;
	error = MCO_SUCCESS;
	finished = 1;	// have not started it yet
	quit = 0;
	pstate = ps;
	lastpstate = ps;
	
	thermd = new ThermObject(&current_k,&max_k,&finished,&quit,&error,&pstate);
	

	quit = FALSE;

	gdata = 0L;

	for (i=0; i<NUM_THWIN_IDS; i++) ids[i] = IDS[i];

	setDialog(ids[THWIN_DLG]);
			
	GetDItem (dp,ids[THWIN_TEXT], &itemType, &h, &r);
	GetIndString(theString,THERM_MESS,text_ids[(int)pstate]);
	SetIText(h,theString);

	SetPort(dp);
	
	ShowWindow(dp);
	DrawDialog(dp);
	
	DrawWindow();
	
}


ThermWin::~ThermWin(void)
{
// get rid of the thread
KillThread();
// now safe to get rid of this
if (thermd) delete thermd;

}

// display the thermometer using the colored pict
Boolean ThermWin::DisplayTherm(void)
{

	Rect ar,r;
	long	maxr;
	Handle h;
	short itemType;
	long 	temp;
	GrafPtr	tdp;
	Str255	theString;

	PicHandle	thePic;
	short 		text_ids[] = PROCESS_STATE_IDS;
	RGBColor  c,old;
	GWorldPtr	BigGW;
	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	PixMapHandle myPixMapHandle;

GetPort(&tdp);
SetPort(dp);

GetDItem (dp, ids[THWIN_ALL], &itemType, &h, &ar);

error = NewGWorld( &BigGW, 32, &ar, 0, 0, 0 );
if (error == 0)
	{

	GetGWorld(&oldGW,&oldGD);
	SetGWorld(BigGW,nil);
	myPixMapHandle = GetGWorldPixMap( BigGW );	/* 7.0 only */
	LockPixels(myPixMapHandle);
	
	EraseRect(&ar);
	
	DrawNormalSur(dp,ids[THWIN_BOX]);
	GetDItem (dp, ids[THWIN_BOX], &itemType, &h, &r);

	GetForeColor(&old);
	c.red = 32000;
	c.green = 32000;
	c.blue = 32000;
	RGBForeColor(&c);
	PaintRect(&r);
	RGBForeColor(&old);

	maxr = r.right;
	temp = ((long)current_k*((long)r.right - (long)r.left))/(long)max_k;
	r.right = r.left + temp;
	if (r.right>maxr) r.right=maxr;

	thePic = GetPicture(Therm_PICT);
	DrawPicture(thePic,&r);
	ReleaseResource((Handle)thePic);


	
	SetGWorld(oldGW,oldGD);	
	CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &ar, &ar, srcCopy, 0 );
	UnlockPixels(myPixMapHandle);
	DisposeGWorld( BigGW );
	
	}

SetPort(tdp); 

}

// always do a yeild to a thread
// may not be the windows thread of course
McoStatus ThermWin::updateWindowData(int changed)
{
OSErr err;

err = YieldToAnyThread();
if (err != 0) return MCO_FAILURE;
return MCO_SUCCESS;
}


// check to see if a timer event has occured
// at timer event check to see if thermometer needs to be updated, 
// an error has occured, or if the thread is finished
Boolean ThermWin::TimerEventPresent(void)
{
if( TickCount() - _last_tick_time2 > 10 ) 
	{
	return TRUE;
	}
return FALSE;
}

// draw the window
McoStatus ThermWin::DrawWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldP;
	
	GetPort(&oldP);

	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	DisplayTherm();
	SetPort(oldP);
	return MCO_SUCCESS;
}	

// update the window
McoStatus ThermWin::UpdateWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldP;
	
	GetPort(&oldP);
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	DisplayTherm();
	DrawDialog( dp );
	EndUpdate( dp );
	
	SetPort(oldP);
	return MCO_SUCCESS;
}			

// get rid of the window's thread
McoStatus 	ThermWin::KillThread(void)
{
long count = 0;
OSErr err;

// first try to do it the easy way
while ((!finished) && (count < 500))
	{
	quit = 1;
	err = YieldToAnyThread();
	if (err != MCO_SUCCESS) return MCO_FAILURE;
	count ++;
	}
if (finished) return MCO_SUCCESS;

// will have to do it the hard way
err = DisposeThread(threadID,0,0);
if (err != MCO_SUCCESS) return MCO_FAILURE;
return MCO_SUCCESS;
}
	
// the thermometer event handler
// checks for timer events and cancel
McoStatus	ThermWin::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
	McoStatus	status = MCO_SUCCESS;
	short		iType;
	Handle		iHandle;
	Rect 		iRect;
	Str255		theString;
	short 		text_ids[] = PROCESS_STATE_IDS;

	// no codes passed back
	*numwc = 0;

	
	if (item == TIMER_ITEM ) // a timer event has occured
		{
		_last_tick_time2 = TickCount();
		if (current_k != last_k)
			{
			last_k = current_k;
			DisplayTherm();
			}
		if (lastpstate != pstate)
			{
			lastpstate = pstate;
			GetDItem (dp,ids[THWIN_TEXT], &iType, &iHandle, &iRect);
			GetIndString(theString,THERM_MESS,text_ids[(int)pstate]);
			SetIText(iHandle,theString);
			}
		if (finished)
			{
			switch (WinType) {
				case GammutSurfaceTherm1:
					*numwc = 2;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					wc[1].wintype = Gammut_Dialog1;
					wc[1].winnum = -1;
					wc[1].code = WE_Open_Window;
					wc[1].doc = doc;	
					*data = gdata;			
					break;
				case GammutSurfaceTherm_Src:
					*numwc = 2;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					wc[1].wintype = Gammut_Dialog_Src;
					wc[1].winnum = -1;
					wc[1].code = WE_Open_Window;
					wc[1].doc = doc;	
					*data = gdata;			
					break;
				case GammutSurfaceTherm_Dst:
					*numwc = 2;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					wc[1].wintype = Gammut_Dialog_Dst;
					wc[1].winnum = -1;
					wc[1].code = WE_Open_Window;
					wc[1].doc = doc;	
					*data = gdata;			
					break;					
				case ProcessTherm:
					*numwc = 1;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					break;
				case ProcessThermClose:
					*numwc = 2;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					wc[1].wintype = No_Dialog;
					wc[1].winnum = 0;
					wc[1].code = WE_CloseDoc;
					wc[1].doc = doc;
					break;
				case GammutCompTherm:
#ifndef	IN_POLAROID
					*numwc = 2;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
					wc[1].wintype = SelAdjWindow;
					wc[1].winnum = -1;
					wc[1].code = WE_Open_Window;
					wc[1].doc = doc;
#else
					*numwc = 1;
					wc[0].wintype = WinType;
					wc[0].winnum = WinNum;
					wc[0].code = WE_Close_Window;
					wc[0].doc = doc;
#endif
					break;
				status = error;	
				}		
			}
		if (error != MCO_SUCCESS)
			{
			status = error;
			*numwc = 1;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].code = WE_Close_Window;
			wc[0].doc = doc;
			// get rid of the thread
			KillThread();
			if (gdata) delete gdata;
			}
		}
	if (item == ids[THWIN_CANCEL])
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_Close_Window;
		wc[0].doc = doc;
		// get rid of the thread
		KillThread();
		if (gdata) delete gdata;
		}
		
	return status;
}
