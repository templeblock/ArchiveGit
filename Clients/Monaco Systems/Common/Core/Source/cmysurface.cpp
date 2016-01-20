#include "cmysurface.h"

#define SCREEN_DELTA	 (0.1)
#define ROUNDOFF_DELTA	(10e-3)


Cmysurface::Cmysurface(int32 one_d_points, double *lab,double *maxL_lab,double *minL_lab)
{
	int32 i, j;
	double *slabh, *labh;
	int32	two_d_points, one_d_egdes;
	int32	two_d_squares, loc;
	int32 	*startidh;
	double  temp_mingrayL, temp_maxgrayL;
	
	
	_one_d_points = one_d_points;
	one_d_egdes = _one_d_points -1;

//specify the grid id
	two_d_squares = (_one_d_points -1)*(_one_d_points -1);

//find maximum and minimum L value
	_maxL = lab[0];
	_minL = lab[(_one_d_points*_one_d_points*_one_d_points-1)*3];
	
	_start_id = new int32[one_d_egdes*one_d_egdes];
	startidh = _start_id;
	for( i = 0; i < one_d_egdes; i++)
		for( j = 0; j < one_d_egdes; j++)
			*startidh++ = (i*_one_d_points + j)*3; 
		

//specify triangle mask
	_tri_id[0] = 0;	//specify this as point 1
	_tri_id[1] = 3; //specify this as point 2
	_tri_id[2] = (1+_one_d_points)*3; //specify this as point 3
	
	_tri_id[3] = 0;	//specify this as point 1
	_tri_id[4] = _one_d_points*3; //specify this as point 2
	_tri_id[5] = (1+_one_d_points)*3; //specify this as point 3
		
	_slab = new double[_one_d_points*_one_d_points*3*6];
	slabh = _slab;
	two_d_points = _one_d_points*_one_d_points;
	
	//c = 0
	labh = lab;
	for(i = 0; i < _one_d_points; i++)
		for( j = 0; j < _one_d_points; j++){
			loc = (i*two_d_points + j)*3;
			*slabh++ = labh[loc];
			*slabh++ = labh[loc+1]; 
			*slabh++ = labh[loc+2]; 
		}	
	
	//c = 1
	labh = lab + (two_d_points - _one_d_points)*3;
	for(i = 0; i < _one_d_points; i++)
		for( j = 0; j < _one_d_points; j++){
			loc = (i*two_d_points + j)*3;
			*slabh++ = labh[loc];
			*slabh++ = labh[loc+1]; 
			*slabh++ = labh[loc+2]; 
		}	

	//m = 0
	labh = lab;
	for(i = 0; i < two_d_points; i++){
		loc = i*_one_d_points*3;
		*slabh++ = labh[loc];
		*slabh++ = labh[loc+1]; 
		*slabh++ = labh[loc+2]; 
	}	
	
	//m = 1
	labh = lab + (_one_d_points - 1)*3;
	for(i = 0; i < two_d_points; i++){
		loc = i*_one_d_points*3;
		*slabh++ = labh[loc];
		*slabh++ = labh[loc+1]; 
		*slabh++ = labh[loc+2]; 
	}	
		
	//y = 0
	labh = lab;
	for(i = 0; i < two_d_points; i++){
		loc = i*3;
		*slabh++ = labh[loc];
		*slabh++ = labh[loc+1]; 
		*slabh++ = labh[loc+2]; 
	}	
	
	//y = 1
	labh = lab + (two_d_points*(_one_d_points - 1))*3;
	for(i = 0; i < two_d_points; i++){
		loc = i*3;
		*slabh++ = labh[loc];
		*slabh++ = labh[loc+1]; 
		*slabh++ = labh[loc+2]; 
	}	
	
	_findminmax();
	temp_mingrayL = _mingrayL;
	temp_maxgrayL = _maxgrayL;
	_mingrayL = -1;
	_maxgrayL = 101;
	_findminmax(maxL_lab,minL_lab);	
	if ((_mingrayL > temp_mingrayL) || (_mingrayL < 0)) _mingrayL = temp_mingrayL;
	if ((_maxgrayL < temp_maxgrayL) || (_maxgrayL > 100)) _maxgrayL = temp_maxgrayL;
}


