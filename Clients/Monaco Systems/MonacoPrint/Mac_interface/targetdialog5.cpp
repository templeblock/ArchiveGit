#include <math.h>
#include "basedialog.h"
#include "targetdialog5.h"
#include "imagetrans.h"
//#include "fortestonly.h"
#include "printmain.h"
#include "csprofile.h"
#include "errordial.h"

#define CropDialog	(2010)

//#define ScannerText	(10)
#define ScannerText	(8)
//#define LoadScanner	(8)
#define LoadScanner	(6)
//#define Measure	(7)
//#define Crop		(6)
#define AutoCrop	(5)
#define Imagearea	(4)
#define ImageareaOpt	(1004)
#define ImageareaCmd	(2004)
#define Rtext		(10)
#define Gtext		(12)
#define Btext		(14)

#define Ltext		(20)
#define atext		(21)
#define btext		(22)

#define SaveCB		(15)
#define ShowGridCB	(16)


static	unsigned char	patterns[] = {
		0xF8, 0xF1, 0xE3, 0xC7, 0x8F, 
		0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 
		0xE3, 0xC7, 0x8F, 0x1F, 0x3E
	};


TargetDialog::TargetDialog(ProfileDoc *dc)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	
	//priority = 1;

	_preview = 0L;
	_off32GWorld = 0L;

	WinType = CropWindow;
	WinNum = 0;

	setDialog(CropDialog);
	frame_button(2);	

	GetDItem( dp, Imagearea, &iType, &iHandle, &_targetRect);
	
	_ants = 0;
	_marchtime = 2;
	_lastindex = 0;
	
	_inittargetwin();
	
	doc = (ProfileDocMac*)dc;
	
	Disable(dp,1);	
	
	_has_scan_table = 0;
	
//text stuff	
	_scan_name[0] = 0x00;
	
	GetDItem( dp, ScannerText, &iType, &iHandle, &iRect);
	SetIText(iHandle,_scan_name);
//end of text stuff	

//rgb stuff
	Str255	theString;
	
	_rgb[0] = 0;
	_rgb[1] = 0;
	_rgb[2] = 0;
	_last_rgb[0] = 0;
	_last_rgb[1] = 0;
	_last_rgb[2] = 0;
	
	GetDItem( dp, Rtext, &iType, &iHandle, &iRect);
	NumToString((long)_rgb[0],theString);
	SetIText(iHandle,theString);

	GetDItem( dp, Gtext, &iType, &iHandle, &iRect);
	NumToString((long)_rgb[1],theString);
	SetIText(iHandle,theString);
	
	GetDItem( dp, Btext, &iType, &iHandle, &iRect);
	NumToString((long)_rgb[2],theString);
	SetIText(iHandle,theString);
//end of rgb stuff

	_targetH = doc->targetBufferH;
	_tar_length = doc->length;
	_tar_width = doc->width;
	drawTarget();
	
	_need_save = 1;
	//GetDItem( dp, SaveCB, &iType, &iHandle, &iRect);
	//SetCtlValue((ControlHandle)(iHandle),_need_save);
	
	_show_grid = 0;
	GetDItem( dp, ShowGridCB, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)(iHandle),_show_grid);
	
	_setgrids();
	
	
}

TargetDialog::~TargetDialog(void)
{
if (_preview) delete _preview;
_preview = 0L;	
if (_off32GWorld) DisposeGWorld(_off32GWorld);
	_off32GWorld = 0L;
}

void TargetDialog::_inittargetwin(void)
{
	long i;

	//Disable(dp,1);	
	
	//not cropped yet
	_hascrop = 0;
	_center = 0;
	_rotang	= 0;
	
	for(i = 0; i < 4; i++){
		_h[i] = 0;
		_v[i] = 0;
		_dh[i] = 0;
		_dv[i] = 0;
	}
			
	_setmarkers();
}

//cursor functions
//depending on the area, change the cursor shap
McoStatus TargetDialog::doPointInWindowActiveRgn(Point mouse,WinEve_Cursors &cursortype)
{
	GrafPtr	savePort;
	GetPort(&savePort);
	SetPort(dp);

	GlobalToLocal(&mouse);

	_getRGB(&mouse);
	
	if(_isinMarker(&mouse)){
		cursortype = WC_SetBack;
		return MCO_SUCCESS;
	}
	if(_isinCrop(&mouse)){
		cursortype = WC_CropFinTemp;
		return MCO_SUCCESS;
	}
	if(_isinWindow(&mouse)){
		if(_hascrop)
			cursortype = WC_CropCancelTemp;
		else
			cursortype = WC_CropTemp;
		return MCO_SUCCESS;	
	}
	cursortype = WC_SetBack;	
	SetPort(savePort);
	
	return MCO_SUCCESS;
}

//mouse location functions

//is inside the target window ?
short	TargetDialog::_isinWindow(Point* mouse)
{
	if( (_targetRect.left > mouse->h) || (_targetRect.right < mouse->h) ||
		(_targetRect.top > mouse->v) || (_targetRect.bottom < mouse->v) )	//outside image area
		return (0);
	else
		return (1);	
}

//is inside the markers rectangle ?
short	TargetDialog::_isinMarker(Point* mouse)
{
	if( ((_mRect[0].left <= mouse->h) && (_mRect[0].right >= mouse->h) &&
		(_mRect[0].top <= mouse->v) && (_mRect[0].bottom >= mouse->v))	||
		((_mRect[1].left <= mouse->h) && (_mRect[1].right >= mouse->h) &&
		(_mRect[1].top <= mouse->v) && (_mRect[1].bottom >= mouse->v))	||
		((_mRect[2].left <= mouse->h) && (_mRect[2].right >= mouse->h) &&
		(_mRect[2].top <= mouse->v) && (_mRect[2].bottom >= mouse->v))	||
		((_mRect[3].left <= mouse->h) && (_mRect[3].right >= mouse->h) &&
		(_mRect[3].top <= mouse->v) && (_mRect[3].bottom >= mouse->v))	)
		return (1);
	else
		return (0);	
}

//is inside the crop window
short	TargetDialog::_isinCrop(Point* mouse)
{
	short	dh, dv;
	short	xh, xv;
	short 	yh, yv;
	long	x1, y1, x2, y2;
	
	dh = mouse->h - _dh[0];
	dv = mouse->v - _dv[0];
	xh = _dh[1] - _dh[0];
	xv = _dv[1] - _dv[0];
	yh = _dh[3] - _dh[0];
	yv = _dv[3] - _dv[0];
	
	x1 = dh*xh + dv*xv;
	y1 = dh*yh + dv*yv;
	

	dh = mouse->h - _dh[2];
	dv = mouse->v - _dv[2];
	xh = _dh[1] - _dh[2];
	xv = _dv[1] - _dv[2];
	yh = _dh[3] - _dh[2];
	yv = _dv[3] - _dv[2];
	
	x2 = dh*xh + dv*xv;
	y2 = dh*yh + dv*yv;
	
	if(x1 > 0 && y1 > 0 && x2 > 0 && y2 > 0)
		return (1);
		
	return (0);	
}	

