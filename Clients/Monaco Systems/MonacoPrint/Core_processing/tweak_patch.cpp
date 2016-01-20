////////////////////////////////////////////////////////////////////////////////
//	tweak_patch.c															  //
//																			  //
//	tweak the patch data so that data is more accurate						  //
//																			  //
//	May 23, 1996															  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "tweak_patch.h"
#include "colortran.h"


Tweak_Element::Tweak_Element(char *n)
{
lab_d[0] = 50;
lab_d[1] = 0;
lab_d[2] = 0;

lab_p[0] = 50;
lab_p[1] = 0;
lab_p[2] = 0;

lab_g[0] = 50;
lab_g[1] = 0;
lab_g[2] = 0;

Lr = 10;
Cr = 10;
type = Calibrate_Tweak;
if (n) strcpy(name,n);
else name[0] = 0;
}

Tweak_Element::~Tweak_Element(void)
{
name[0] = 0;
}


Tweak_Patch::Tweak_Patch(Tweak_Types t)
{
type = t;
labTableH=McoNewHandle(sizeof(double)*33*33*33*3+1);
Init_Table();
}


Tweak_Patch::~Tweak_Patch(void)
{
if (labTableH != 0L) McoDeleteHandle(labTableH);
}

// Find L,a, or b value that is x%
// used to calculate the color ranges within the dialog
double Tweak_Patch::findPerc(double range, double perc)
{
double R,v;

if (range == 100) return 128;
R = (100-range)*0.00008;

v = log(perc);

v = sqrt(-v/R);
return v;
}

McoStatus Tweak_Patch::Init_Table(void)
{
int i,j,k,m,n;
double	L_dist,a_dist,b_dist;
double  dL,da,db;
double *lab;
double L,a,b;
double stepL;

stepL = 100.0/32.0;

lab = (double *)McoLockHandle(labTableH);

if (type == Calibrate_Tweak)
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = -16; j <= 16; j++){				
			if (j < 16) a = j*8;
			else a = 127.0;
			for(m = -16; m <= 16; m++){
				if (m < 16) b = m*8;
				else b = 127.0;
				lab[0] = L;
				lab[1] = a;
				lab[2] = b;
				lab +=3;
			}
		}
	}
else 	// set to zero
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = -16; j <= 16; j++){				
			if (j < 16) a = j*8;
			else a = 127.0;
			for(m = -16; m <= 16; m++){
				if (m < 16) b = m*8;
				else b = 127.0;
				lab[0] = 0;
				lab[1] = 0;
				lab[2] = 0;
				lab +=3;
			}
		}
	}
McoUnlockHandle(labTableH);
return MCO_SUCCESS;
}
// Build a table using non-uniform data
McoStatus Tweak_Patch::Build_Table(McoHandle tdH,McoHandle tdrH, int32 num)
{
McoStatus status;
double	*tweakData;
double	*tweakDataRef;
int	i,j,k,m;
double 	L, a, b;
double  *lab;
double  labt[3];
double  stepL = 100.0/32.0;
short	end;
Linearcal		*calib;


tweakData = (double*)McoLockHandle(tdH);
tweakDataRef = (double*)McoLockHandle(tdrH);

	calib = new Linearcal(20,num,num);
	if (calib == 0L) return MCO_MEM_ALLOC_ERROR;

	end = 0;
	while(1){
		//forword calibration(C(L,a,b)) 
		status = calib->loaddirect(tweakData, tweakDataRef, &end);
		//reverse calibration(L(C,M,Y))
		//status = calib->loaddirect(_patchref, _patch, &end);
		if(status != MCO_SUCCESS)
			return status;
		if(end == 1) //compute
			break;
	}	
	
	status = calib->compute();
	
	lab = (double *)McoLockHandle(labTableH);
	
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = -16; j <= 16; j++){				
			if (j < 16) a = j*8;
			else a = 127.0;
			for(m = -16; m <= 16; m++){
				if (m < 16) b = m*8;
				else b = 127.0;
				calib->_applyf(L,a,b,lab);
				lab +=3;
			}
		}
	}
	
McoUnlockHandle(labTableH);
McoUnlockHandle(tdH);
McoUnlockHandle(tdrH);
return MCO_SUCCESS;
}

