/*********************************************/
/* logowin2.h							 	 */
/* Display 3D gammut surface in window 		 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#ifndef IN_LOGOWIN2_WIN
#define IN_LOGOWIN2_WIN

// quickdraw 3D headers

#include "QD3D.h"
#include "QD3DMath.h"
#include "QD3DDrawContext.h"
#include "QD3DShader.h"
#include "QD3DTransform.h"
#include "QD3DGroup.h"
#include "QD3DCamera.h"

#include "mdialog.h"
#include "gammutwinSupp.h"
#include "mcomem.h"
#include "mcostat.h"
#include "xyztorgb.h"
#include "qd3Dwin.h"

// The base class

#define LW_IDS { 1804, 1, -1, -1, -1}

#define LW_TEXTURE 16002

class LogoWin2:public QD3DWin{
private:
protected:
	int32	tick_start;
	XyztoRgb 	*xyztorgb;
	double		monitor_z;

public:
	
	
//public:
	
	LogoWin2(XyztoRgb *xtor,double mg);
	~LogoWin2(void);


	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
	TQ3GeometryObject	NewLogoObject(void);
	McoStatus ChangeGeometry (McoQ3DType type, long num);
	TQ3GeometryObject NewPolyGCIELabRef();
	
	McoStatus updateWindowData(int changed);
	Boolean alwaysUpdate(void) { return TRUE;}
	Boolean	TimerEventPresent(void);
	
};


#endif