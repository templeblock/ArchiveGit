//////////////////////////////////////////////////////////////////////////////////////////
//   buildtableindex.cpp																//
//																						//
//	build a set of tables that index into the large inverse table						//
//																						//
//	 Sept 17, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#include "scantarg.h"
#include "buildtableindex.h"
//j #include "errordial.h"
#include <math.h>

BuildTableIndex::BuildTableIndex(int fd,int md,ThermObject	*td)
{
FullDim = fd;
ManyDim = md;
ManyKTable = 0L;

InvTable = 0L;

FullTable = new unsigned char[FullDim*FullDim*FullDim*9];
if (!FullTable) return;

ManyKTable = new unsigned char[ManyDim*ManyDim*ManyDim*ManyDim*7];
if (!ManyKTable) return;

thermd = td;

InitializeTables();
}

BuildTableIndex::~BuildTableIndex(void)
{
if (FullTable) delete FullTable;
if (ManyKTable) delete ManyKTable;
}

McoStatus BuildTableIndex::InitializeTables(void)
{
int c,i,j,k,l,m;

if (!ManyKTable) return MCO_OBJECT_NOT_INITIALIZED;

c = 0;
for (i=0; i<FullDim; i++)
	for (j=0; j<FullDim; j++)
		for (k=0; k<FullDim; k++)
			for (l=0; l<9; l++)
				if (l == 0)  FullTable[c++] = 0;
				else if (l%2) FullTable[c++] = 255;
				else FullTable[c++] = 0;

c = 0;
for (i=0; i<ManyDim; i++)
	for (j=0; j<ManyDim; j++)
		for (k=0; k<ManyDim; k++)
			for (l=0; l<ManyDim; l++)
				for (m=0; m<7; m++)
					if (m == 0)  ManyKTable[c++] = 0;
					else if (m%2) ManyKTable[c++] = 255;
					else ManyKTable[c++] = 0;
					
return MCO_SUCCESS;
}

McoStatus BuildTableIndex::SetInverseTable(double *it,int itd)
{

if (!ManyKTable) return MCO_OBJECT_NOT_INITIALIZED;

InvTable = it;
InvDim = itd;
return MCO_SUCCESS;
}

// find the maximum difference between adjacent lab values in the table
McoStatus BuildTableIndex::FindMaxLabDiff(void)
{
int c,i,j,k,l,m;
int i1,j1,k1,l1;
int32	in1,in2,in3;
double L1,a1,b1,L2,a2,b2;
int		invDimM = InvDim -1;
int		m1,m2;
int32	index,index2;

if (!InvTable) return MCO_OBJECT_NOT_INITIALIZED;

L = 0;
a = 0;
b = 0;

m1 = InvDim*InvDim*InvDim;
m2 = InvDim*InvDim;


for (i=1; i<invDimM; i++)
	{
	if (thermd && !thermd->DisplayTherm(0,Process_GammutSurface)) return MCO_CANCEL;
	in1 = i*InvDim*InvDim*InvDim*3;
	for (j=1; j<invDimM; j++)
		{
		in2 = j*InvDim*InvDim*3;
		for (k=1; k<invDimM; k++)
			{
			in3 = InvDim*3;
			for (l=1; l<invDimM; l++)
				{
				index = in1+in2+in3+l*3;
				L1 = InvTable[index];
				a1 = InvTable[index+1];
				b1 = InvTable[index+2];
				for (i1 = -1; i1<=1; i1++)
					for (j1 = -1; j1<=1; j1++)
						for (k1 = -1; k1<=1; k1++)
							for (l1 = -1; l1<=1; l1++)
								{
								index2 = index + (i1*m1+j1*m2+k1*InvDim+l1)*3;
								L2 = InvTable[index2];
								a2 = InvTable[index2+1];
								b2 = InvTable[index2+2];
								L = MaxVal(L,fabs(L1-L2));
								a = MaxVal(a,fabs(a1-a2));
								b = MaxVal(b,fabs(b1-b2));
								}
				}
			}
		}
	}
	
// divide by two because if differece is greater than x/2, it is out of gamut	
L = L/2;
a = a/2;
b = b/2;	
	
return MCO_SUCCESS;
}