short TargetDialog::_samepara(short h1, short v1, short h2, short v2,
								short h3, short v3, short h4, short v4,
								short i1, short w1, short i2, short w2,
								short i3, short w3, short i4, short w4) 
{
	if( h1 == i1 && v1 == w1 && h2 == i2 && v2 == w2 &&
		h3 == i3 && v3 == w3 && h4 == i4 && v4 == w4	)
		return (1);
	return (0);
}		


McoStatus	TargetDialog::_loadscanner(void)
{	
	McoStatus 	status;
	CsFormat 	*scannerf;
	CM2Header	_iheader;	//input profile header
	CMTagRecord tags[30];
	long 		_itagcount;
	long 		i, j, k;
	SFTypeList			filelist;
	McoColorSync	scan,mon,lab,*list[2];
	
	filelist[0] = 'prof';
	StandardGetFile(NULL, 1, filelist, &_sReply);
	UpdateWindow();
	if(!_sReply.sfGood)	return MCO_CANCEL;

	_has_scan_table = 0;
	
	status = scan.setFSSpec(&(_sReply.sfFile));
	if (status) return status;
	
	if (scan.getDataSpace() != cmRGB24Space) return MCO_FAILURE;
	
	if (!doc->scanner) delete doc->scanner;
	doc->scanner = new McoXformIccMac;
	if (!doc->scanner) return MCO_MEM_ALLOC_ERROR;
	
	lab.GetProfile(LAB_PROFILE_NAME);
		
	list[0] = &scan;
	list[1] = &lab;
	status = doc->scanner->buildCW(list,2);	
	if (status) return status;
	
	if (_preview) delete _preview;
	_preview = new McoXformIccMac;
	if (!_preview) return MCO_MEM_ALLOC_ERROR;
	
	mon.setType(Mco_ICC_System);
	
	list[0] = &scan;
	list[1] = &mon;
	status = _preview->buildCW(list,2);
	if (status) return status;

	_has_scan_table = 1;

	short	iType;
	Handle	iHandle;
	Rect	iRect;

	memcpy((char*)_scan_name, (char*)(_sReply.sfFile.name), 64);
	GetDItem( dp, ScannerText, &iType, &iHandle, &iRect);
	SetIText(iHandle,_scan_name);

	_draw();
	
	return status;
}



//get rgb value functions
void TargetDialog::_getRGB(Point* mouse)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	Str255	theString;
	unsigned char 	labc[3];
	double	lab[3];

	PixMapHandle PixMap32Handle;
	long 		*srcBaseAddr, *dstBaseAddr, srcRowLongs, dstRowLongs;
	char 		mmuMode;
	McoStatus	status;
	
	if (!_off32GWorld) return;

	if( (_targetRect.left <= mouse->h) && (_targetRect.right >= mouse->h) &&
		(_targetRect.top <= mouse->v) && (_targetRect.bottom >= mouse->v) ){	//inside image area
			
		PixMap32Handle = GetGWorldPixMap( _off32GWorld );
		//LockPixels( PixMap32Handle );

		dstBaseAddr = (long *) GetPixBaseAddr ( PixMap32Handle );	/* get the address of the pixmap */
		dstRowLongs = ((**PixMap32Handle).rowBytes & 0x7fff) >> 2;	/* get the _length increment */

	// 	copy image to the offscreen window	
		mmuMode = true32b;
		SwapMMUMode(&mmuMode);
		dstBaseAddr += dstRowLongs*(mouse->v - _targetRect.top);
		dstBaseAddr += (mouse->h - _targetRect.left);
		_rgb[0] = ((*dstBaseAddr << 8) >> 24);
		_rgb[1] = ((*dstBaseAddr << 16) >> 24);
		_rgb[2] = ((*dstBaseAddr << 24) >> 24);
		SwapMMUMode(&mmuMode);
	}
	
	if((_last_rgb[0] != _rgb[0]) && (_last_rgb[1] != _rgb[1]) && (_last_rgb[2] != _rgb[2])){
		_last_rgb[0] = _rgb[0];
		_last_rgb[1] = _rgb[1];
		_last_rgb[2] = _rgb[2];
	
		GetDItem( dp, Rtext, &iType, &iHandle, &iRect);
		NumToString((long)_rgb[0],theString);
		SetIText(iHandle,theString);

		GetDItem( dp, Gtext, &iType, &iHandle, &iRect);
		NumToString((long)_rgb[1],theString);
		SetIText(iHandle,theString);
		
		GetDItem( dp, Btext, &iType, &iHandle, &iRect);
		NumToString((long)_rgb[2],theString);
		SetIText(iHandle,theString);
		
		
		if(doc->scanner){
			status = doc->scanner->evalColors(_rgb,labc,1);
			if (status) return;	
			
			lab[0] = (double)labc[0]/2.55;
			lab[1] = labc[1] - 128;
			lab[2] = labc[2] - 128;

			GetDItem( dp, Ltext, &iType, &iHandle, &iRect);
			NumToString((long)lab[0],theString);
			SetIText(iHandle,theString);

			GetDItem( dp, atext, &iType, &iHandle, &iRect);
			NumToString((long)lab[1],theString);
			SetIText(iHandle,theString);
			
			GetDItem( dp, btext, &iType, &iHandle, &iRect);
			NumToString((long)lab[2],theString);
			SetIText(iHandle,theString);
		}
		
		BeginUpdate(dp);
		UpdateDialog(dp, dp->visRgn);
		EndUpdate(dp);				
	}	
}	

//need to change this
void TargetDialog::findgrids(Rect *selectrect)
{	
	double wscale, lscale, top, left;

	doc->targetBufferH = _targetH;
	doc->length = _tar_length;
	doc->width = _tar_width;

/*		
	wscale = (double)(_sRect.right - _sRect.left)/(double)(_targetRect.right - _targetRect.left);	
	lscale = (double)(_sRect.bottom - _sRect.top)/(double)(_targetRect.bottom - _targetRect.top);

	left = (double)(_sRect.left - _targetRect.left)/(double)(_targetRect.right - _targetRect.left);
	top = (double)(_sRect.top - _targetRect.top)/(double)(_targetRect.bottom - _targetRect.top); 

	selectrect->left = (short)_tar_width*left;
	selectrect->right = (short)(_tar_width*(wscale + left));
	selectrect->top = (short)_tar_length*top;
	selectrect->bottom = (short)(_tar_length*(lscale + top));	
*/			
}



Boolean TargetDialog::KeyFilter(EventRecord *theEvent,short *itemHit )
{


if (theEvent->modifiers & 0x0800) // The option key is being pressed
	{
	*itemHit += 2000;
	return( FALSE );
	}
if (theEvent->modifiers & 0x0100) // The command key is being pressed
	{
	*itemHit += 1000;
	return( FALSE );
	}
return( FALSE );	// all others
} 


