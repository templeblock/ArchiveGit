#pragma once

#define AVG(a, b) (((a)+(b))>>1)
inline BYTE TOLUM(BYTE r, BYTE g, BYTE b)
{
	return 
	(r < g ? (r > b ? AVG(b, g) : (g > b ? AVG(g, r) : AVG(b, r))) :
			 (g > b ? AVG(b, r) : (r > b ? AVG(g, r) : AVG(b, g))));
}

#define TOPERCENT(n)	(((100L * (n)) + 127) / 255)
#define TOGRAY(n)		(((255L * (n)) +  50) / 100)
#define TOANGLE(n)		(((359L * (n)) + 125) / 251)
#define FROMANGLE(n)	(((251L * (n)) + 179) / 359)

#define RGBMAX 255
#define HSLMAX 255
#define HMAX 252 // must be divisible by 6
#define SMAX 255
#define LMAX 255
#define DEGREE60  ((1*HMAX)/6)
#define DEGREE120 ((2*HMAX)/6)
#define DEGREE180 ((3*HMAX)/6)
#define DEGREE240 ((4*HMAX)/6)
#define DEGREE300 ((5*HMAX)/6)
#define DEGREE360 ((6*HMAX)/6)
#define UNDEFINED 0

typedef struct
{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;

typedef struct
{
	BYTE hue;
	BYTE sat;
	BYTE lum;
} HSL;

BYTE RGBtoL(RGBS* pRGB);
void RGBtoHSL(BYTE R, BYTE G, BYTE B, HSL* pHSL);
void HSLtoRGB(BYTE hue, BYTE sat, BYTE lum, RGBS* pRGB);
