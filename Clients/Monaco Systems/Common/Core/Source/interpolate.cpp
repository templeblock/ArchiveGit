#include "interpolate.h"
#include "spline.h"
#include "splint.h"

#if defined(_WIN32)
	#pragma warning (disable:4101)
#endif

//interpolate 3D space, every entry holds 3 data
void linearinterp3Dreg(double *src, long srcs, double *des, long dess)
{
	long i, j, k, n;
	double *tempd, *temps;
	register double w1, w2, w3, w4, w5, w6, w7, w8;
	register long p1, p2, p3, p4, p5, p6, p7, p8;
	register double sgap[128], dgap[128];
	register double w[128];
	register long index[128];
	register double rhat, ghat, bhat;	
	register double wi, wj, wk;
	register long ri, gi, bi;
	
	temps = src;
	tempd = des;

	if(srcs == dess){	//if the same, don't need interpolate
		for(i = 0; i < dess; i++){
			for(j = 0; j < dess; j++){
				for( k = 0; k < dess; k++){
					*tempd++ = *src++;
					*tempd++ = *src++;
					*tempd++ = *src++;
				}
			}
		}
	}			
	else {
		for(i = 0; i < srcs; i++)
			sgap[i] = (double)i/(double)(srcs - 1);
		for(i = 0; i < dess; i++)
			dgap[i] = (double)i/(double)(dess - 1);

//compute index			
		for(i = 0; i < dess; i++){
			for(n = srcs - 1; n >= 0; n--){
				if(dgap[i] >= sgap[n]){
					index[i] = n;
					break;
				}
			}
		}
		
//compute weight			
		for(i = 0; i < dess; i++){
			for(n = srcs - 1; n >= 0; n--){
				if(dgap[i] >= sgap[n]){
					w[i] = (dgap[i] - sgap[n])*(double)(srcs-1);
					break;
				}
			}
		}

		for(i = 0; i < dess; i++){
			for(j = 0; j < dess; j++){
				for( k = 0; k < dess; k++){
	
	//weight
					wi = w[i];
					wj = w[j];	
					wk = w[k];
					
					w1 = (1-wi)*(1-wj)*(1-wk);
					w2 = (1-wi)*wj*(1-wk);
					w3 = (1-wi)*wj*wk;
					w4 = (1-wi)*(1-wj)*wk;
					
					w5 = wi*(1-wj)*(1-wk);
					w6 = wi*wj*(1-wk);
					w7 = wi*wj*wk;
					w8 = wi*(1-wj)*wk;
	
	//index				
					ri = index[i];
					gi = index[j];
					bi = index[k];
					
					ri *= srcs*srcs;
					gi *= srcs;
								
					//red channel
					p1 = (ri+gi+bi)*3;
					p2 = p1 + srcs*3;
					p3 = p2 + 3;
					p4 = p1 + 3;
					
					p5 = p1 + srcs*srcs*3;
					p6 = p5 + srcs*3;
					p7 = p6 + 3;
					p8 = p5 + 3;
					
					
					rhat = (src[p1++]*w1  + src[p2++]*w2 +	
							src[p3++]*w3  + src[p4++]*w4 + 
							src[p5++]*w5  + src[p6++]*w6 + 	
							src[p7++]*w7  + src[p8++]*w8); 
					
					ghat = (src[p1++]*w1  + src[p2++]*w2 +
							src[p3++]*w3  + src[p4++]*w4 + 
							src[p5++]*w5  + src[p6++]*w6 + 
							src[p7++]*w7  + src[p8++]*w8); 
									
					bhat = (src[p1++]*w1  + src[p2++]*w2 +
							src[p3++]*w3  + src[p4++]*w4 + 
							src[p5++]*w5  + src[p6++]*w6 + 
							src[p7++]*w7  + src[p8++]*w8); 
		
					//move to blue channel
			
					*des++ = rhat;
					*des++ = ghat;
					*des++ = bhat;
				}
			}
		}	
	}	
}				


