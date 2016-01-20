//color conversion functions
#include <math.h>
#include "matrix.h"
#include "colorconv.h"

#define SIXOVER (16.0/116.0)
#define SEVENDOT (7.7867)
#define pi		(3.1415926535)
#define DEGREE_PER_PI ( 180/pi )

static double _adjustangle(double angle, short phase);

//LAB to normalized xyz
void labtonxyz(double *LAB, double *xyz)
{
	double L, A, B, fx, fy, fz;
	
	L = LAB[0];
	A = LAB[1];
	B = LAB[2];	
	
	fy = (L+16.0)/116.0;
	fx = A/500.0 + fy;
	fz = fy - B/200.0;
	
	if(fx <= 0.2069)
		//use formula 7.7867*(Y/Yn)+16/116
		xyz[0] = (fx-SIXOVER)/SEVENDOT;
	else
		//use formula (Y/Yn)^1/3
		xyz[0] = pow(fx, 3.0);

	if(fy <= 0.2069)
		//use formula 7.7867*(Y/Yn)+16/116
		xyz[1] = (fy-SIXOVER)/SEVENDOT;
	else
		//use formula (Y/Yn)^1/3
		xyz[1] = pow(fy, 3.0);

	if(fz <= 0.2069)
		//use formula 7.7867*(Y/Yn)+16/116
		xyz[2] = (fz-SIXOVER)/SEVENDOT;
	else
		//use formula (Y/Yn)^1/3
		xyz[2] = pow(fz, 3.0);

}

//LAB to normalized xyz in place
void labtonxyzinplace(double *LAB, long num)
{
	double xyz[3];
	
	for(long i = 0; i < num; i++){
		labtonxyz(LAB, xyz);
		*LAB++ = xyz[0];
		*LAB++ = xyz[1];
		*LAB++ = xyz[2];
	}
}


//normalized xyz to LAB
void nxyztoLab(double *xyz, double *LAB)
{
	double fx, fy, fz;
	double nx, ny, nz;
	double L, A, B;
	
	nx = *xyz++;
	ny = *xyz++;
	nz = *xyz;
	
	if(nx > 0.008856)
		//fx = powd( nx, 1.0/3.0);
		fx = pow( nx, 1.0/3.0);
	else
		fx = 7.7867*nx + SIXOVER;
			
	if(ny > 0.008856)
		//fy = powd( ny, 1.0/3.0);
		fy = pow( ny, 1.0/3.0);
	else
		fy = 7.7867*ny + SIXOVER;
		
	if(nz > 0.008856)
		//fz = (double)powd( nz, 1.0/3.0);
		fz = (double)pow( nz, 1.0/3.0);
	else
		fz = 7.7867*nz + SIXOVER;
	
	L = 116.0*fy - 16.0;
	if(L > 100.0) L = 100.0;
	if(L < 0.0)	L = 0.0;
	
	A = 500.0*(fx-fy);
	if(A > 127.0) A = 127.0;
	if(A < -128.0) A = -128.0;
	
	B = 200.0*(fy-fz);
	if(B > 127.0) B = 127.0;
	if(B < -128.0) B = -128.0;
	
	*LAB++ = L;
	*LAB++ = A;
	*LAB   = B;
}

//normalized xyz to LAB in place
void nxyztoLabinplace(double *xyz, long num)
{
	double LAB[3];
	
	for(long i = 0; i < num; i++){
		nxyztoLab(xyz, LAB);
		*xyz++ = LAB[0];
		*xyz++ = LAB[1];
		*xyz++ = LAB[2];
	}
}

//normalized xyz to specified xyz
void nxyztoxyz(double *inxyz, double *white, double *outxyz)
{
	long i;

	for(i = 0; i < 3; i++)
		outxyz[i] *= inxyz[i]*white[i];
}