// Load a table
McoStatus Tweak_Patch::Load_Table(FileFormat *fileF)
{
McoStatus state;
double *table;
int32	magic,version;

state = fileF->relRead(sizeof(int32),(char*)&magic);
if (state != MCO_SUCCESS) return state;
if (magic != TWEAK_MAGIC_NUM) return MCO_FILE_DATA_ERROR;

state = fileF->relRead(sizeof(int32),(char*)&version);
if (state != MCO_SUCCESS) return state;
if (version != TWEAK_VERSION_NUM) return MCO_FILE_DATA_ERROR;

table = (double *)McoLockHandle(labTableH);
state = fileF->relRead(sizeof(double)*33*33*33*3,(char*)table);
if (state != MCO_SUCCESS) return state;
McoUnlockHandle(labTableH);
return MCO_SUCCESS;
}

// Save the table
McoStatus Tweak_Patch::Save_Table(FileFormat *fileF)
{
McoStatus state;
double *table;
int32	magic = TWEAK_MAGIC_NUM;
int32	version = TWEAK_VERSION_NUM;

state = fileF->relWrite(sizeof(int32),(char*)&magic);
if (state != MCO_SUCCESS) return state;

state = fileF->relWrite(sizeof(int32),(char*)&version);
if (state != MCO_SUCCESS) return state;

table = (double *)McoLockHandle(labTableH);
state = fileF->relWrite(sizeof(double)*33*33*33*3,(char*)table);
if (state != MCO_SUCCESS) return state;
McoUnlockHandle(labTableH);
return MCO_SUCCESS;
}


// Load non-uniform data file and build a table
McoStatus Tweak_Patch::Load_Data(FILE *bf)
{
McoHandle tdH;
McoHandle tdrH;
double 	  *td,*tdr;
int32	i,num;

if (bf == 0L) return MCO_FILE_OPEN_ERROR;

fscanf(bf,"%d",&num);

tdH = McoNewHandle(sizeof(double)*num*3);
tdrH = McoNewHandle(sizeof(double)*num*3);

td = (double *)McoLockHandle(tdH);
tdr = (double *)McoLockHandle(tdrH);

for (i=0; i<num; i++)
	{
	fscanf(bf,"%lf %lf %lf",&td[0],&td[1],&td[2]);
	td += 3;
	}
for (i=0; i<num; i++)
	{
	fscanf(bf,"%lf %lf %lf",&tdr[0],&tdr[1],&tdr[2]);	
	tdr += 3;
	}
McoUnlockHandle(tdH);
McoUnlockHandle(tdrH);
Build_Table(tdH,tdrH,num);
McoDeleteHandle(tdH);
McoDeleteHandle(tdrH);
return MCO_SUCCESS;
}

