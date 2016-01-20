/*********************************************/
/* qd3Dwin.h							 	 */
/* Display 3D gammut surface in window 		 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#ifndef IN_QD3D_WIN
#define IN_QD3D_WIN

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

#include "winevent.h"

// The custum lab attribute type

#define LabAttributeType 'laba'

struct labData {
	double L;
	double a;
	double b;
	};


// The base class

typedef enum {
 QD3D_Dial = 0,	
 QD3D_Box,
 QD3D_Zoom,
 QD3D_Rotate,
 QD3D_Translate,
 QD3D_Info
 };




#define NUM_QD3D_IDS 6
#define MAX_NUM_QD3D_IDS 300

#define doTheRightThing 5000

// A structure that encloses the apple Quickdraw 3D object,
// allows for easy id of objects

#define MAX_NUM_OBJECTS 25

typedef enum {
	MP_GammutSurface = 0, 	// A gammut seurface object
	MP_CIELabRef_PG,		// A refrence object in polygon form
	MP_CIELabRef_PL,		// A refrence object in line form
	MP_CIEFreq,				// Labels for the CIE refrence
	MP_LxyAx,				// Lxy axies
	MP_LxyBox,				// Lxy Box
	MP_Logo,				// MonacoPRINT logo
	MP_GamutSlice,			// A slice through the gamut as used by tweak
	MP_TweakAx,				// a & b axies for tweak
	MP_Circle,				// the circle used in tweak
	MP_Arrow,				// the arrow used in tweak
	MP_TweakMark1,			// the first tweak mark
	MP_TweakMark2,			// the second tweak mark
	MP_BlackBox
	} McoQ3DType;
	
// codes for the below	
typedef enum {	
	QD3D_C_Rotate = 0,
	QD3D_C_Scale,
	QD3D_C_Trans
	};

	


class McoQ3DObject {
private:
TQ3Object			object;
McoQ3DType			type;
long				num;

protected:
public:

McoQ3DObject(TQ3Object o, McoQ3DType t, long num);
Boolean IsObject(TQ3Object o);
Boolean IsTypeNum(McoQ3DType t, long num);

};


class QD3DWin:public virtual baseWin{
private:
protected:
public:
	
 	WindowPtr   the_window;

	
	DocumentRec		Document;
	long		 num_objects;
	McoQ3DObject *objects[MAX_NUM_OBJECTS];
	Rect	box_rect;
	
	short Current_Button;
	
	int	  ids[MAX_NUM_QD3D_IDS];
	float ambient,point,fill;
	
	
	
//public:
	
	QD3DWin();
	~QD3DWin();
	
	void DisposeDocumentData( DocumentPtr theDocument);
	TQ3Status DocumentDraw3DData( DocumentPtr theDocument );
	McoStatus InitDocumentData( DocumentPtr theDocument );
	void DocumentDrawOnscreen(DocumentPtr theDocument, Rect *clipRect);

	McoStatus 	DrawWindow(void);
	McoStatus 	UpdateWindow(void);
	McoStatus   forceUpdateData(void);
	McoStatus 	updateWindowData(int changed);
	McoStatus	ProcessMouseDown(Point ClickPoint,int code);

	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);


	TQ3GeometryObject MarkerFromText(unsigned char *st,double x, double y, double z);

	void DoTransform(int code, double v1, double v2,double v3);
	int16 hit(void);

	void GetRotation(TQ3Matrix4x4 *rotation);
	void SetRotation(TQ3Matrix4x4 *rotation);


	McoQ3DObject *getObject(TQ3Object object);
	void	addObject(TQ3Object object,McoQ3DType type,long num);
	void	removeObject(TQ3Object object);
	virtual McoStatus get3Dinfo(Point clickPoint) {return MCO_SUCCESS;}
	
};


#endif