//normalized xyz to specified xyz in place
void nxyztoxyzinplace(double *inxyz, double *white, long size)
{
	long i, j;
	
	for(i = 0; i < size; i++){
		for(j = 0; j < 3; j++)
			inxyz[j] *= white[j];
			
		inxyz += 3;
	}
}			

//LAB to LCH
void labtolch(double *LAB, double *lch)
{
	double L, A, B, C, H;
	
	L = LAB[0];
	A = LAB[1];
	B = LAB[2];	
	
	C = sqrt(A*A + B*B);
	
	if(A == 0){//
		if(B == 0)
			H = 0;
		else if(B > 0)
			H = 90;
		else
			H = 270;
	}
	else{ // A != 0
		
		if( A > 0 ){ //phase 1 and 4
			if( B >= 0 )
				H = DEGREE_PER_PI*_adjustangle(atan(B/A), 1);
			else
				H = DEGREE_PER_PI*_adjustangle(atan(B/A), 4);	
		}
		else{ //phase 2 and 3
			if( B >= 0 )
				H = DEGREE_PER_PI*_adjustangle(atan(B/A), 2);
			else
				H = DEGREE_PER_PI*_adjustangle(atan(B/A), 3);	
		}			
	}
	
	*lch++ = L;
	*lch++ = C;
	*lch = H;
}	
	
//return angle from 0 to 2¹		
static double _adjustangle(double angle, short phase)
{
	switch(phase){
		case(1):
			return (angle);
		
		case(2):
			return (pi + angle);
			
		case(3):
			return (pi + angle);
			
		case(4):
			return (2*pi + angle);
	}
	
	return 0;
}					

//LCH to LAB
void lchtolab(double *lch, double *LAB)
{
	double L, A, B, C, H;
	double bovera;
	
	L = lch[0];
	C = lch[1];
	H = lch[2];	
	
	if( H == 90 || H == 270 ){
		A = 0;
		if( H == 90 )
			B = C;
		else
			B = -C;	
	}
	else{	
		bovera = tan(H/DEGREE_PER_PI);
		A = C/( sqrt(1+ bovera*bovera) );
		B = A*bovera;
		
		if( 0 <= H  && H < 90){
			A = fabs(A);
			B = fabs(B);
		}
		else if( 90 <= H  && H < 180){
			A = -fabs(A);
			B = fabs(B);
		}
		else if( 180 <= H && H < 270){
			A = -fabs(A);
			B = -fabs(B);
		}	
		else if( 270 <= H && H < 360){
			A = fabs(A);
			B = -fabs(B);
		}
	}
	
	*LAB++ = L;
	*LAB++ = A;
	*LAB = B;
}

//lab to ntscrgb value from (0 to 255)
//using D50, ntsc, gamma 1
//precompute matrix is:
//    2.2608   -0.9234   -0.3111
//  -1.0409    2.0206   -0.0204
//   -0.0750    0.0306    1.2625
void labtontscrgb(double *LAB, unsigned char *rgb)
{
	double nxyz[3];
	short r, g, b;
	
	labtonxyz(LAB, nxyz);
	
	r = (2.2608*nxyz[0] - 0.9234*nxyz[1] - 0.3111*nxyz[2])*255;
	if( r < 0)
		r = 0;
	if( r > 255)
		r = 255;	
	g = (-1.0409*nxyz[0] + 2.0206*nxyz[1] - 0.0204*nxyz[2])*255; 
	if( g < 0)
		g = 0;
	if( g > 255)
		g = 255;	
	b = (-0.0750*nxyz[0] + 0.0306*nxyz[1] + 1.2625*nxyz[2])*255; 
	if( b < 0)
		b = 0;
	if( b > 255)
		b = 255;	
	
	rgb[0] = (unsigned char)r;
	rgb[1] = (unsigned char)g; 
	rgb[2] = (unsigned char)b; 
}	
		
