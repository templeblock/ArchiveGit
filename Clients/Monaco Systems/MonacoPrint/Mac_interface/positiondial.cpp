/* a simple dialog that lets the user posiotin the table */

#include "mcostat.h"
#include "tcr.h"
#include "positiondial.h"
#include "basewin.h"

#define PositionID 	2014
#define PS_HookItem 3
#define PS_OK		1
#define PS_Cancel	2
#define PS_Number	4
#define PS_Up		5
#define PS_Down		6
#define PS_Left		7
#define PS_Right	8
#define PS_Pict		11

#define NumbersText	2003

#define FirstPict 	4000

extern UserItemUPP  myOK;

pascal Boolean MyDlgFilter(DialogPtr, EventRecord *, short *);

pascal Boolean MyDlgFilter(DialogPtr theDlg,EventRecord *theEvent,
	short *itemHit )
{
	if ( theEvent->what != keyDown )	// just looking for keystrokes
		return(FALSE);

	switch ( (theEvent->message) & charCodeMask ) {
		case 0x0d:	// Return pressed or ...
		case 0x03:	// ... Enter pressed
			*itemHit = PS_OK;
			return( TRUE );	// Note: pascal-style TRUE
		case 0x1b:
			*itemHit = PS_Cancel;	// Esc pressed
			return( TRUE );
		case 0x1C:
			*itemHit = PS_Left;
			return(TRUE);
		case 0x1D:
			*itemHit = PS_Right;
			return(TRUE);
		case 0x1E:
			*itemHit = PS_Up;
			return(TRUE);
		case 0x1F:
			*itemHit = PS_Down;
			return(TRUE);
		default:
			return( FALSE );	// all others
	}
}

McoStatus PositionTable(Table *tcom,int32 *position,int num,int set)
{
	Rect r;
	Str255 theString,numSt;
	short item;
	DialogPtr dp;
	short itemType;
	DialogTHndl dt;
	Handle cn1,h;
	short wi,hi;
	double	m;
	PicHandle	pict;
	UserItemUPP	myFilter;
	
	tcom->turnOffline();

	dt = (DialogTHndl) GetResource ('DLOG', PositionID);
	if (dt==0L) return MCO_RSRC_ERROR;
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
	
	dp = GetNewDialog (PositionID, nil, (WindowPtr) -1);
	if (dp==0L) return MCO_RSRC_ERROR;
	GetDItem (dp, PS_HookItem, &itemType, &h				  , &r);
	SetDItem (dp, PS_HookItem,  itemType, (Handle) myOK, &r);
	
	GetDItem (dp,PS_Number, &itemType, &cn1, &r);
	GetIndString(theString,NumbersText,num);
	SetIText(cn1,theString);
	

	

//	SysBeep(1);
	ShowWindow(dp);
	
	SetPort(dp);
	// draw the picture
	pict = GetPicture(FirstPict+num-1+set*3);
	if (pict != 0L) 
		{
		GetDItem (dp,PS_Pict, &itemType, &cn1, &r);
		DrawPicture(pict,&r);
		ReleaseResource((Handle)pict);
		}
	myFilter = NewUserItemProc(MyDlgFilter);
	do 
		{
		ModalDialog (0L, &item);
		if (isPressed(0x38)) m = 10;
		else m = 1;
		switch (item)
			{
			case PS_Up:
				tcom->moveDistance(0,m*0.5);
				break;
			case PS_Down:
				tcom->moveDistance(0,-m*0.5);
				break;
			case PS_Left:
				tcom->moveDistance(-m*0.5,0);
				break;
			case PS_Right:
				tcom->moveDistance(m*0.5,0);
				break;
			}
		
		} while ((item != PS_OK) && (item != PS_Cancel));
		
	tcom->turnOnline();	
		
	if (item == PS_OK) tcom->getLocation(position,1);
	DisposDialog (dp);
	HPurge ((Handle) dt);
	DisposeRoutineDescriptor(myFilter);
	ReleaseResource((Handle)dt);
	if (item == PS_OK) return MCO_SUCCESS;
	if (item == PS_Cancel) return MCO_CANCEL;
	return MCO_FAILURE;
}





