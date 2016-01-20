/******************************************************************/
/* basewin.c													  */
/* The base window definition									  */
/* Has been made compatiable with the original dialog code		  */
/* so names are as if this is a dialog							  */
/* May 5, 1995													  */
/******************************************************************/

#include <math.h>
#include "basewin.h"
#include "pascal_string.h"
#include "think_bugs.h"
#include "monacoprint.h"
#include <QDOffscreen.h>

//extern QDGlobals			*qdPtr;	//we'll use this to stuff the apps A5 into
extern UserItemUPP  myOK;
extern UserItemUPP  my3D;

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

short isPressed(unsigned short k )
// k =  any keyboard scan code, 0-127
{
	unsigned char km[16];

	GetKeys( (unsigned long *) km);
	return ( ( km[k>>3] >> (k & 7) ) & 1);
}


baseWin::~baseWin()
{
if (dp) CloseDialog(dp);
}

McoStatus 	baseWin::MoveToFront(Boolean select)
{
if (select) SelectWindow(dp);
return MCO_SUCCESS;
}


McoStatus 	baseWin::MoveWindow(Point where)
{
DragWindow(dp, where, &qd.screenBits.bounds);
return MCO_SUCCESS;
}


int32 baseWin::GetSlider(Point clickPoint,short item)
{
	int32			current_value;
	int32			value_min;
	int32			value_max;
	int32			inc;
	short 			itemType;
	Handle 			cn1;
	McoStatus		state;
	Rect			wr;
	ControlHandle	sliderControl;
	short			part;

	//GlobalToLocal(&clickPoint);

	part = FindControl(clickPoint,dp,(ControlHandle*)&sliderControl);
	GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r ); // Read control
	current_value = GetCtlValue ( (ControlHandle)sliderControl);   // set the current patch
	value_min = GetCtlMin((ControlHandle)sliderControl);  
	value_max = GetCtlMax((ControlHandle)sliderControl); 
	inc = (value_max - value_min)/20;
	if (inc < 1) inc = 1;
	switch (part) {
		case inDownButton: 
			current_value++;
			if (current_value > value_max) current_value = value_max;
			break;
		case inUpButton:
			current_value--;
			if (current_value < value_min) current_value = value_min;
			break;
		case inPageDown:
			current_value += inc;
			if (current_value > value_max) current_value = value_max;
			break;
		case inPageUp:
			current_value -= inc;
			if (current_value < value_min) current_value = value_min;
			break;
		}
	SetCtlValue ( (ControlHandle)sliderControl,current_value);
return current_value;
}

// set the monaco   slider to the specified picts and offsets
McoStatus baseWin::setSlider(int16 *picts,int16 *offsets, int16 slider_id,RefConType	*theRefCon)
{
	short 		itemType,itemtype;
	ControlHandle	sliderControl;

	if (picts == NULL) return MCO_OBJECT_NOT_INITIALIZED;
	if (offsets == NULL) return MCO_OBJECT_NOT_INITIALIZED;
	
	theRefCon->PICTIDs[Disable_PICT] = picts[0];
	theRefCon->PICTIDs[Enable_Unpress_PICT] = picts[1];
	theRefCon->PICTIDs[Enable_Press_PICT] = picts[2];
	theRefCon->PICTIDs[Disable_Slide_PICT] = picts[3];
	theRefCon->PICTIDs[Enable_Unpress_Slide_PICT] = picts[4];
	theRefCon->PICTIDs[Enable_Press_Slide_PICT] = picts[5];
	theRefCon->h_d = offsets[0];
	theRefCon->h_i = offsets[1];
	theRefCon->v_d = offsets[2];
	theRefCon->text_h = offsets[3];
	theRefCon->text_v = offsets[4];
	theRefCon->text_min_h = offsets[5];
	theRefCon->text_min_v = offsets[6];
	theRefCon->text_max_h = offsets[7];
	theRefCon->text_max_v = offsets[8];
		
	theRefCon->dragProc = (ProcPtr)nil;
	theRefCon->dragProcRefCon = 0; 
	
	SetRect(&theRefCon->leftRect,offsets[9],offsets[10],offsets[11],offsets[12]);
	SetRect(&theRefCon->rightRect,offsets[13],offsets[14],offsets[15],offsets[16]);
	
	theRefCon->leftVal = offsets[17];
	theRefCon->rightVal = offsets[18];
	
	theRefCon->textID = offsets[19];
	// set the slider

	GetDItem ( dp, slider_id, &itemtype, (Handle*)&sliderControl, &r );
	SetCRefCon ( sliderControl, (long)theRefCon );

return MCO_SUCCESS;
} 