//luv to LAB		
void luvtolab(double *luv, double *lab)
{
double t;
double xyz[3];
double c, d;
double X, Y, Z;
double bot;


t = (16.0+luv[0])/116.0;
t = t*t*t;
xyz[1] = t;

Y = t*100;

c = luv[1];
d = luv[2];

bot = 12*d;
X = 27*c/bot;
Z = -(9*c - 36 + 60*d)/bot;

xyz[0] = X*Y/96.442;

xyz[2] = Z*Y/82.521;

nxyztoLab(xyz,lab);
}

//added by James on 1/22/96		
void sxyztolab(double *Lxy, double *lab)
{
double t;
double xyz[3];
double X, Y, Z;

t = (16.0+Lxy[0])/116.0;
t = t*t*t;
xyz[1] = t;

Y = t*100;


X = Lxy[1]*Y/Lxy[2];
Z = (1-Lxy[1]-Lxy[2])*Y/Lxy[2];

xyz[0] = X/96.442;

xyz[2] = Z/82.521;

nxyztoLab(xyz,lab);
}


//rgb to normalized xyz
//note rgb is the rgb with gamma value
//mat is the rgb to nxyz conversion matrix
//the returned xyz is the normarlized xyz
void rgbtonxyz(double *mat, double gamma, unsigned char* rgb, double *nxyz, long num)
{
	long i, j;
	double gammat[256];
	double r, g, b, x, y, z;
	
	for( i = 0; i < 256; i++)
		gammat[i] = pow((double)i/255.0, gamma);
			
	for(i = 0; i < num; i++){
		j = 3*i;
		r = gammat[rgb[j]];
		g = gammat[rgb[j+1]];
		b = gammat[rgb[j+2]];
		x = mat[0]*r + mat[1]*g + mat[2]*b;
		y = mat[3]*r + mat[4]*g + mat[5]*b;
		z = mat[6]*r + mat[7]*g + mat[8]*b;
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		if(z < 0) z = 0;
		
		nxyz[j] = x;
		nxyz[j+1] = y;
		nxyz[j+2] = z;
	}	
}

//rgb to normalized xyz
//note rgb is the rgb with gamma value
//mat is the rgb to nxyz conversion matrix
//the returned xyz is the normarlized xyz which overwrite the input
//rgb values
void rgbtonxyzover(double *mat, double gamma, double *xyz, long num)
{
	long i, j;
	double gammat[256];
	double r, g, b, x, y, z;
	
	for( i = 0; i < 256; i++)
		gammat[i] = pow((double)i/255.0, gamma);
			
	for(i = 0; i < num; i++){
		j = 3*i;
		r = gammat[(unsigned char)(xyz[j] + 0.5)];
		g = gammat[(unsigned char)(xyz[j+1] + 0.5)];
		b = gammat[(unsigned char)(xyz[j+2] + 0.5)];
		x = mat[0]*r + mat[1]*g + mat[2]*b;
		y = mat[3]*r + mat[4]*g + mat[5]*b;
		z = mat[6]*r + mat[7]*g + mat[8]*b;
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		if(z < 0) z = 0;
		
		xyz[j] = x;
		xyz[j+1] = y;
		xyz[j+2] = z;
	}	
}

void rgbtonxyzover2(double *mat, double *rgammat, double *ggammat,
	double *bgammat, double *xyz, long num)
{
	long i, j;
	double r, g, b, x, y, z;

	for(i = 0; i < num; i++){
		j = 3*i;
		r = rgammat[(unsigned char)(xyz[j] + 0.5)];
		g = ggammat[(unsigned char)(xyz[j+1] + 0.5)];
		b = bgammat[(unsigned char)(xyz[j+2] + 0.5)];
		x = mat[0]*r + mat[1]*g + mat[2]*b;
		y = mat[3]*r + mat[4]*g + mat[5]*b;
		z = mat[6]*r + mat[7]*g + mat[8]*b;
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		if(z < 0) z = 0;
		
		xyz[j] = x;
		xyz[j+1] = y;
		xyz[j+2] = z;
	}	
}

