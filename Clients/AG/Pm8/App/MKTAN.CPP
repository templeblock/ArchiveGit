#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862

double const PI = _PI;
double const PI2 = _PI*2.0;
double const DEG2RADC = _PI/180.0;
double const RAD2DEGC = 180.0/_PI;

int main(int argc, char **argv)
{
	double dAngle;
	int nIndex;

// A test!

	dAngle = atan2(32767, 1);
	printf("tan 89.95 is %f\n", tan(89.95*DEG2RADC));

#if 0
	for (nIndex = 0; nIndex < 90*10; nIndex++)
	{
		double dAngle = 0.1*nIndex;
		double dTan = tan(dAngle*DEG2RADC);
		long lTan = long(dTan*65536.0);
		printf("0x%08lx", lTan);

		if (dAngle != 89.9000)
		{
			printf(",");
		}

		printf(((nIndex % 6) == 5) ? "\n" : " ");
	}
#endif
	return 0;
}
