//////////////////////////////////////////////////////////////////////////////
// calibtab.cpp																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "calibtab.h"
#include "pascal_string.h"
#include "fullname.h"
#include "think_bugs.h"
#include "curvecont.h"
#include "gridcont.h"
#include "errordial.h"

#define ST_DITL_ID	6001

#define GRAPH_ID	1
#define GRAPH_PICT	145

#define IMPORT		58
#define EXPORT		59
#define DESCRIPTION 61

#define MEASURE 	64

#define BUTTON_STRINGS 	17010
#define STOP_BUTTON  	2
#define MEASURE_BUTTON 	3


#define CURVE_IDS {1,-1,-1,-1,-1,63,6102,-1,1,3,145,5,36,36,13}
#define GRID_IDS {-1,52,2,10,5,53}

CalibTab::CalibTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{
	int i;
	int	num_cmp[1];
	double	hmi[1],hma[1],vmi[1],vma[1];
	int		ec[5],pr[5];
	int		maxhand,alldata;

	doc = dc;
	
	short	curve_ids[] = CURVE_IDS;
	short	grid_ids[] = GRID_IDS;


// set up the slider

	
// set up the controls


	numControls = 0;
	controls[0] = new CurveControl(dp,curve_ids,startNum,0,1,(baseWin*)this,0);
	if (controls[0]) 
		{
		numControls++;
		num_cmp[0] = 4;
		hmi[0] = 0;
		hma[0] = 100;
		vmi[0] = 0;
		vma[0] = 2.0;
		((CurveControl*)(controls[0]))->SetUpCurves(1,num_cmp,hmi,hma,vmi,vma);
		}
		
		
		
	// set up editable fields	
	for (i=0; i<5; i++) ec[i] = 1;
	ec[0] = 0;	
	
	for (i=0; i<5; i++) pr[i] = 3;
	pr[0] = 0;	
	
	alldata = 1;
	for (i=0; i<4; i++) if (doc->printData->setting.linearX[i*MAX_LINEAR_HAND+doc->printData->setting.numLinearHand[i]-1] != 100) alldata = 0;
	
	if ((doc->printData->setting.numLinearHand[3] > 0) && (alldata))
		{
		maxhand = 0;
		for (i=0; i<4; i++) if (doc->printData->setting.numLinearHand[i] >= maxhand) maxhand = doc->printData->setting.numLinearHand[i];
		numhand = maxhand;
		}
	else numhand = 26;
	
	// create the grid control
	controls[1] = new GridControl(dp,grid_ids,startNum,5,numhand,ec,pr);
	if (controls[1])  numControls++;	
	
	//copy the data into the controls
	CopyIntoControls(-1);
	
	// set up the parameters

	CopyParameters();
	
	patchD = 0L;
	measure = 0L;
	
	last_changed = 0;
	
	_last_tick_time2 = 0;
}


CalibTab::~CalibTab(void)
{
int i;
if (patchD) delete patchD;
if (measure) delete measure;
for (i=0; i<numControls; i++) if (controls[i]) delete controls[i];
}

McoStatus CalibTab::setUpNewGrid(int newhand)
{
	short	grid_ids[] = GRID_IDS;
	int		ec[5],pr[5];
	int 	i;

	// set up editable fields	
	for (i=0; i<5; i++) ec[i] = 1;
	ec[0] = 0;	
	
	for (i=0; i<5; i++) pr[i] = 3;
	pr[0] = 0;	

	if (controls[1]) delete controls[1];

	numhand = newhand;
	controls[1] = new GridControl(dp,grid_ids,startNum,5,numhand,ec,pr);
	if (controls[1]) return MCO_SUCCESS;
	return MCO_MEM_ALLOC_ERROR;
}


