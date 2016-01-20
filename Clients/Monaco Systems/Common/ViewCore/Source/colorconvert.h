#ifndef COLOR_CONVERT_H
#define COLOR_CONVERT_H

#include "define.h"

// Convert a Yxy color to an XYZ color.
void convertYxyToXYZ(YxyColorType aYxyColor, XYZColorType *anXYZColor);

// Convert an  XYZ color to a Yxy color.
void convertXYZToYxy(XYZColorType anXYZColor, YxyColorType *aYxyColor);

// convert normalized (by white point) XYZ color intensity to L*a*b* color intensity
void convertXYZToLab(XYZColorType anXYZColor, LabColorType *aLabColor);

// convert L*a*b* color intensity to normalized (by white point) XYZ color intensity. 
void convertLabToXYZ(LabColorType aLabColor, XYZColorType *anXYZColor);

// Convert RGB to XYZ color intensities.
void convertRGBToXYZ(RGBNumber rgb, XYZColorType R, XYZColorType G, XYZColorType B, 
		RGBNumber gamma, XYZColorType *resultXYZColor);

// Convert XYZ color intensities to normalized RGB values.
void convertXYZToRGB(XYZColorType anXYZColor, XYZColorType R, XYZColorType G, 
		XYZColorType B, RGBNumber gamma, RGBNumber *rgb);


#endif //end of COLOR_CONVERT_H