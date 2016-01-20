////////////////////////////////////////////////////////////////////////////////////////////////
//	The new calibration dialog																  //
//  blackdialog.h																			  //
//  By Peter Zhang, modifieded by James Vogh												  //
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BLACK_DIALOG_H
#define BLACK_DIALOG_H

#include "basewin.h"
#include "spline.h"
#include "splint.h"
#include "profiledocinf.h"
#include "printertype.h"

#define MAX_SIMU_NAMES	(75)
#define MAX_PRINTERTYPES (250)

//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class BlackDialog:public baseWin{

private:
protected:
	
	//data passed to the printdialog
	ConData* _condataptr;
	
	//note!! in order to be compatible with spline and splint function
	//the data point start from 1 instead of 0
	
	//what type of black? None,Light,Medium,Heavy or Maximum
	short _black_type;
	
	
	Str32	_simunames[MAX_SIMU_NAMES];	//simulation names, at most 75
	//added on 1/8 for separation type (GCR or UCR)
	short	_separation_type;
	int		num_sims;
	
	Str32	printernames[MAX_SIMU_NAMES];
	int		num_printers;
	
	//note!! at most, there are 30 hands altogether
	//how many hands need to be drawn
	short _num_hands;
	double _x_hands[31];
	double _y_hands[31];
	double _y2_hands[31];

	//hands in curve window
	double _xc_hands[31];
	double _yc_hands[31];
	

	//how many points need to be drawn so the concentration
	//is enough? I guess 50.
	//lookup table for y points
	double _yd_points[101];

	//points in curve window space
	//which can be draw directly on the curve window
	short _xc_points[51];
	short _yc_points[51];
	
		
	//the size of curve window
	short _left, _bottom, _hor, _ver;
	
	//last site of hand
	Point _phand;
	
	//pointer to input black table
	McoHandle _blackH;

	//pointer to simulation table
	McoHandle _simupatchH;
	FSSpec	_simuFile;

	//clear rect of window
	Rect _clearrect;
	
	//added on 8/25
	long _total_ink_limit;
	long _black_ink_limit;
	
	double *_total_limit_ptr;
	double *_black_limit_ptr;
	
	//Added on 9/28, what type of table, photoshop, ICC, or rgb to rgb
	short _table_type;
	
	//Added on 9/28, what type of gamut comp
	short _gamut_comp_type;
	
	
	int window_size; 	// 0 == small, 1 == big
	
	McoStatus openInkSimulation(void);
	
	McoStatus loadLastPrinter(void);
	McoStatus saveLastPrinter(void);
	
	McoStatus saveprinterfile(void);	
	McoStatus loadprinterfile(void);
	
	McoStatus loadprinterfile(int32 filenum,long ioDirID,short myVRef,unsigned char *filename);

	McoStatus setParameters(void);
	McoStatus getParameters(void);


	void openPrinterTypes(void);
	
	//choose black type	
	void _choose_black_type(void);

	//create initial spline points	
	void _init_spline_points(void);

	// create the points 
	void _spline_points(void);
	
	// Draw the curve 
	void _draw_curvewin(void);

	//convert local position to data in x, y coordinate
	void _from_local_to_data(short h, short v, double *x, double *y);

	// The mouse is down and a handle is being draged
	void _hand_curvewin(void);

	//save to input black table
	McoStatus _saveblack(void);
	
	void _choose_total_limit(void);
	void _choose_black_limit(void);
	void _save_resource(void);
	void _check_limit(short curitem, short lastitem);
	void _scale_spline_points(double scale);
	void _check_limit_immed(short curitem, short lastitem);
	
	// Added by James
	McoStatus DrawColor(void);
	void MyPickAColor(void);
	void _Setradiobutton(short curitem, short lastitem);
	void _Setradiostate(short item, short value);

	// Added by Peter
	McoStatus _loadsimufile(void);
	
	ProfileDocInf *doc;
	short _last_table_type;
	
	PrinterType **printers;
	
	int		current_printer;
	
public:


	BlackDialog(Document *doc);
	~BlackDialog(void);
	
	
	
	//abstract function, should be filled by subclass
	//hit means mouse select contents area
	Boolean alwaysUpdate(void) { return TRUE;}
	McoStatus DrawWindow(void);
	McoStatus UpdateWindow(void);
	McoStatus updateWindowData(int changed);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
	void _Settabletype(void);
	void _Setucr_ucr_gcr_type(void);
};

#endif //