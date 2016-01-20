//////////////////////////////////////////////////////////////////////////////
//  gridcont.cpp															//
//	the conrol for grids													//
//  created 10/14/97														//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#include "gridcont.h"
#include "pascal_string.h"
#include <math.h>

#define Grid_Up		2000
#define Grid_Down	2001
#define Grid_Left	2002
#define Grid_Right	2003
#define Delete_Key	2004

GridControl::GridControl(WindowPtr wp, short *control_ids,int startNum,int nc, int nr, int *ec,int *pr):control(wp)
{
int i,j;
Rect r;
ControlHandle h;
short itemType;

num_cols = nc;
num_rows = nr;

num_d_cols = control_ids[G_NumCols];
num_d_rows = control_ids[G_NumRows];
	
for (i=0; i<NUM_GRID_IDS; i++) ids[i] = control_ids[i];

for (i=0; i<3; i++) if (ids[i] != -1) ids[i] += startNum;
ids[G_SelStart] += startNum;

active_col = -1;
active_row = 0;

last_active_col = -1;
last_active_row = 0;

new_position = 0;

min = 0L;
max = 0L;

data = new double*[num_cols];
if (!data) goto bail;
valid = new short*[num_cols];
if (!valid) goto bail;
min = new double[num_cols];
if (!min) goto bail;
max = new double[num_cols];
if (!max) goto bail;
perc = new int[num_cols];
if (!perc) goto bail;


edit_cols = new int[num_cols];
if (!edit_cols) goto bail;

for (i=0; i<num_cols; i++) data[i] = 0L;

highlight = new short*[num_cols];
if (!highlight) goto bail;

for (i=0; i<num_cols; i++) 
	{
	edit_cols[i] = ec[i];
	perc[i] = pr[i];
	if ((edit_cols[i]) && (active_col == -1)) active_col = i;
	data[i] = new double[num_rows];
	if (!data[i]) goto bail;
	for (j=0; j<num_rows; j++) data[i][j] = 0.0;
	valid[i] = new short[num_rows];
	if (!valid[i]) goto bail;
	for (j=0; j<num_rows; j++) valid[i][j] = 1-edit_cols[i];
	highlight[i] = new short[num_rows];
	if (!highlight[i]) goto bail;
	for (j=0; j<num_rows; j++) highlight[i][j] = 0;
	}
	
contents = new unsigned char **[num_d_cols];
if (!contents) goto bail;	
	
highlight_p = new short*[num_d_cols];
if (!highlight_p) goto bail;
	
for (i=0; i<num_d_cols; i++)
	{
	contents[i] = new unsigned char *[num_d_rows];
	if (!contents[i]) goto bail;
	for (j=0; j<num_d_rows; j++) 
		{
		contents[i][j] = new unsigned char[20];
		if (!contents[i][j]) goto bail;
		contents[i][j][0] = 0;
		}
		
	highlight_p[i] = new short[num_d_rows];
	if (!highlight_p[i]) goto bail;
	for (j=0; j<num_d_rows; j++) highlight_p[i][j] = 0;
	}	
	
//	for (j=0; j<num_rows; j++) highlight[1][j] = 1;
del_x = 0L;
del_y = 0L;
num_del = 0;
	
slider_c = 0;
slider_r = 0;

last_slider_c = -1;
last_slider_r = -1;

if (ids[G_Slider_H] != -1)
	{
	GetDItem ( dp, ids[G_Slider_H], &itemType, (Handle*)&h, &r );
	SetCtlMax (h,(int32)num_cols-num_d_cols);
	}
	
if (ids[G_Slider_V] != -1)
	{
	GetDItem ( dp, ids[G_Slider_V], &itemType, (Handle*)&h, &r );
	SetCtlMax (h,(int32)num_rows-num_d_rows);
	}	

	
return;
bail:
if (data)
	{
	for (i=0; i<num_cols; i++)
		{
		if (data[i]) delete data[i];
		data[i] = 0L;
		}
	delete data;
	data = 0L;
	}
if (valid)
	{
	for (i=0; i<num_cols; i++)
		{
		if (valid[i]) delete valid[i];
		valid[i] = 0L;
		}
	delete valid;
	valid = 0L;
	}	
if (min) delete min;
min = 0L;
if (max) delete max;
max = 0L;

if (edit_cols) delete edit_cols;
edit_cols = 0L;
if (perc) delete perc;
perc = 0L;

if (contents)
	{
	for (i=0; i<num_d_cols; i++) if (contents[i])
		{
		for (j=0; j<num_d_rows; j++) if (contents[i][j]) 
			{
			delete contents[i][j];
			contents[i][j] = 0L;
			}
		delete contents[i];
		contents[i] = 0L;
		}
	delete contents;
	contents = 0L;
	}
	
if (highlight)
	{
	for (i=0; i<num_cols; i++)
		{
		if (highlight[i]) delete highlight[i];
		highlight[i] = 0L;
		}
	delete highlight;
	highlight = 0L;
	}

if (highlight_p)
	{
	for (i=0; i<num_d_cols; i++)
		{
		if (highlight_p[i]) delete highlight_p[i];
		highlight_p[i] = 0L;
		}
	delete highlight_p;
	highlight_p = 0L;
	}

num_cols = 0;
num_rows = 0;
}