void linearinterp3D(double *src, double *sgap, long srcs, double *des, double *dgap, long dess)
{
	long i, j, k, n;
	double *tempd, *temps;
	register double w1, w2, w3, w4, w5, w6, w7, w8;
	register long p1, p2, p3, p4, p5, p6, p7, p8;
	register double w[128];
	register long index[128];
	register double rhat, ghat, bhat;	
	register double wi, wj, wk;
	register long ri, gi, bi;
	
	temps = src;
	tempd = des;

//compute index			
	for(i = 0; i < dess; i++){
		for(n = srcs - 1; n >= 0; n--){
			if(dgap[i] >= sgap[n]){
				index[i] = n;
				break;
			}
		}
	}
	
//compute weight			
	for(i = 0; i < dess; i++){
		for(n = srcs - 1; n >= 0; n--){
			if(dgap[i] >= sgap[n]){
				if(dgap[i] == sgap[n])
					w[i] = 0;
				else	
					w[i] = (dgap[i] - sgap[n])/(sgap[n+1]-sgap[n]);
				break;
			}
		}
	}

//fix out of bound problem
	index[dess-1] = srcs - 2;
	w[dess-1] = 1.0;


	for(i = 0; i < dess; i++){
		for(j = 0; j < dess; j++){
			for( k = 0; k < dess; k++){

//weight
				wi = w[i];
				wj = w[j];	
				wk = w[k];
				
				w1 = (1-wi)*(1-wj)*(1-wk);
				w2 = (1-wi)*wj*(1-wk);
				w3 = (1-wi)*wj*wk;
				w4 = (1-wi)*(1-wj)*wk;
				
				w5 = wi*(1-wj)*(1-wk);
				w6 = wi*wj*(1-wk);
				w7 = wi*wj*wk;
				w8 = wi*(1-wj)*wk;

//index				
				ri = index[i];
				gi = index[j];
				bi = index[k];
				
				ri *= srcs*srcs;
				gi *= srcs;
							
				//red channel
				p1 = (ri+gi+bi)*3;
				p2 = p1 + srcs*3;
				p3 = p2 + 3;
				p4 = p1 + 3;
				
				p5 = p1 + srcs*srcs*3;
				p6 = p5 + srcs*3;
				p7 = p6 + 3;
				p8 = p5 + 3;
				
				
				rhat = (src[p1++]*w1  + src[p2++]*w2 +	
						src[p3++]*w3  + src[p4++]*w4 + 
						src[p5++]*w5  + src[p6++]*w6 + 	
						src[p7++]*w7  + src[p8++]*w8); 
				
				ghat = (src[p1++]*w1  + src[p2++]*w2 +
						src[p3++]*w3  + src[p4++]*w4 + 
						src[p5++]*w5  + src[p6++]*w6 + 
						src[p7++]*w7  + src[p8++]*w8); 
								
				bhat = (src[p1++]*w1  + src[p2++]*w2 +
						src[p3++]*w3  + src[p4++]*w4 + 
						src[p5++]*w5  + src[p6++]*w6 + 
						src[p7++]*w7  + src[p8++]*w8); 
	
				//move to blue channel
		
				*des++ = rhat;
				*des++ = ghat;
				*des++ = bhat;
			}
		}
	}
	
}				