McoStatus TargetDialog::DoEvents(short item, Point location, WindowCode *wc, int32 *numwc,void **data, Boolean &changed)
{
	//Point location;
	//short item;
	PenState penState;
	DialogPtr whichdiag;
	GrafPtr	savePort;
	short 	the_part;
	WindowPtr	which_window;
	McoStatus	state;
	Rect	selectrect;
	
	Changed = FALSE;
	*numwc = 0;
	state = MCO_SUCCESS;



	GetPort(&savePort);
	SetPort((GrafPtr)dp);	
	
	GlobalToLocal(&location);
	
	GetPenState(&penState);

	switch(item){
		case (1): //Ok
			//findgrids();
			*numwc = 1;
			wc[0].code = WE_Close_Window;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			findgrids(&selectrect);
			doc->aftercrop(&selectrect, doc->waitingPatches);
			break;
		
		case (3): //Cancel
			*numwc = 1;
			wc[0].code = WE_Close_Window;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			doc->aftercropCancel();
			break;
		
		case AutoCrop:
			//_autocrop();
			break;
		
		//case Crop:
		//	_setcrop();
		//	break;
			
		case Imagearea:
		case ImageareaOpt:
		case ImageareaCmd:
			_crop(item, location);
			break;
			
		case LoadScanner:
			state = _loadscanner();
			McoErrorAlert(state);
			if(state != MCO_SUCCESS)
				UpdateWindow();
			break;
			
		//case Measure:
		//	_measure();
		//	break;	
		
	//	case SaveCB:
	//		_handle_save();
	//		break;
			
		case ShowGridCB:
			_handle_show_grid();		
			break;
	}	
			

	SetPort(savePort);		
	return state;
}

void TargetDialog::_handle_show_grid(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;

	_show_grid = !_show_grid;
	GetDItem( dp, ShowGridCB, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)(iHandle),_show_grid);
	_drawgrids();	
}


void TargetDialog::_handle_save(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;

	//_need_save = !_need_save;
	//GetDItem( dp, SaveCB, &iType, &iHandle, &iRect);
	//SetCtlValue((ControlHandle)(iHandle),_need_save);
}

//null event
McoStatus	TargetDialog::updateWindowData(int changed)
{
	GrafPtr	savePort;
	GetPort(&savePort);
	SetPort((GrafPtr)dp);	

	//_changecursor();
	
//	if(_has_scan_table && _hascrop)
	if(_has_scan_table)
		Enable(dp,1);
	else
		Disable(dp,1);	

	if(_hascrop){
		_drawmarchingants();
	}	
		
	SetPort(savePort);	
		
	return MCO_SUCCESS;
}


McoStatus	TargetDialog::UpdateWindow(void)
{
	GrafPtr	savePort;
	long index;
	PenState	penState;	/* the saved pen state on entry to procedure */
		
	GetPort(&savePort);
	SetPort((GrafPtr)dp);	
	BeginUpdate( dp );
	_draw();
	_drawmarkers();
					
	GetPenState(&penState);
	PenMode(patXor);
	PenPat((PatPtr)&patterns[_lastindex]);
	_drawcroplines(_dh, _dv);
	SetPenState(&penState);
	UpdateDialog(dp, dp->visRgn);
	EndUpdate( dp );
	SetPort((GrafPtr)savePort);	
	
	return MCO_SUCCESS;
}


void	TargetDialog::_findMarker(Point* mouse)
{

	if((_mRect[0].left <= mouse->h) && (_mRect[0].right >= mouse->h) &&
		(_mRect[0].top <= mouse->v) && (_mRect[0].bottom >= mouse->v))
		_cmarker = 0;
	else if((_mRect[1].left <= mouse->h) && (_mRect[1].right >= mouse->h) &&
		(_mRect[1].top <= mouse->v) && (_mRect[1].bottom >= mouse->v))
		_cmarker = 1;
	else if((_mRect[2].left <= mouse->h) && (_mRect[2].right >= mouse->h) &&
		(_mRect[2].top <= mouse->v) && (_mRect[2].bottom >= mouse->v))
		_cmarker = 2;
	else if((_mRect[3].left <= mouse->h) && (_mRect[3].right >= mouse->h) &&
		(_mRect[3].top <= mouse->v) && (_mRect[3].bottom >= mouse->v))
		_cmarker = 3;
}

void	TargetDialog::_crop(short item, Point start)
{

	if(_hascrop == 0)
		_startcrop(start);
	else{
		//is mouse in marker window
		if( _isinMarker(&start) ){
			_findMarker(&start);
			//rotate if cmd key is pressed
			if(item ==  ImageareaCmd)
				_rotatebydrag(start);
			else if( item == ImageareaOpt)
				_movebydrag(start);
			else	
				_sizebydrag(start);
		}
		else if( _isinCrop(&start) ){
			_handlecrop(start);
		}
		else{ //outside the croprectangle but inside the target window
			_disablecrop(start);
		}
	}					
}

short TargetDialog::_handlecrop(Point start)
{
	McoStatus status;
	
	status = _cropimage();
	McoErrorAlert(status);
	if(status == MCO_SUCCESS)
		drawTarget();
	else
		UpdateWindow();	
}	


void TargetDialog::_disablecrop(Point start)
{
	_draw();
	_inittargetwin();
}	


//functions for set drawing markers
void TargetDialog::_setmarkers(void)
{
	long i;
	double	ch, cv;
	double	th, tv;
	
	if(_rotang == 0){
		for(i = 0; i < 4; i++){
			_mRect[i].left = (short)(_h[i] + 0.5) - 3;
			_mRect[i].top = (short)(_v[i] + 0.5) - 3;
			_mRect[i].right = (short)(_h[i] + 0.5) + 3;
			_mRect[i].bottom = (short)(_v[i] + 0.5) + 3;
		}
	}
	else{
		ch = (_h[0] + _h[2])/2.0;
		cv = (_v[0] + _v[2])/2.0;
		for(i = 0; i < 4; i++){
			th = _h[i];
			tv = _v[i];
			_rotatepoint(_rotang, ch, cv, &th, &tv);
			_mRect[i].left = (short)(th + 0.5) - 3;
			_mRect[i].top = (short)(tv + 0.5) - 3;
			_mRect[i].right = (short)(th + 0.5) + 3;
			_mRect[i].bottom = (short)(tv + 0.5) + 3;
		}	
	}	
	
	for(i = 0; i < 4; i++){
		if(_mRect[i].left < _targetRect.left)
			_mRect[i].left = _targetRect.left;
		if(_mRect[i].right > _targetRect.right)	
			_mRect[i].right = _targetRect.right;
		if(_mRect[i].top < _targetRect.top)	
			_mRect[i].top = _targetRect.top;
		if(_mRect[i].bottom > _targetRect.bottom)	
			_mRect[i].bottom = _targetRect.bottom;
	}				
}



//drawing functions

//refresh the target
void	TargetDialog::_refreshtarget(void)
{
	PixMapHandle PixMap32Handle;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	Rect 		desbound;
	RGBColor	c,oldbackcolor,oldforecolor;
	
	
// the size of the window
	desbound.left 	= _targetRect.left;
	desbound.right 	= _targetRect.right;
	desbound.top    = _targetRect.top;
	desbound.bottom = _targetRect.bottom;

//  from offscreen window to real window
	GetGWorld(&oldGW,&oldGD);
	SetGWorld((CGrafPtr)dp,nil);

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

	PixMap32Handle = GetGWorldPixMap( _off32GWorld );
	LockPixels( PixMap32Handle );
	CopyBits( (BitMap*)*PixMap32Handle, (BitMap*)&dp->portBits, &desbound, &desbound, srcCopy + ditherCopy, 0 );
	UnlockPixels(PixMap32Handle);
//	DisposeGWorld( _off32GWorld );

	RGBBackColor(&oldbackcolor);
	RGBForeColor(&oldforecolor);	

	SetGWorld(oldGW,oldGD);

//draw controls
	DrawControls((WindowPtr)dp);	
	ShowWindow((WindowPtr)dp);
	SelectWindow((WindowPtr)dp);	
	
	BeginUpdate(dp);
	UpdateDialog(dp, dp->visRgn);
	EndUpdate(dp);	
}