GridControl::~GridControl(void)
{
int i,j;

if (data)
	{
	for (i=0; i<num_cols; i++)
		{
		if (data[i]) delete data[i];
		data[i] = 0L;
		}
	delete data;
	data = 0L;
	}
if (valid)
	{
	for (i=0; i<num_cols; i++)
		{
		if (valid[i]) delete valid[i];
		valid[i] = 0L;
		}
	delete valid;
	valid = 0L;
	}		
if (min) delete min;
min = 0L;
if (max) delete max;
max = 0L;

if (edit_cols) delete edit_cols;
edit_cols = 0L;
if (perc) delete perc;
if (contents)
	{
	for (i=0; i<num_d_cols; i++) if (contents[i])
		{
		for (j=0; j<num_d_rows; j++) if (contents[i][j]) 
			{
			delete contents[i][j];
			contents[i][j] = 0L;
			}
		delete contents[i];
		contents[i] = 0L;
		}
	delete contents;
	contents = 0L;
	}
	
if (highlight)
	{
	for (i=0; i<num_cols; i++)
		{
		if (highlight[i]) delete highlight[i];
		highlight[i] = 0L;
		}
	delete highlight;
	highlight = 0L;
	}

if (highlight_p)
	{
	for (i=0; i<num_d_cols; i++)
		{
		if (highlight_p[i]) delete highlight_p[i];
		highlight_p[i] = 0L;
		}
	delete highlight_p;
	highlight_p = 0L;
	}	

if (del_x) delete del_x;
del_x = 0L;
if (del_y) delete del_y;
del_y = 0L;	
	
perc = 0L;
dp = 0L;	
}


// set up the delete array
McoStatus 	GridControl::SetUpDelete(int nd,double *x,double *y)
{

int i;

if (del_x) delete del_x;
if (del_y) delete del_y;
num_del = nd;

del_x = new double[nd];
if (!del_x) return MCO_MEM_ALLOC_ERROR;
del_y = new double[nd];
if (!del_x) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<nd; i++)
	{
	del_x[i] = x[i];
	del_y[i] = y[i];
	}
return MCO_SUCCESS;
}


McoStatus 	GridControl::DeleteHighlightedItems(void)
{
int i,j,i2,j2,n;

for (i=0; i<num_cols; i++)
	for (j= 0; j<num_rows; j++) if (highlight[i][j])
		{
		highlight[i][j] = 0;
		if (num_del == 0)
			{
			valid[i][j] = 0;
			}
		else
			{
			for (i2=0; i2<num_del; i2++)
				{
				if (data[0][0] >= del_x[i2]) n = 0;
				else 
					{
					for (j2=0; j2<num_rows-1; j2++)
						{
						if ((data[0][j2] < del_x[i2]) && (data[0][j2+1] >= del_x[i2]))
							{
							if (fabs(data[0][j2] - del_x[i2]) < fabs(data[0][j2+1] - del_x[i2])) n = j;
							else n = j2+1;
							break;
							}
						}
					if (j2 == num_rows-1) n = j2;
					}
				if (j == n) break;
				}
			if ((j == n) && (i2 < num_del)) data[i][j] = del_y[i2];
			else valid[i][j] = 0;
			}
		}
		
last_slider_c = -1;
last_slider_r = -1;		
		
return MCO_SUCCESS;
}