//linear interpolation in 1D case
//src_points is number of points in one interpolation node
void linearinterp1D(double *src, double *sgap, long srcs, double *des, 
double *dgap, long dess, long src_points)
{
	long i, j, n;
	double *tempd, *temps;
	register double w1, w2;
	register long p1, p2;
	register double w[128];
	register long index[128];
	register double rhat, ghat, bhat;	
	register double wi;
	register long ri;
	
	temps = src;
	tempd = des;

//compute index			
	for(i = 0; i < dess; i++){
		for(n = srcs - 1; n >= 0; n--){
			if(dgap[i] >= sgap[n]){
				index[i] = n;
				break;
			}
		}
	}
	
//compute weight			
	for(i = 0; i < dess; i++){
		for(n = srcs - 1; n >= 0; n--){
			if(dgap[i] >= sgap[n]){
				if(dgap[i] == sgap[n])
					w[i] = 0;
				else	
					w[i] = (dgap[i] - sgap[n])/(sgap[n+1]-sgap[n]);
				break;
			}
		}
	}

//fix out of bound problem
	index[dess-1] = srcs - 2;
	w[dess-1] = 1.0;

	for(i = 0; i < dess; i++){
	
		for(j = 0; j < src_points; j++){

	//weight
			wi = w[i];
			
			w1 = (1-wi);
			w2 = wi;

	//index				
			ri = index[i];
						
			p1 = ri*src_points*3 + j*3;
			p2 = p1 + src_points*3;
			
			
			rhat = (src[p1++]*w1  + src[p2++]*w2); 
			
			ghat = (src[p1++]*w1  + src[p2++]*w2); 
							
			bhat = (src[p1++]*w1  + src[p2++]*w2); 
		
			*des++ = rhat;
			*des++ = ghat;
			*des++ = bhat;
		}
	}
}				


//index is 4 dimension, and data is 3, such as CMYK to LAB
//table: contents of the table
//grid points in every dimension
//src is source data, srcsize is the size of data in pixels
//over means overwrite the original source data
void linearinterp4to3over(unsigned char *table, long gridp, unsigned char *src, long srcsize)
{
	long i, j, n;
	register double w[16];
	register long p[16];
	register double wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	long	datachannel;
	double 	sum;
	unsigned char cmyk[4];
	short	gstep;	//grid step
	unsigned char *des;
	
	
	datachannel = 3;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	srcs3 = srcs2*srcs;	
	
//assume from 0 to 255 which 256
	gstep = 256/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = src;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		ki = (long)k/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		wq = (double)(k - ki*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk)*(1-wq);
		w[1] = (1-wi)*wj*(1-wk)*(1-wq);
		w[2] = (1-wi)*wj*wk*(1-wq);
		w[3] = (1-wi)*(1-wj)*wk*(1-wq);
		
		w[4] = wi*(1-wj)*(1-wk)*(1-wq);
		w[5] = wi*wj*(1-wk)*(1-wq);
		w[6] = wi*wj*wk*(1-wq);
		w[7] = wi*(1-wj)*wk*(1-wq);
	
		w[8] = (1-wi)*(1-wj)*(1-wk)*wq;
		w[9] = (1-wi)*wj*(1-wk)*wq;
		w[10] = (1-wi)*wj*wk*wq;
		w[11] = (1-wi)*(1-wj)*wk*wq;
		
		w[12] = wi*(1-wj)*(1-wk)*wq;
		w[13] = wi*wj*(1-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(1-wj)*wk*wq;
	
	
		ri *= srcs3;
		gi *= srcs2;
		bi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + 1;
		p[9] = p[1] + 1;
		p[10] = p[2] + 1;
		p[11] = p[3] + 1;
	
		p[12] = p[4] + 1;
		p[13] = p[5] + 1;
		p[14] = p[6] + 1;
		p[15] = p[7] + 1;
		
		for( j = 0; j < 16; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = (unsigned char)sum;
		}				
	
		for(j = 0; j < datachannel; j++)
			*des++ = cmyk[j];
	}	
}


//worked
//table is a index 3 to data 4 table, such as lab to cmyk table
//the src and dest have the same number of index, but different size
//src has 3 dimension, dest has 4
void linearinterp3to4(unsigned char *table, long gridp, unsigned char *src, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	long	datachannel;
	double 	sum;
	unsigned char cmyk[4];
	short	gstep;	//grid step
	unsigned char *des;
	
	
	datachannel = 4;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 256/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = (unsigned char)sum;
		}				
	
		for(j = 0; j < datachannel; j++)
			*des++ = cmyk[j];
	}	
}