// Modify the table
McoStatus Tweak_Patch::Modify_Table(int num_tw,Tweak_Element **tweaks)
{
int i,j,k,m,n = 0;
double	L_dist,a_dist,b_dist;
double  dL,da,db,dc,dh;
double *lab,*labp;
double	L,a,b;
double	L2,a2,b2;
double	scale;
double stepL;
double Lr,Cr;
double lch1[3],lch2[3],labt[3];

stepL = 100.0/32.0;
lab = (double *)McoLockHandle(labTableH);
labp = lab;


if (type == Calibrate_Tweak) 
	{
	for (k=0; k<num_tw; k++) if (tweaks[k]->type == Calibrate_Tweak)
		{
		lab = labp;
		dL = tweaks[k]->lab_p[0] - tweaks[k]->lab_g[0];
		da = tweaks[k]->lab_p[1] - tweaks[k]->lab_g[1];
		db = tweaks[k]->lab_p[2] - tweaks[k]->lab_g[2];

		Lr = tweaks[k]->Lr;
		Cr = tweaks[k]->Cr;
		Cr = Cr * 2.55;
		
		if ((Lr == 0) || (Cr == 0)) return MCO_FAILURE;
		Lr = (12500*0.00008*100+100*(Lr*Lr)-12500*2.995732274)/(Lr*Lr);
		Cr = (12500*0.00008*100+100*(Cr*Cr)-12500*2.995732274)/(Cr*Cr);

			for(i = 0; i < 33; i++){
				L = i*stepL;
				L_dist = L-tweaks[k]->lab_g[0];
				L_dist *= L_dist;
				L_dist *= (100-Lr)*0.00008;
				for(j = -16; j <= 16; j++){				
					if (j < 16) a = j*8;
					else a = 127.0;
					a_dist = a - tweaks[k]->lab_g[1];
					a_dist *= a_dist;
					a_dist *= (100-Cr)*0.00008;
					for(m = -16; m <= 16; m++){
						if (m < 16) b = m*8;
						else b = 127.0;
						b_dist = b - tweaks[k]->lab_g[2];
						b_dist *= b_dist;
						b_dist *= (100-Cr)*0.00008;
						scale = exp(-(L_dist+a_dist+b_dist));	
						//save to a structure
						L2 = L+dL;
						a2 = a+da;
						b2 = b+db;
						lab[0] = (1-scale)*lab[0]+scale*L2;
						lab[1] = (1-scale)*lab[1]+scale*a2;
						lab[2] = (1-scale)*lab[2]+scale*b2;
						lab +=3;
						n+=3;
					}
				}
			}
		}
	}
else if (type == GamutComp_Tweak) 
	{	
	for (k=0; k<num_tw; k++) if (tweaks[k]->type == GamutComp_Tweak)
		{
		lab = labp;
		labtolch(tweaks[k]->lab_p,lch1);
		labtolch(tweaks[k]->lab_g,lch2);
		dL = lch2[0] - lch1[0];
		dc = lch2[1] - lch1[1];
		dh = lch2[2] - lch1[2];
		
		if (dh > 180) dh = dh - 360;
		if (dh < -180) dh = 360 + dh;

		Lr = tweaks[k]->Lr;
		Cr = tweaks[k]->Cr;
		Cr = Cr * 2.55;
		
		if ((Lr == 0) || (Cr == 0)) return MCO_FAILURE;
		Lr = (12500*0.00008*100+100*(Lr*Lr)-12500*2.995732274)/(Lr*Lr);
		Cr = (12500*0.00008*100+100*(Cr*Cr)-12500*2.995732274)/(Cr*Cr);

			for(i = 0; i < 33; i++){
				L = i*stepL;
				L_dist = L-tweaks[k]->lab_d[0];
				L_dist *= L_dist;
				L_dist *= (100-Lr)*0.00008;
				for(j = -16; j <= 16; j++){				
					if (j < 16) a = j*8;
					else a = 127.0;
					a_dist = a - tweaks[k]->lab_d[1];
					a_dist *= a_dist;
					a_dist *= (100-Cr)*0.00008;
					for(m = -16; m <= 16; m++){
						if (m < 16) b = m*8;
						else b = 127.0;
						b_dist = b - tweaks[k]->lab_d[2];
						b_dist *= b_dist;
						b_dist *= (100-Cr)*0.00008;
						scale = exp(-(L_dist+a_dist+b_dist));	
						//save to a structure
						lab[0] = (1-scale)*lab[0]+scale*dL;
						lab[1] = (1-scale)*lab[1]+scale*dc;
						lab[2] = (1-scale)*lab[2]+scale*dh;
						lab +=3;
						n+=3;
					}
				}
			}
		}
	}
McoUnlockHandle(labTableH);
return MCO_SUCCESS;
}

