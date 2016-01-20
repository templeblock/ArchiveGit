//////////////////////////////////////////////////////////////////////////////
// tonetab.cpp																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "tonetab.h"
#include "curvecont.h"
#include "fullname.h"
#include "think_bugs.h"
#include "gridcont.h"
#include "errordial.h"

#define ST_DITL_ID	6002
#define GRAPH_ID	1
#define GRAPH_PICT	146

#define EXPORT		59
#define IMPORT		58
#define DESCRIPTION 61

#define CURVE_IDS {1,-1,-1,-1,-1,63,6102,-1,1,3,146,5,36,36,13}
#define GRID_IDS {-1,52,2,10,5,53}

ToneTab::ToneTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{

	int i;
	int	num_cmp[1];
	double	hmi[1],hma[1],vmi[1],vma[1];
	int		ec[5],pr[5];
	double	xd[2],yd[2];

	doc = dc;
	
	short	curve_ids[] = CURVE_IDS;
	short	grid_ids[] = GRID_IDS;

// set up the slider



	
// set up the controls


	numControls = 0;
	controls[0] = new CurveControl(dp,curve_ids,startNum,1,0,(baseWin*)this,0);
	if (controls[0]) 
		{
		numControls++;
		num_cmp[0] = 4;
		hmi[0] = 0;
		hma[0] = 100;
		vmi[0] = 0;
		vma[0] = 100;
		((CurveControl*)(controls[0]))->SetUpCurves(1,num_cmp,hmi,hma,vmi,vma);
		}
		
	// set up editable fields	
	for (i=0; i<5; i++) ec[i] = 1;
	ec[0] = 0;	
	
	for (i=0; i<5; i++) pr[i] = 1;
	pr[0] = 0;		
	// create the grid control
	controls[1] = new GridControl(dp,grid_ids,startNum,5,101,ec,pr);
	if (controls[1])  numControls++;	
	
	xd[0] = 0;
	xd[1] = 100;
	yd[0] = 0;
	yd[1] = 100;
	((GridControl*)(controls[1]))->SetUpDelete(2,xd,yd);
	
	//copy the data into the controls
	CopyIntoControl(-1);		
		
		
// set up the parameters

	CopyParameters();	
	
	last_changed = 0;	
	
}


ToneTab::~ToneTab(void)
{
int i;

for (i=0; i<numControls; i++) if (controls[i]) delete controls[i];

}

void ToneTab::CopyIntoControl(int cnt_num)
{
double	XVales[101];
double	mm[2];
int i;
int hand[2];

if (!controls[0]) return;

if (cnt_num != -1) doc->changedLinearTone = 1;

	if ((cnt_num != 0) || (cnt_num == -1))
		{

		((CurveControl*)(controls[0]))->CopyIntoCurves(0,0,doc->printData->setting.numToneHand[0],0,100,0,100,
			doc->printData->setting.toneX,doc->printData->setting.toneY);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,1,doc->printData->setting.numToneHand[1],0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND],&doc->printData->setting.toneY[MAX_TONE_HAND]);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,2,doc->printData->setting.numToneHand[2],0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND*2],&doc->printData->setting.toneY[MAX_TONE_HAND*2]);
		((CurveControl*)(controls[0]))->CopyIntoCurves(0,3,doc->printData->setting.numToneHand[3],0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND*3],&doc->printData->setting.toneY[MAX_TONE_HAND*3]);
			
		}
		
if (!controls[1]) return;

	if  ((cnt_num != 1) || (cnt_num == -1))
		{
		
		((CurveControl*)(controls[0]))->getActiveHand(hand);
		
		mm[0] = 0;
		mm[1] = 100;
		
	//  use the 1% increment standard

		for (i=0; i<101; i++) XVales[i] = i;
		((GridControl*)(controls[1]))->CopyIntoGrid(0,0,101,mm,XVales,XVales);
				
			
		((GridControl*)(controls[1]))->CopyIntoGrid(0,1,doc->printData->setting.numToneHand[0],mm,doc->printData->setting.toneX,doc->printData->setting.toneY);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,2,doc->printData->setting.numToneHand[1],mm,&doc->printData->setting.toneX[MAX_TONE_HAND],&doc->printData->setting.toneY[MAX_TONE_HAND]);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,3,doc->printData->setting.numToneHand[2],mm,&doc->printData->setting.toneX[2*MAX_TONE_HAND],&doc->printData->setting.toneY[2*MAX_TONE_HAND]);
		((GridControl*)(controls[1]))->CopyIntoGrid(0,4,doc->printData->setting.numToneHand[3],mm,&doc->printData->setting.toneX[3*MAX_TONE_HAND],&doc->printData->setting.toneY[3*MAX_TONE_HAND]);

		hand[0]++;
		((GridControl*)(controls[1]))->setActiveItem(hand);
		}		
		
		
}