//normalized xyz to rgb
//mat is the nxyz to rgb conversion matrix
void nxyztorgb(double *mat, double gamma, double *nxyz, unsigned char* rgb, long num)
{
	long i, j;
	char gammat[256];
	double r, g, b, x, y, z;
	
	gamma = 1.0/gamma;
	for( i = 0; i < 256; i++)
		gammat[i] = (255.0*pow((double)i/255.0, gamma) + 0.5);

	for( i = 0; i < num; i++){
		j = 3*i;
		x = nxyz[j];
		y = nxyz[j+1];
		z = nxyz[j+2];
		r = mat[0]*x + mat[1]*y + mat[2]*z;
		g = mat[3]*x + mat[4]*y + mat[5]*z;
		b = mat[6]*x + mat[7]*y + mat[8]*z;
		if(r < 0) r = 0;
		if(r > 1) r = 1;
		if(g < 0) g = 0;
		if(g > 1) g = 1;
		if(b < 0) b = 0;
		if(b > 1) b = 1;
		
		rgb[j] = gammat[(short)(255*r + 0.5)];
		rgb[j+1] = gammat[(short)(255*g + 0.5)];
		rgb[j+2] = gammat[(short)(255*b + 0.5)];
	}		
}

void nxyztorgb2(double *mat, double *rgammat, double *ggammat,
	double *bgammat, double *nxyz, unsigned char* rgb, long num)
{
	long i, j;
	double r, g, b, x, y, z;

	for( i = 0; i < num; i++){
		j = 3*i;
		x = nxyz[j];
		y = nxyz[j+1];
		z = nxyz[j+2];
		r = mat[0]*x + mat[1]*y + mat[2]*z;
		g = mat[3]*x + mat[4]*y + mat[5]*z;
		b = mat[6]*x + mat[7]*y + mat[8]*z;
		if(r < 0) r = 0;
		if(r > 1) r = 1;
		if(g < 0) g = 0;
		if(g > 1) g = 1;
		if(b < 0) b = 0;
		if(b > 1) b = 1;
		
		rgb[j] = (unsigned char)(255.0*rgammat[(short)(255*r + 0.5)] + 0.5);
		rgb[j+1] = (unsigned char)(255.0*ggammat[(short)(255*g + 0.5)] + 0.5);
		rgb[j+2] = (unsigned char)(255.0*bgammat[(short)(255*b + 0.5)] + 0.5);
	}		
}

//build rgb to xyz conversion
//return the value in rgbtoxyz and xyztorgb 
//if return 0, ok, else something wrong in the conversion
McoStatus buildrgbxyz(double *monitor, double *xyztorgb, double *rgbtoxyz)
{
	double k1, k2, k3;
	Matrix phosmat(3);
	Matrix phosmat2(3);
	Matrix wcolmat(3, 1);
	Matrix cmat(3);
	Matrix ccolmat(3, 1);
	Matrix rgbtoxyzmat(3);
	Matrix xyztorgbmat(3);
	
	
	phosmat.setval(1, 1, monitor[3]);
	phosmat.setval(2, 1, monitor[4]);
	phosmat.setval(3, 1, (1.0 - monitor[3] - monitor[4]));
	phosmat.setval(1, 2, monitor[5]);
	phosmat.setval(2, 2, monitor[6]);
	phosmat.setval(3, 2, (1.0 - monitor[5] - monitor[6]));
	phosmat.setval(1, 3, monitor[7]);
	phosmat.setval(2, 3, monitor[8]);
	phosmat.setval(3, 3, (1.0 - monitor[7] - monitor[8]));

	phosmat2 = phosmat;
	
	//compute inverse phosphor matrix
	phosmat.inv();

	//singular matrix or something strange happened
	if( phosmat.get_status() != CL_SUCCESS)
		return CL_SINGULAR;
		
	//load white 
	wcolmat.setval(1, 1, (monitor[1]/monitor[2]));
	wcolmat.setval(2, 1, (monitor[2]/monitor[2]));
	wcolmat.setval(3, 1, ((1.0 - monitor[1] - monitor[2])/monitor[2]));

	//compute k1, k2, k3
	ccolmat = phosmat*wcolmat;
	ccolmat.getval(1, 1, &k1);
	ccolmat.getval(2, 1, &k2);
	ccolmat.getval(3, 1, &k3);
	
	//load k1,k2,k3 and inverse
	cmat.setval(1, 1, k1);
	cmat.setval(2, 2, k2);
	cmat.setval(3, 3, k3);

	//compute the RGB to XYZ conversion matrix;
	rgbtoxyzmat = phosmat2*cmat;
		
	//compute the xyz to rgb conversion matrix
	xyztorgbmat = rgbtoxyzmat;
	xyztorgbmat.inv();
	
	//singular matrix or something strange happened
	if(xyztorgbmat.get_status() != CL_SUCCESS)
		return CL_SINGULAR;
	
	//copy the conversion matrix to the data struct
	rgbtoxyzmat.savestruct(rgbtoxyz);	
	xyztorgbmat.savestruct(xyztorgb);
	
	return CL_SUCCESS;
} 


