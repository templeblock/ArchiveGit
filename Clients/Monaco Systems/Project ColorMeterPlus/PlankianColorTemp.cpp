/* plankcolortemp.c */
#include "stdafx.h"
#include "math.h"

typedef struct CTData {
	DWORD  sindex;
	double dRMK;
	double dTemp;
	double du;
	double dv;
	double dslope;
}CTPoint;


static CTPoint sgColorTempData[31] = {

	 0,0.0,100000000.0,.18006,.2635,-.24341,
	 1,10.,10000.0,.18066,.26589,-.25479,
	 2,20.,50000.0,.18133,.26846,-.26876,
	 3,30.,33333.0,.18208,.27119,-.28539,
	 4,40.,25000.0,.18293,.27407,-.30740,
	 5,50.,20000.0,.18388,.27709,-.32675,
	 6,60.,16667.0,.18494,.28021,-.35156,
	 7,70.,14286.0,.18611,.28342,-.37915,
	 8,80.,12500.0,.18740,.28668,-.40955,
	 9,90.,11111.0,.18880,.28997,-.44278,
	10,100.,10000.0,.19032,.29326,-.47888,
	11,125.,8000.0,.19462,.30141,-.58204,
	12,150.,6667.0,.19962,.30921,-.70471,
	13,175.,5714.0,.20525,.31647,-.84901,
	14,200.,5000.0,.21142,.32312,-1.0182,
	15,225.,4444.0,.21807,.32909,-1.2168,
	16,250.,4000.0,.22511,.33439,-1.4512,
	17,275.,3636.0,.23247,.33904,-1.7298,
	18,300.,3333.0,.24010,.34308,-2.0637,
	19,325.,3077.0,.24702,.34655,-2.4681,
	20,350.,2857.0,.25591,.34951,-2.9641,
	21,375.,2677.0,.26400,.35200,-3.5814,
	22,400.,2500.0,.27218,.35407,-4.3633,
	23,425.,2353.0,.28039,.35577,-5.3762,
	24,450.,2222.0,.28863,.35714,-6.7262,
	25,475.,2105.0,.29685,.35823,-8.5955,
	26,500.,2000.0,.30505,.35907,-11.324,
	27,525.,1905.0,.31320,.35968,-15.628,
	28,550.,1818.0,.32129,.36011,-23.325,
	29,575.,1739.0,.32931,.36038,-40.770,
	30,600.,1667.0,.33724,.36051,-116.45
};


short Pxy2Kelvin(double x, double y	, double *TempInKelvin)
{
	/* This function calculates the correlated color temperature of 
	an xy point measurement. The method is described in Color Science
	Concepts and Mehods, Quantitative Data and Formulae,  Wyszecki and Stiles

  */

	double u,v,Temp,d0,d1 ;
	short i;

	u = (2.*x)/(-x+6.*y+1.5);
	v = (3.*y)/(-x+6.*y+1.5);	// convert data to uv space


	for(i=0; i<30; i++)
	{

		d0 = (v - sgColorTempData[i].dv)- (sgColorTempData[i].dslope*(u - sgColorTempData[i].du));
		d0 = d0/(sqrt(1+ sgColorTempData[i].dslope*sgColorTempData[i].dslope));

		d1 = (v - sgColorTempData[i+1].dv)- (sgColorTempData[i+1].dslope*(u - sgColorTempData[i+1].du));
		d1 = d1/(sqrt(1+ sgColorTempData[i+1].dslope*sgColorTempData[i+1].dslope));
		
		if(d1 == 0) break; //point is on a locus;
				
		if(d0/d1 <= 0)  break;	// point is on the locus or between two points

	}

	if(i==30)
	{
	*TempInKelvin= 0.00;
	return (-1);
	}

	Temp = (1.0/ sgColorTempData[i].dTemp +((d0/(d0-d1)*( 1.0/ sgColorTempData[i+1].dTemp-1.0/ sgColorTempData[i].dTemp))));
	Temp = 1/Temp;

	*TempInKelvin = Temp;

	return (0);
}