//redraw the target
void	TargetDialog::_draw(void)
{
	PixMapHandle PixMap32Handle,PixMapTempHandle;
	GDHandle	oldGD;
	GWorldPtr	oldGW,tempGWorld;
	Rect 		desbound;
	RGBColor	c,oldbackcolor,oldforecolor;
	
	if (!_off32GWorld) return;
	
// the size of the window
	desbound.left 	= _targetRect.left;
	desbound.right 	= _targetRect.right;
	desbound.top    = _targetRect.top;
	desbound.bottom = _targetRect.bottom;

//  from offscreen window to real window
	GetGWorld(&oldGW,&oldGD);
	SetGWorld((CGrafPtr)dp,nil);

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

	if( NewGWorld( &tempGWorld, 32, &desbound, 0, 0, 0 ) ){
		tempGWorld = NULL;
		return;
	}	
	
	PixMapTempHandle = GetGWorldPixMap( tempGWorld );
	PixMap32Handle = GetGWorldPixMap( _off32GWorld );
	LockPixels( PixMapTempHandle );
	LockPixels( PixMap32Handle );
	CopyBits( (BitMap*)*PixMap32Handle, (BitMap*)*PixMapTempHandle, &desbound, &desbound, srcCopy + ditherCopy, 0 );
	UnlockPixels(PixMap32Handle);

	if (_preview) _preview->evalPixMap(*PixMapTempHandle);	
	
	CopyBits( (BitMap*)*PixMapTempHandle, (BitMap*)&dp->portBits, &desbound, &desbound, srcCopy + ditherCopy, 0 );
	UnlockPixels(PixMapTempHandle);
	DisposeGWorld( tempGWorld );
//	FrameRect(&desbound);

	MoveTo(_targetRect.left, _targetRect.top);
	LineTo(_targetRect.right, _targetRect.top);
	LineTo(_targetRect.right, _targetRect.bottom);
	LineTo(_targetRect.left, _targetRect.bottom);
	LineTo(_targetRect.left, _targetRect.top);

	RGBBackColor(&oldbackcolor);
	RGBForeColor(&oldforecolor);
	
	SetGWorld(oldGW,oldGD);

	if(_show_grid)
		_drawgrids();

//draw controls
	DrawControls((WindowPtr)dp);	
	ShowWindow((WindowPtr)dp);
	SelectWindow((WindowPtr)dp);	
	
//	BeginUpdate(dp);
//	UpdateDialog(dp, dp->visRgn);
//	EndUpdate(dp);	
}

void TargetDialog::_drawmarchingants(void)
{
	long 		i, index;
	PenState	penState;
		
	GetPenState(&penState);
	PenMode(patXor);
		
	index = _lastindex;		
	
	if ((TickCount() > _lastantmarch + _marchtime)){
		PenPat((PatPtr)&patterns[_lastindex]);
		_drawcroplines(_dh, _dv);
		index = index < 7 ? index + 1 : 0;
		PenPat((PatPtr)&patterns[index]);
		_drawcroplines(_dh, _dv);
		_lastantmarch = TickCount();
	}
	
	_lastindex = index;
	SetPenState(&penState);
}

//ok
void TargetDialog::_drawmarkers(void)
{
	PenState	penState;	/* the saved pen state on entry to procedure */
	short i;

	GetPenState(&penState);
	PenMode(patXor);
	PenPat(&(qd.black));

	for(i = 0; i < 4; i++)
		FrameRect(&_mRect[i]);

	SetPenState(&penState);
}

//which means select last or current to draw
void TargetDialog::_drawcroplines(short *h, short *v)
{	
	long i;
	
	MoveTo(h[0], v[0]);
	for(i = 1; i < 4; i++)
		LineTo(h[i], v[i]);
	LineTo(h[0], v[0]);		
}


void TargetDialog::_drawgrids(void)
{
	PenState	penState;	/* the saved pen state on entry to procedure */
	short i;

	GetPenState(&penState);
	PenMode(patXor);
	PenPat(&(qd.black));

	for(i = 0; i < doc->_patches->num_patches; i++)
		FrameRect(&_gRect[i]);

	SetPenState(&penState);

}

void TargetDialog::_setgrids(void)
{
	long i;
	McoPatches *patches;
	Rect	rect;
	long width, length;
	short h, v;
	
	patches = (doc->_patches);
	
	width = (_targetRect.right - _targetRect.left);
	length = (_targetRect.bottom - _targetRect.top);

	rect.left = _targetRect.left;
	rect.right = _targetRect.left + 2 + (short)((patches->patches[0].right - patches->patches[0].left)*width + 0.5);
	rect.top = _targetRect.left;
	rect.bottom = _targetRect.left + 2 + (short)((patches->patches[0].bottom - patches->patches[0].top)*length + 0.5);

	for(i = 0; i < patches->num_patches; i++){
		h = (short)(patches->patches[i].left*width + 0.5);
		v = (short)(patches->patches[i].top*length + 0.5);
		_gRect[i].left = h + rect.left;
		_gRect[i].right = h + rect.right;
		_gRect[i].top = v + rect.top; 
		_gRect[i].bottom = v + rect.bottom; 
	}
}

void TargetDialog::_setcroplines(void)
{
	long i;
	double	ch, cv;
	double	th, tv;
	
	if(_rotang == 0){
		for(i = 0; i < 4; i++){
			_dh[i] = (short)(_h[i] + 0.5);
			_dv[i] = (short)(_v[i] + 0.5);
		}
	}
	else{
		ch = (_h[0] + _h[2])/2.0;
		cv = (_v[0] + _v[2])/2.0;
		for(i = 0; i < 4; i++){
			th = _h[i];
			tv = _v[i];
			_rotatepoint(_rotang, ch, cv, &th, &tv);
			_dh[i] = (short)(th + 0.5);
			_dv[i] = (short)(tv + 0.5);
		}	
	}						
}

void TargetDialog::_initforsize(double *h, double *v)
{
	long i;
	double ch, cv;
	double	th, tv;
	
	if(_rotang == 0){
		for(i = 0; i < 4; i++){
			h[i] = (double)_h[i];
			v[i] = (double)_v[i];
		}
	}
	else{
		ch = (_h[0] + _h[2])/2.0;
		cv = (_v[0] + _v[2])/2.0;
		for(i = 0; i < 4; i++){
			th = (double)_h[i];
			tv = (double)_v[i];
			_rotatepoint(_rotang, ch, cv, &th, &tv);
			h[i] = th;
			v[i] = tv;
		}	
	}						
}


void TargetDialog::_drawcropwindow(void)
{
	_drawmarchingants();
	_drawmarkers();	
}