// copy the data into the grid
McoStatus 	GridControl::CopyIntoGrid(int cl_x,int cl_y,int num_h,double *y_mm, double *datax,double *datay)
{
int i,j,n,c,r;
Str255	s;
int Changed = 0;
int	*tvalid;

if (num_cols == 0) return MCO_FAILURE;

min[cl_y] = y_mm[0];
max[cl_y] = y_mm[1];



tvalid = new int[num_rows];


for (i=0; i<num_rows; i++) 
	{
	tvalid[i] = valid[cl_y][i];
	valid[cl_y][i] = 0;
	}

if (cl_x != cl_y) 
	{
	for (i=0; i<num_h; i++)
		{
		if (data[cl_x][0] >= datax[i]) n = 0;
		else 
			{
			for (j=0; j<num_rows-1; j++)
				{
				if ((data[cl_x][j] < datax[i]) && (data[cl_x][j+1] >= datax[i]))
					{
					if (fabs(data[cl_x][j] - datax[i]) < fabs(data[cl_x][j+1] - datax[i])) n = j;
					else n = j+1;
					break;
					}
				}
			if (j == num_rows-1) n = j;
			}
			
							
		data[cl_y][n] = datay[i];
		valid[cl_y][n] = 1;
		
		c = cl_y-slider_c;
		r = n-slider_r;
		if ((c>=0) && (c<num_d_cols) && (r>=0) && (r<num_d_rows))
			{
			DoubleToString (data[cl_y][n], s,perc[cl_y]);
			if (s[0] > 18) s[0] = 18;
			if (!cmp_str(contents[c][r],s))
				{
				copy_str(contents[c][r],s);
				Changed = 1;
				}
			}
		}
	}
else
	{
	for (i=0; i<num_h; i++)
		{
		
		data[cl_x][i] = datay[i];
		valid[cl_x][i] = 1;
		
		c = cl_x-slider_c;
		r = i-slider_r;
		if ((c>=0) && (c<num_d_cols) && (r>=0) && (r<num_d_rows))
			{
			DoubleToString (data[cl_x][i], s,perc[cl_x]);
			if (s[0] > 18) s[0] = 18;
			if (!cmp_str(contents[c][r],s))
				{
				copy_str(contents[c][r],s);
				Changed = 1;
				}
			}
		}
	}
	
c = cl_y-slider_c;
if ((c>=0) && (c<num_d_cols)) for (i=0; i<num_d_rows; i++)
	{
	r = i + slider_r;
	if (!valid[cl_y][r]) copy_str(contents[c][i],"\p ");
	}		
	
for (i=0; i<num_rows; i++) 
	{
	if (tvalid[i] != valid[cl_y][i]) Changed = 1;
	}
	
delete tvalid;	
	
if (Changed) UpdateControl();	
return MCO_SUCCESS;
}

// copy the data out of the grid
McoStatus 	GridControl::CopyOutofGrid(int cl_x,int cl_y,int *num_h,double *datax, double *datay)
{
int i,c = 0;

if (num_cols == 0) return MCO_FAILURE;


for (i=0; i< num_rows; i++)
	{
	if (valid[cl_y][i]) 
		{
		datax[c] = data[cl_x][i];
		datay[c] = data[cl_y][i];
		c++;
		}
	}

*num_h = c;
return MCO_SUCCESS;
}

McoStatus 	GridControl::CopyXOutofGrid(int *nx,double **datax)
{
int i;
double *datax2;

*nx = num_rows;

datax2 = new double[num_rows];
*datax = datax2;

for (i=0; i<num_rows; i++)
	{
	datax2[i] = data[0][i];
	}
	
return MCO_SUCCESS;
}

void GridControl::setActiveItem(int *item)
{
int  i,n,c,r;

if ((item[0] <0) || (item[1] <0)) return;

c = item[0];
n = 0;
for (i=0; i<num_rows; i++) if (valid[c][i])
	{
	if (n == item[1]) break;
	n++;
	}

if (i == num_rows) return;
r = i;

active_col = c;
active_row = r;

if (c - slider_c < 0) slider_c = 0;
if (c - slider_c >= num_d_cols) slider_c = c - num_d_cols +1;

if (r - slider_r < 0) slider_r = 0;
if (r - slider_r >= num_d_rows) slider_r = r - num_d_rows +1; 
UpdateControl();
}

// is this one of the controls sliders
Boolean GridControl::isMyObject(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;
if (titem == ids[G_Slider_V])
	{
	if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
		{
		*item = titem;
		SetPort(oldP);
		return TRUE;
		}
	}
if (titem == ids[G_Slider_H])
	{
	if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
		{
		*item = titem;
		SetPort(oldP);
		return TRUE;
		}
	}	
SetPort(oldP);
return FALSE;
}