Cmysurface::~Cmysurface(void)
{

if (_start_id) delete _start_id;
if (_slab) delete _slab;
}

//function to find the maxL and minL of gray scale
void Cmysurface::_findminmax(void)
{
	McoStatus status;
	int32 i,j,k;
	int32 total_tri;	//total # of triangles
	int32 start;
	double sdata[9];	//data of core matrix
	double rdata[3]; 	//data of right matrix
	Matrix rmat(3, 1);
	Matrix lmat(3, 1);
	Matrix s(3);
	double the1, the2, t;
	int32 two_d_offset, total_squares;
	int32 p1, p2, p3;
	double inlab[3];
	double L;
	
	//set inlab(p4 is always 100, p0 is always 0)
	inlab[0] = 100;
	inlab[1] = 0;
	inlab[2] = 0;
	
	
	_mingrayL = 100;
	_maxgrayL = 0;

		
	two_d_offset = 3*_one_d_points*_one_d_points;
	total_squares = (_one_d_points-1)*(_one_d_points-1);
	
	for(i = 0; i < 6; i++){//c=0,c=1,...
		for( j = 0; j < total_squares; j++){//squares in one plane
			for( k = 0; k <= 3; k += 3){//two triangles in one square
				start = i*two_d_offset + _start_id[j];
				
				//x0 - x3( y0 is 0 0 0
				p3 = start + _tri_id[2+k];
				rdata[0] = -_slab[p3];
				rdata[1] = -_slab[p3+1];
				rdata[2] = -_slab[p3+2];
				rmat.loadstruct(rdata);
				
				//core matrix
				p1 = start + _tri_id[k];
				p2 = start + _tri_id[1+k];
				
				//left col
				sdata[0] = _slab[p1] - _slab[p3];
				sdata[3] = _slab[p1+1] - _slab[p3+1];
				sdata[6] = _slab[p1+2] - _slab[p3+2];
				
				//middle col
				sdata[1] = _slab[p2] - _slab[p3];
				sdata[4] = _slab[p2+1] - _slab[p3+1];
				sdata[7] = _slab[p2+2] - _slab[p3+2];
				
				//right col
				sdata[2] = -inlab[0];
				sdata[5] = -inlab[1];
				sdata[8] = -inlab[2];
				s.loadstruct(sdata);
				
				//compute
				s.inv();
				if( (status = s.get_status()) == MCO_SUCCESS){ //singular or ?
					
					lmat = s*rmat;
					lmat.getval(1, 1, &the1);
					lmat.getval(2, 1, &the2);
					lmat.getval(3, 1, &t);
					
					//check where is the crossing point, or even it exists
					if( the1 >= 0 && the2 >= 0 && the1+the2 <= 1){ //cross the current triangle
						if( t >= 0 && t <= 1){
						
							L = t*inlab[0];
							if( L > _maxgrayL)
								_maxgrayL = L;
								
							if( L < _mingrayL)
								_mingrayL = L;	
						}
						//else t < 0 on the reverse side of gray axis
					}
				}		
			}	
		}
	}
	
	return;
}					

	
// added by James on 11/11
McoStatus Cmysurface::getMinMax(double &maxgrayL, double &mingrayL)
{
maxgrayL = _maxgrayL;
mingrayL = _mingrayL;
return MCO_SUCCESS;
}