McoStatus	TargetDialog::drawTarget(void)
{
	McoStatus 	status;
	PixMapHandle PixMap32Handle, PixMap8Handle;
	long 		*srcBaseAddr, *dstBaseAddr, srcRowLongs, dstRowLongs;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	char 		mmuMode;
	Rect desbound, srcbound;
	long row, col, plane;
	McoHandle imageHandle;
	unsigned char *image, *imageh;
	unsigned long red;
	unsigned long green;
	unsigned long blue;
	long i, j;
	McoHandle	shrinkH, targetH;
	long des_w, des_l;
	long		shrinki;
	unsigned char *insrc, *indest;

	targetH = _targetH;
	des_w = _targetRect.right - _targetRect.left;
	des_l = _targetRect.bottom - _targetRect.top;
	
	shrinki = des_w*des_l*3;
	shrinkH = McoNewHandle(shrinki);
	if(!shrinkH) return MCO_MEM_ALLOC_ERROR;
	insrc = (unsigned char *)McoLockHandle(targetH);
	indest = (unsigned char *)McoLockHandle(shrinkH);
	//resize	
	image_simple_size_non(insrc, indest, _tar_width, _tar_length, des_w, des_l, 3);

	McoLockHandle(targetH);
	McoUnlockHandle(shrinkH);

//for test
//	saveRGBTiff(shrinkH, des_w, des_l);

// the size of the window
	desbound.left 	= _targetRect.left;
	desbound.right 	= _targetRect.right;
	desbound.top    = _targetRect.top;
	desbound.bottom = _targetRect.bottom;

	if (_off32GWorld) DisposeGWorld(_off32GWorld);
	_off32GWorld = 0L;

	if( NewGWorld( &_off32GWorld, 32, &desbound, 0, 0, 0 ) ){
		_off32GWorld = NULL;
		return MCO_MEM_ALLOC_ERROR;
	}	

	PixMap32Handle = GetGWorldPixMap( _off32GWorld );
	(**PixMap32Handle).hRes = 72;
	(**PixMap32Handle).vRes = 72;

	dstBaseAddr = (long *) GetPixBaseAddr ( PixMap32Handle );	/* get the address of the pixmap */
	dstRowLongs = ((**PixMap32Handle).rowBytes & 0x7fff) >> 2;	/* get the _length increment */

	indest = (unsigned char *)McoLockHandle(shrinkH);
	LockPixels( PixMap32Handle );

// 	copy image to the offscreen window	
	mmuMode = true32b;
	SwapMMUMode(&mmuMode);
	for( i = 0; i < des_l; i++ ){
		long* dstAddr = dstBaseAddr;
		for( j = 0; j < des_w; j++ ){
			red = (unsigned long)*indest++;
			green = (unsigned long)*indest++;
			blue = (unsigned long)*indest++;
			*dstAddr++ = (red << 16) | (green << 8) | blue;
		}
		dstBaseAddr += dstRowLongs;
	}	
	SwapMMUMode(&mmuMode);
	
	if (_preview) status = _preview->evalPixMap(*PixMap32Handle);

	McoUnlockHandle(shrinkH);
	McoDeleteHandle(shrinkH);
	UnlockPixels(PixMap32Handle);
		
	_draw();
	return MCO_SUCCESS;
}


//crop functions
void	TargetDialog::_startcrop(Point start)
{
	long 		i, index;
	Point		mouse;		/* the current mouse location */
	Rect		cRect;	/* the current rectangle to be framed */
	PenState	penState;	/* the saved pen state on entry to procedure */
	unsigned char lastPat;
	short		th[4], tv[4];
	short		h[4], v[4];
	short		sh[4], sv[4];
		
	GetPenState(&penState);
	PenMode(patXor);

	index = 0;		
	PenPat((PatPtr)&patterns[index]);
	for(i = 0; i < 4; i++){
		th[i] = start.h;
		tv[i] = start.v;
	}	
	mouse.h = start.h;
	mouse.v = start.v;
	
	_lastantmarch = TickCount();

	while (StillDown()) {
		GetMouse(&mouse);
		_setselectrect(&cRect, start.h, start.v, mouse.h, mouse.v);
		
		h[0] = cRect.left;
		v[0] = cRect.top;
		h[1] = cRect.right;
		v[1] = cRect.top;
		h[2] = cRect.right;
		v[2] = cRect.bottom;
		h[3] = cRect.left;
		v[3] = cRect.bottom;
		
		if ((TickCount() > _lastantmarch + _marchtime) || 
			!_samepara(h[0],v[0],h[1],v[1],h[2],v[2],h[3],v[3],
					  th[0],tv[0],th[1],tv[1],th[2],tv[2],th[3],tv[3])){
			
			_drawcroplines(th, tv);
			index = index < 7 ? index + 1 : 0;
			PenPat((PatPtr)&patterns[index]);
			
			_drawcroplines(h, v);	
			_copylines(h, v, th, tv);
			_lastantmarch = TickCount();
			
			//_hascrop = 1;
		}
	}
	
	_lastindex = index;
	
	SetPenState(&penState);
	
	if( (start.h != mouse.h) && (start.v != mouse.v))
		_hascrop = 1;

	if(_hascrop){
		_rotang = 0;
		for(i = 0; i < 4; i++){
			_h[i] = (double)h[i];
			_v[i] = (double)v[i];
		}	
		_setcroplines();
		_setmarkers();
		_drawmarkers();
	}	

}

void TargetDialog::_copylines(short *sh, short *sv, short *dh, short *dv)
{
	long i;
	
	for(i = 0; i < 4; i++){
		dh[i] = sh[i];
		dv[i] = sv[i];
	}
}		

void TargetDialog::_rotatebydrag(Point start)	
{	
	long 		i, index;
	Point		mouse;		
	PenState	penState;	
		
	GetPenState(&penState);
	PenMode(patXor);

	index = _lastindex;
	PenPat((PatPtr)&patterns[index]);	
	_lastantmarch = TickCount();

	while (StillDown()) {
//	while (1) {
		GetMouse(&mouse);
				
		if ((TickCount() > _lastantmarch + _marchtime) || 
			(start.h != mouse.h) || (start.v != mouse.v) ){	
					
			_drawcroplines(_dh, _dv);			
			_drawmarkers();
			
			index = index < 7 ? index + 1 : 0;
			PenPat((PatPtr)&patterns[index]);
				
			_rotatecrop(start.h, start.v, mouse.h, mouse.v);	
			_setcroplines();
			_setmarkers();
			_drawcroplines(_dh, _dv);			
			_drawmarkers();			

			start.h = mouse.h;
			start.v = mouse.v;
			_lastantmarch = TickCount();			
		}
	}
	
	_lastindex = index;	
	SetPenState(&penState);	
}

