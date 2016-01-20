#ifndef IN_MONACO_SLIDER
#define IN_MONACO_SLIDER


#include <QDOffscreen.h>

typedef enum {
	Disable_PICT = 0,
	Enable_Unpress_PICT,
	Enable_Press_PICT,
	Disable_Slide_PICT,
	Enable_Unpress_Slide_PICT,
	Enable_Press_Slide_PICT
	} Slider_PICTS;



typedef struct RefConType
	{
	long		PICTIDs[6];		/* A set of PICT ids	*/
	long			h_d,h_i;		// horizontal displacement and inset
	long			v_d;			// vertical displacement and inset
	long			text_h,text_v;	// Location of text relative to the rectangle (-32000 = do not display)
	long			text_min_h,text_min_v;	// Location of min text relative to the rectangle (-32000 = do not display)
	long			text_max_h,text_max_v;	// Location of max text relative to the rectangle (-32000 = do not display)
	long			dragProcRefCon;	/* A user-defined refCon to be used by the drag proc					*/
	ProcPtr	dragProc;			/* Pointer to a drag proc that will be called whenever the value	*/
								/* changes during the drag portion of the control activation		*/
	short			textID;
	DialogPtr		dp;
	Rect			leftRect;
	Rect			rightRect;
	long			leftVal;
	long			rightVal;
	} RefConType;

typedef	RefConType		*RefConTypePtr;
typedef	RefConTypePtr	*RefConTypeHdl;

#define OFFSET_TEXT_ID 19

#ifdef	_DEBUG_

#endif

void			DrawCntl ( ControlHandle );
long			TestCntl ( ControlHandle, Point* );
void			CalcCRgns ( ControlHandle, RgnHandle, Boolean );
#ifndef INTEST
pascal void	DragProc ( ControlHandle theControl, long refCon, ProcPtr proc );
#endif
long			DragCntl ( ControlHandle theControl );
void 			DrawHandle(Rect *BigBox,int min,int max, int val,PicHandle thePic);
void			DrawPICT ( Rect *box, short pictID );
void 			DrawBoth(ControlHandle theControl,RefConTypePtr	theRefCon,Rect *BigBox,PicHandle thePic,PicHandle thePic2,Boolean l, Boolean r);
void 			Gray32Rect(GWorldPtr src,Rect *dataRect);
//void 			DrawBoth(Rect *BigBox,int min,int max, int val,PicHandle thePic,PicHandle thePic2);

#endif