// get the min and max for the desired lab vaules
// used when non-neutral paper color is selected
McoStatus Cmysurface::_findminmax(double *maxL_lab,double *minL_lab)
{
double	lab[3];
double  boundlab[3],boundcmy[4];
int16	yes;
McoStatus state;
double	v1,v2;

lab[0] = 0;
lab[1] = maxL_lab[1];
lab[2] = maxL_lab[2];


state = findBoundaryNoNorm(maxL_lab,lab, boundlab, boundcmy,&yes,0.98, 0x7FFFFFFF);
if (state != MCO_SUCCESS) return state;
if (!yes) return MCO_FAILURE;
v1 = boundlab[0];

lab[0] = 100; // set just below maximum  value
lab[1] = maxL_lab[1];
lab[2] = maxL_lab[2];

state = findBoundaryNoNorm(minL_lab,lab, boundlab, boundcmy,&yes,0.98, 0x7FFFFFFF);
if (state != MCO_SUCCESS) return state;
if (!yes) return MCO_FAILURE;
v2 = boundlab[0];

if (v1 > v2)
	{
	_maxgrayL = v1;
	_mingrayL = v2;
	}
else
	{
	_maxgrayL = v2;
	_mingrayL = v1;	
	}
return state;
}



//checking all the sequence for the time being
McoStatus Cmysurface::findboundary(double *inlab, double *boundlab, double *boundcmy, int16* yes)
{

	double	r1,r2,r3;
	register double	a1,a2,a3,a4,a5,a6,a7,a8,a9;
	double	s1,s2,s3;
	double	determ;
	McoStatus status;
	int32 i,j,k;
	int32 total_tri;	//total # of triangles
	int32 start;
	double sdata[9];	//data of core matrix
	double rdata[3]; 	//data of right matrix
	Matrix rmat(3, 1);
	Matrix lmat(3, 1);
	Matrix s(3);
	double the1, the2, t;
	int32 two_d_offset, total_squares;
	int32 p1, p2, p3;
	double L;
	
	//assume no compression needed
	*yes = 0;
	
	//screen L value, min and max
	L = inlab[0];
	if( L > _maxgrayL){
		L = _maxgrayL - 0.1;
		*yes = 1;
	}	
	if( L < _mingrayL){
		L = _mingrayL + 0.1;	
		*yes = 1;
	}	
	
	//screen a,b value
	if( fabs(inlab[1]) < SCREEN_DELTA && fabs(inlab[2]) < SCREEN_DELTA ){
		boundlab[0] = L;
		boundlab[1] = inlab[1];
		boundlab[2] = inlab[2];
		return MCO_SUCCESS;
	}		
	
	//some optimization here to improve the speed
	//first guess which plane the point may cross
	/*
	if( inlab[1] >= 0 && inlab[2] >= 0){
		
		sq[0] = 0;
		sq[1] = 1;
		if(inlab[1] > inlab[2] ){
			sq[2] = 
	*/
	
	two_d_offset = 3*_one_d_points*_one_d_points;
	total_squares = (_one_d_points-1)*(_one_d_points-1);
	
	for(i = 0; i < 6; i++){//c=0,c=1,...
		for( j = 0; j < total_squares; j++){//squares in one plane
			for( k = 0; k <= 3; k += 3){//two triangles in one square
				start = i*two_d_offset + _start_id[j];
				
				//x0 - x3
				p3 = start + _tri_id[2+k];
				r1 = L - _slab[p3];
				r2 = -_slab[p3+1];
				r3 = -_slab[p3+2];
				//rmat.loadstruct(rdata);
				
				//core matrix
				p1 = start + _tri_id[k];
				p2 = start + _tri_id[1+k];
				
				//left col
				a1 = _slab[p1] - _slab[p3];
				a4 = _slab[p1+1] - _slab[p3+1];
				a7 = _slab[p1+2] - _slab[p3+2];
				
				//middle col
				a2 = _slab[p2] - _slab[p3];
				a5 = _slab[p2+1] - _slab[p3+1];
				a8 = _slab[p2+2] - _slab[p3+2];
				
				//right col
				a3 = 0;
				a6 = -inlab[1];
				a9 = -inlab[2];
				//s.loadstruct(sdata);
				
				determ = a1*(a5*a9 - a6*a8) - a2*(a4*a9 - a6*a7) + a3*(a4*a8 - a5*a7);
				
				if (determ == 0) return MCO_SINGULAR;
								
				s1 = (a5*a9 - a6*a8)*r1 - (a2*a9 - a3*a8)*r2 + (a2*a6 - a3*a5)*r3;
				s1 = s1/determ;
				
				s2 = - (a4*a9 - a6*a7)*r1 + (a1*a9 - a3*a7)*r2 - (a1*a6 - a3*a4)*r3;
				s2 = s2/determ;
				
				s3 = (a4*a8 - a5*a7)*r1 - (a1*a8 - a2*a7)*r2 + (a1*a5 - a2*a4)*r3;
				s3 = s3/determ;


				the1 = s1;
				the2 = s2;
				t = s3;
								
				//check where is the crossing point, or even it exists
				if( the1 >= 0 && the2 >= 0 && the1+the2 <= 1){ //cross the current triangle
					if( t > 1){ //inlab is inside the gamut
						boundlab[0] = L;
						boundlab[1] = inlab[1];
						boundlab[2] = inlab[2];
						return MCO_SUCCESS;
					}
					else if( t >= 0){ //inlab is outside the gamut, need compression
					
						//compute the crossing point
						boundlab[0] = L;
						boundlab[1] = inlab[1]*t;
						boundlab[2] = inlab[2]*t;
						*yes = 1;
						//for test only, find corresponding c,m,y value
						/*
						boundcmy[0] = _scmy[p1]*the1+_scmy[p2]*the2+_scmy[p3]*(1-the1-the2);
						boundcmy[1] = _scmy[p1+1]*the1+_scmy[p2+1]*the2+_scmy[p3+1]*(1-the1-the2);
						boundcmy[2] = _scmy[p1+2]*the1+_scmy[p2+2]*the2+_scmy[p3+2]*(1-the1-the2);
						*/
						return MCO_SUCCESS;
					}
					//else t < 0 on the reverse side of gray axis
				}	
			}	
		}
	}
	
	return MCO_FAILURE; //should change this return value
}						
	