void TargetDialog::_sizebydrag(Point start)	
{	
	long 		i, index;
	Point		mouse;		/* the current mouse location */
	PenState	penState;	/* the saved pen state on entry to procedure */
	double		h[4], v[4];
		
	GetPenState(&penState);
	PenMode(patXor);

	index = _lastindex;
	PenPat((PatPtr)&patterns[index]);	
	_lastantmarch = TickCount();

	_initforsize(h, v);
	
	while (StillDown()) {
//	while (1) {
		GetMouse(&mouse);
				
		if ( (TickCount() > _lastantmarch + _marchtime) || 
			(start.h != mouse.h) || (start.v != mouse.v) ){
			
			_drawcroplines(_dh, _dv);			
			_drawmarkers();
						
			index = index < 7 ? index + 1 : 0;
			PenPat((PatPtr)&patterns[index]);
				
			_sizecrop(start.h, start.v, mouse.h, mouse.v, h, v);	
			_drawcroplines(_dh, _dv);
/*			
			if( (_dh[0] != _dh[3]) || (_dv[0] != _dv[1]) ){
				_dh[3] = _dh[0];
				_dv[1] = _dv[0];
			}	
*/				 			
			_drawmarkers();			

			start.h = mouse.h;
			start.v = mouse.v;
			_lastantmarch = TickCount();			
		}
	}
	
	_lastindex = index;
	
	_setcroplines();
	_setmarkers();
	
	SetPenState(&penState);	
}

void TargetDialog::_movebydrag(Point start)	
{	
	long 		i, index;
	Point		mouse, initpoint;		/* the current mouse location */
	Rect		cRect;	/* the current rectangle to be framed */
	PenState	penState;	/* the saved pen state on entry to procedure */
	unsigned char lastPat;
	Rect		rrect;
		
	GetPenState(&penState);
	PenMode(patXor);

	index = _lastindex;
	PenPat((PatPtr)&patterns[index]);	
	_lastantmarch = TickCount();

	_findclipregion(&start, &rrect);

	initpoint.h = start.h;
	initpoint.v = start.v;	

	while (StillDown()) {
	
		GetMouse(&mouse);
		_clipmovemouse(&(mouse.h), &(mouse.v), &rrect);
				
		if ( (TickCount() > _lastantmarch + _marchtime) || 
			(start.h != mouse.h) || (start.v != mouse.v) ){
			
			_drawcroplines(_dh, _dv);
			_drawmarkers();
						
			index = index < 7 ? index + 1 : 0;
			PenPat((PatPtr)&patterns[index]);
			
			_movecrop(start.h, start.v, mouse.h, mouse.v);
			_drawcroplines(_dh, _dv);			
			_drawmarkers();			
			
			start.h = mouse.h;
			start.v = mouse.v;
			
			_lastantmarch = TickCount();			
		}
	}
	
	_lastindex = index;

	_endofdrag(&initpoint, &mouse);
	_setcroplines();
	_setmarkers();
	
	SetPenState(&penState);	
}


void TargetDialog::_setselectrect(Rect*	rect, short left, short top, short right, short bottom)
{

	if (left > right) {
		rect->left = right;
		rect->right = left;
	} else {
		rect->left = left;
		rect->right = right;
	}
	if (top > bottom) {
		rect->top = bottom;
		rect->bottom = top;
	} else {
		rect->top = top;
		rect->bottom = bottom;
	}

	//the mouse selection must be inside the target region
	if(rect->left < _targetRect.left)
		rect->left = _targetRect.left;
	if(rect->right > _targetRect.right)
		rect->right = _targetRect.right;
	if(rect->top < _targetRect.top) 		
		rect->top = _targetRect.top;
	if(rect->bottom > _targetRect.bottom)  
		rect->bottom = _targetRect.bottom;	
}



//there is bug inside this
//just do the rectangle
void TargetDialog::_sizecrop(short ph, short pv, short ch, short cv, double *h, double *v)
{
	long 	i;
	short 	dh, dv;
	double	cenh, cenv;
	double	th[4], tv[4];
	short	th2[4], tv2[4];
	short	marker[] = {2, 3, 0, 1};
	double 	x, y;

	
	dh = ch - ph;
	dv = cv - pv;
	
	for(i = 0; i < 4; i++){
		th[i] = h[i];
		tv[i] = v[i];
	}
		
	th[_cmarker] += dh;
	tv[_cmarker] += dv;
	
	cenh = (th[_cmarker] + th[marker[_cmarker]])/2.0;
	cenv = (tv[_cmarker] + tv[marker[_cmarker]])/2.0;


	switch(_cmarker){
		case(0):
			_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], h[3], v[3], h[1], v[1], &x, &y);
			break;
			
		case(1):
			_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], h[2], v[2], h[0], v[0], &x, &y);
			break;
			
		case(2):
			_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], h[1], v[1], h[3], v[3], &x, &y);
			break;
			
		case(3):
			_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], h[0], v[0], h[2], v[2], &x, &y);
			break;
	}		

	i = 9;
	while(!_sizable(cenh, cenv, x, y)){		
		th[_cmarker] = h[_cmarker] + 0.1*i*dh;
		tv[_cmarker] = v[_cmarker] + 0.1*i*dv;
		
		cenh = (th[_cmarker] + th[marker[_cmarker]])/2.0;
		cenv = (tv[_cmarker] + tv[marker[_cmarker]])/2.0;

		switch(_cmarker){
			case(0):
				_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
						th[_cmarker], tv[_cmarker], h[3], v[3], h[1], v[1], &x, &y);
				break;
				
			case(1):
				_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
						th[_cmarker], tv[_cmarker], h[2], v[2], h[0], v[0], &x, &y);
				break;
				
			case(2):
				_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
						th[_cmarker], tv[_cmarker], h[1], v[1], h[3], v[3], &x, &y);
				break;
				
			case(3):
				_findlength(th[marker[_cmarker]], tv[marker[_cmarker]], 
						th[_cmarker], tv[_cmarker], h[0], v[0], h[2], v[2], &x, &y);
				break;
		}
		
		i--;		
	}


	//set redraw lines _dh and _dv
	switch(_cmarker){
		case(0):
		case(2):
			_sizepoint(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], &h[3], &v[3]);
			_sizepoint(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], &h[1], &v[1]);
			break;
			
		case(1):
		case(3):
			_sizepoint(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], &h[2], &v[2]);
			_sizepoint(th[marker[_cmarker]], tv[marker[_cmarker]], 
					th[_cmarker], tv[_cmarker], &h[0], &v[0]);
			break;
	}

	h[_cmarker] = th[_cmarker];
	v[_cmarker] = tv[_cmarker];
	
	for(i = 0; i < 4; i++){
		_dh[i] = (short)(h[i] + 0.5);
		_dv[i] = (short)(v[i] + 0.5);
	}
		
	//set redraw markers _mRect
	for(i = 0; i < 4; i++){
		_mRect[i].left = _dh[i] - 3;
		_mRect[i].top = _dv[i] - 3;
		_mRect[i].right = _dh[i] + 3;
		_mRect[i].bottom = _dv[i] + 3;
	}

	for(i = 0; i < 4; i++){
		if(_mRect[i].left < _targetRect.left)
			_mRect[i].left = _targetRect.left;
		if(_mRect[i].right > _targetRect.right)	
			_mRect[i].right = _targetRect.right;
		if(_mRect[i].top < _targetRect.top)	
			_mRect[i].top = _targetRect.top;
		if(_mRect[i].bottom > _targetRect.bottom)	
			_mRect[i].bottom = _targetRect.bottom;
	}				

	_h[0] = cenh - x/2.0;
	_v[0] = cenv - y/2.0;
	_h[1] = cenh + x/2.0;
	_v[1] = cenv - y/2.0; 
	_h[2] = cenh + x/2.0;
	_v[2] = cenv + y/2.0;
	_h[3] = cenh - x/2.0;
	_v[3] = cenv + y/2.0; 	
}