McoStatus baseWin::DrawColorBox(short item,double *rgb)
{
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	GWorldPtr	BigGW;
	Point		offset;


	PixMapHandle myPixMapHandle;

	Rect		bounds;
	Rect		winRect2;

	QDErr		error;
	GrafPtr			oldGraf;
	RGBColor	tc,c,oldbackcolor;
	Rect 		drawRect;

	Rect r;
	Handle h;
	short itemType;


	GetDItem (dp,item, &itemType, &h, &r);
	
	#ifdef IN_POLAROID
	InsetRect (&r, 3, 3);
	#endif

	//FrameRect(&r);
	//InsetRect (&r, 2, 2);

	GetPort(&oldGraf);
	SetPort(dp);

	GetForeColor(&tc);
	GetBackColor(&oldbackcolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);
	
	error = NewGWorld( &BigGW, 32, &r, 0, 0, 0 );
	if (error == 0)
	{
		GetGWorld(&oldGW,&oldGD);
		SetGWorld(BigGW,nil);
		myPixMapHandle = GetGWorldPixMap( BigGW );	/* 7.0 only */
		LockPixels(myPixMapHandle);


		c.red=(int)rgb[0]<<8;
		c.green=(int)rgb[1]<<8;
		c.blue=(int)rgb[2]<<8;

		FrameRect(&r);
		RGBForeColor(&c);
		PaintRect(&r);
		c.red = 0;
		c.green = 0;
		c.blue = 0;
		RGBForeColor(&c);
		FrameRect(&r);
		
		
		
		// return to old gworld
		SetGWorld(oldGW,oldGD);	
		
		CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &r, &r, srcCopy + ditherCopy, 0 );
		UnlockPixels(myPixMapHandle);
		DisposeGWorld( BigGW );
	}
	RGBForeColor(&tc);
	RGBBackColor(&oldbackcolor);
return MCO_SUCCESS;


/*




	Rect r;
	Handle h;
	short itemType;
	RGBColor	c;
	RGBColor	oldbackcolor;
	RGBColor	tc;
	
	GrafPtr			oldGraf;

	GetPort(&oldGraf);
	SetPort(dp);
	
GetBackColor(&oldbackcolor);
c.red = 65535;
c.green = 65535;
c.blue = 65535;
RGBBackColor(&c);	
	
	
GetForeColor(&tc);	

GetDItem (dp,item, &itemType, &h, &r);
InsetRect (&r, -2, -2);

FrameRect(&r);
InsetRect (&r, 1, 1);

c.red=(int)rgb[0]<<8;
c.green=(int)rgb[1]<<8;
c.blue=(int)rgb[2]<<8;

RGBForeColor(&c);
PaintRect(&r);
RGBForeColor(&tc);
RGBBackColor(&oldbackcolor);
return MCO_SUCCESS; */
}


McoStatus baseWin::setDialog(int32 dialogid)
{
	Rect	r;
	long	it;
	short wi,hi;
	
	dt = (DialogTHndl)GetResource('DLOG',dialogid);
	if( dt == NULL )
		return MCO_FAILURE;
	
	HLock ((Handle)dt);
	r = (**dt).boundsRect;
	
	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;
	
	#define menuHeight 20
	
	// exactly centered 
	OffsetRect (&r, -r.left, -r.top);
	OffsetRect (&r, (wi - r.right) / 2,
					(hi - r.bottom - menuHeight) / 3 + menuHeight);
	(**dt).boundsRect = r;

	#undef menuHeight
	
	dp = GetNewDialog(dialogid, nil, (WindowPtr) -1);
	ReleaseResource((Handle)dt);
	dt = NULL;

	if( dp == NULL)
		return MCO_FAILURE;

	
	return MCO_SUCCESS;
}	

McoStatus baseWin::setDialog(int32 dialogid,int h, int v)
{
	Rect	r;
	long	it;
	short wi,hi;
	int		h_off,v_off;
	
	dt = (DialogTHndl)GetResource('DLOG',dialogid);
	if( dt == NULL )
		return MCO_FAILURE;
	
	HLock ((Handle)dt);
	r = (**dt).boundsRect;
	
	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;
	
	#define menuHeight 20
	
	// exactly centered 
	OffsetRect (&r, -r.left, -r.top);
	h_off = h+(wi - r.right) / 2;
	v_off = v+(hi - r.bottom - menuHeight) / 3 + menuHeight;
	
	if (h_off > wi) h_off = wi -10;
	if (h_off < 0) h_off = 0;
	
	if (v_off > hi) v_off = hi-10;
	if (v_off < menuHeight) v_off = 2*menuHeight;
	
	OffsetRect (&r, h_off,v_off);
	(**dt).boundsRect = r;

	#undef menuHeight
	
	dp = GetNewDialog(dialogid, nil, (WindowPtr) -1);
	ReleaseResource((Handle)dt);
	dt = NULL;

	if( dp == NULL)
		return MCO_FAILURE;

	
	return MCO_SUCCESS;
}	