McoStatus CalibTab::setUpMeasure(void)
{
McoStatus state = MCO_SUCCESS;
int i;

if (measure) delete measure;
measure = 0L;
if (patchD) delete patchD;

patchD = new RawData;
if (!patchD) return MCO_MEM_ALLOC_ERROR;

state = patchD->SetUp(CMYKPatches,4,0,26,0L);
if (state) return state;

for (i=0; i<26; i++) patchD->format.linearValues[i] = i*4;

switch (((ProfileDocInf*)doc)->defaults->input_type) {
	case DT_DTP51:			// strip readers
	case DT_XriteDTP41:
		patchD->patches.strips = 1;
		patchD->patches.sheets = 2;
		patchD->patches.rows = 16;
		patchD->patches.columns = 7;
		patchD->patches.starting = 0;
		patchD->patches.direction = 1;
		patchD->patches.width = 142.5;
		patchD->patches.height = 198.5;
		break;
	default:			// everything else
		patchD->patches.strips = 0;
		patchD->patches.sheets = 100000;
		patchD->patches.rows = 13;
		patchD->patches.columns = 8;
		patchD->patches.starting = 0;
		patchD->patches.direction = 1;
		patchD->patches.width = 142.5;
		patchD->patches.height = 198.5;
		break;
	}


measure = new measureDevice((ProfileDocInf*)doc,patchD,0);
if (!measure) return MCO_MEM_ALLOC_ERROR;
if (measure->state == MCO_CANCEL) return MCO_SUCCESS;
if (measure->state) return measure->state;


state = measure->DoStart();
if (state) return state;

if (numhand != 26) state = setUpNewGrid(26);
if (state) return state;

state = CopyPatchIntoDoc();

return state;
}

McoStatus CalibTab::CopyPatchIntoDoc(void)
{
McoStatus state;
double		*data;
int LAB = 0;
double	t,t2,t3;
double  max[6];
int32	s;
int i,j;
double	paper[3];

if (!patchD) return MCO_OBJECT_NOT_INITIALIZED;

if (!patchD->linearDataH) return MCO_OBJECT_NOT_INITIALIZED;

data = (double *)McoLockHandle(patchD->linearDataH);

s = patchD->format.numLinear;

//  see if data is in lab or density format
for (i=0; i<patchD->format.numc; i++)
	{
	for (j=0; j<patchD->format.numLinear; j++)
		{
		if ((fabs(data[i*(s)*3+j*3+1]) > 0.01) && (fabs(data[i*(s)*3+j*3+1]) < 999))  LAB = 1;
		if ((fabs(data[i*(s)*3+j*3+2]) > 0.01) && (fabs(data[i*(s)*3+j*3+1]) < 999))  LAB = 1;
		}
	}

if (LAB) patchD->getPaperLab(paper);

for (i=0; i<patchD->format.numc; i++)
	{
	doc->printData->setting.numLinearHand[i] = 0;
	doc->printData->setting.linearX[0] = 0;
	doc->printData->setting.linearY[0] = 0;
	max[i] = 2.0;	
	for (j=0; j<s; j++) 
		{
		if (data[i*s*3+j*3] > -999)
			{
			doc->printData->setting.numLinearHand[i]++;
			if (j == 0) doc->printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 0.0;
			else if (LAB)
				{
				t = data[i*(s)*3+j*3]-paper[0];	
				t2 = t*t;
				t = data[i*(s)*3+j*3+1]-paper[1];	
				t2 += t*t;
				t = data[i*(s)*3+j*3+2]-paper[2];	
				t2 += t*t;
				t3 = sqrt(t2)/65.0;
				if (t3 > max[i]) max[i] = t3;
				doc->printData->setting.linearY[i*MAX_LINEAR_HAND+j] = t3;
				}
			else doc->printData->setting.linearY[i*MAX_LINEAR_HAND+j] = data[i*(s)*3+j*3];
			doc->printData->setting.linearX[i*MAX_LINEAR_HAND+j] = patchD->format.linearValues[j];
			}
		}
	for (j=0; j<s; j++)
		{
		doc->printData->setting.linearY[i*MAX_LINEAR_HAND+j] = 2.0*doc->printData->setting.linearY[i*MAX_LINEAR_HAND+j]/max[i];
		}		
	}		

CopyIntoControls(-1);

return MCO_SUCCESS;
}

