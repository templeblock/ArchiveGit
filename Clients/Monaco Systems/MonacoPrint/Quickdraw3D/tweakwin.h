/*********************************************/
/* tweakwin.h							 	 */
/* Selectivly adjust colors					 */
/* Jun 3 1996							     */
/* New version used QD 3D					 */
/* Aug 20 1997								 */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#ifndef IN_TWEAK_WIN
#define IN_TWEAK_WIN

#include "mcomem.h"
#include "mcostat.h"
#include "xyztorgb.h"

#include "winevent.h"
#include <QDOffscreen.h>
#include "tweak_patch.h"
#include "profiledocinf.h"
#include "techkon.h"

#include "QD3D.h"
#include "QD3DMath.h"
#include "QD3DDrawContext.h"
#include "QD3DShader.h"
#include "QD3DTransform.h"
#include "QD3DGroup.h"
#include "QD3DCamera.h"
#include "xyztorgb.h"
#include "qd3Dwin.h"
#include "mcoiccmac.h"

	
#define NUM_TWWN_IDS (6)
	
// window is a 3D window
// Qickdraw 3D is used to display the items in the pick window
// Color interpolation is the primary advantage of using QD3D
class TweakWin:public QD3DWin{
private:
protected:
	
	RefConType *refCon1,*refCon2;
	ProfileDocInf	*doc;
	
	Tweak_Element	*current_tweak;
	int			tweak_num;
	
	Tweak_Element	**backupTweaks;
	int				numbackupTweaks;
	
	double desiredRGB[3];
	
	double printedRGB[3];
	
	double gammutRGB[3];
	
	double SliderL;
	short		*_indexlut;
	double		_minL,_maxL;

	int Current_Text;
	int32	_last_tick_time2;
	
	McoXformIccMac *lab_rgb_xform;
	
	McoHandle gammutSurfaceH;
	
	
	double	rgb_d[3],rgb_p[3],rgb_g[3];
	
	Boolean validWin;
	
	McoStatus _backupTweaks(void);
	McoStatus _restoreTweaks(void);

	McoStatus _setUpTweaks(void);

	void _changeToCalib(void);
	void _changeToGamut(void);
	
	void _changeTweakData(int which);
	void _updatSliderL(void);
	
	void _setUpTweak(int which);
	void _setUpTweakMenu(void);
	McoStatus _copyTextFromDial(Boolean *Changed);
	McoStatus _copyTextToDial(void);
	
	void _checkMaxVals(double *lab);
	void ConvertToRGB(double *lab,double *rgb);
	void ConvertToLAB(double *rgb,double *lab);
	void ConvertToGammutC(double *lab,double *labc);
	int isColorInGamut(double *lab);
	
	void _converttoTwPrint(double *lab,double *labo);
	void _converttoTwGamut(double *lab,double *labo);
	
	McoStatus NewTweak(void);
	McoStatus DeleteTweak(void);
	
	void Clear3DWin(void);
	
	// build a set of luts that is used to find the index into a gamutsurface
	McoStatus buildIndexLut(double *gammutSurface);
	long getgammutSurfaceIndex(double L,double h);
	
	McoStatus get3Dinfo(Point clickPoint,double *lab,int gamutCheck,int *ingamut);
	
	
public:
	
	TweakWin(ProfileDocInf *dc);
	~TweakWin(void);
	
	TQ3GeometryObject 	BlackBox(void);
	
	McoStatus AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject);
	McoStatus	ModifyGammutObject(McoHandle gammutSurfaceH);
	TQ3GeometryObject NewGammutObject(McoHandle gammutSurfaceH);
	TQ3GeometryObject New_ab_ax(void);
	short SetUpCircle(TQ3PolyLineData *linedata,double *lab1,double Lr,double Cr,double currentL);

	McoStatus ModifyCircle(double *lab,double Lr,double Cr,double currentL);
	TQ3GeometryObject NewCircle(double *lab1,double Lr,double Cr,double currentL);
	
	McoStatus ModifyArrow(double *lab1,double *lab2);
	// build an arrow
	TQ3GeometryObject NewArrow(double *lab1,double *lab2);
	
	TQ3GeometryObject NewPolyy(double *disp,double *polypoints,double scale,int num_pp,TQ3ColorRGB *color);
	
	McoStatus ModifyPoly(double *disp,double *polypoints,double scale,int num_pp);
	// make a cross 
	TQ3GeometryObject NewCross(double *lab);
	// modify a cross
	McoStatus ModifyCross(double *lab);

	
	// change the geometry of the object type with number num
	McoStatus ChangeGeometry (McoQ3DType type, long num);
	
	McoStatus updateWindowData(int changed);
	
	McoStatus DrawWinowParts(void);
	McoStatus DrawWindow(void);
	
	McoStatus SaveTweakedPatches(void);
	
	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);
	
	McoStatus CopyParameters(void);
	McoStatus UpdateWindow(void);
	Boolean isMyObject(Point where,short *item);
	Boolean TimerEventPresent(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
	McoStatus SetUpDial(void);
};


#endif