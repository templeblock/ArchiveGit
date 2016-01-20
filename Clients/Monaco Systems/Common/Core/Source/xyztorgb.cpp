#include "xyztorgb.h"
#include <math.h>

#define POWER31 (1.0/3.0)
#define FACTOR 	(16.0/116.0)



McoStatus ConvertMonitor(PhotoMonitor *pmonitor, Monitor *monitor)
{

monitor->setup[0] = (double)(pmonitor->gamma)/100;
monitor->setup[1] = (double)(pmonitor->whiteX)/10000;
monitor->setup[2] = (double)(pmonitor->whiteY)/10000;
monitor->setup[3] = (double)(pmonitor->RedX)/10000;
monitor->setup[4] = (double)(pmonitor->RedY)/10000;
monitor->setup[5] = (double)(pmonitor->GreenX)/10000;
monitor->setup[6] = (double)(pmonitor->GreenY)/10000;
monitor->setup[7] = (double)(pmonitor->BlueX)/10000;
monitor->setup[8] = (double)(pmonitor->BlueY)/10000;
return MCO_SUCCESS;
}

XyztoRgb::XyztoRgb(void)
{
	_xyztorgbt = new Matrix(3);
	if(!_xyztorgbt)	delete this;
	
	_rgbtoxyzt = new Matrix(3);
	if(!_rgbtoxyzt) delete this;
}


XyztoRgb::~XyztoRgb(void)
{
	if(_xyztorgbt){
		delete _xyztorgbt;
		_xyztorgbt = 0;
	}	
	if(_rgbtoxyzt){
		delete _rgbtoxyzt;
		_rgbtoxyzt = 0;
	}	
}

McoStatus XyztoRgb::getLumScale(double* xyz, double maxrgb, double gamma, double* scale)
{
	int32 i, j;
	double rgb[3], conxyz[3];
	double L, conL;
	
//apply reverse gamma(actually it is the gamma passed in by photoshop)
//on maxrgb
	if(maxrgb > 255.0)
		maxrgb = 255.0;
	for(i = 0; i < 3; i++)
		rgb[i] = pow( maxrgb/255.0, gamma);	

/*
//do conversion from rgb to xyz
	Matrix tempmat(1, 3);
	tempmat.loadstruct(rgb);
 	Matrix rgbmat = tempmat*(*_rgbtoxyzt);

	rgbmat.savestruct(conxyz);
*/
	
//compute the scaling factor from Y value according to Lab to
//XYZ transformation
		
	L = 116.0*pow(xyz[1], POWER31) - 16.0;
	conL = 116.0*pow(rgb[1], POWER31) - 16.0;	
	
	if(conL == 0.0)
		return MCO_FAILURE;
	else
		*scale = conL/L;
		
	return MCO_SUCCESS;
}			

//scale factor is for L channel in Lab
McoStatus XyztoRgb::scaleLuminance(double* xyz, double scale)
{
	double *x, *y, *z;
	double x31, y31, z31, newx, newz, deltay31;
	double newy[264];
	double white;
	int32	i;
	
	y = xyz +1;
	for(i = 0; i < 264; i++){
		//note, newy[i] is the 1/3 power of yN/yn
		newy[i] = scale*pow(*y, POWER31) + FACTOR*(1-scale);
		y += 3;
	}	

	x = xyz;
	y = xyz+1;
	z = xyz+2;
		
	for(i = 0 ; i < 264; i++){
		deltay31 = newy[i] - pow(*y, POWER31);
		
		newx = pow( pow(*x, POWER31) + deltay31, 3.0);
		newz = pow( pow(*z, POWER31) + deltay31, 3.0);
		*x = newx;
		x += 3;
		*y = pow( newy[i], 3.0);
		y += 3;
		*z = newz;
		z += 3;
	}
	return MCO_SUCCESS;	
}	

