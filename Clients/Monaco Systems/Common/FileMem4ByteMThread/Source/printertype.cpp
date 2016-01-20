////////////////////////////////////////////////////////////////////////////////////
// describes setting for a particular printer and loads in and saves data		  //
// printertype.cpp																  //
// James Vogh																	  //
// (c) Monaco Systems Inc.														  //
// 6/27/97																		  //
////////////////////////////////////////////////////////////////////////////////////

#include "printertype.h"
#include "mcostat.h"
#include "fileformat.h"
#include "monacoprint.h"
#include "mcomem.h"
#include "spline.h"
#include "splint.h"

#ifndef _WIN32
	#include "think_bugs.h"
#endif

PrinterType::PrinterType(void)
{
int i;
// set up some resonable defaults first

	data.tabletype = 2;	// 1 = photoshp, 2 = ICC, 3 = CRD, 4 = RGB, 5 = ICC CMYK, 6 = ICC TEK
	data.separation_type = 1;	//GCR == 0, UCR == 1
	data.black_type = 2; 		//None==1,Light==2,Medium==3,Heavy==4 or Maximum==5
	data.total_ink_limit = 400;	// from 0 to 400
	data.black_ink_limit = 100;	// from 0 to 100
	data.saturation = 90;		// from 0 to 100, 100 is max saturation
	data.col_balance = 100;	// 0 = neutral, 100 = paper color
	data.L_linear = 90;		// 0 = linear, 100 = compressed
	data.ink_linerize = 0;	// lineraize ink L
	data.ink_neutralize = 0; // neutralize ink sim for paper color
	data.ink_highlight = 0;  // lock the highlight point (L goes from minL to 100)
	data.rev_table_size = 1;	//reverse table size, large = 17, small = 9
	data.simulate = 0;	//simulate or not, if == 1, yes
	data.smooth = 0;		//amount of smoothing to apply
	data.smooth_sim = 0;	//amount of smoothing to apply to simulation
	data.col_bal_cut = 94; // defines where to cuttoff the color balnce function, 0== cuttoff at L = 0, 256 = cuttoff at L = 95
	data.K_only = 1;		// I

	
	num_hands = 4;
	x_hands[0] = 0;
	y_hands[0] = 0;
	x_hands[1] = 40;
	y_hands[1] = 0;
	x_hands[2] = 85;
	y_hands[2] = 50;
	x_hands[3] = 100;
	y_hands[3] = 100;
	
	num_tweaks = 0;
	tweaks = new Tweak_Element*[MAX_NUM_TWEAKS];
	for (i=0; i<MAX_NUM_TWEAKS; i++) tweaks[i] = 0L;
	
	sim_num = 0;
	
}

PrinterType::~PrinterType(void)
{
int i;
if(tweaks){
	for (i=0; i<MAX_NUM_TWEAKS; i++) if (tweaks[i]) delete tweaks[i];
	delete tweaks;
}

}

//save to input black table when window is closed
McoStatus	PrinterType::setblack(McoHandle blacktableH)
{
	short 	i;
	double *black;
	double *y2_hands;
	double ypn;
	
	y2_hands = new double[num_hands];
	
	black = (double*)McoLockHandle(blacktableH);
	
	ypn=(y_hands[num_hands-1]-y_hands[num_hands-2])/
		(x_hands[num_hands-1]-x_hands[num_hands-2]);
	
	spline_0(x_hands,y_hands,num_hands,1E30,1E30,y2_hands);
//L to Black table	
	for( i = 0; i <= 100; i++)
		splint_0(x_hands,y_hands,y2_hands,num_hands,i, black+i);

//fix on 8/23
//clipping the _yd_points which on the left of letfmost hand
//to the value of the leftmost hand
//on the right of rightmost hand to the value of the rightmost hand
	for( i = 0; i <= 100; i++){
		if( i <= x_hands[0] ) //left of the leftmost hand
			black[i] = y_hands[0];
		if( i >= x_hands[num_hands-1] ) //left of the leftmost hand
			black[i] = y_hands[num_hands-1];
	}

//clipping the table and normaliz to 1
	for( i = 0; i <= 100; i++){
		if(black[i] < 0)
			black[i] = 0;
		else if(black[i] > 100)
			black[i] = 1.0;
		else
			black[i] /= 100.0;
	}

				
	McoUnlockHandle(blacktableH);
	
	delete y2_hands;
	
	return MCO_SUCCESS;
}	