void CalibTab::CopyIntoControls(int cnt_num)
{
int i,alldata;
double	XVales[26];
double	mm[2];

if (!controls[0]) return;

if (cnt_num != -1) doc->changedLinearTone = 1;

	if ((cnt_num != 0) || (cnt_num == -1))
		{

		((CurveControl*)(controls[0]))->CopyIntoCurves(0,0,doc->printData->setting.numLinearHand[0],0,100,0,2.0,
			doc->printData->setting.linearX,doc->printData->setting.linearY);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,1,doc->printData->setting.numLinearHand[1],0,100,0,2.0,
			&doc->printData->setting.linearX[MAX_LINEAR_HAND],&doc->printData->setting.linearY[MAX_LINEAR_HAND]);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,2,doc->printData->setting.numLinearHand[2],0,100,0,2.0,
			&doc->printData->setting.linearX[MAX_LINEAR_HAND*2],&doc->printData->setting.linearY[MAX_LINEAR_HAND*2]);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,3,doc->printData->setting.numLinearHand[3],0,100,0,2.0,
			&doc->printData->setting.linearX[MAX_LINEAR_HAND*3],&doc->printData->setting.linearY[MAX_LINEAR_HAND*3]);

		}
if (!controls[1]) return;

	if  ((cnt_num != 1) || (cnt_num == -1))
		{
		
		mm[0] = 0;
		mm[1] = 100;
		
		alldata = 1;
		for (i=0; i<4; i++) if (doc->printData->setting.linearX[i*MAX_LINEAR_HAND+doc->printData->setting.numLinearHand[i]-1] != 100) alldata = 0;

		// if data is already present then use its increment (whatever it is)
		if ((doc->printData->setting.numLinearHand[3] > 0) && (alldata))
			{
			((GridControl*)(controls[1]))->CopyIntoGrid(0,0,doc->printData->setting.numLinearHand[0],mm,doc->printData->setting.linearX,doc->printData->setting.linearX);
			}
		else // otherwise use the 4% increment standard
			{
			for (i=0; i<26; i++) XVales[i] = i*4;
			((GridControl*)(controls[1]))->CopyIntoGrid(0,0,26,mm,XVales,XVales);
			}
			
		mm[1] = 2.0;	
			
		((GridControl*)(controls[1]))->CopyIntoGrid(0,1,doc->printData->setting.numLinearHand[0],mm,doc->printData->setting.linearX,doc->printData->setting.linearY);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,2,doc->printData->setting.numLinearHand[1],mm,&doc->printData->setting.linearX[MAX_LINEAR_HAND],&doc->printData->setting.linearY[MAX_LINEAR_HAND]);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,3,doc->printData->setting.numLinearHand[2],mm,&doc->printData->setting.linearX[2*MAX_LINEAR_HAND],&doc->printData->setting.linearY[2*MAX_LINEAR_HAND]);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,4,doc->printData->setting.numLinearHand[3],mm,&doc->printData->setting.linearX[2*MAX_LINEAR_HAND],&doc->printData->setting.linearY[3*MAX_LINEAR_HAND]);
		}
}