// is the clickpoint in the grid?
Boolean GridControl::_isMyItem(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;
int r,c;
Boolean flag = FALSE;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;

*item = titem;

titem -= ids[G_FirstCol];

if (titem >= 0)
	{
	c = titem/num_d_rows;
	r = titem%num_d_rows;

	if (c < num_cols) if (edit_cols[c]) flag = TRUE;
	}

SetPort(oldP);
return flag;
}

// redraw the control
McoStatus 	GridControl::CopyParameters(void)
{
int i,j;
int r,c;
Rect rec;
ControlHandle h;
short itemType;
short item;


// set the sliders
if (ids[G_Slider_H] != -1)
	{
	GetDItem ( dp, ids[G_Slider_H], &itemType, (Handle*)&h, &rec );
	SetCtlValue (h,(int32)slider_c);
	SetCtlMax (h,(int32)num_cols-num_d_cols);
	}
	
if (ids[G_Slider_V] != -1)
	{
	GetDItem ( dp, ids[G_Slider_V], &itemType, (Handle*)&h, &rec );
	SetCtlValue (h,(int32)slider_r);
	SetCtlMax (h,(int32)num_rows-num_d_rows);
	}
	
//UpdateControl();	
return MCO_SUCCESS; 	
}

McoStatus GridControl::_updateContents(void)
{
unsigned char ***tempContents;
int i,j;
Str255	s;
int r,c;
McoStatus	state = MCO_SUCCESS;

// copy the contents of contents
tempContents = new unsigned char **[num_d_cols];
if (!tempContents)  {state = MCO_MEM_ALLOC_ERROR; goto bail;}
	
for (i=0; i<num_d_cols; i++)
	{
	tempContents[i] = new unsigned char *[num_d_rows];
	if (!tempContents[i]) {state = MCO_MEM_ALLOC_ERROR; goto bail;}
	for (j=0; j<num_d_rows; j++) 
		{
		tempContents[i][j] = new unsigned char[20];
		if (!tempContents[i][j]) {state = MCO_MEM_ALLOC_ERROR; goto bail;}
		copy_str(tempContents[i][j],contents[i][j]);
		}
	}	


// write new text into contents

for (i=0; i<num_d_cols; i++)
	{
	for (j=0; j<num_d_rows; j++) 
		{
		if ((last_slider_r != -1) && (last_slider_c != -1))
			{
			r = j+slider_r - last_slider_r;
			c = i+slider_c - last_slider_c;
			if ((r>=0) && ( r < num_d_rows) && (c>=0) && (c < num_d_cols))
				{
				copy_str(contents[i][j],tempContents[c][r]);
				}
			else
				{
				c = i+slider_c;
				r = j+slider_r;
				if ((c>-1) && (c<num_cols) && (r>-1) && (r<num_rows))
					{
					DoubleToString (data[c][r], s,perc[c]);
					if (s[0] > 18) s[0] = 18;
					if (valid[c][r]) copy_str(contents[i][j],s);
					else copy_str(contents[i][j],"\p ");
					}
				else copy_str(contents[i][j],"\p ");
				}
			}
		else
			{
			c = i+slider_c;
			r = j+slider_r;
			if ((c>-1) && (c<num_cols) && (r>-1) && (r<num_rows))
				{
				DoubleToString (data[c][r], s,perc[c]);
				if (s[0] > 18) s[0] = 18;
				if (valid[c][r]) copy_str(contents[i][j],s);
				else copy_str(contents[i][j],"\p ");
				}
			else copy_str(contents[i][j],"\p ");
			}
		}
	}
	
last_slider_r = slider_r;
last_slider_c = slider_c;	
	
if (tempContents)
	{
	for (i=0; i<num_d_cols; i++) if (tempContents[i])
		{
		for (j=0; j<num_d_rows; j++) if (tempContents[i][j]) 
			{
			delete tempContents[i][j];
			tempContents[i][j] = 0L;
			}
		delete tempContents[i];
		tempContents[i] = 0L;
		}
	delete tempContents;
	tempContents = 0L;
	}
	
bail:	
	
return state;

}