McoStatus baseWin::UpdateWindow(void)	
{			
	Rect updateRect;
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	DrawDialog( dp );
	EndUpdate( dp );
	
	return MCO_SUCCESS;
}		

pascal void frame_item(DialogPtr dp, short item);

pascal void frame_item(DialogPtr dp, short item)
{
	Rect r;
	Handle h;
	short itemType;
	GrafPtr	saveport;

	GetPort(&saveport);
	SetPort(dp);
	GetDItem (dp, item, &itemType, &h, &r);

	PenNormal ();
	PenSize (3, 3);
	
	InsetRect (&r, -4, -4);
	FrameRoundRect (&r, 16, 16);

	PenNormal ();
	SetPort(saveport);
}


void box_item(DialogPtr dp, short item)
{
	Rect r;
	Handle h;
	short itemType;
	GrafPtr	saveport;

	GetPort(&saveport);
	SetPort(dp);
	GetDItem (dp, item, &itemType, &h, &r);

	PenNormal ();
	PenSize (2, 2);
	
	#ifndef IN_POLAROID
	InsetRect (&r, -3, -3);
	#endif
	FrameRect (&r);

	PenNormal ();
	SetPort(saveport);
}


void unbox_item(DialogPtr dp, short item)
{
	Rect r;
	Handle h;
	short itemType;
	GrafPtr	saveport;
	RGBColor back,temp;
	
	GetBackColor(&back);
	GetForeColor(&temp);
	RGBForeColor(&back);

	GetPort(&saveport);
	SetPort(dp);
	GetDItem (dp, item, &itemType, &h, &r);

	PenNormal ();
	PenSize (2, 2);
	
	#ifndef IN_POLAROID
	InsetRect (&r, -3, -3);
	#endif
	FrameRect (&r);

	PenNormal ();
	SetPort(saveport);
	
	RGBForeColor(&temp);
}



void Disable(DialogPtr inDialog, short inItemNum)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	
	if (inItemNum <0) return;
	
	GetDItem(inDialog,inItemNum,&iType,&iHandle,&iRect);
	iType |= itemDisable;
	SetDItem(inDialog,inItemNum,iType,iHandle,&iRect);
	
	if (iType & ctrlItem) {
		HiliteControl((ControlHandle) iHandle,255);
	}
}


void Enable(DialogPtr inDialog, short inItemNum)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	

	if (inItemNum <0) return;
	
	GetDItem(inDialog,inItemNum,&iType,&iHandle,&iRect);
	iType &= ~itemDisable;
	SetDItem(inDialog,inItemNum,iType,iHandle,&iRect);
	
	if (iType & ctrlItem) {
		HiliteControl((ControlHandle) iHandle,0);
	}
}




pascal void DrawNormalSur(DialogPtr dp, short item);

// Draw a 3D like border around a rectangle
pascal void DrawNormalSur(DialogPtr dp, short item)
{
RGBColor	c;
Rect 		drawRect;
Handle h;
short itemType;


GetDItem (dp, item, &itemType, &h, &drawRect);

InsetRect (&drawRect, -3, -3);

drawRect.bottom--;
drawRect.right--;

c.red = 0;
c.green = 0;
c.blue = 0;
RGBForeColor(&c);	
// draw a black rounded box around the button
MoveTo(drawRect.left+1,drawRect.top);
LineTo(drawRect.right-1,drawRect.top);

MoveTo(drawRect.left+1,drawRect.bottom);
LineTo(drawRect.right-1,drawRect.bottom);

MoveTo(drawRect.left,drawRect.top+1);
LineTo(drawRect.left,drawRect.bottom-1);

MoveTo(drawRect.right,drawRect.top+1);
LineTo(drawRect.right,drawRect.bottom-1);

c.red = 56576;
c.green = 56576;
c.blue = 56576;
RGBForeColor(&c);	

MoveTo(drawRect.left+2,drawRect.top + 1);
LineTo(drawRect.right-2,drawRect.top + 1);

MoveTo(drawRect.left+1,drawRect.top + 2);
LineTo(drawRect.left+1,drawRect.bottom - 2);

c.red = 30464;
c.green = 30464;
c.blue = 30464;
RGBForeColor(&c);	

MoveTo(drawRect.left+2,drawRect.bottom - 1);
LineTo(drawRect.right-2,drawRect.bottom - 1);

MoveTo(drawRect.right-1,drawRect.top+2);
LineTo(drawRect.right-1,drawRect.bottom-2);

c.red = 65280;
c.green = 65280;
c.blue = 65280;
RGBForeColor(&c);

MoveTo(drawRect.left+3,drawRect.top + 2);
LineTo(drawRect.right-3,drawRect.top + 2);

MoveTo(drawRect.left+2,drawRect.top + 3);
LineTo(drawRect.left+2,drawRect.bottom - 3);

MoveTo(drawRect.left+1,drawRect.top + 1);
LineTo(drawRect.left+2,drawRect.top + 2);

c.red = 34816;
c.green = 34816;
c.blue = 34816;
RGBForeColor(&c);

MoveTo(drawRect.left+3,drawRect.bottom - 2);
LineTo(drawRect.right-3,drawRect.bottom - 2);

MoveTo(drawRect.right-2,drawRect.top+3);
LineTo(drawRect.right-2,drawRect.bottom-3);

c.red = 21760;
c.green = 21760;
c.blue = 21760;
RGBForeColor(&c);

MoveTo(drawRect.right-1,drawRect.bottom - 1);
LineTo(drawRect.right-2,drawRect.bottom - 2);
}








