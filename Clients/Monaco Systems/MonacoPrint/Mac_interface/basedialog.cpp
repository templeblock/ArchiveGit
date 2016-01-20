#include "basedialog.h"

extern long *_WindowPtrs;
extern long _NumWindows;

extern UserItemUPP  myOK;
extern UserItemUPP  my3D;


short BaseDialog::dialogCounter = 0;			// the window counter that 0 uses to 
											// stack its windows

Rect BaseDialog::screensize = {0, 0, 0, 0};

#if USESROUTINEDESCRIPTORS
RoutineDescriptor BaseDialog::_rd = {0,0,0,0,0,0,0, {0,0,0,0,0,0,0}};
#endif
#if USESROUTINEDESCRIPTORS
RoutineDescriptor BaseDialog::_sd = {0,0,0,0,0,0,0, {0,0,0,0,0,0,0}};
#endif



//find screensize
void BaseDialog::findScreenSize(void)
{
	Point	p;
	GDHandle	curDevice = GetDeviceList();

	BaseDialog::screensize = qd.screenBits.bounds;
	GetMouse(&p);

	while(curDevice){
	if (TestDeviceAttribute(curDevice,screenDevice) &&
			TestDeviceAttribute(curDevice,screenActive) &&
			PtInRect(p,&(*curDevice)->gdRect))
			BaseDialog::screensize = (*curDevice)->gdRect;
			curDevice = GetNextDevice(curDevice);
	}
	
	BaseDialog::screensize.top += GetMBarHeight();
}

pascal void frame_item(DialogPtr dp, short item);
pascal void DrawNormalSur(DialogPtr dp, short item);

void BaseDialog::createDescriptor(void)
{
#if USESROUTINEDESCRIPTORS
	_rd.goMixedModeTrap = _MixedModeMagic;
	_rd.version = kRoutineDescriptorVersion;
	_rd.routineDescriptorFlags = kSelectorsAreNotIndexable;
	_rd.reserved1 = 0;
	_rd.reserved2 = 0;
	_rd.selectorInfo = 0;
	_rd.routineCount = 0;
	_rd.reserved1 = 0;
	_rd.routineRecords[0].procInfo = uppUserItemProcInfo;
	_rd.routineRecords[0].reserved1 = 0;
	_rd.routineRecords[0].ISA = kPowerPCISA;
	_rd.routineRecords[0].routineFlags = kProcDescriptorIsAbsolute|kFragmentIsPrepared|kUseNativeISA;
	_rd.routineRecords[0].procDescriptor = (ProcPtr)frame_item;
	_rd.routineRecords[0].reserved2 = 0;
	_rd.routineRecords[0].selector = 0;

// added by James to support 3d box effect

	_sd.goMixedModeTrap = _MixedModeMagic;
	_sd.version = kRoutineDescriptorVersion;
	_sd.routineDescriptorFlags = kSelectorsAreNotIndexable;
	_sd.reserved1 = 0;
	_sd.reserved2 = 0;
	_sd.selectorInfo = 0;
	_sd.routineCount = 0;
	_sd.reserved1 = 0;
	_sd.routineRecords[0].procInfo = uppUserItemProcInfo;
	_sd.routineRecords[0].reserved1 = 0;
	_sd.routineRecords[0].ISA = kPowerPCISA;
	_sd.routineRecords[0].routineFlags = kProcDescriptorIsAbsolute|kFragmentIsPrepared|kUseNativeISA;
	_sd.routineRecords[0].procDescriptor = (ProcPtr)DrawNormalSur;
	_sd.routineRecords[0].reserved2 = 0;
	_sd.routineRecords[0].selector = 0;

#endif
}

BaseDialog::BaseDialog(void)
{
dialogptr = NULL;
}


BaseDialog::~BaseDialog(void)
{

	int i,j;

/*	for (i=0; i<_NumWindows; i++) if (_WindowPtrs[i] == GetWRefCon(dialogptr)) break;
	_NumWindows--;
	for (j=i; j<_NumWindows; i++) _WindowPtrs[j] = _WindowPtrs[j+1]; */

	if (dialogptr != NULL) CloseDialog(dialogptr);
/*
	if( dialogptr != NULL ){
		DisposDialog(dialogptr);
		dialogptr = NULL;
	}
	if( dialoghandle != NULL ){
		HPurge ((Handle) dialoghandle);
		ReleaseResource((Handle)dialoghandle);
		dialoghandle = NULL;
	}
*/
}