short TargetDialog::_sizable(double ch, double cv, double width, double height)
{
	long 	i;
	Point	p;
	short	th[4], tv[4], h, v;
	short	cenh, cenv;
	short	val;
	
	if(width <= 0 || height <= 0)
		return (0);
		
	th[0] = (short)(ch - width/2 + 0.5);
	tv[0] = (short)(cv - height/2 + 0.5);
	th[1] = (short)(ch + width/2 + 0.5);
	tv[1] = (short)(cv - height/2 + 0.5); 

	th[2] = (short)(ch + width/2 + 0.5);
	tv[2] = (short)(cv + height/2 + 0.5);
	th[3] = (short)(ch - width/2 + 0.5);
	tv[3] = (short)(cv + height/2 + 0.5); 

	cenh = (short)(ch + 0.5);
	cenv = (short)(cv + 0.5);
	for(i = 0; i < 4; i++){
		_rotatepointshort(_rotang, cenh, cenv, &th[i], &tv[i]);
		p.h = th[i];
		p.v = tv[i];
		val = _isinWindow(&p);
		if(!val)	return val;
	}

	return 1;				
}

/*
void TargetDialog::_findlength(short cenh, short cenv, 
					short diah, short diav, short hx, short vx, 
					short hy, short vy, double *x, double *y)
{
	double ax, ay, bx, by;
	double xproj, yproj;
	double dx, dy;
	
	dx = (double)(diah - cenh);
	dy = (double)(diav - cenv);

	ax = (double)(hx - cenh);
	ay = (double)(vx - cenv);
	xproj = sqrt((double)(ax*ax + ay*ay));
	ax = ax/xproj;
	ay = ay/xproj;

	bx = (double)(hy - cenh);
	by = (double)(vy - cenv);
	yproj = sqrt((double)(bx*bx + by*by));
	bx = bx/yproj;
	by = by/yproj;
		
	//width	
	*x = dx*ax + dy*ay;
	//length
	*y = dx*bx + dy*by;
}
*/

void TargetDialog::_findlength(double cenh, double cenv, 
					double diah, double diav, double hx, double vx, 
					double hy, double vy, double *x, double *y)
{
	double ax, ay, bx, by;
	double xproj, yproj;
	double dx, dy;
	
	dx = (double)(diah - cenh);
	dy = (double)(diav - cenv);

	ax = (double)(hx - cenh);
	ay = (double)(vx - cenv);
	xproj = sqrt((double)(ax*ax + ay*ay));
	ax = ax/xproj;
	ay = ay/xproj;

	bx = (double)(hy - cenh);
	by = (double)(vy - cenv);
	yproj = sqrt((double)(bx*bx + by*by));
	bx = bx/yproj;
	by = by/yproj;
		
	//width	
	*x = dx*ax + dy*ay;
	//length
	*y = dx*bx + dy*by;
}

/*
void TargetDialog::_sizepoint(short cenh, short cenv, 
					short diah, short diav, short *h, short *v)
{
	double ax, ay;
	double dist, diag;
	
	ax = (double)(*h - cenh);
	ay = (double)(*v - cenv);
			
	diag = sqrt((double)(ax*ax + ay*ay));
	ax = ax/diag;
	ay = ay/diag;
			
	//distance
	dist = (diah - cenh)*ax + (diav - cenv)*ay;
	
	*v = (short)(cenv + ay*dist + 0.5);
	*h = (short)(cenh + ax*dist + 0.5);
}
*/

void TargetDialog::_sizepoint(double cenh, double cenv, 
					double diah, double diav, double *h, double *v)
{
	double ax, ay;
	double dist, diag;
	
	ax = (double)(*h - cenh);
	ay = (double)(*v - cenv);
			
	diag = sqrt((double)(ax*ax + ay*ay));
	ax = ax/diag;
	ay = ay/diag;
			
	//distance
	dist = (diah - cenh)*ax + (diav - cenv)*ay;
	
	*v = cenv + ay*dist;
	*h = cenh + ax*dist;
}


//ph and pv are previous mouse location
//ch and cv are current mouse location
void TargetDialog::_rotatecrop(short ph, short pv, short ch, short cv)
{
	double	cenh, cenv;
	double	ax, ay;
	double 	diag;	//diagnol distance
	double 	mdist;	//mouse distance
	double	mx, my;	//mouse moving x and y value
	double theta, delta;	//rotation angle
	long 	i;


	cenh = (_h[0] + _h[2])/2.0;
	cenv = (_v[0] + _v[2])/2.0;

	ax = _h[_cmarker] - cenh;
	ay = _v[_cmarker] - cenv;		
	diag = sqrt(ax*ax + ay*ay);
	mx = (double)(ch - ph);
	my = (double)(cv - pv);
	mdist = sqrt(mx*mx + my*my);
	
	if(mdist == 0)
		theta = 0;
	else	
		theta = ((double)(mx*(-ay) + my*ax)/(diag*diag*mdist));
		
	//check angle
	delta = _rotang + theta;
	if(delta > 0.1)
		delta = 0.1;
	if(delta < -0.1)
		delta = -0.1;
				
	//check frames
	i = 9;
	while(!_rotatable(delta)){
		delta = _rotang + i*0.1*theta; 
		if(delta > 0.1)
			delta = 0.1;
		if(delta < -0.1)
			delta = -0.1;
		i--;	
	}
	
	_rotang = delta;
		
}

short TargetDialog::_rotatable(double delta)
{
	long i;
	double ch, cv, th, tv;
	Point p;
	short	val;
	
	ch = (_h[0] + _h[2])/2.0;
	cv = (_v[0] + _v[2])/2.0;
	for(i = 0; i < 4; i++){
		th = _h[i];
		tv = _v[i];
		_rotatepoint(delta, ch, cv, &th, &tv);
		p.h = (short)(th + 0.5);
		p.v = (short)(tv + 0.5);
		val = _isinWindow(&p);
		if(!val)	return val;
	}

	return 1;		
}	

//check horizontal direction
short	TargetDialog::_hormovable(short *h, short *v)
{
	long i;
	short ch, cv, th, tv;
	Point p;
	short	val;
	
	ch = (short)((h[0] + h[2])/2.0 + 0.5);
	cv = (short)((v[0] + v[2])/2.0 + 0.5);
	for(i = 0; i < 4; i++){
		th = h[i];
		tv = v[i];
		_rotatepointshort(_rotang, ch, cv, &th, &tv);
		
		if(th < _targetRect.left || th > _targetRect.right)
			return (0);
	}

	return (1);	
}	
	
//check vertical direction
short	TargetDialog::_vermovable(short *h, short *v)
{
	long i;
	short ch, cv, th, tv;
	Point p;
	short	val;
	
	ch = (short)((h[0] + h[2])/2.0 + 0.5);
	cv = (short)((v[0] + v[2])/2.0 + 0.5);
	for(i = 0; i < 4; i++){
		th = h[i];
		tv = v[i];
		_rotatepointshort(_rotang, ch, cv, &th, &tv);
		
		if(tv < _targetRect.top || tv > _targetRect.bottom)
			return (0);
	}

	return (1);	
}	

