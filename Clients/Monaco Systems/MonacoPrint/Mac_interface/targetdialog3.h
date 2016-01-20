#ifndef TARGET_DIALOG_H
#define TARGET_DIALOG_H

#include <QDOffscreen.h>
#include <AppleEvents.h>
#include <stdio.h>
#include <Script.h>
#include "basewin.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "think_bugs.h"
#include "calibrate.h"
#include "profiledoc.h"


//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class TargetDialog:public baseWin{

private:
protected:
	long 		_lastantmarch;
	long 		_marchtime;
	GWorldPtr	_gw;
	Rect		_targetRect;

//crop rotation angle
	double 		_rotang;
	
//crop center
	short		_center;
		
//crop points
//geometry of the 4 points
	short		_h[4];
	short		_v[4];

//points for drawing
	short		_dh[4];
	short		_dv[4];

//mark rectangle
	Rect		_mRect[4];	
	
//the marker been selected by user	
	short		_cmarker;	

//has a crop mark or not
	short 		_hascrop;
	
	Boolean		_ants;
	long		_tar_length, _tar_width;
	long 		_lastindex;
	Str255		_scan_name;
	unsigned char	_last_rgb[3];
	unsigned char	_rgb[3];

	StandardFileReply _sReply;
	long		_has_scan_table;	//the scanner table

	//cursors
	Cursor	_eyedropCrsr, _crossCrsr;
	Cursor	_cropCrsr, _cancelCrsr, _finishCrsr;	
	long	_currentcursor;
	long 	_lastcursor;

	//gworld for redrawing
	GWorldPtr _off32GWorld;

//init functions
	void	_inittargetwin(void);

//compare functions
	short	_isinWindow(Point* mouse);
	short	_isinMarker(Point* mouse);
	short	_isinCrop(Point* mouse);
	short	_hormovable(short *h, short *v);
	short	_vermovable(short *h, short *v);

	short	_rotatable(double delta);
	short	_sizable(double ch, double cv, double width, double height);

	short larger(short a, short b);
	short smaller(short a, short b);
	short	_samepara(short h1, short v1, short h2, short v2,
					short h3, short v3, short h4, short v4,
					short i1, short w1, short i2, short w2,
					short i3, short w3, short i4, short w4); 


	void	_crop(short item, Point start);
	void SetSelectRect(Rect*, short, short, short, short);

	
	
	
	McoStatus _loadscanner(void);
	void	_getRGB(Point* mouse);

//drawing functions	
	void	_refreshtarget(void);
	void	_draw(void);
	void 	_drawmarchingants(void);
	void	_drawcroplines(short *h, short *v);
	void	_drawmarkers(void);
	void	_drawcropwindow(void);

//command functions
	void	_startcrop(Point start);
	void	_rotatebydrag(Point start);		
	void	_sizebydrag(Point start);
	void	_movebydrag(Point start);		
	void	_rotatepoint(double ang, short cenh, short cenv, short *h, short *v);
	void	_sizepoint(short cenh, short cenv, 
					short diah, short diav, short *h, short *v);

//data set functions
	void	_setselectrect(Rect*	rect, short left, short top, short right, short bottom);
	void 	_sizecrop(short ph, short pv, short ch, short cv);
	void 	_rotatecrop(short ph, short pv, short ch, short cv);
	void	_movecrop(short ph, short pv, short ch, short cv);
	short	_handlecrop(Point start);
	void	_disablecrop(Point start);

	void	_setmarkers(void);
	void	_setcroplines(void);

//utility functions
	void 	_copylines(short *sh, short *sv, short *dh, short *dv);

//	
	short	isme(WindowPtr win);

	void	_findMarker(Point* start);

	void	_findlength(short cenh, short cenv, 
				short diah, short diav, short *h, short *v, double *x, double *xy);
		
	ProfileDoc *doc;	
		
public:
	TargetDialog(ProfileDoc *dc);
	~TargetDialog(void);

	//abstract function, should be filled by subclass
	//hit means mouse select contents area		
	McoStatus	drawTarget(long width, long length, McoHandle targetH);
	void 	findgrids(Rect *);

	// do null events
	Boolean alwaysUpdate(void) { return TRUE;}
	McoStatus	doPointInWindowActiveRgn(Point mouse,WinEve_Cursors &cursortype);
	Boolean	KeyFilter(EventRecord *theEvent,short *itemHit );
	McoStatus	DoEvents(short item, Point location, Window_Events &code,
	 					 MS_WindowTypes &wintype, int32 &winnum,void **data, Boolean &changed);
	McoStatus	UpdateWindow(void);
	McoStatus	updateWindowData(void);

};

#endif //