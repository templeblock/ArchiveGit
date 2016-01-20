#include "colorconvert.h"
#include <math.h>

// Convert a Yxy color to an XYZ color.
void convertYxyToXYZ(YxyColorType aYxyColor, XYZColorType *anXYZColor) {
	(*anXYZColor).X = aYxyColor.x * aYxyColor.Y / aYxyColor.y;
	(*anXYZColor).Y = aYxyColor.Y;
	(*anXYZColor).Z = (1.0 - aYxyColor.x - aYxyColor.y) * aYxyColor.Y / aYxyColor.y;
}


// Convert an  XYZ color to a Yxy color.
void convertXYZToYxy(XYZColorType anXYZColor, YxyColorType *aYxyColor) {
	double sum = anXYZColor.X + anXYZColor.Y + anXYZColor.Z;
	
	(*aYxyColor).Y = anXYZColor.Y;
	(*aYxyColor).x = anXYZColor.X / sum;
	(*aYxyColor).y = anXYZColor.Y / sum;
}


// convert normalized (by white point) XYZ color intensity to L*a*b* color intensity
void convertXYZToLab(XYZColorType anXYZColor, LabColorType *aLabColor) {
	double fx, fy, fz;

	if (anXYZColor.X <= 0.008856) fx = 7.7867 * anXYZColor.X + 16.0/116.0;
	else fx = pow(anXYZColor.X,1.0/3.0);
	
	if (anXYZColor.Y <= 0.008856) fy = 7.7867 * anXYZColor.Y + 16.0/116.0;
	else fy = pow(anXYZColor.Y,1.0/3.0);

	if (anXYZColor.Z <= 0.008856) fz = 7.7867 * anXYZColor.Z + 16.0/116.0;
	else fz = pow(anXYZColor.Z,1.0/3.0);

	aLabColor->L = 116.0 * fy - 16.0;
	aLabColor->a = 500.0 * (fx - fy);
	aLabColor->b = 200.0 * (fy - fz);
}


// convert L*a*b* color intensity to normalized (by white point) XYZ color intensity. 
void convertLabToXYZ(LabColorType aLabColor, XYZColorType *anXYZColor) {
	double fx, fy, fz;
	
	fy = (aLabColor.L + 16.0)/116.0;
	fx = (aLabColor.a / 500.0) + fy;
	fz = fy - (aLabColor.b / 200.0);

	if (fx <= 0.20689) anXYZColor->X = (fx - (16.0/116.0))/7.7867;
	else anXYZColor->X = fx*fx*fx;
	
	if (fy <= 0.20689) anXYZColor->Y = (fy - (16.0/116.0))/7.7867;
	else anXYZColor->Y = fy*fy*fy;

	if (fz <= 0.20689) anXYZColor->Z = (fz - (16.0/116.0))/7.7867;
	else anXYZColor->Z = fz*fz*fz;
}

                           

// Convert RGB to XYZ color intensities.
void convertRGBToXYZ(RGBNumber rgb, XYZColorType R, XYZColorType G, XYZColorType B, 
		RGBNumber gamma, XYZColorType *resultXYZColor) {

	// factor in gamma
	rgb.red 		= pow(rgb.red, gamma.red);
	rgb.green 	= pow(rgb.green, gamma.green);
	rgb.blue 	= pow(rgb.green, gamma.blue);
	
	resultXYZColor->X = rgb.red * R.X + rgb.green * G.X + rgb.blue * B.X;
	resultXYZColor->Y = rgb.red * R.Y + rgb.green * G.Y + rgb.blue * B.Y;
	resultXYZColor->Z = rgb.red * R.Z + rgb.green * G.Z + rgb.blue * B.Z;
}


// Convert XYZ color intensities to normalized RGB values.
void convertXYZToRGB(XYZColorType anXYZColor, XYZColorType R, XYZColorType G, 
		XYZColorType B, RGBNumber gamma, RGBNumber *rgb) {

	//determinant	
	double det = R.X*(G.Y*B.Z - G.Z*B.Y) + R.Y*(G.Z*B.X - G.X*B.Z) + R.Z*(G.X*B.Y - G.Y*B.X);

	if (det<0.000000001) {
		// Report error or something
		// det should never be zero unless the colors are linearly dependent
		// or one or more of the color has zero intensity.
		;
	}

		
	// Red component
	rgb->red 	= (anXYZColor.X*(G.Y*B.Z - G.Z*B.Y) + anXYZColor.Y*(G.Z*B.X - G.X*B.Z) 
				+ anXYZColor.Z*(G.X*B.Y - G.Y*B.X)) / det;
	if (rgb->red < 0.0) rgb->red = 0.0;
	if (rgb->red > 1.0) rgb->red = 1.0;

	// Green component
	rgb->green 	= (anXYZColor.X*(R.Z*B.Y - R.Y*B.Z) + anXYZColor.Y*(R.X*B.Z - R.Z*B.X) 
				+ anXYZColor.Z*(R.Y*B.X - R.X*B.Y)) / det;
	if (rgb->green < 0.0) rgb->green = 0.0;
	if (rgb->green > 1.0) rgb->green = 1.0;

	// Blue component
	rgb->blue 	= (anXYZColor.X*(R.Y*G.Z - R.Z*G.Y) + anXYZColor.Y*(R.Z*G.X - R.X*G.Z) 
				+ anXYZColor.Z*(R.X*G.Y - R.Y*G.X)) / det;
	if (rgb->blue < 0.0) rgb->blue = 0.0;
	if (rgb->blue > 1.0) rgb->blue = 1.0;

	rgb->red 	= pow(rgb->red, 1.0/gamma.red);
	rgb->green 	= pow(rgb->green, 1.0/gamma.green);
	rgb->blue 	= pow(rgb->blue, 1.0/gamma.blue);
}

