/*      Thermometer.c
		
		Draw a dialog with a thermometer
		
		(c) Copyright 1993, Monaco Systems Inc.
		by James Vogh
		
*/

#include "Thermometer.h"


#define THERM_IDs_1	{16002, 1, 2, -1}
#define THERM_IDs_2	{16003, 1, 2, 3}
#define THERM_MESS 17008

#ifdef __POWERPC
//QDGlobals	qd;
#endif

//extern QDGlobals			*qdPtr;	//we'll use this to stuff the apps A5 into

void box_d_item(DialogPtr dp, short item)
{
	Rect r;
	Handle h;
	short itemType;

	GetDItem (dp, item, &itemType, &h, &r);

	PenNormal ();
	PenSize (2, 2);
	
	InsetRect (&r, -2, -2);
	FrameRect (&r);

	PenNormal ();

}


McoStatus ThermDialog::SetUpTherm(int therm_num)
{

		Rect 		r;
		Handle 		h;
		short 		itemType;
		short 		wi,hi;
		short		ids1[] = THERM_IDs_1;
		short		ids2[] = THERM_IDs_2;
		int 		i;
		Str255		theString;

	Quit = FALSE;

	//if (therm_num == 3) 
	for (i=0; i<NUM_THERM_IDS; i++) ids[i] = ids2[i];
	//else for (i=0; i<NUM_THERM_IDS; i++) ids[i] = ids1[i];

	GetPort(&olddp);

	dt = (DialogTHndl) GetResource ('DLOG',ids[THERM_DLG]);
	
	if (! dt) return(MCO_RSRC_ERROR);
	
	HNoPurge ((Handle) dt);

	r = (**dt).boundsRect;
	
	
	#define menuHeight 20

	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;

	OffsetRect (&r, -r.left, -r.top);
	OffsetRect (&r, (wi - r.right) / 2,
					(hi - r.bottom - menuHeight) / 3 + menuHeight);
	(**dt).boundsRect = r;

	#undef menuHeight
	

	dp = GetNewDialog (ids[THERM_DLG], nil, (WindowPtr) -1);


	
	if (! dp) 
		{
		HPurge ((Handle) dt);
		return(MCO_RSRC_ERROR);
		}
			
	GetDItem (dp,ids[THERM_TEXT], &itemType, &h, &r);
	GetIndString(theString,THERM_MESS,therm_num);
	SetIText(h,theString);

	SetPort(dp);
	
	ShowWindow(dp);
	DrawDialog(dp);
	
	box_d_item(dp,ids[THERM_BOX]); 
	return MCO_SUCCESS;
}

Boolean ThermDialog::DisplayTherm(int k,int maxk,unsigned char *st)
{

	Rect r;
	int	maxr;
	Handle h;
	short itemType;
	long 	temp;
	GrafPtr	tdp;
	WindowPtr   	window;
	short       	thePart;
	Rect        	screenRect, updateRect;
	Point			aPoint = {100, 100};
	Boolean 		QuitFlag = FALSE;
	short			item = 0;
	EventRecord 	event;
	short		the_part;
	DialogPtr 	hitdp;

GetPort(&tdp);
SetPort(dp);

	if (st != NULL)
		{
		GetDItem (dp,ids[THERM_TEXT], &itemType, &h, &r);
		SetIText(h,st);
		}


	GetDItem (dp, ids[THERM_BOX], &itemType, &h, &r);
	
	if (k == -1) k = old_k;
	if (maxk == -1) maxk = old_maxk;

	maxr = r.right;
	temp = ((long)k*((long)r.right - (long)r.left))/(long)maxk;
	r.right = r.left + temp;
	if (r.right>maxr) r.right=maxr;

	old_k = k;
	old_maxk = maxk;	

	PenNormal ();
	PenPat(&qd.gray);
	PaintRect(&r);

	PenNormal ();

SetPort(tdp); 

if (GetNextEvent(0xFFFF,&event))
	{
	if (event.what == mouseDown) 
		{
		the_part = FindWindow(event.where, &window);
		if (IsDialogEvent(&event)) 
			{
			DialogSelect(&event,&hitdp,&item);
			if ((hitdp == dp) && (item == ids[THERM_CANCEL])) 
				{
				Quit = TRUE;
				return FALSE;
				}
			}
		else switch (the_part) {
			case inSysWindow:
				SystemClick (&event, window);
				break;
			case inDrag:
				screenRect = (**GetGrayRgn()).rgnBBox;
				DragWindow( window, event.where, &screenRect );
				break;
			}
		}
	else if (event.what == keyDown)
		{
		if ((event.modifiers & 0x0100) && ( (event.message) & charCodeMask == 0x02E)) 
			{
			Quit = TRUE;
			return FALSE;
			}
		}
	else if (event.what == updateEvt)
		{
		window = (WindowPtr)event.message;
		updateRect = (**(window->visRgn)).rgnBBox;
		SetPort( window ) ;
		BeginUpdate( window );
		DrawDialog( window );
		if (dp == window) box_d_item(dp,ids[THERM_BOX]); 
		EndUpdate( window );
		}
	}
if (Quit) return FALSE;
return TRUE;
}

void  ThermDialog::RemoveTherm(void)
{

HideWindow(dp);
DisposDialog (dp);
HPurge ((Handle) dt);
ReleaseResource((Handle)dt);

SetPort(olddp);

}