//build rgb to xyz conversion
//note the values are abosolute based on the input white point
//return the value in rgbtoxyz and xyztorgb 
//if return 0, ok, else something wrong in the conversion
McoStatus newbuildrgbxyz(double *monitor, double *xyztorgb, double *rgbtoxyz)
{
	double k1, k2, k3;
	Matrix phosmat(3);
	Matrix wcolmat(3, 1);
	Matrix ccolmat(3, 1);
	Matrix rgbtoxyzmat(3);
	Matrix xyztorgbmat(3);
	
	//load chromaticity(phosphor) matrix
	phosmat.setval(1, 1, monitor[3]);
	phosmat.setval(2, 1, monitor[4]);
	phosmat.setval(3, 1, (1.0 - monitor[3] - monitor[4]));
	phosmat.setval(1, 2, monitor[5]);
	phosmat.setval(2, 2, monitor[6]);
	phosmat.setval(3, 2, (1.0 - monitor[5] - monitor[6]));
	phosmat.setval(1, 3, monitor[7]);
	phosmat.setval(2, 3, monitor[8]);
	phosmat.setval(3, 3, (1.0 - monitor[7] - monitor[8]));

	rgbtoxyzmat = phosmat;
	
	//inverse chromaticity(phosphor) matrix
	phosmat.inv();

	//singular matrix or something strange happened
	if( phosmat.get_status() != CL_SUCCESS)
		return CL_SINGULAR;
		
	//load white 
	wcolmat.setval(1, 1, monitor[1]/monitor[2]);
	wcolmat.setval(2, 1, 1);
	wcolmat.setval(3, 1, (1.0 - monitor[1] - monitor[2])/monitor[2]);

	//compute k1, k2, k3
	ccolmat = phosmat*wcolmat;
	ccolmat.getval(1, 1, &k1);
	ccolmat.getval(2, 1, &k2);
	ccolmat.getval(3, 1, &k3);
	
	//substitude k1,k2,k3 back to the chromaticity matrix
	//to get the RGB to XYZ conversion matrix;
	rgbtoxyzmat.coltimes(1, k1);
	rgbtoxyzmat.coltimes(2, k2);
	rgbtoxyzmat.coltimes(3, k3);
			
	//compute the xyz to rgb conversion matrix
	xyztorgbmat = rgbtoxyzmat;
	xyztorgbmat.inv();
	
	//singular matrix or something strange happened
	if(xyztorgbmat.get_status() != CL_SUCCESS)
		return CL_SINGULAR;
	
	//copy the conversion matrix to the data struct
	rgbtoxyzmat.savestruct(rgbtoxyz);	
	xyztorgbmat.savestruct(xyztorgb);
	
	return CL_SUCCESS;
} 