//worked
//table is a index 3 to data 4 table, such as lab to cmyk table
//the src and dest have the same number of index, but different size
//src has 3 dimension, dest has 4
// the sort version of the above function
void linearinterp3to4(unsigned short *table, long gridp, unsigned short *src, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	long	datachannel;
	double 	sum;
	unsigned short cmyk[4];
	short	gstep;	//grid step
	unsigned short *des;
	
	
	datachannel = 4;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 65536/(gridp - 1);		
		
	unsigned short* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = (unsigned short)sum;
		}				
	
		for(j = 0; j < datachannel; j++)
			*des++ = cmyk[j];
	}	
}



//worked
//table is a index 3 to data 3 table, such as rgb to lab table
//the src and dest have the same number of index, but different size
//src has 3 dimension, dest has 3
//src are unsigned char data, while dest are double data
//this is used for scanner Icc from rgb to lab conversion
//table is unsigned char data
void linearinterp3to3c(unsigned char *table, long gridp, unsigned char *src, long srcsize, double *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	long	datachannel;
	double 	sum;
	double	cmyk[4];
	short	gstep;	//grid step
	double *des;
	
	datachannel = 3;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 256/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = sum;
		}				
	
		*des++ = cmyk[0]/256.0;
		*des++ = cmyk[1]/256.0 - 128.0;
		*des++ = cmyk[2]/256.0 - 128.0;
	}			
		
}


//worked
//table is a index 3 to data 3 table, such as rgb to lab table
//the src and dest have the same number of index, but different size
//src has 3 dimension, dest has 3
//src are unsigned char data, while dest are double data
//this is used for scanner Icc from rgb to lab conversion
//table is unsigned short data
void linearinterp3to3s(unsigned short *table, long gridp, unsigned char *src, long srcsize, double *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	long	datachannel;
	double 	sum;
	double  cmyk[4];
	short	gstep;	//grid step
	double *des;
	
	
	datachannel = 3;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 256/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = sum;
		}				
	
		*des++ = cmyk[0]/655.35;
		*des++ = cmyk[1]/256.0 - 128.0;
		*des++ = cmyk[2]/256.0 - 128.0;
	}			
		
}


//this is not working yet
//table is a index 3 to data 4 table, such as lab to cmyk table
//the src and dest have the same number of index, but different size
//src has 3 dimension, dest has 4
void linearinterp3to4d(double *table, long gridp, unsigned char *src, long srcsize, double *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	long	datachannel;
	double 	sum;
	double	cmyk[4];
	short	gstep;	//grid step
	double *des;
	
	
	datachannel = 4;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 256/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)c/gstep;
		gi = (long)m/gstep;
		bi = (long)y/gstep;
		
	//compute weight			
		wi = (double)(c - ri*gstep)/(double)gstep;
		wj = (double)(m - gi*gstep)/(double)gstep;	
		wk = (double)(y - bi*gstep)/(double)gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = sum;
		}				
	
		for(j = 0; j < datachannel; j++)
			*des++ = cmyk[j];
	}	
}


//procedure for 2D spline interpolation
//assume regular grid points
//precompute 2nd derivative parameter along the 2nd axis(y axis)
//xa should contain the grid points in 2nd axis
void spline2d(double *xa1, double *xa2, double *ya, long m, long n, double *y2a)
{
	long i;
	
	for(i = 0; i < m; i++)
		spline(xa2-1, ya+i*n-1, n, 1.0e30, 1.0e30, y2a+i*n-1);
}


