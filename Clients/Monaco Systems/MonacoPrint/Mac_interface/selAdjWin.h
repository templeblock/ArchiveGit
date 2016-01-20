/*********************************************/
/* selAdjWin.h							 	 */
/* Selectivly adjust colors					 */
/* Jun 3 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#ifndef IN_SADJ_WIN
#define IN_SADJ_WIN

#include "mcomem.h"
#include "mcostat.h"
#include "xyztorgb.h"

#include "winevent.h"
#include <QDOffscreen.h>
#include "tweak_patch.h"
#include "profiledocinf.h"
#include "techkon.h"

typedef enum {
	SADJ_ID = 0,
	SADJ_OK_BOX,
	SADJ_APPLY,
	SADJ_LOAD,
	SADJ_SAVE,
	SADJ_DIFFER,
	SADJ_Des_Box,
	SADJ_Des_Device,
	SADJ_Des_L,
	SADJ_Des_a,
	SADJ_Des_b,
	SADJ_Des_R,
	SADJ_Des_G,
	SADJ_Des_B,
	SADJ_Pri_Box,
	SADJ_Pri_Device,
	SADJ_Pri_L,
	SADJ_Pri_a,
	SADJ_Pri_b,
	SADJ_Pri_R,
	SADJ_Pri_G,
	SADJ_Pri_B,
	SADJ_Gam_Box,
	SADJ_Gam_L,
	SADJ_Gam_a,
	SADJ_Gam_b,
	SADJ_PICK_BOX,
	SADJ_VIEW_BOX,
	SADJ_VIEW_SLIDER,
	SADJ_VIEW_DES,
	SADJ_VIEW_PRI,
	SADJ_LIGHT_RANGE,
	SADJ_COLOR_RANGE,
	SADJ_VIEW_D_L,
	SADJ_VIEW_D_a,
	SADJ_VIEW_D_b,
	SADJ_VIEW_P_L,
	SADJ_VIEW_P_a,
	SADJ_VIEW_P_b,
	SADJ_SAVE_PATCH };
	
	
/* The PICT ids for the slider */
/* disable back, enable unpress back, enable press, disable slide, enable unpress slide, enable press slide */

#define SADJ_PICT_IDS {18002,18002,18002,18003,18003,18003}

/* The offsets etc. used by the sliders */
/* horz offset, horz inset, vert offset, horz text, vert text, horz text, vert text, horz text, vert text */

#define SADJ_OFFSET {-1,10,13,-32000,12,-32000,22,-32000,22}
	
	
	
#define NUM_SADJ_IDS 41

#define SADJ_IDS {5000,44,37,3,38,43,34,59,17,18,19,11,12,13,35,60,26,27,28,23,24,25,61,64,63,62,40,36,39,41,42,4,5,51,49,50,55,57,56,58}


#define VIEW_RANGE 0.33333


class CAdjWin:public baseWin{
private:
protected:
public:
	
 	WindowPtr   the_window;
	
	XyztoRgb 	*xyztorgb;
	double		gamma;
	int	  ids[NUM_SADJ_IDS];
	
	double desiredLAB[3];
	double desiredRGB[3];
	
	double printedLAB[3];
	double printedRGB[3];
	
	double gammutLAB[3];
	double gammutRGB[3];
	
	int32 desiredLABi[3];
	int32 desiredRGBi[3];
	
	int32 printedLABi[3];
	int32 printedRGBi[3];
	
	int32 gammutLABi[3];
	
	double rgb1[128*128*3];
	double rgb2[128*128*3];
	
	double Lr,Cr;
	double SliderL;
	


	int Current_Text;
	int32	_last_tick_time2;
	
	McoHandle theRefConHand;
	Tweak_Patch *tweak;
	ProfileDocInf	*doc;
	
	double	xa1[33],xa2[33];
	
	double	rgb_d[3],rgb_p[3];

Boolean	KeyHasBeenHit;
int32	LastKeyPress;
	
	
//public:
	
	CAdjWin(ProfileDocInf *dc,XyztoRgb *xtr,double g);
	~CAdjWin();
	
	void SetViewLRange(void);
	void ConvertToRGB(double *lab,double *rgb);
	void ConvertToLAB(double *rgb,double *lab);
	
	McoStatus interpolate_33(double *lab,double *labc,double *table);
	void ConvertToGammutC(double *lab,double *labc);

	Boolean checkChanged(void);
	McoStatus CopyTextFromDial(void);
	McoStatus CopyViewTextToDial(double *labd,double *labp);
	McoStatus CopyTextToDial(short w);

	McoStatus CopyMultColorBox(GWorldPtr src,double *rgb);
	McoStatus DrawMultColorBox(short item,double *rgb);
	McoStatus CreateMultColorBox(double *lab,double Cr,double *rgb);
	McoStatus getBoxPointColor(double *labOut,double *labCent, double Cr, short item, Point where);
	McoStatus updateWindowData(int changed);
	McoStatus DrawWinowParts(void);
	McoStatus DrawWindow(void);
	McoStatus UpdateWindow(void);
	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);
	Boolean   isMyObject(Point where,short *item);
	McoStatus SaveTweakedPatches(void);
	Boolean TimerEventPresent(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);

};


#endif