// Modify the table
McoStatus Tweak_Patch::Modify_Table(double *lab_d,double *lab_p,double Lr,double Cr)
{
int i,j,k,m,n = 0;
double	L_dist,a_dist,b_dist;
double  dL,da,db;
double *lab,*labp;
double	L,a,b;
double	L2,a2,b2;
double	scale;
double stepL;

stepL = 100.0/32.0;
lab = (double *)McoLockHandle(labTableH);
labp = lab;

dL = lab_p[0] - lab_d[0];
da = lab_p[1] - lab_d[1];
db = lab_p[2] - lab_d[2];

Cr = Cr * 2.55;

if ((Lr == 0) || (Cr == 0)) return MCO_FAILURE;
Lr = (12500*0.00008*100+100*(Lr*Lr)-12500*2.995732274)/(Lr*Lr);
Cr = (12500*0.00008*100+100*(Cr*Cr)-12500*2.995732274)/(Cr*Cr);


	for(i = 0; i < 33; i++){
		L = i*stepL;
		L_dist = L-lab_d[0];
		L_dist *= L_dist;
		L_dist *= (100-Lr)*0.00008;
		for(j = -16; j <= 16; j++){				
			if (j < 16) a = j*8;
			else a = 127.0;
			a_dist = a - lab_d[1];
			a_dist *= a_dist;
			a_dist *= (100-Cr)*0.00008;
			for(m = -16; m <= 16; m++){
				if (m < 16) b = m*8;
				else b = 127.0;
				b_dist = b - lab_d[2];
				b_dist *= b_dist;
				b_dist *= (100-Cr)*0.00008;
				scale = exp(-(L_dist+a_dist+b_dist));	
				//save to a structure
				L2 = L+dL;
				a2 = a+da;
				b2 = b+db;
				lab[0] = (1-scale)*lab[0]+scale*L2;
				lab[1] = (1-scale)*lab[1]+scale*a2;
				lab[2] = (1-scale)*lab[2]+scale*b2;
				lab +=3;
				n+=3;
			}
		}
	}
McoUnlockHandle(labTableH);
return MCO_SUCCESS;
}



// Modify the table to adjust the brightness
McoStatus Tweak_Patch::Modify_Table_L(double light_adj)
{
int i,j,k,m,n = 0;
double	L_dist,a_dist,b_dist;
double  dL,da,db;
double *lab;
double	L,a,b;
double	L2,a2,b2;
double	scale;
double stepL;

stepL = 100.0/32.0;
lab = (double *)McoLockHandle(labTableH);


	for(i = 0; i < 33; i++){
		L = i*stepL;
		dL = light_adj*(100 - L)/100.0;
		//dL = light_adj;
		for(j = -16; j <= 16; j++){				
			if (j < 16) a = j*8;
			else a = 127.0;

			for(m = -16; m <= 16; m++){
				if (m < 16) b = m*8;
				else b = 127.0;

				//save to a structure
				lab[0] = lab[0]+dL;
				lab +=3;
				n+=3;
			}
		}
	}
return MCO_SUCCESS;
}



// Modify a color
McoStatus Tweak_Patch::Modify_Color(double *lab_in,double *lab_out,double *lab_d,double *lab_p,double Lr,double Cr)
{
int i,j,k,m,n;
double	L_dist,a_dist,b_dist;
double  dL,da,db;
double *lab;
double	L,a,b;
double	scale;


lab = (double *)McoLockHandle(labTableH);

dL = lab_p[0] - lab_d[0];
da = lab_p[1] - lab_d[1];
db = lab_p[2] - lab_d[2];

L_dist = lab_d[0] - lab_in[0];
L_dist *= L_dist;
L_dist *= (100-Lr)*0.00008;
a_dist = lab_d[1] - lab_in[1];
a_dist *= a_dist;
a_dist *= (100-Cr)*0.00008;
b_dist = lab_d[2] - lab_in[2];
b_dist *= b_dist;
b_dist *= (100-Cr)*0.00008;
scale = exp(-(L_dist+a_dist+b_dist));	

lab_out[0] = lab_in[0]+scale*dL;
lab_out[1] = lab_in[1]+scale*da;
lab_out[2] = lab_in[2]+scale*db;

return MCO_SUCCESS;
}



// evaluate using the table