#ifndef _WIN32
McoStatus PrinterType::Save(void)
{
	int i;
	StandardFileReply 	soutReply;
	Str255				prompt;
	Str255				outname = "\p";
	McoStatus status = MCO_SUCCESS;
	FileFormat *filef;
	int   magic_num = PRINTERTYPE_MAGICNUM;
	int   version = PRINTERTYPE_VERSION;
	
	GetIndString(prompt,PROMPT_STRINGS,SETTINGS_PROMPT);
	
	StandardPutFile(prompt, outname, &soutReply);
	if(!soutReply.sfGood) return MCO_CANCEL;



	filef = new FileFormat;

	status = filef->createFilewithtype(&soutReply.sfFile,soutReply.sfScript,MONACO_SIG,'MCO6');
	if (status != MCO_SUCCESS) goto bail;

	status = filef->relWrite(sizeof(int),(char*)&magic_num);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&version);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(ConData),(char*)&data);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&num_hands);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(double)*31,(char*)x_hands);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(double)*31,(char*)y_hands);
	if (status != MCO_SUCCESS) goto bailclose;
	
	status = filef->relWrite(sizeof(int),(char*)&sim_num);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&num_tweaks);
	if (status != MCO_SUCCESS) goto bailclose;

	for (i=0; i<num_tweaks; i++)
		{
		status = filef->relWrite(sizeof(Tweak_Element),(char*)tweaks[i]);
		if (status != MCO_SUCCESS) goto bailclose;
		}

	bailclose:
	filef->closeFile();
	bail:
	delete filef;
	return status;
}

McoStatus PrinterType::Save(FSSpec *fspec)
{
	int i;
	McoStatus status = MCO_SUCCESS;
	FileFormat *filef;
	int   magic_num = PRINTERTYPE_MAGICNUM;
	int   version = PRINTERTYPE_VERSION;
	
	filef = new FileFormat;

//	status = filef->createFilewithtype(fspec,smSystemScript,'Mprn','MCO6');
//	if (status != MCO_SUCCESS) goto bail;

	status = filef->openFile(fspec);
	if (status != MCO_SUCCESS) goto bail;

	status = filef->relWrite(sizeof(int),(char*)&magic_num);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&version);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(ConData),(char*)&data);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&num_hands);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(double)*31,(char*)x_hands);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(double)*31,(char*)y_hands);
	if (status != MCO_SUCCESS) goto bailclose;
	
	status = filef->relWrite(sizeof(int),(char*)&sim_num);
	if (status != MCO_SUCCESS) goto bailclose;

	status = filef->relWrite(sizeof(int),(char*)&num_tweaks);
	if (status != MCO_SUCCESS) goto bailclose;

	for (i=0; i<num_tweaks; i++)
		{
		status = filef->relWrite(sizeof(Tweak_Element),(char*)tweaks[i]);
		if (status != MCO_SUCCESS) goto bailclose;
		}

	filef->setInfo(fspec,MONACO_SIG,'MCO6');

	bailclose:
	filef->closeFile();
	bail:
	delete filef;
	return status;
}


McoStatus PrinterType::Load(FSSpec *fspec)
{
int i;
McoStatus status = MCO_SUCCESS;
FileFormat *filef;
int   magic_num;
int   version;


filef = new FileFormat;

status = filef->openFile(fspec);
if (status != MCO_SUCCESS) goto bail;

status = filef->relRead(sizeof(int),(char*)&magic_num);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(int),(char*)&version);
if (status != MCO_SUCCESS) goto bailclose;
if (version != PRINTERTYPE_VERSION) goto bailclose;

status = filef->relRead(sizeof(ConData),(char*)&data);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(int),(char*)&num_hands);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(double)*31,(char*)x_hands);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(double)*31,(char*)y_hands);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(int),(char*)&sim_num);
if (status != MCO_SUCCESS) goto bailclose;

status = filef->relRead(sizeof(int),(char*)&num_tweaks);
if (status != MCO_SUCCESS) goto bailclose;

for (i=0; i<num_tweaks; i++)
	{
	if (!tweaks[i]) 
	tweaks[i] = new Tweak_Element(0L);
	if (!tweaks[i]) goto bailclose;
	status = filef->relRead(sizeof(Tweak_Element),(char*)tweaks[i]);
	if (status != MCO_SUCCESS) goto bailclose;
	}
	


bailclose:
filef->closeFile();
bail:
delete filef;
return status;
}

#endif

//double 	lab_d[3];
//double	lab_p[3];
//double	Lr,Cr;
//Tweak_Types	type;
//char	name[40];


// copy p1 into p2
McoStatus copyprinterType(PrinterType *p1, PrinterType *p2)
{
int i,j;

p2->data = p1->data;

p2->num_hands = p1->num_hands;
for (i=0; i<p2->num_hands; i++)
	{
	p2->x_hands[i] = p1->x_hands[i];
	p2->y_hands[i] = p1->y_hands[i];
	}
p2->sim_num = p1->sim_num;	
p2->num_tweaks = p1->num_tweaks;

for (i=0; i<p1->num_tweaks; i++)
	{
	if (p2->tweaks[i]) delete p2->tweaks[i];
	p2->tweaks[i] = new Tweak_Element(0L);
	for (j=0; j<3; j++) 
		{
		p2->tweaks[i]->lab_d[j] = p1->tweaks[i]->lab_d[j];
		p2->tweaks[i]->lab_p[j] = p1->tweaks[i]->lab_p[j];
		p2->tweaks[i]->lab_g[j] = p1->tweaks[i]->lab_g[j];
		}
	p2->tweaks[i]->Lr = p1->tweaks[i]->Lr;
	p2->tweaks[i]->Cr = p1->tweaks[i]->Cr;
	p2->tweaks[i]->type = p1->tweaks[i]->type;
	for (j=0; j<40; j++) p2->tweaks[i]->name[i] = p1->tweaks[i]->name[i];
	}

return MCO_SUCCESS;
}