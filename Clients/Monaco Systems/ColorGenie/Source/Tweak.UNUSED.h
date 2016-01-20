#pragma once

#include "mcostat.h"
//j #include "profiledocinf.h"
//j #include "windowtab.h"
#include "tweak_patch.h"
//j #include "mcoicc.h"

//j #include "Qd3dAll.h"

class TweakTab {
private:
protected:
//j	RefConType	*refCon1;
//j	RefConType	*refCon2;
//j	ProfileDocInf	*doc;
	
	Tweak_Element	*current_tweak;
	int			tweak_num;
	
	double		SliderL;
	double		_minL,_maxL;
	
	int			doneGamut;
	int			startGamut;
	
	double desiredRGB[3];
	
	double printedRGB[3];
	
	double gammutRGB[3];
	
	double	rgb_d[3],rgb_g[3],rgb_p[3];
	
	int 	validWin;
	
	
	long _last_tick_time2;
	long Current_Text;
	
//j	McoXformIcc *lab_rgb_xform;
	
	McoHandle gammutSurfaceH;
	
	void _init();
	
	void _changeToCalib(void);
	void _changeToGamut(void);
	
	void _changeTweakData(int which);
	void _updatSliderL(void);
	
	void _setUpTweak(int which);
	McoStatus _setUpTweaks(void);
	void _setUpTweakMenu(void);
	McoStatus _copyTextFromDial(void);
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
	
	McoStatus SaveTweakedPatches(void);
public:
//j	TweakTab(ProfileDoc *dc,WindowPtr d);	
	~TweakTab(void);
	
	McoStatus updateWindowData(void);
	
	// Get the active region
//j	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
//j	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);
	
	McoStatus CopyParameters(void);
	McoStatus UpdateWindow(void);
	Boolean isMyObject(Point where,short *item);
	Boolean TimerEventPresent(void);
//j	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
	McoStatus SetUpDial(void);


	};