void CalibTab::CopyOutofControls(int cnt_num)
{
int		numh;

// curves are not editable so just copy out of the grid 

if (cnt_num != 1) return;

	((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,1,&numh,doc->printData->setting.linearX,doc->printData->setting.linearY);
	doc->printData->setting.numLinearHand[0] = numh;
	
	((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,2,&numh,&doc->printData->setting.linearX[MAX_LINEAR_HAND],&doc->printData->setting.linearY[MAX_LINEAR_HAND]);
	doc->printData->setting.numLinearHand[1] = numh;
	
	((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,3,&numh,&doc->printData->setting.linearX[MAX_LINEAR_HAND*2],&doc->printData->setting.linearY[MAX_LINEAR_HAND*2]);
	doc->printData->setting.numLinearHand[2] = numh;
	
	((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,4,&numh,&doc->printData->setting.linearX[MAX_LINEAR_HAND*3],&doc->printData->setting.linearY[MAX_LINEAR_HAND*3]);
	doc->printData->setting.numLinearHand[3] = numh;

}


McoStatus CalibTab::CopyParameters(void)
{
int i;
McoStatus status;

for (i=0; i<numControls; i++) 
	{
	status = controls[i]->CopyParameters();
	if (status) return status;
	}
return MCO_SUCCESS;
}

// check to see if the current X's in doc match the current X's in the grid
// if all X's in the doc also exist in the grid then the grid does not need to be resized
int	CalibTab::doesXMatch(void)
{
int match = 0;
int i,j,k;
int	numh;
double *data;

if (!controls[1]) return 0;

((GridControl*)(controls[1]))->CopyXOutofGrid(&numh,&data);


for (i=0; i<3; i++)
	for (j=0; j<doc->printData->setting.numLinearHand[i]; j++)
		{
		for (k=0; k<numh; k++)
			{
			if (doc->printData->setting.linearX[MAX_LINEAR_HAND*i+j] == data[k]) break;
			}
		if (k == numh) 
			{
			delete data;
			return 0;
			}
		}
		
delete data;
return 1;
}

McoStatus CalibTab::updateWindowData(int changed)
{
Rect r1;
short	iType;
Handle	iHandle;
Str255	text;
char	desc[100];
int 	result;
int newhand;

if (!doc) return MCO_OBJECT_NOT_INITIALIZED;
if (!doc->calCurves) return MCO_OBJECT_NOT_INITIALIZED;

// see if export is valid
GetDItem (dp, DESCRIPTION+startNum, &iType, (Handle*)&iHandle, &r1);
GetIText(iHandle,text);
ptocstr(text);
strcpy(desc,doc->printData->setting.linearDesc);
if (strcmp((char*)text,desc))
	{
	ctopstr(desc);
	SetIText(iHandle,(unsigned char*)desc);
	}
	
newhand = numhand;	

if (last_changed) changed = 0;
last_changed = 0;

if ((doc->printData->setting.numLinearHand[3] > 0) && 
	(doc->printData->setting.linearX[3*MAX_LINEAR_HAND+doc->printData->setting.numLinearHand[3]-1] == 100)) 
	{
	if (!doesXMatch())
		{
		newhand = doc->printData->setting.numLinearHand[3];
		if (newhand != numhand) changed = 1;
		}
	}
	

// see if data has changed and update the controls if it has
if ((changed) && (!measure)) setUpNewGrid(newhand);
if (changed) CopyIntoControls(-1);



return MCO_SUCCESS;
}


McoStatus CalibTab::UpdateWindow(void)	
{			
PicHandle	pict;
Rect r1;
short	iType;
Handle	iHandle;
int i;
McoStatus status;

for (i=0; i<numControls; i++) 
	{
	status = controls[i]->UpdateControl();
	if (status) return status;
	}

return MCO_SUCCESS;
}

Boolean CalibTab::isMyObject(Point where,short *item) 
{
int i;

Boolean flag;
for (i=0; i<numControls; i++) 
	{
	flag = controls[i]->isMyObject(where,item);
	if (flag) return flag;
	}
return FALSE;
}

Boolean CalibTab::TimerEventPresent(void)
{
if (!measure) return FALSE;
if( TickCount() - _last_tick_time2 > 1 ) 
	{
	return TRUE;
	}
return FALSE;
}

// handle keyboard events
Boolean 	CalibTab::KeyFilter(EventRecord *theEvent,short *itemHit )
{
int i;

Boolean flag;
for (i=0; i<numControls; i++) 
	{
	flag = controls[i]->KeyFilter(theEvent,itemHit);
	if (flag) return flag;
	}
return( FALSE );	// all others
}

// The event handler for the main menu
McoStatus	CalibTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
short item2 = item - startNum;
StandardFileReply 	soutReply,sinReply;
Str255				prompt;
Str255				outname = "\p";
Str255				inname;
SFTypeList			filelist;
Rect r1;
short	iType;
Handle	iHandle;
int					didit = 0;
int		i,j;
McoStatus 			status;		
int			Changed = 0;
int			Finished = 0;
int			Patch = 0;				
int			act_c,act_r;
WindowPtr	oldP;

GetPort(&oldP);
SetPort( dp ) ;
	
	// no codes passed back
	*numwc = 0;
	
	if (item != TIMER_ITEM ) for (i=0; i<numControls; i++) 
		{
		status = controls[i]->DoEvents(item,clickPoint,wc,numwc,data,changed,&didit);
		if (changed) 
			{
			doc->changedLinearTone = 1;
			last_changed = 1;
			CopyOutofControls(i);
			CopyIntoControls(i);
			for (j=0; j<numControls; j++) if (j != i)
				{
				controls[j]->UpdateControl();
				}
			}
		if ((measure) && (i ==1)) if (((GridControl*)(controls[1]))->newPosition())
			{
			((GridControl*)(controls[1]))->getActive(&act_r,&act_c);
			Patch = (act_c-1)*numhand+act_r;
			measure->setCurrentPatch(Patch);
			}

		else last_changed = 0;
		if (status) return status;
		if (didit) return status;
		}
	if (item2 == EXPORT)
		{
		if (doc->calCurves)
			{
			GetIndString(prompt,PROMPT_STRINGS,STANDARD_PROMPT);
			StandardPutFile(prompt, outname, &soutReply);
			if(soutReply.sfGood)
				{
				memcpy(outname, soutReply.sfFile.name, *(soutReply.sfFile.name)+1);	
				PathNameFromDirID(soutReply.sfFile.parID, soutReply.sfFile.vRefNum, outname);
				ptocstr(outname);
				status = doc->calCurves->exportLinear((char*)outname);
				if (status) McoErrorAlert(status);
				else 
					{
					FileFormat ff;
	
					ff.setInfo(&soutReply.sfFile,MONACO_SIG,'TEXT');
					}
				}
			}
		}
	else if (item2 == IMPORT)
		{
		if (doc->calCurves)
			{	
			filelist[0] = 'TEXT';
			StandardGetFile(0L, 1, filelist, &sinReply);
			if(sinReply.sfGood)	
				{
				memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
				PathNameFromDirID(sinReply.sfFile.parID, sinReply.sfFile.vRefNum, inname);
				ptocstr(inname);	
				status = doc->calCurves->importLinear((char*)inname);
				if (status) McoErrorAlert(status);
				else 
					{
					updateWindowData(1);
					doc->changedLinearTone = 1;
					}
				status = MCO_SUCCESS;
				}
			}
		}
	else if (item2 == MEASURE)
		{
		if (!measure)
			{
			status = setUpMeasure();
			if (status) 
				{
				McoErrorAlert(status);
				if (measure) delete measure;
				measure = 0L;
				if (status == MCO_CANCEL) status = MCO_SUCCESS;
				}
			else 
				{
				doc->changedLinearTone = 1;
				GetIndString(prompt,BUTTON_STRINGS,STOP_BUTTON);
				GetDItem (dp, item, &iType, (Handle*)&iHandle, &r1);
				SetCTitle((ControlHandle)iHandle,prompt);
				}
			}
		else
			{
			delete measure;
			measure = 0L;
			GetIndString(prompt,BUTTON_STRINGS,MEASURE_BUTTON);
			GetDItem (dp, item, &iType, (Handle*)&iHandle, &r1);
			SetCTitle((ControlHandle)iHandle,prompt);
			changed = 1;			
			}
		}
	else if (item == TIMER_ITEM ) // a timer event has occured
		{   
		_last_tick_time2 = TickCount();
		if (measure) measure->ReadPatches(&Changed,&Finished,&Patch);
		if (Changed)
			{
			doc->changedLinearTone = 1;
			CopyPatchIntoDoc();
			act_c = 1+Patch/numhand;
			act_r = Patch%numhand;
			if (controls[1]) ((GridControl*)(controls[1]))->setActive(act_r,act_c);
			}
		if (Finished)
			{
			delete measure;
			measure = 0L;
			GetIndString(prompt,BUTTON_STRINGS,MEASURE_BUTTON);
			GetDItem (dp, MEASURE+startNum, &iType, (Handle*)&iHandle, &r1);
			SetCTitle((ControlHandle)iHandle,prompt);				
			}
		}		
	else
		{
		GetDItem (dp, DESCRIPTION+startNum, &iType, (Handle*)&iHandle, &r1);
		GetIText(iHandle,inname);
		if (inname[0] > 100) inname[0] = 100;
		ptocstr(inname);
		strcpy(doc->printData->setting.linearDesc,(char*)inname);
		}	
		
	SetPort(oldP);	
	return MCO_SUCCESS;
}