void ToneTab::CopyOutofControl(int cnt_num)
{
int	numh;

	if (cnt_num == 0)
		{
		((CurveControl*)(controls[0]))->CopyOutofCurves(0,0,&numh,0,100,0,100,
			doc->printData->setting.toneX,doc->printData->setting.toneY);
		doc->printData->setting.numToneHand[0] = numh;	
			
		((CurveControl*)(controls[0]))->CopyOutofCurves(0,1,&numh,0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND],&doc->printData->setting.toneY[MAX_TONE_HAND]);
		doc->printData->setting.numToneHand[1] = numh;		
			
		((CurveControl*)(controls[0]))->CopyOutofCurves(0,2,&numh,0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND*2],&doc->printData->setting.toneY[MAX_TONE_HAND*2]);
		doc->printData->setting.numToneHand[2] = numh;	
		
		((CurveControl*)(controls[0]))->CopyOutofCurves(0,3,&numh,0,100,0,100,
			&doc->printData->setting.toneX[MAX_TONE_HAND*3],&doc->printData->setting.toneY[MAX_TONE_HAND*3]);
		doc->printData->setting.numToneHand[3] = numh;	
		}
		
	if (cnt_num == 1) 
		{

		((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,1,&numh,doc->printData->setting.toneX,doc->printData->setting.toneY);
		doc->printData->setting.numToneHand[0] = numh;
		
		((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,2,&numh,&doc->printData->setting.toneX[MAX_TONE_HAND],&doc->printData->setting.toneY[MAX_TONE_HAND]);
		doc->printData->setting.numToneHand[1] = numh;
		
		((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,3,&numh,&doc->printData->setting.toneX[MAX_TONE_HAND*2],&doc->printData->setting.toneY[MAX_TONE_HAND*2]);
		doc->printData->setting.numToneHand[2] = numh;
		
		((GridControl*)(controls[cnt_num]))->CopyOutofGrid(0,4,&numh,&doc->printData->setting.toneX[MAX_TONE_HAND*3],&doc->printData->setting.toneY[MAX_TONE_HAND*3]);
		doc->printData->setting.numToneHand[3] = numh;
		}
	
}

McoStatus ToneTab::CopyParameters(void)
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


McoStatus ToneTab::updateWindowData(int changed)
{
Rect r1;
short	iType;
Handle	iHandle;
Str255	text;
char	desc[100];
int 	result;
int newhand;

if (!doc) return MCO_OBJECT_NOT_INITIALIZED;
if (!doc->printData) return MCO_OBJECT_NOT_INITIALIZED;

// see if export is valid
GetDItem (dp, DESCRIPTION+startNum, &iType, (Handle*)&iHandle, &r1);
GetIText(iHandle,text);
ptocstr(text);
strcpy(desc,doc->printData->setting.toneDesc);
if (strcmp((char*)text,desc))
	{
	ctopstr(desc);
	SetIText(iHandle,(unsigned char*)desc);
	}
	
if (last_changed) changed = 0;
last_changed = 0;	
	
if (changed) CopyIntoControl(-1);

return MCO_SUCCESS;
}



McoStatus ToneTab::UpdateWindow(void)	
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

Boolean ToneTab::isMyObject(Point where,short *item) 
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

// handle keyboard events
Boolean 	ToneTab::KeyFilter(EventRecord *theEvent,short *itemHit )
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

// update data in the window while a control is being used
McoStatus   ToneTab::updateDataForControl(int num)
{
CopyOutofControl(num);
CopyIntoControl(num);
return MCO_SUCCESS;
}


// The event handler for the main menu
McoStatus	ToneTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
int					didit = 0;
int		i,j;
McoStatus 			status = MCO_SUCCESS;	
short 				item2 = item - startNum;
Str255				prompt;
StandardFileReply 	soutReply,sinReply;
Str255				outname = "\p";
SFTypeList			filelist;
Str255				inname;
Rect r1;
short	iType;
Handle	iHandle;
WindowPtr	oldP;

GetPort(&oldP);
SetPort( dp ) ;

	// no codes passed back
	*numwc = 0;
	
	for (i=0; i<numControls; i++) 
		{
		status = controls[i]->DoEvents(item,clickPoint,wc,numwc,data,changed,&didit);
		if (status) return status;
		if (changed) 
			{
			doc->changedLinearTone = 1;
			last_changed = 1;
			CopyOutofControl(i);
			CopyIntoControl(i);
			}
		else last_changed = 0;		
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
				status = doc->calCurves->exportTone((char*)outname);
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
				status = doc->calCurves->importTone((char*)inname);
				if (status) McoErrorAlert(status);
				else 
					{
					CopyIntoControl(-1);
					doc->changedLinearTone = 1;
					}
				status = MCO_SUCCESS;
				}
			}
		}
	else
		{
		GetDItem (dp, DESCRIPTION+startNum, &iType, (Handle*)&iHandle, &r1);
		GetIText(iHandle,inname);
		if (inname[0] > 100) inname[0] = 100;
		ptocstr(inname);
		strcpy(doc->printData->setting.toneDesc,(char*)inname);
		}
	SetPort(oldP);		
	return status;
}