McoStatus Tweak_Patch::eval(RawData *inD,RawData *outD,int32 num)
{
int	i;
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
double *cmykp,*labp;
double *table;
double lch[3];

cmykp = (double*)McoLockHandle(inD->dataH);
labp = (double*)McoLockHandle(outD->dataH);

table = (double *)McoLockHandle(labTableH);


if (type == Calibrate_Tweak) for (i=0; i<num; i++)
	{
	a = cmykp[0]*0.32;
	b = 16 + cmykp[1]*0.125;
	c = 16 + cmykp[2]*0.125;

	if (a >= 32) a = 31.999999;
	if (a < 0) a = 0;
	if (b >= 32) b = 31.999999;
	if (b < 0) b = 0;
	if (c >= 32) c = 31.999999;
	if (c < 0) c = 0;
				
	a1 = ((int32)a)*3267;
	a2 = a1 + 3267;
	b1 = ((int32)b)*99;
	b2 = b1 + 99;
	c1 = ((int32)c)*3;
	c2 = c1 + 3;

	t = a - (int32)a;
	u = b - (int32)b;
	v = c - (int32)c;

	r1 = (1-u)*(1-v);
	r2 = (1-u)*v;
	r3 = u*(1-v);
	r4 = u*v;
	r5 = t*r1;
	r6 = t*r2;
	r7 = t*r3;
	r8 = t*r4;
	a = (1-t);
	r1 = a*r1;
	r2 = a*r2;
	r3 = a*r3;
	r4 = a*r4;

	p1 = table +a1+b1+c1;
	p2 = table +a1+b1+c2;
	p3 = table +a1+b2+c1;
	p4 = table +a1+b2+c2;
	p5 = table +a2+b1+c1;
	p6 = table +a2+b1+c2;
	p7 = table +a2+b2+c1;
	p8 = table +a2+b2+c2;
				
	labp[0] = r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));
		
	labp[1] = r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));	
		
	labp[2] = r1*(*(p1)) +
		r2*(*(p2)) +
		r3*(*(p3)) +
		r4*(*(p4)) +
		r5*(*(p5)) +
		r6*(*(p6)) +
		r7*(*(p7)) +
		r8*(*(p8));
	cmykp+=3;
	labp+=3;
	}

else for (i=0; i<num; i++)
	{
	a = cmykp[0]*0.32;
	b = 16 + cmykp[1]*0.125;
	c = 16 + cmykp[2]*0.125;

	if (a >= 32) a = 31.999999;
	if (a < 0) a = 0;
	if (b >= 32) b = 31.999999;
	if (b < 0) b = 0;
	if (c >= 32) c = 31.999999;
	if (c < 0) c = 0;
				
	a1 = ((int32)a)*3267;
	a2 = a1 + 3267;
	b1 = ((int32)b)*99;
	b2 = b1 + 99;
	c1 = ((int32)c)*3;
	c2 = c1 + 3;

	t = a - (int32)a;
	u = b - (int32)b;
	v = c - (int32)c;

	r1 = (1-u)*(1-v);
	r2 = (1-u)*v;
	r3 = u*(1-v);
	r4 = u*v;
	r5 = t*r1;
	r6 = t*r2;
	r7 = t*r3;
	r8 = t*r4;
	a = (1-t);
	r1 = a*r1;
	r2 = a*r2;
	r3 = a*r3;
	r4 = a*r4;

	p1 = table +a1+b1+c1;
	p2 = table +a1+b1+c2;
	p3 = table +a1+b2+c1;
	p4 = table +a1+b2+c2;
	p5 = table +a2+b1+c1;
	p6 = table +a2+b1+c2;
	p7 = table +a2+b2+c1;
	p8 = table +a2+b2+c2;
	
	labtolch(labp,lch);
				
	lch[0] += r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));
		
	lch[1] += r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));	
		
	lch[2] += r1*(*(p1)) +
		r2*(*(p2)) +
		r3*(*(p3)) +
		r4*(*(p4)) +
		r5*(*(p5)) +
		r6*(*(p6)) +
		r7*(*(p7)) +
		r8*(*(p8));

	if (lch[2] <0) lch[2] = 360 + lch[2];
	if (lch[2] >=360) lch[2] = lch[2] - 360;	
	
	lchtolab(lch,labp);	
		
	cmykp+=3;
	labp+=3;
	}	
	
McoUnlockHandle(inD->dataH);
McoUnlockHandle(outD->dataH);
McoUnlockHandle(labTableH);							 
return MCO_SUCCESS;
}


// evaluate using the table