// redraw the control
McoStatus 	GridControl::UpdateControl(void)
{
int i,j;
int r,c;
Rect rec;
ControlHandle h;
short itemType;
short item;
Str255	temp;
short high;
short	la;
RGBColor	c1,c2,c3,fc,bc;
int		lar,lac;

GetForeColor(&fc);
GetBackColor(&bc);
// RED
c1.red = 0;
c1.green = 0;
c1.blue = 0;
c2.red = 65535;
c2.green = 65535;
c2.blue = 65535;
c3 = bc;

lar = last_active_row-slider_r;
lac = last_active_col-slider_c;

if (active_row != last_active_row) new_position = 1;
if (active_col != last_active_col) new_position = 1;
last_active_row = active_row;
last_active_col = active_col;

// set the sliders
if (ids[G_Slider_H] != -1)
	{
	GetDItem ( dp, ids[G_Slider_H], &itemType, (Handle*)&h, &rec );
	SetCtlValue (h,(int32)slider_c);
	}
	
if (ids[G_Slider_V] != -1)
	{
	GetDItem ( dp, ids[G_Slider_V], &itemType, (Handle*)&h, &rec );
	SetCtlValue (h,(int32)slider_r);
	}
	
// set the active item
item = ids[G_FirstCol]+(active_col-slider_c)*num_d_rows + (active_row-slider_r);
		
SelIText(dp,item,0,32767);

if ((last_slider_c != slider_c) || (last_slider_r != slider_r)) _updateContents();

// set the text
for (i=0; i<num_d_cols; i++)
	for (j=0; j<num_d_rows; j++)
		{
		c = i+slider_c;
		r = j+slider_r;
		GetItemText(ids[G_FirstCol]+i*num_d_rows+j,temp);
		high = 0;
		la = 0;
		if ((highlight[c][r]) && (!highlight_p[i][j])) high = 1;
		if ((!highlight[c][r]) && (highlight_p[i][j])) high = 1;
		if ((i == lac) && (j == lar)) la = 1;
		if ((!cmp_str(temp,contents[i][j])) || (high) || (la))
			{
			if (highlight[c][r])
				{
				RGBForeColor(&c2);
				RGBBackColor(&c1);
				highlight_p[i][j] = 1;
				}		
			else
				{
				RGBForeColor(&c1);
				RGBBackColor(&c3);
				highlight_p[i][j] = 0;
				}			
			if ((high) || (la)) SetItemTextForce(ids[G_FirstCol]+i*num_d_rows+j,contents[i][j]);
			else SetItemText(ids[G_FirstCol]+i*num_d_rows+j,contents[i][j]);
			
			}

		}
RGBForeColor(&fc);
RGBBackColor(&bc);		
		
return MCO_SUCCESS;
}


// read text out of the dialog
McoStatus 	GridControl::_copyOutText(Boolean *changed)
{
int i,j;
Boolean ch = FALSE;
int r,c;
Str255	temp;

for (i=0; i<num_d_cols; i++)
	for (j=0; j<num_d_rows; j++)
		{
		c = i+slider_c;
		r = j+slider_r;
		GetItemText(ids[G_FirstCol]+i*num_d_rows+j,temp);
		if (!cmp_str(temp,contents[i][j]))
			{
			ch = GetItemTextFromNum(ids[G_FirstCol]+i*num_d_rows+j,&data[c][r],min[c],max[c],perc[c]);
			valid[c][r]=1;
			if (temp[0] > 19) temp[0] = 19;
			copy_str(contents[i][j],temp);
			}
		*changed = *changed | ch;
		}
return MCO_SUCCESS;
}


// handle keyboard events
Boolean 	GridControl::KeyFilter(EventRecord *theEvent,short *itemHit )
{
	switch ( (theEvent->message) & charCodeMask ) {
		case 0x0d:	// Return pressed or ...
		case 0x03:	// ... Enter pressed
			*itemHit = Grid_Down;
			return( TRUE );	// Note: pascal-style TRUE
		case 0x1E:
			*itemHit = Grid_Up;
			return( TRUE );
		case 0x1F:
			*itemHit = Grid_Down;
			return( TRUE );
		case 0x1C:
			*itemHit = Grid_Left;
			return( TRUE );
		case 0x1D:
			*itemHit = Grid_Right;
			return( TRUE );
		case 0x08:
			*itemHit = Delete_Key;
			return (TRUE);
		} 
	
		
return( FALSE );	// all others
}