// build the two tables
McoStatus BuildTableIndex::Build(void)
{
int32	index,index2;
int c,i,j,k,l,m;
McoStatus status = MCO_SUCCESS;
int32	in1,in2,in3;
int	h1,i1,j1,k1,l1;
int	Lm,am,bm;
int	Lp,ap,bp;
double	L1,a1,b1;
int	Lm1,Lp1,am1,ap1,bm1,bp1;
int	Km,Kp;
int32	m1,m2,m3;
int32 n1,n2;
int		invDimM = InvDim -1;
double	fulld = FullDim-1; 
double	manyd = ManyDim-1;
unsigned char *FullTablep,*ManyKTablep;


if (!ManyKTable) return MCO_OBJECT_NOT_INITIALIZED;

status = FindMaxLabDiff();
if (status != MCO_SUCCESS) return status;

n1 = FullDim*FullDim*9;
n2 = FullDim*9;

m1 = ManyDim*ManyDim*ManyDim*7;
m2 = ManyDim*ManyDim*7;
m3 = ManyDim*7;

for (i=1; i<invDimM; i++)  //k
	{
	if (thermd && !thermd->DisplayTherm(0,Process_GammutSurface)) return MCO_CANCEL;
	in1 = i*InvDim*InvDim*InvDim*3;
	for (j=1; j<invDimM; j++)  //y
		{
		in2 = j*InvDim*InvDim*3;
		for (k=1; k<invDimM; k++) //m
			{
			in3 = k*InvDim*3;
			for (l=1; l<invDimM; l++) //c
				{
				index = in1+in2+in3+l*3;
				L1 = InvTable[index];
				a1 = InvTable[index+1];
				b1 = InvTable[index+2];
				
				// find the index of lab values to run through
				// for the full table
				Lm1 = MaxVal(0,floor(fulld*(L1-L)/100.0));
				Lp1 = MinVal(FullDim-1,ceil(fulld*(L1+L)/100.0))+1;

				am1 = MaxVal(0,floor(fulld*(128+a1-a)/255.0));
				ap1 = MinVal(FullDim-1,ceil(fulld*(128+a1+a)/255.0))+1;
				
				bm1 = MaxVal(0,floor(fulld*(128+b1-b)/255.0));
				bp1 = MinVal(FullDim-1,ceil(fulld*(128+b1+b)/255.0))+1;

				for (i1 = Lm1; i1 < Lp1; i1++)
					{
					for (j1 = am1; j1 < ap1; j1++)
						{
						FullTablep = FullTable + i1*n1+j1*n2+bm1*9;
						for (k1 = bm1; k1 < bp1; k1++)
							{				
							*FullTablep++ = 1;
							*FullTablep = MinVal(*FullTablep,l-1);
							FullTablep++;
							*FullTablep = MaxVal(*FullTablep,l+1);
							FullTablep++;
							*FullTablep = MinVal(*FullTablep,k-1);
							FullTablep++;
							*FullTablep = MaxVal(*FullTablep,k+1);
							FullTablep++;
							*FullTablep = MinVal(*FullTablep,j-1);
							FullTablep++;
							*FullTablep = MaxVal(*FullTablep,j+1);
							FullTablep++;
							*FullTablep = MinVal(*FullTablep,i-1);
							FullTablep++;
							*FullTablep = MaxVal(*FullTablep,i+1);
							FullTablep++;
							}
						}
					}	
						
				// find the index of lab values to run through
				// for the full table
				Lm1 = MaxVal(0,floor(manyd*(L1-L)/100.0));
				Lp1 = MinVal(ManyDim-1,ceil(manyd*(L1+L)/100.0))+1;

				am1 = MaxVal(0,floor(manyd*(128+a1-a)/255.0));
				ap1 = MinVal(ManyDim-1,ceil(manyd*(128+a1+a)/255.0))+1;
				
				bm1 = MaxVal(0,floor(manyd*(128+b1-b)/255.0));
				bp1 = MinVal(ManyDim-1,ceil(manyd*(128+b1+b)/255.0))+1;
				
				// find K tables to search over
				
				Km = MaxVal(0,floor((double)(manyd*(i-1)/(double)InvDim)));
				Kp = MaxVal(ManyDim-1,floor((double)(manyd*(i+1)/(double)InvDim)))+1;

				for (h1 = Km; h1 < Kp; h1++)
					for (i1 = Lm1; i1 < Lp1; i1++)
						for (j1 = am1; j1 < ap1; j1++)
							{
							ManyKTablep = ManyKTable + h1*m1+i1*m2+j1*m3+bm1*7;
							for (k1 = bm1; k1 < bp1; k1++)
								{
								*ManyKTablep++ = 1;
								*ManyKTablep = MinVal(*ManyKTablep,l-1);
								ManyKTablep++;
								*ManyKTablep = MaxVal(*ManyKTablep,l+1);
								ManyKTablep++;
								*ManyKTablep = MinVal(*ManyKTablep,k-1);
								ManyKTablep++;
								*ManyKTablep = MaxVal(*ManyKTablep,k+1);
								ManyKTablep++;
								*ManyKTablep = MinVal(*ManyKTablep,j-1);
								ManyKTablep++;
								*ManyKTablep = MaxVal(*ManyKTablep,j+1);
								ManyKTablep++;
								}
							}
				}
			}
		}
	}
return MCO_SUCCESS;
}				