//assume regular grid points
//ask contains the reqest data in 2D, the format is interleave
//xa should contain all the grid points
void splint2d(double *xa1, double *xa2, double *ya, long m, long n, double *y2a, double *ask, double *ans)
{
	long i, j, k;
	double *tempz, *temp2x, *axis1;
	
	tempz = new double[m];
	temp2x = new double[m];

//interpolation along y axis
	for(i = 0; i < m; i++)
		splint(xa2-1, ya+i*n-1, y2a+i*n-1, n, ask[1], tempz+i);

//compute the 2nd derivative of the 1st axis(x)
	spline(xa1-1, tempz-1, m, 1.0e30, 1.0e30, temp2x-1);

//interpolation along x axis	
	splint(xa1-1, tempz-1, temp2x-1, m, ask[0], ans);
	
	delete []tempz;
	delete []temp2x;
}
	

//procedure for 3D spline interpolation
//assume regular grid points
//precompute 2nd derivative parameter along the 3rd axis(z axis)
//xa1, xa2, xa3 should contain the grid points in 3rd axis
//m, n, p are the number of grid points in those axis
void spline3d(double *xa1, double *xa2, double *xa3, double *ya, long m, long n, long p, double *y2a)
{
	long i, j;
	
	for(i = 0; i < m; i++)
		for( j = 0; j < n; j++)
			spline(xa3-1, ya+(i*n+j)*p-1, p, 1.0e30, 1.0e30, y2a+(i*n+j)*p-1);
}

//assume regular grid points
//ask contains the reqest data in 3D, the format is interleave
//xa should contain all the grid points
void splint3d(double *xa1, double *xa2, double *xa3, double *ya, long m, long n, long p, double *y2a, double *ask, double *ans)
{
	long i, j, k;
	double *tempz, *tempy, *tempx, *tempw;
	double *axis2, *axis1;
	
	tempz = new double[m*n];
	tempy = new double[m*n];
	tempx = new double[m];
	tempw = new double[m];
	
//	spline curve along z axis	
	for(i = 0; i < m; i++)
		for( j = 0; j < n; j++)
			splint(xa3-1, ya+(i*n+j)*p-1,y2a+(i*n+j)*p-1,p,ask[2],tempz+i*n+j);
			
//	spline curve along y axis
	for(i = 0; i < m; i++)
		spline(xa2-1, tempz+i*n-1, n, 1.0e30, 1.0e30, tempy+i*n-1);
		
	for(i = 0; i < m; i++)
		splint(xa2-1, tempz+i*n-1, tempy+i*n-1,n,ask[1],tempx+i);

//	spline curve along x axis
	spline(xa1-1, tempx-1, m, 1.0e30, 1.0e30, tempw-1);
	splint(xa1-1, tempx-1, tempw-1, m, ask[0], ans);
	
	delete []tempz;
	delete []tempy;
	delete []tempx;
	delete []tempw;
}

// interpolate 3 to 3 where both are double
// and use table where table is 33x33x33

void interpolate_33(double *lab,double *labc,double *table)
{
double	a,b,c;
long	a1,a2,b1,b2,c1,c2;
long	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;


a = lab[0]*0.32;
b = (lab[1]+128)*(32.0/255.0);
c = (lab[2]+128)*(32.0/255.0);



if (a >= 32) a = 31.999999;
if (a < 0) a = 0;
if (b >= 32) b = 31.999999;
if (b < 0) b = 0;
if (c >= 32) c = 31.999999;
if (c < 0) c = 0;
			
a1 = ((long)a)*3267;
a2 = a1 + 3267;
b1 = ((long)b)*99;
b2 = b1 + 99;
c1 = ((long)c)*3;
c2 = c1 + 3;

t = a - (long)a;
u = b - (long)b;
v = c - (long)c;

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1+c1;
p2 = table +a1+b1+c2;
p3 = table +a1+b2+c1;
p4 = table +a1+b2+c2;
p5 = table +a2+b1+c1;
p6 = table +a2+b1+c2;
p7 = table +a2+b2+c1;
p8 = table +a2+b2+c2;
			
labc[0] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
labc[1] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
labc[2] = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
					 
}