/*
McoStatus BaseDialog::setDialog(int32 dialogid)
{
	Rect r = BaseDialog::screensize;
	Rect	rect;
	
	dialoghandle = (DialogTHndl)GetResource('DLOG',dialogid);
	if( dialoghandle == NULL )
		return MCO_FAILURE;
	
	HNoPurge ((Handle)dialoghandle);
	rect = (**dialoghandle).boundsRect;
	
	// exactly centered 
	OffsetRect(&rect, (r.right + r.left)/2 - (rect.right +
					rect.left)/2, (r.top + r.bottom)/2 -
					(rect.top + rect.bottom)/2);

	// 1/2 empty space above -> 1/3 empty space above 
	OffsetRect(&rect,0,-(rect.top - r.top)/3);

	(**dialoghandle).boundsRect = rect;
	
	dialogptr = GetNewDialog(dialogid, NULL, (WindowPtr) -1);
	if( dialogptr == NULL)
		return MCO_FAILURE;
		
	SetWRefCon(dialogptr, (long)this);
	
	return MCO_SUCCESS;

}	
*/


McoStatus BaseDialog::setDialog(int32 dialogid)
{
	Rect r = BaseDialog::screensize;
	Rect	rect;
	long	it;
	short wi,hi;
	
	dialoghandle = (DialogTHndl)GetResource('DLOG',dialogid);
	if( dialoghandle == NULL )
		return MCO_FAILURE;
	
	HLock ((Handle)dialoghandle);
	rect = (**dialoghandle).boundsRect;
	
	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;
	
	#define menuHeight 20
	
	// exactly centered 
	OffsetRect (&rect, -rect.left, -rect.top);
	OffsetRect (&rect, (wi - rect.right) / 2,
					(hi - rect.bottom - menuHeight) / 3 + menuHeight);

	(**dialoghandle).boundsRect = rect;
//	HUnlock ((Handle)dialoghandle);

/*
	ChangedResource( (Handle)dialoghandle );	// tag as changed
	if (ResError() == noErr ) {	// always check this!
		WriteResource( (Handle)dialoghandle );	// record changes to disk 
	}

	ReleaseResource((Handle)dialoghandle);
	dialoghandle = NULL;
*/	
	dialogptr = GetNewDialog(dialogid, &dialogrec, (WindowPtr) -1);
	ReleaseResource((Handle)dialoghandle);
	dialoghandle = NULL;

	if( dialogptr == NULL)
		return MCO_FAILURE;
		
	SetWRefCon(dialogptr, (long)this);

// Add this window to list of windows

	//it = (long)this;
	//_WindowPtrs[_NumWindows] = it;
	//_NumWindows++;
	
	return MCO_SUCCESS;
}	

short BaseDialog::getDialogNumber(void)
{
	return	dialogCounter;
}

long BaseDialog::getDialogKind(void)
{
	return dialogkind;
}

void BaseDialog::setDialogKind(long kind)
{
	dialogkind = kind;
}
		
		
void BaseDialog::close(void)
{
	delete this;
}	


/****
 * GeBaseDialogRect
 *
 *		Return the windows rectangle.
 *		This method is provided so subclasses don't have
 *		to go poking into the instance variables.
 *
 ****/

void BaseDialog::getDialogRect (Rect *theRect, Boolean entireWindow)
{
	*theRect = dialogptr->portRect;
	if (!entireWindow) {
		theRect->top += 1;
		theRect->left += 1;
		theRect->right -= 16;
		theRect->bottom -= 16;
	}
}


void BaseDialog::frame_button(void)
{
	Handle			item_handle;
	short			item_type;
	Rect			item_rect;

	GetDItem (dialogptr, 2, &item_type, &item_handle			, &item_rect);


	SetDItem (dialogptr, 2,  item_type, (Handle)myOK, &item_rect);

}


// Added By James for 3D box

void BaseDialog::threeD_box(int item)
{
	Handle			item_handle;
	short			item_type;
	Rect			item_rect;

	GetDItem (dialogptr, item, &item_type, &item_handle			, &item_rect);


	SetDItem (dialogptr, item,  item_type, (Handle) my3D, &item_rect);

}





