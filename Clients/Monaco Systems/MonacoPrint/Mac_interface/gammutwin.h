/*********************************************/
/* gammutwin.h							 	 */
/* Display 3D gammut surface in window 		 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#ifndef IN_GAMMUTWIN_WIN
#define IN_GAMMUTWIN_WIN

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

// Data used to open the window



typedef enum {
	GMW_L_text = 6,
	GMW_a_text,
	GMW_b_text,
	GMW_color_box
	};

#define GS_IDS { 3000, 1, 11, 10, 12, 13, 17, 18, 19, 20}

#define NUM_GMW_IDS 10

// The base class

class GamutWin:public QD3DWin{
private:
protected:
public:
	
	long	start,end;
	McoHandle gammutSurfaceH;
	
	XyztoRgb 	*xyztorgb;
	double	monitor_z;
	
//public:
	
	GamutWin(long s, long e, McoHandle gSH,int32 winnum,MS_WindowTypes wint,XyztoRgb *xtor,double mg,unsigned char *patchFName);
	~GamutWin();
	McoStatus LoadXyzToRgb(void);

	McoStatus AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject);
	TQ3GeometryObject NewGammutObject(long start, long end,McoHandle gammutSurfaceH);
	TQ3GeometryObject NewPolyGCIELabRef(void);
	TQ3GeometryObject NewPolyLCIELabRef(void);
	TQ3GeometryObject NewLxyAx(void);

	McoStatus ChangeGeometry (McoQ3DType type, long num);
	McoStatus showLabValue(TQ3Point3D *xyzpoint);
	McoStatus get3Dinfo(Point clickPoint);
	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);

};


#endif