McoStatus XyztoRgb::xyz2rgb(Monitor *monitor)
{
	double k1, k2, k3;
	Matrix phosmat(3);
	Matrix phosmat2(3);
	Matrix wcolmat(3, 1);
	Matrix cmat(3);
	Matrix ccolmat(3, 1);
	
	memcpy((char*)&_monitor, (char*)monitor, sizeof(Monitor));
	phosmat.setval(1, 1, _monitor.setup[3]);
	phosmat.setval(2, 1, _monitor.setup[4]);
	phosmat.setval(3, 1, (1.0 - _monitor.setup[3] - _monitor.setup[4]));
	phosmat.setval(1, 2, _monitor.setup[5]);
	phosmat.setval(2, 2, _monitor.setup[6]);
	phosmat.setval(3, 2, (1.0 - _monitor.setup[5] - _monitor.setup[6]));
	phosmat.setval(1, 3, _monitor.setup[7]);
	phosmat.setval(2, 3, _monitor.setup[8]);
	phosmat.setval(3, 3, (1.0 - _monitor.setup[7] - _monitor.setup[8]));

	phosmat2 = phosmat;
	
	//compute inverse phosphor matrix
	phosmat.inv();

	if( phosmat.get_status() != MCO_SUCCESS)
		return MCO_SINGULAR;
		
	//load white 
//	Matrix wcolmat(3, 1);
	wcolmat.setval(1, 1, (_monitor.setup[1]/_monitor.setup[2]));
	wcolmat.setval(2, 1, (_monitor.setup[2]/_monitor.setup[2]));
	wcolmat.setval(3, 1, ((1.0 - _monitor.setup[1] - _monitor.setup[2])/_monitor.setup[2]));

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
	*_rgbtoxyzt = phosmat2*cmat;

	//transvers of RGB to XYZ conversion matrix
	_rgbtoxyzt->T();	
		
	//compute the xyz to rgb conversion matrix
	//actually, it's the transverse of conversion matrix
	*_xyztorgbt = *_rgbtoxyzt;
	_xyztorgbt->inv();
	if(_xyztorgbt->get_status() != MCO_SUCCESS)
		return MCO_SINGULAR;
		
	return MCO_SUCCESS;
}	


//now only support patch == 1
McoStatus XyztoRgb::getGamma(double* xyz, int16 *colors, int16 instartloc, int16 patches, double* outgamma)
{
	Matrix xyzmat(patches, 3);
	
	int16 startloc = instartloc*3;
	xyzmat.loadstruct(&xyz[startloc]);
	
	Matrix tempmat2 = xyzmat*(*_xyztorgbt);
	
	tempmat2*255.0;

	double r, g, b;
	for(int32 i = 1; i <= patches; i++){
		tempmat2.getval(1, 1, &r);
		tempmat2.getval(1, 2, &g);
		tempmat2.getval(1, 3, &b);
	}
	
	double refaverg = (r+g+b)/765.0;
	double meaaverg = (colors[startloc] + colors[startloc+1] + colors[startloc+2])/765.0;
	
	double gamma = log(meaaverg)/log(refaverg);
	
	//note we return inverse gamma, so it is compatible with photoshop	
	*outgamma = (1.0/gamma);
	
	return MCO_SUCCESS;
}


McoStatus XyztoRgb::compute(double* xyz, double ingamma, long num)
{
	int32	i,n;
	double *xyzh;
	double xw,zw;
	double	xyz_rgb[9];
	double a1,a2,a3;
	double b1,b2,b3;
	double c1,c2,c3;
	double a,b,c;
	
	//fix the normalization on 8/11
	//normalize the channel by the using current white point
	xw = _monitor.setup[1]/_monitor.setup[2];
	zw = (1.0 - _monitor.setup[1] - _monitor.setup[2])/_monitor.setup[2];
	xyzh = xyz;
	for( i = 0; i < num; i++){
		xyzh[0] *= xw;
		xyzh[2] *= zw;
		xyzh += 3;
	}
			
	_xyztorgbt->savestruct(xyz_rgb);
	
	a1 = xyz_rgb[0];
	a2 = xyz_rgb[3];
	a3 = xyz_rgb[6];
	
	b1 = xyz_rgb[1];
	b2 = xyz_rgb[4];
	b3 = xyz_rgb[7];
	
	c1 = xyz_rgb[2];
	c2 = xyz_rgb[5];
	c3 = xyz_rgb[8];
	
/*	a1 = xyz_rgb[0];
	a2 = xyz_rgb[1];
	a3 = xyz_rgb[2];
	
	b1 = xyz_rgb[3];
	b2 = xyz_rgb[4];
	b3 = xyz_rgb[5];
	
	c1 = xyz_rgb[6];
	c2 = xyz_rgb[7];
	c3 = xyz_rgb[9]; */
	
	for( i = 0; i < num; i++){
		n = i*3;
		a = xyz[n++];
		b = xyz[n++];
		c = xyz[n];
		n -= 2;
		xyz[n++] =   a1*a + a2*b + a3*c;
		xyz[n++] = b1*a + b2*b + b3*c;
		xyz[n] = c1*a + c2*b + c3*c;
		}
	
	//xyzmat.loadstruct(xyz);
	
	//Matrix tempmat2 = xyzmat*(*_xyztorgbt);

	//tempmat2.savestruct(xyz);
	
	//apply the gamma curve
	//fix ambient light on 8/11
	//double gamma = _monitor.setup[11]/ingamma;
	double gamma = 1.0/ingamma;
	
	long num3 = num*3;
	for( i = 0 ; i < num3; i++){
		if(xyz[i] < 0.0)
			xyz[i] = 0.0;
		xyz[i] = 255.0*pow( xyz[i], gamma);	
	}
	
	return MCO_SUCCESS;
}			


