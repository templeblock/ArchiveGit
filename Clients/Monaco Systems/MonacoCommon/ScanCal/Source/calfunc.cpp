#include "calfunc.h"

void func2(double *in, double *out)
{
	out[0] = in[0];
	out[1] = 1.0;
}
	
void func3(double *in, double *out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void func4(double *in, double *out)
{
	func3(in, out);
	out[3] = 1.0;
}

void func9(double *in, double *out)
{
	double a, b, c;
	
	out[0] = a = in[0];
	out[1] = b = in[1];
	out[2] = c = in[2];
	
	out[3] = a*a;
	out[4] = b*b;
	out[5] = c*c;
	
	out[6] = a*b;
	out[7] = a*c;
	out[8] = b*c;
	
	return;
}	

void func10(double *in, double *out)
{
	func9(in, out);
	out[9] = 1.0;
	return;
}	


void func19(double *in, double *out)
{
	double a, b, c;
	double aa, bb, cc;
	double ab, ac, bc;
	
	out[0] = a = in[0];
	out[1] = b = in[1];
	out[2] = c = in[2];
	
	out[3] = aa = a*a;
	out[4] = bb = b*b;
	out[5] = cc = c*c;
	
	out[6] = ab = a*b;
	out[7] = ac = a*c;
	out[8] = bc = b*c;
	
	out[9] = aa*a;
	out[10] = aa*b;
	out[11] = aa*c;
	out[12] = ab*b;
	out[13] = ab*c;
	out[14] = ac*c;
	out[15] = bb*b;
	out[16] = bb*c;
	out[17] = bc*c;
	out[18] = cc*c;
}

void func20(double *in, double *out)
{
	func19(in, out);
	out[19] = 1.0;
	return;
}	
	
				
			