// given a lab value and K, find the CMY indexes to search over, or indicate that the color is out of gamut
// return a -1 in cmy_index[0] if out of gamut
McoStatus BuildTableIndex::GetIndex(double *lab,double K,int *cmy_index)
{
register int i,j,k,l;
int cm,cp,mm,mp,ym,yp;
register int index;
int	Km,Kp;
int	Lm1,Lp1,am1,ap1,bm1,bp1;
int	gamut;
int32	m1,m2,m3;
double	ManyDimM = (ManyDim-1.0);
int 	im,jm,km;
unsigned char *ManyKTablep;


if (!ManyKTable) return MCO_OBJECT_NOT_INITIALIZED;

// find K tables to search over

Km = MaxVal(0,floor(ManyDimM*(K)/100.0));
Kp = MinVal(ManyDimM,ceil(ManyDimM*(K)/100.0))+1;

// find the table entries to seach through

Lm1 = MaxVal(0,floor(ManyDimM*(lab[0])/100.0));
Lp1 = MinVal(ManyDimM,ceil(ManyDimM*(lab[0])/100.0))+1;

am1 = MaxVal(0,floor(ManyDimM*(128.0+lab[1])/255.0));
ap1 = MinVal(ManyDimM,ceil(ManyDimM*(128.0+lab[1])/255.0))+1;

bm1 = MaxVal(0,floor(ManyDimM*(128.0+lab[2])/255.0));
bp1 = MinVal(ManyDimM,ceil(ManyDimM*(128.0+lab[2])/255.0))+1;

for (i=0; i<6; i++) 
	if (i%2) cmy_index[i] = 0;
	else cmy_index[i] = 255;
	
cm = cmy_index[0];
cp = cmy_index[1];
mm = cmy_index[2];
mp = cmy_index[3];
ym = cmy_index[4];
yp = cmy_index[5];

m1 = ManyDim*ManyDim*ManyDim*7;
m2 = ManyDim*ManyDim*7;
m3 = ManyDim*7;

gamut = 0;
im = Km*m1;
for (i=Km; i<Kp; i++)
	{
	jm = Lm1*m2;
	for (j=Lm1; j<Lp1; j++)
		{
		km = am1*m3;
		for (k=am1; k<ap1; k++)
			{
			index = im+jm+km+bm1*7;
			ManyKTablep = ManyKTable + index;
			for (l=bm1; l<bp1; l++)
				{
				gamut = gamut | *ManyKTablep;
				if (*ManyKTablep) 
					{
					ManyKTablep++;
					cm = MinVal(cm,*ManyKTablep);
					ManyKTablep++;
					cp = MaxVal(cp,*ManyKTablep);
					ManyKTablep++;
					mm = MinVal(mm,*ManyKTablep);
					ManyKTablep++;
					mp = MaxVal(mp,*ManyKTablep);
					ManyKTablep++;
					ym = MinVal(ym,*ManyKTablep);
					ManyKTablep++;
					yp = MaxVal(yp,*ManyKTablep);
					ManyKTablep++;
					}
				else ManyKTablep += 7;
				}
			}
		}
	}

cmy_index[0] = cm;
cmy_index[1] = cp;
cmy_index[2] = mm;
cmy_index[3] = mp;
cmy_index[4] = ym;
cmy_index[5] = yp;

// indicate that lab value is out of gamut				
if (!gamut) cmy_index[0] = -1;				
return MCO_SUCCESS;
}					



// given a lab value and K, find the CMY indexes to search over, or indicate that the color is out of gamut
// return a -1 in cmy_index[0] if out of gamut
McoStatus BuildTableIndex::GetMinMaxK(double *lab,double *minK,double *maxK,short *gamut)
{
register int i,j,k,l;
int cm,cp,mm,mp,ym,yp;
register int index;
int	Km,Kp;
int	Lm1,Lp1,am1,ap1,bm1,bp1;
int32	m1,m2,m3;
double	FullDimM = (FullDim-1.0);
int 	im,jm,km;
unsigned char *FullTablep;


if (!ManyKTable) return MCO_OBJECT_NOT_INITIALIZED;


// find the table entries to seach through

Lm1 = MaxVal(0,floor(FullDimM*(lab[0])/100.0));
Lp1 = MinVal(FullDimM,ceil(FullDimM*(lab[0])/100.0))+1;

am1 = MaxVal(0,floor(FullDimM*(128.0+lab[1])/255.0));
ap1 = MinVal(FullDimM,ceil(FullDimM*(128.0+lab[1])/255.0))+1;

bm1 = MaxVal(0,floor(FullDimM*(128.0+lab[2])/255.0));
bp1 = MinVal(FullDimM,ceil(FullDimM*(128.0+lab[2])/255.0))+1;

Km = 255;
Kp = 0;

m1 = FullDim*FullDim*9;
m2 = FullDim*9;

*gamut = 0;

jm = Lm1*m1;
for (j=Lm1; j<Lp1; j++)
	{
	km = am1*m2;
	for (k=am1; k<ap1; k++)
		{
		index = jm+km+bm1*9;
		FullTablep = FullTable + index;
		for (l=bm1; l<bp1; l++)
			{
			if (*FullTablep) 
				{
				*gamut = 1;
				FullTablep+=7;
				Km = MinVal(Km,*FullTablep);
				FullTablep++;
				Kp = MaxVal(Kp,*FullTablep);
				FullTablep++;
				}
			else FullTablep+=9;
			}
		}
	}

if (!(*gamut))
	{
	*minK = 0;
	*maxK = 100;
	}
else
	{
	*minK = 100*(double)Km/(double)(InvDim-1);
	*maxK = 100*(double)Kp/(double)(InvDim-1);
	}
return MCO_SUCCESS;
}					


double BuildTableIndex::getSpacing(void)
{
return (sqrt(L*L+a*a+b*b));
}