//checking all the sequence for the time being
// Modified by James to be faster
// The code assumes 3x3 matrix and uses registers
McoStatus Cmysurface::findboundarynew(double *inlab, double *inlab2, double *boundlab, double *boundcmy, int16* yes,double gammutExp, int32 singCount)
{
	double	r1,r2,r3;
	register double	a1,a2,a3,a4,a5,a6,a7,a8,a9;
	double	s1,s2,s3;
	double	determ;

	McoStatus status;
	int32 i,j,k;
	int32 total_tri;	//total # of triangles
	int32 start;
//	double sdata[9];	//data of core matrix
//	double rdata[3]; 	//data of right matrix
//	Matrix rmat(3, 1);
//	Matrix lmat(3, 1);
//	Matrix s(3);
	double the1, the2, t;
	int32 two_d_offset, total_squares;
	int32 p1, p2, p3;
	double L, A, B;
	double max = -100;
	int16	yes2 = 0;
	int16	sing_c = 0;

// Need to adjust 
	

	
	
	//assume no compression needed
	*yes = 0;

/*	
	//screen L value, min and max
	L = inlab2[0];
	if( L > _maxgrayL){
		L = _maxgrayL - 0.1;
		*yes = 1;
	}	
	if( L < _mingrayL){
		L = _mingrayL + 0.1;	
		*yes = 1;
	}	
	
	//screen a,b value
	if( fabs(inlab[1]) < SCREEN_DELTA && fabs(inlab[2]) < SCREEN_DELTA ){
		boundlab[0] = L;
		boundlab[1] = inlab[1];
		boundlab[2] = inlab[2];
		return MCO_SUCCESS;
	}
*/	
		
	L = inlab2[0];
	
	// Scale L to be within the boundaries
	L = _mingrayL + 0.1 + L * (_maxgrayL - 0.1 - _mingrayL)/100.0;
	//L = _minL + L * (_maxL  - _minL)/100.0;
	
	A = inlab2[1];
	B = inlab2[2];	
	two_d_offset = 3*_one_d_points*_one_d_points;
	total_squares = (_one_d_points-1)*(_one_d_points-1);
	
	for(i = 0; i < 6; i++){//c=0,c=1,...
		for( j = 0; j < total_squares; j++){//squares in one plane
			for( k = 0; k <= 3; k += 3){//two triangles in one square
				start = i*two_d_offset + _start_id[j];
				
				//x0 - x3
				p3 = start + _tri_id[2+k];
				r1 = L - _slab[p3];
				r2 = A -_slab[p3+1];
				r3 = B -_slab[p3+2];
				//rmat.loadstruct(rdata);
				
				//core matrix
				p1 = start + _tri_id[k];
				p2 = start + _tri_id[1+k];
				
				//left col
				a1 = _slab[p1] - _slab[p3];
				a4 = _slab[p1+1] - _slab[p3+1];
				a7 = _slab[p1+2] - _slab[p3+2];
				
				//middle col
				a2 = _slab[p2] - _slab[p3];
				a5 = _slab[p2+1] - _slab[p3+1];
				a8 = _slab[p2+2] - _slab[p3+2];
				
				//right col
				a3 = L - inlab[0];
				a6 = A -inlab[1];
				a9 = B -inlab[2];
				
				
				determ = a1*(a5*a9 - a6*a8) - a2*(a4*a9 - a6*a7) + a3*(a4*a8 - a5*a7);
				
				if (determ != 0) 
					{
								
					yes2 = 1;			
								
					s1 = (a5*a9 - a6*a8)*r1 - (a2*a9 - a3*a8)*r2 + (a2*a6 - a3*a5)*r3;
					s1 = s1/determ;
					
					s2 = - (a4*a9 - a6*a7)*r1 + (a1*a9 - a3*a7)*r2 - (a1*a6 - a3*a4)*r3;
					s2 = s2/determ;
					
					s3 = (a4*a8 - a5*a7)*r1 - (a1*a8 - a2*a7)*r2 + (a1*a5 - a2*a4)*r3;
					s3 = s3/determ;
					
					//compute
					//s.inv();
					//if( (status = s.get_status()) != MCO_SUCCESS) //singular or ?
					//	return status;
					
					the1 = s1;
					the2 = s2;
					t = s3;
					
					
					//check where is the crossing point, or even it exists
					if( the1 >= 0 && the2 >= 0 && the1+the2 <= 1)
						{ //cross the current triangle
						if( t > 1)
							{ //inlab is inside the gamut
							}
						else if( t >= 0)
							{ //inlab is outside the gamut, need compression
						
							//compute the crossing point
							if(t > max)
								max = t;
							*yes = 1;
							//for test only, find corresponding c,m,y value
							
							}
						//else t < 0 on the reverse side of gray axis
						}		
					}
				else
					{
					sing_c++;
					if (sing_c > singCount) return MCO_SINGULAR;
					}
			}	
		}
	}
	
	if (*yes == 1)
		{
		boundlab[0] = L+(inlab[0] - L)*max*gammutExp;
		boundlab[1] = A+(inlab[1] - A)*max*gammutExp;
		boundlab[2] = B+(inlab[2] - B)*max*gammutExp;
		}
	else
		{
		boundlab[0] = inlab[0]; //_mingrayL + 0.1 + inlab[0] * (_maxgrayL - 0.1 - _mingrayL)/100.0;
		boundlab[1] = inlab[1];
		boundlab[2] = inlab[2];
		}
	
	if (!yes2) return MCO_SINGULAR;
	return MCO_SUCCESS; //should change this return value
}						
	
	
//checking all the sequence for the time being
// Modified by James to be faster
// The code assumes 3x3 matrix and uses registers
McoStatus Cmysurface::findBoundaryNoNorm(double *inlab, double *inlab2, double *boundlab, double *boundcmy, int16* yes,double gammutExp,int32 singCount)
{
	double	r1,r2,r3;
	register double	a1,a2,a3,a4,a5,a6,a7,a8,a9;
	double	s1,s2,s3;
	double	determ;

	McoStatus status;
	int32 i,j,k;
	int32 total_tri;	//total # of triangles
	int32 start;

	double the1, the2, t;
	int32 two_d_offset, total_squares;
	int32 p1, p2, p3;
	double L, A, B;
	double max = -100;
	int16	yes2 = 0;
	int16	sing_c = 0;

// Need to adjust 
	
	//assume no compression needed
	*yes = 0;

	L = inlab2[0];
	if (L < _mingrayL)
		{
		*yes = 0;
		return MCO_SUCCESS;
		}
	if (L > _maxgrayL)
		{
		*yes = 0;
		return MCO_SUCCESS;
		}
	
	A = inlab2[1];
	B = inlab2[2];	
	two_d_offset = 3*_one_d_points*_one_d_points;
	total_squares = (_one_d_points-1)*(_one_d_points-1);
	
	for(i = 0; i < 6; i++){//c=0,c=1,...
		for( j = 0; j < total_squares; j++){//squares in one plane
			for( k = 0; k <= 3; k += 3){//two triangles in one square
				start = i*two_d_offset + _start_id[j];
				
				//x0 - x3
				p3 = start + _tri_id[2+k];
				r1 = L - _slab[p3];
				r2 = A -_slab[p3+1];
				r3 = B -_slab[p3+2];
				//rmat.loadstruct(rdata);
				
				//core matrix
				p1 = start + _tri_id[k];
				p2 = start + _tri_id[1+k];
				
				//left col
				a1 = _slab[p1] - _slab[p3];
				a4 = _slab[p1+1] - _slab[p3+1];
				a7 = _slab[p1+2] - _slab[p3+2];
				
				//middle col
				a2 = _slab[p2] - _slab[p3];
				a5 = _slab[p2+1] - _slab[p3+1];
				a8 = _slab[p2+2] - _slab[p3+2];
				
				//right col
				a3 = L - inlab[0];
				a6 = A -inlab[1];
				a9 = B -inlab[2];
				
				
				determ = a1*(a5*a9 - a6*a8) - a2*(a4*a9 - a6*a7) + a3*(a4*a8 - a5*a7);
				
				if (determ != 0) 
					{
					yes2 = 1;
								
					s1 = (a5*a9 - a6*a8)*r1 - (a2*a9 - a3*a8)*r2 + (a2*a6 - a3*a5)*r3;
					s1 = s1/determ;
					
					s2 = - (a4*a9 - a6*a7)*r1 + (a1*a9 - a3*a7)*r2 - (a1*a6 - a3*a4)*r3;
					s2 = s2/determ;
					
					s3 = (a4*a8 - a5*a7)*r1 - (a1*a8 - a2*a7)*r2 + (a1*a5 - a2*a4)*r3;
					s3 = s3/determ;
					
					the1 = s1;
					the2 = s2;
					t = s3;
					
					//check where is the crossing point, or even it exists
					if( the1 >= 0 && the2 >= 0 && the1+the2 <= 1){ //cross the current triangle
					if( t > 1){ //inlab is inside the gamut
						//	boundlab[0] = inlab[0];
						//	boundlab[1] = inlab[1];
						//	boundlab[2] = inlab[2];
						//	return MCO_SUCCESS;
						}
					else if( t >= 0){ //inlab is outside the gamut, need compression
						
							if(t > max)
								max = t;
							*yes = 1;
						}
						//else t < 0 on the reverse side of gray axis
					}
				}
			else
				{
				sing_c++;
				if (sing_c > singCount) return MCO_SINGULAR;
				}	
			}	
		}
	}
	
	if (*yes == 1)
		{
		boundlab[0] = L+(inlab[0] - L)*max*gammutExp;
		boundlab[1] = A+(inlab[1] - A)*max*gammutExp;
		boundlab[2] = B+(inlab[2] - B)*max*gammutExp;
		}
	else
		{
		boundlab[0] = inlab[0]; //_mingrayL + 0.1 + inlab[0] * (_maxgrayL - 0.1 - _mingrayL)/100.0;
		boundlab[1] = inlab[1];
		boundlab[2] = inlab[2];
		}
	
	if (!yes2) return MCO_SINGULAR;
	
	return MCO_SUCCESS; //should change this return value
}							