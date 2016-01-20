/* 	
	mathutilities.h

	
	
		Math macros copied from EscherMath.h .

	Change Log:															 

	09/06/94	rdd Created after the macros were removed for A1c1 API changes.
*/

/******************************************************************************
 **																			 **
 **							Miscellaneous Functions							 **
 **																			 **
 *****************************************************************************/
 
#define uMath_DegreesToRadians(x)	((x) *  kQ3Pi / 180.0)
#define uMath_RadiansToDegrees(x)	((x) * 180.0 / kQ3Pi)

#define uMath_Ceil(x)			((float)ceil((double)(x)))
#define uMath_Floor(x)			((float)floor((double)(x)))

#define uMath_Min(x,y)			((x) <= (y) ? (x) : (y))
#define uMath_Max(x,y)			((x) >= (y) ? (x) : (y))

#define uMath_Fabs(x)			((float)fabs((double)(x)))
#define uMath_Abs(x)			((long)fabs((double)(x)))

#define uMath_Pow(x,y)			((float)pow((double)(x),(double)(y)))

#define uMath_Sqrt(x)			((float)sqrt((double)(x)))

#define uMath_Fmod(x,y)			((float)fmod((double)(x),(double)(y)))

#define uMath_Frexp(x,nPtr)		((float)frexp((double)(x),(int *)(nPtr)))

#define uMath_Ldexp(x,n)		((float)ldexp((double)(x),(int)(n)))

#define uMath_Log10(x)			((float)log10((double)(x)))	

#define uMath_Modf(x,nPtr)		((float)modf((double)(x),(double *)(nPtr)))

#define uMath_Exp(x)			((float)exp((double)(x)))

#define uMath_Log(x)			((float)log((double)(x)))
		

/******************************************************************************
 **																			 **
 **							Trigonometric Functions							 **
 **																			 **
 *****************************************************************************/
 
#define uMath_Sin(x)			((float)sin((double)(x)))
#define uMath_Cos(x)			((float)cos((double)(x)))
#define uMath_Tan(x)			((float)tan((double)(x)))

#define uMath_Asin(x)			((float)asin((double)(x)))
#define uMath_Acos(x)			((float)acos((double)(x)))
#define uMath_Atan(x)			((float)atan((double)(x)))
#define uMath_Atan2(x,y)		((float)atan2((double)(x),(double)(y)))

#define uMath_Sinh(x)			((float)sinh((double)(x)))
#define uMath_Cosh(x)			((float)cosh((double)(x)))
#define uMath_Tanh(x)			((float)tanh((double)(x)))

#define uMath_Sin_Deg(x)		((float)sin((double)Q3Math_DegreesToRadians((x))))
#define uMath_Cos_Deg(x)		((float)cos((double)Q3Math_DegreesToRadians((x))))
#define uMath_Tan_Deg(x)		((float)tan((double)Q3Math_DegreesToRadians((x))))

#define uMath_Asin_Deg(x)		((float)Q3Math_RadiansToDegrees(asin((double)(x))))
#define uMath_Acos_Deg(x)		((float)Q3Math_RadiansToDegrees(acos((double)(x))))
#define uMath_Atan_Deg(x)		((float)Q3Math_RadiansToDegrees(atan((double)(x))))
#define uMath_Atan2_Deg(x,y)	((float)Q3Math_RadiansToDegrees(atan2((double)(x),(double)(y))))

#define uMath_Sinh_Deg(x)		((float)sinh((double)Q3Math_DegreesToRadians((x))))
#define uMath_Cosh_Deg(x)		((float)cosh((double)Q3Math_DegreesToRadians((x))))
#define uMath_Tanh_Deg(x)		((float)tanh((double)Q3Math_DegreesToRadians((x))))