// update either the active item or the slider
McoStatus 	GridControl::_updatePosition(int f)
{

if (f==1) // update the slider
	{
	if (active_col < slider_c) slider_c = active_col;
	if (active_col >= slider_c + num_d_cols) slider_c = 1+active_col-num_d_cols;
	if (active_row < slider_r) slider_r = active_row;
	if (active_row >= slider_r + num_d_rows) slider_r = 1+active_row-num_d_rows;
	}
else  // update the active item
	{
	if (active_col < slider_c) active_col = slider_c;
	if (active_col >= slider_c + num_d_cols) active_col = slider_c + num_d_cols -1;
	if (active_row < slider_r) active_row = slider_r;
	if (active_row >= slider_r + num_d_rows) active_row = slider_r + num_d_rows -1;	
	}
}

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

McoStatus GridControl::setActive(int act_r,int act_c)
{
active_row = act_r;
active_row = MinVal(active_row,num_rows-1);
active_row = MaxVal(active_row,0);
active_col = act_c;
active_col = MinVal(active_col,num_cols-1);
active_col = MaxVal(active_col,0);
_updatePosition(1);
UpdateControl();
return MCO_SUCCESS;
}

void GridControl::getActive(int *act_r,int *act_c)
{
*act_r = active_row;
*act_c = active_col;
}

int GridControl::newPosition(void)
{
int newp;

newp = new_position;
new_position = 0;
return newp;
}




McoStatus	GridControl::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed,int *didit)
{
int		i,j,col;
Changed = FALSE;
*didit = FALSE;
Point	mpoint;
short	titem;

	// check to see if text has changed
	_copyOutText(&Changed);
	if (item == Grid_Down)
		{
		*didit = TRUE;
		active_row++;
		if (active_row >= num_rows)
			{
			if (active_col < num_cols-1)
				{
				active_col++;
				active_row = 0;
				}
			else active_row--;
			}
		_updatePosition(1);
		UpdateControl();
		}
	else if (item == Grid_Up)
		{
		*didit = TRUE;
		active_row--;
		if (active_row < 0)
			{
			if ((active_col > 0) && (edit_cols[active_col-1]))
				{
				active_row = num_rows-1;
				active_col--;
				}
			else active_row++;
			}
		_updatePosition(1);
		UpdateControl();
		}
	else if (item == Grid_Left)
		{
		*didit = TRUE;
		active_col--;
		if (active_col<0) active_col = 0;
		_updatePosition(1);
		UpdateControl();
		}
	else if (item == Grid_Right)
		{
		*didit = TRUE;
		active_col++;
		if (active_col>num_cols-1) active_col = num_cols-1;
		_updatePosition(1);
		UpdateControl();
		}
	else if (item == Delete_Key)
		{
		DeleteHighlightedItems();
		Changed = 1;
		UpdateControl();
		}
	else if (Changed)
		{
		*didit = TRUE;
		}
	// did user click within the grid
	else if (_isMyItem(clickPoint,&titem))
		{
		*didit = TRUE;
		active_row = slider_r+(titem-ids[G_FirstCol])%num_d_rows;
		active_col = slider_c+(titem-ids[G_FirstCol])/num_d_rows;
		UpdateControl();
		}
	else if ((item == ids[G_Slider_V]) && (ids[G_Slider_V] != -1))
		{
		*didit = TRUE;
		GetMouse(&mpoint);
		do {
			slider_r = GetSlider(mpoint,item);
			_updatePosition(0);
			UpdateControl();
			GetMouse(&mpoint);
			} while ( StillDown ( ) );
		}		
	else if ((item == ids[G_Slider_H]) && (ids[G_Slider_H] != -1))
		{
		*didit = TRUE;
		GetMouse(&mpoint);
		do {
			slider_c = GetSlider(mpoint,item);
			_updatePosition(0);
			UpdateControl();
			GetMouse(&mpoint);
			} while ( StillDown ( ) );
		}
	else if ((item >= ids[G_SelStart]) && (item < ids[G_SelStart]+num_d_cols))
		{
		col = item - ids[G_SelStart] - slider_c;
		if (edit_cols[col])
			{
			active_col = col;
			for (i=0; i<num_cols; i++)
				for (j=0; j<num_rows; j++)
					{
					if (i != col) highlight[i][j] = 0;
					else  highlight[i][j] =  1 - highlight[i][j];
					}
			UpdateControl();
			}
		}

changed = changed | Changed;
return MCO_SUCCESS;		
}								