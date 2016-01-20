//////////////////////////////////////////////////////////////////////////////
//  gridcont.h																//
//	the conrol for grids													//
//  created 10/14/97														//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_GRIDCONTROL
#define IN_GRIDCONTROL

#include "control.h"

typedef enum {
	G_Slider_H = 0,
	G_Slider_V,
	G_FirstCol,
	G_NumRows,
	G_NumCols,
	G_SelStart
	};

#define NUM_GRID_IDS (G_SelStart+1)

class GridControl:public control {
private:
protected:


int		num_cols;
int		num_rows;

int		num_d_cols;
int		num_d_rows;

int 	active_col;
int		active_row;

int 	last_active_col;
int		last_active_row;

int 	new_position;

int		slider_c;
int		slider_r;

int		last_slider_c;
int		last_slider_r;


int		*edit_cols;

double	**data;
short	**valid;

short	**highlight;
short	**highlight_p;

double	*del_x;
double	*del_y;
int 	num_del;

double	*min;
double	*max;

int		*perc;



unsigned char ***contents;

short	ids[NUM_GRID_IDS];

// is the clickpoint in the grid?
Boolean _isMyItem(Point where,short *item) ;
// read text out of the dialog
McoStatus 	_copyOutText(Boolean *changed);

// update either the active item or the slider
McoStatus 	_updatePosition(int f);

McoStatus _updateContents(void);

public:

GridControl(WindowPtr wp, short *control_ids,int startNum,int nc, int nr, int *ec,int *pr);	
~GridControl(void);

// is this one of the controls sliders
Boolean isMyObject(Point where,short *item);

McoStatus 	CopyParameters(void);

// set up the delete array
McoStatus 	SetUpDelete(int nd,double *x,double *y);

// update the data in the screen
McoStatus 	UpdateControl(void);

McoStatus 	DeleteHighlightedItems(void);

McoStatus 	CopyIntoGrid(int cl_x,int cl_y,int num_h,double *y_mm, double *datax,double *datay);
McoStatus	CopyOutofGrid(int cl_x,int cl_y,int *num_h,double *datax, double *datay);
McoStatus 	CopyXOutofGrid(int *nx,double **datax);

void 		setActiveItem(int *item);

// handle keyboard events
Boolean 	KeyFilter(EventRecord *theEvent,short *itemHit );

// set the active position
McoStatus setActive(int act_r,int act_c);

// get the active position
void getActive(int *act_r,int *act_c);

// return true if the position has been changed
int newPosition(void);

// do the events for the control
McoStatus	DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed,int *didit);

};

#endif