/****
 * GeBaseDialogRect
 *
 *		Return the windows rectangle.
 *		This method is provided so subclasses don't have
 *		to go poking into the instance variables.
 *
 ****/

void baseWin::getDialogRect (Rect *theRect, Boolean entireWindow)
{
	*theRect = dp->portRect;
	if (!entireWindow) {
		theRect->top += 1;
		theRect->left += 1;
		theRect->right -= 16;
		theRect->bottom -= 16;
	}
}


void baseWin::frame_button(int item)
{
	Handle			item_handle;
	short			item_type;
	Rect			item_rect;


	GetDItem (dp, item, &item_type, &item_handle			, &item_rect);

	SetDItem (dp, item,  item_type, (Handle)myOK, &item_rect);


}


// Added By James for 3D box

void baseWin::threeD_box(int item)
{
	Handle			item_handle;
	short			item_type;
	Rect			item_rect;


	GetDItem (dp, item, &item_type, &item_handle			, &item_rect);

	SetDItem (dp, item,  item_type, (Handle)my3D, &item_rect);

}


void baseWin::SetItemText(short item,unsigned char *st)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s2;
	
	if (item == -1) return;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,s2);
	if (!cmp_str(st,s2)) SetIText (h, st);

}

void baseWin::SetItemTextForce(short item,unsigned char *st)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s2;
	
	if (item == -1) return;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	SetIText (h, st);

}

void baseWin::GetItemText(short item,unsigned char *st)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s2;
	
	if (item == -1) return;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,st);

}

void baseWin::SetItemTextToNum(short item,long num)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s,s2;
	int32		temp_i;
	
	if (item == -1) return;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,s2);
	if ((cmp_str("\p-",s2)) && (num ==0)) return;
	if ((cmp_str("\p",s2)) && (num ==0)) return;
	NumToString (num, s);
	if (!cmp_str(s,s2)) SetIText (h, s);

}

void baseWin::SetItemTextToNum(short item,double num,int perc)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s,s2;
	int32		temp_i;
	
	if (item == -1) return;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,s2);
	if ((cmp_str("\p-",s2)) && (num ==0)) return;
	if ((cmp_str("\p",s2)) && (num ==0)) return;
	temp_i = (long)(num+0.5);
	DoubleToString (num, s,perc);
	if (!cmp_str(s,s2)) SetIText (h, s);

}

int baseWin::GetItemTextFromNum(short item,long *num,long min,long max)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s,s2;
	long		temp_i;
	int			diff = 0;
	
	if (item == -1) return 0;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,s2);
	if (cmp_str("\p-",s2)) 
		{
		*num = 0;
		return 0;
		}
	if (cmp_str("\p",s2))
		{
		return 0;
		} 		
	StringToNum (s2,&temp_i);
	temp_i = MaxVal(temp_i,min);
	temp_i = MinVal(temp_i,max);
	if (temp_i != *num) diff = 1;
	*num = temp_i;
	
	return diff;

}

int baseWin::GetItemTextFromNum(short item,double *num,double min,double max,int perc)
{
	short 		itemtype;
	Handle		h;
	Rect		r;
	Str255		s,s2;
	long		temp_i;
	double		temp_d;
	int			diff = 0;
	
	if (item == -1) return 0;
	
	GetDItem (dp,item, &itemtype, &h, &r);
	GetIText(h,s2);
	if (cmp_str("\p-",s2)) 
		{
		*num = 0;
		return 0;
		}
	if (cmp_str("\p",s2))
		{
		return 0;
		} 
			
	StringToDouble(s2,&temp_d);
	DoubleToString(*num,s,perc);
		

	if (!cmp_str(s,s2)) diff = 1;
	*num = temp_d;

	return diff;
}