/*
void TargetDialog::_rotatepoint(double ang, short cenh, short cenv, short *h, short *v)
{
	short th;
	
	th = *h;
	*h = (short)((th - cenh)*cos(ang) - (*v - cenv)*sin(ang) + cenh + 0.5);
	*v = (short)((th - cenh)*sin(ang) + (*v - cenv)*cos(ang) + cenv + 0.5);
}
*/

void TargetDialog::_rotatepointshort(double ang, short cenh, short cenv, short *h, short *v)
{
	double th, tv;
	
	th = (double)*h;
	tv =  (double)*v;
	_rotatepoint(ang, (double)cenh, (double)cenv, &th, &tv);
	*h = (short)(th + 0.5);
	*v = (short)(tv + 0.5);
}


void TargetDialog::_rotatepoint(double ang, double cenh, double cenv, double *h, double *v)
{
	double th;
	
	th = *h;
	*h = (th - cenh)*cos(ang) - (*v - cenv)*sin(ang) + cenh;
	*v = (th - cenh)*sin(ang) + (*v - cenv)*cos(ang) + cenv;
}

/*
void TargetDialog::_movecrop(short ph, short pv, short ch, short cv)
{
	long i;
	short dh;
	short dv;
	short th[4], tv[4];
	
	dh = ch - ph;
	dv = cv - pv;
	
	for(i = 0; i < 4; i++){
		th[i] = (short)(_h[i] + 0.5 + dh);
		tv[i] = (short)(_v[i] + 0.5 + dv);
	}	

	while(!_hormovable(th, tv)){
		if(dh > 0){
			dh--;
			for(i = 0; i < 4; i++)
				th[i] -= 1;
		}
		else{
			dh++;
			for(i = 0; i < 4; i++)
				th[i] += 1;
		}
	}
	
	while(!_vermovable(th, tv)){
		if(dv > 0){
			dv--;
			for(i = 0; i < 4; i++)
				tv[i] -= 1;
		}
		else{
			dv++;
			for(i = 0; i < 4; i++)
				tv[i] += 1;
		}
	}	
	
	for(i = 0; i < 4; i++){
		_h[i] += dh;
		_v[i] += dv;
	}	
}	
*/


void TargetDialog::_movecrop(short ph, short pv, short ch, short cv)
{
	long i;
	short dh;
	short dv;
	short th[4], tv[4];
	
	dh = ch - ph;
	dv = cv - pv;

	for(i = 0; i < 4; i++){
		_dh[i] += dh;
		_dv[i] += dv;
	}
	
	for(i = 0; i < 4; i++){
		_mRect[i].left = _dh[i] - 3;
		_mRect[i].top = _dv[i] - 3;
		_mRect[i].right = _dh[i] + 3;
		_mRect[i].bottom = _dv[i] + 3;
	}	
	
	for(i = 0; i < 4; i++){
		if(_mRect[i].left < _targetRect.left)
			_mRect[i].left = _targetRect.left;
		if(_mRect[i].right > _targetRect.right)	
			_mRect[i].right = _targetRect.right;
		if(_mRect[i].top < _targetRect.top)	
			_mRect[i].top = _targetRect.top;
		if(_mRect[i].bottom > _targetRect.bottom)	
			_mRect[i].bottom = _targetRect.bottom;
	}				
	
}	


short TargetDialog::_smaller(short a, short b)
{
	return ((a <= b)? a: b);
}

short TargetDialog::_larger(short a, short b)
{
	return ((a >= b)? a: b);
}		


//clip the mouse location within the restricted region
void TargetDialog::_findclipregion(Point *start, Rect	*rrect)
{
	short	left, right;
	short	top, bottom;
	
	left = _smaller(_dh[0], _dh[3]);
	right = _larger(_dh[1], _dh[2]);
	
	top = _smaller(_dv[0], _dv[1]);
	bottom = _larger(_dv[2], _dv[3]);

/*	
	rrect->left = _dh[_cmarker] + (_targetRect.left - left);
	rrect->right = _dh[_cmarker] + (_targetRect.right - right);
	rrect->top = _dv[_cmarker] + (_targetRect.top - top);
	rrect->bottom = _dv[_cmarker] + (_targetRect.bottom - bottom);
*/
	rrect->left = start->h + (_targetRect.left - left);
	rrect->right = start->h + (_targetRect.right - right);
	rrect->top = start->v + (_targetRect.top - top);
	rrect->bottom = start->v + (_targetRect.bottom - bottom);
}


//clip the mouse location within the restricted region
void TargetDialog::_clipmovemouse(short *mh, short *mv, Rect *rrect)
{
	short	left, right;
	short	top, bottom;
	short	h, v;
	
	h = *mh;
	v = *mv;
	
	if(h < rrect->left)
		h = rrect->left;
	if(h > rrect->right)
		h = rrect->right;
	if( v < rrect->top)
		v = rrect->top;
	if( v > rrect->bottom)
		v = rrect->bottom;
		
	*mh = h;
	*mv = v;
}

void TargetDialog::_endofdrag(Point *initpoint, Point *mouse)
{
	short i;
	short dh;
	short dv;
	
	dh = mouse->h - initpoint->h;
	dv = mouse->v - initpoint->v;
	
	for(i = 0; i < 4; i++){
		_h[i] += dh;
		_v[i] += dv;
	}

}


McoStatus	TargetDialog::_cropimage(void)
{
	long i, j;
	long des_w, des_l, st_w, st_l;
	long		shrinki;
	unsigned char *insrc, *indest;
	McoHandle	shrinkH, targetH;
	double	scale;

	scale = (double)(_h[1] - _h[0])/(double)(_targetRect.right - _targetRect.left);
	des_w = (long)(scale*_tar_width + 0.5);
	scale = (double)(_h[0] - _targetRect.left)/(double)(_targetRect.right - _targetRect.left);
	st_w = (long)(scale*_tar_width + 0.5);
	scale = (double)(_v[3] - _v[0])/(double)(_targetRect.bottom - _targetRect.top);
	des_l = (long)(scale*_tar_length + 0.5);
	scale = (double)(_v[0] - _targetRect.top)/(double)(_targetRect.bottom - _targetRect.top);
	st_l = (long)(scale*_tar_length + 0.5);
		
	targetH = _targetH;
	shrinki = des_w*des_l*3;
	shrinkH = McoNewHandle(shrinki);
	if(!shrinkH) return MCO_MEM_ALLOC_ERROR;
	insrc = (unsigned char *)McoLockHandle(targetH);
	indest = (unsigned char *)McoLockHandle(shrinkH);

	//resize and rotate
	imagerotate(insrc, _tar_length, _tar_width,
				indest, st_l, st_w, des_l, des_w, _rotang, 3);	

	//reset the target length
	_tar_width = des_w;
	_tar_length = des_l;
	
	McoUnlockHandle(targetH);
	McoDeleteHandle(targetH);
	
	McoUnlockHandle(shrinkH);
	
//for test
//	saveRGBTiff(shrinkH, des_w, des_l);
	
	_targetH = shrinkH;
	doc->targetBufferH = _targetH;
	
	_inittargetwin();
	
	return MCO_SUCCESS;
}