McoStatus XyztoRgb::computeInv(double* xyz, double ingamma, long num)
{
	int32	i,n;
	double *xyzh;
	double xw,zw;
	double	xyz_rgb[9];
	double a1,a2,a3;
	double b1,b2,b3;
	double c1,c2,c3;
	double a,b,c;
	
	long num3 = num*3;
	for( i = 0 ; i < num3; i++){
		if(xyz[i] < 0.0)
			xyz[i] = 0.0;
		xyz[i] = pow( xyz[i]/255.0, ingamma);	
	}
	
	
	_rgbtoxyzt->savestruct(xyz_rgb);
	
	a1 = xyz_rgb[0];
	a2 = xyz_rgb[3];
	a3 = xyz_rgb[6];
	
	b1 = xyz_rgb[1];
	b2 = xyz_rgb[4];
	b3 = xyz_rgb[7];
	
	c1 = xyz_rgb[2];
	c2 = xyz_rgb[5];
	c3 = xyz_rgb[8];
	
	for( i = 0; i < num; i++){
		n = i*3;
		a = xyz[n++];
		b = xyz[n++];
		c = xyz[n];
		n -= 2;
		xyz[n++] =   a1*a + a2*b + a3*c;
		xyz[n++] = b1*a + b2*b + b3*c;
		xyz[n] = c1*a + c2*b + c3*c;
		}
		
//	Matrix xyzmat(num, 3);
	
//	xyzmat.loadstruct(xyz);
	
//	Matrix tempmat2 = xyzmat*(*_rgbtoxyzt);

//	tempmat2.savestruct(xyz);
	
	//apply the gamma curve
	//fix ambient light on 8/11
	//double gamma = _monitor.setup[11]/ingamma;

	//fix the normalization on 8/11
	//normalize the channel by the using current white point
	xw = _monitor.setup[1]/_monitor.setup[2];
	zw = (1.0 - _monitor.setup[1] - _monitor.setup[2])/_monitor.setup[2];
	xyzh = xyz;
	for( i = 0; i < num; i++){
		xyzh[0] = xyzh[0]/xw;
		xyzh[2] = xyzh[2]/zw;
		xyzh += 3;
	}

	return MCO_SUCCESS;
}			




// added by James, loads the xyztorgb matrix into an array for processing by other code
McoStatus XyztoRgb::getMatrix(double *matrix)
{
_xyztorgbt->savestruct(matrix);
return MCO_SUCCESS;
}


McoStatus XyztoRgb::getrgb2xyzMatrix(double *matrix)
{
_rgbtoxyzt->savestruct(matrix);
return MCO_SUCCESS;
}

// Added by James to compute any size array and output to unsigned char

McoStatus XyztoRgb::eval(double* xyz, unsigned char *rgb, int32 num, double ingamma)
{
	int32	i;
	double *xyzh;
	double xw,zw;
	
	//fix the normalization on 8/11
	//normalize the channel by the using current white point
	xw = _monitor.setup[1]/_monitor.setup[2];
	zw = (1.0 - _monitor.setup[1] - _monitor.setup[2])/_monitor.setup[2];
	xyzh = xyz;
	for( i = 0; i < num; i++){
		xyzh[0] *= xw;
		xyzh[2] *= zw;
		xyzh += 3;
	}
		
	Matrix xyzmat(num, 3);
	
	xyzmat.loadstruct(xyz);
	
	Matrix tempmat2 = xyzmat*(*_xyztorgbt);

	tempmat2.savestruct(xyz);
	
	//apply the gamma curve
	//fix ambient light on 8/11
	double gamma = _monitor.setup[11]/ingamma;
	
	for( i = 0 ; i < num*3; i++){
		if(xyz[i] < 0.0)
			xyz[i] = 0.0;
		rgb[i] = (unsigned char)(255.0*pow( xyz[i], gamma));	
	}
	
	return MCO_SUCCESS;
}										