McoStatus Tweak_Patch::eval(double *cmykp,double *labp,int32 num)
{
int	i;
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
double *table;
double	lch[3];

if (type != Calibrate_Tweak) return MCO_FAILURE;

table = (double *)McoLockHandle(labTableH);

if (type == Calibrate_Tweak) for (i=0; i<num; i++)
	{
	a = cmykp[0]*0.32;
	b = 16 + cmykp[1]*0.125;
	c = 16 + cmykp[2]*0.125;

	if (a >= 32) a = 31.999999;
	if (a < 0) a = 0;
	if (b >= 32) b = 31.999999;
	if (b < 0) b = 0;
	if (c >= 32) c = 31.999999;
	if (c < 0) c = 0;
				
	a1 = ((int32)a)*3267;
	a2 = a1 + 3267;
	b1 = ((int32)b)*99;
	b2 = b1 + 99;
	c1 = ((int32)c)*3;
	c2 = c1 + 3;

	t = a - (int32)a;
	u = b - (int32)b;
	v = c - (int32)c;

	r1 = (1-u)*(1-v);
	r2 = (1-u)*v;
	r3 = u*(1-v);
	r4 = u*v;
	r5 = t*r1;
	r6 = t*r2;
	r7 = t*r3;
	r8 = t*r4;
	a = (1-t);
	r1 = a*r1;
	r2 = a*r2;
	r3 = a*r3;
	r4 = a*r4;

	p1 = table +a1+b1+c1;
	p2 = table +a1+b1+c2;
	p3 = table +a1+b2+c1;
	p4 = table +a1+b2+c2;
	p5 = table +a2+b1+c1;
	p6 = table +a2+b1+c2;
	p7 = table +a2+b2+c1;
	p8 = table +a2+b2+c2;
				
	labp[0] = r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));
		
	labp[1] = r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));	
		
	labp[2] = r1*(*(p1)) +
		r2*(*(p2)) +
		r3*(*(p3)) +
		r4*(*(p4)) +
		r5*(*(p5)) +
		r6*(*(p6)) +
		r7*(*(p7)) +
		r8*(*(p8));	
		
	cmykp+=3;
	labp+=3;
	}

	
McoUnlockHandle(labTableH);							 
return MCO_SUCCESS;
}



// evaluate using the table

McoStatus Tweak_Patch::eval(double *labo,double *labg,double *labt,int32 num)
{
int	i;
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
double *table;
double	lch[3];

if (type == Calibrate_Tweak) return MCO_FAILURE;

table = (double *)McoLockHandle(labTableH);

for (i=0; i<num; i++)	
	{
	a = labo[0]*0.32;
	b = 16 + labo[1]*0.125;
	c = 16 + labo[2]*0.125;

	if (a >= 32) a = 31.999999;
	if (a < 0) a = 0;
	if (b >= 32) b = 31.999999;
	if (b < 0) b = 0;
	if (c >= 32) c = 31.999999;
	if (c < 0) c = 0;
				
	a1 = ((int32)a)*3267;
	a2 = a1 + 3267;
	b1 = ((int32)b)*99;
	b2 = b1 + 99;
	c1 = ((int32)c)*3;
	c2 = c1 + 3;

	t = a - (int32)a;
	u = b - (int32)b;
	v = c - (int32)c;

	r1 = (1-u)*(1-v);
	r2 = (1-u)*v;
	r3 = u*(1-v);
	r4 = u*v;
	r5 = t*r1;
	r6 = t*r2;
	r7 = t*r3;
	r8 = t*r4;
	a = (1-t);
	r1 = a*r1;
	r2 = a*r2;
	r3 = a*r3;
	r4 = a*r4;

	p1 = table +a1+b1+c1;
	p2 = table +a1+b1+c2;
	p3 = table +a1+b2+c1;
	p4 = table +a1+b2+c2;
	p5 = table +a2+b1+c1;
	p6 = table +a2+b1+c2;
	p7 = table +a2+b2+c1;
	p8 = table +a2+b2+c2;
				
	labtolch(labg,lch);			
				
	lch[0] += r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));
		
	lch[1] += r1*(*(p1++)) +
		r2*(*(p2++)) +
		r3*(*(p3++)) +
		r4*(*(p4++)) +
		r5*(*(p5++)) +
		r6*(*(p6++)) +
		r7*(*(p7++)) +
		r8*(*(p8++));	
		
	lch[2] += r1*(*(p1)) +
		r2*(*(p2)) +
		r3*(*(p3)) +
		r4*(*(p4)) +
		r5*(*(p5)) +
		r6*(*(p6)) +
		r7*(*(p7)) +
		r8*(*(p8));
		
	if (lch[2] <0) lch[2] = 360 + lch[2];
	if (lch[2] >=360) lch[2] = lch[2] - 360;	
		
	lchtolab(lch,labt);		
		
	labo+=3;
	labg+=3;
	labt+=3;
	}	
	
	
McoUnlockHandle(labTableH);							 
return MCO_SUCCESS;
}