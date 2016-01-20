#include "iccindex.h"

//generate rgb/cmyk index
Iccindex::Iccindex(unsigned char* table, long size, long dimen)
{
	long i, j, k, l;
	unsigned char r[256];
	double gap;	
	unsigned char *temp;
	
	gap = 255.0/(size - 1);
	long c = 0;
	for(i = size-1; i >= 0; i--){
		r[c] = (unsigned char)(i*gap + 0.5);
		c++;
	}	
	
	temp = table;
	switch(dimen){
		case(3): //RGB case
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
				}
		break;
		
		case(4): //CMYK case		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
					for(l = 0; l < size; l++)
					{
						*temp++ = r[size - 1 - i];
						*temp++ = r[size - 1 - j];
						*temp++ = r[size - 1 - k];
						*temp++ = r[size - 1 - l];
					}
		break;
		
	}	
	
}

Iccindex::Iccindex(double* table, long size, long dimen)
{
	long i, j, k, l;
	double r[256];
	double gap;	
	double *temp;
	
	gap = 255.0/(size - 1);
	long c = 0;
	for(i = size-1; i >= 0; i--){
		r[c] = i*gap;
		c++;
	}	
	
	temp = table;
	switch(dimen){
		case(3):
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
				}
		break;
		
		case(4):		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
					for(l = 0; l < size; l++)
					{
						*temp++ = r[size - 1 - i];
						*temp++ = r[size - 1 - j];
						*temp++ = r[size - 1 - k];
						*temp++ = r[size - 1 - l];
					}
		break;
		
	}	
	
}

//unsigned char version
Iccindex::Iccindex(long type, unsigned char* table, long size, long packsize)
{
	long i, j, k, l, m;
	unsigned char r[256], A[256];
	double gap;	
	unsigned char *temp;
	
	gap = 255.0/(size - 1);
	for(i = 0; i < size; i++){
		r[i] = (unsigned char)(i*gap + 0.5);
	}
	
	temp = table;
	switch(type){
		case(GRAYSCALE_INDEX): //GRAYSCALE
		for(i = 0; i < size; i++){
			*temp++ = r[i];
			for(m = 1; m < packsize; m++)
			temp++;
		}
		break;		
		
		case(RGB_INDEX): //RGB case
		case(CMY_INDEX):
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;

		case(LAB_INDEX): //LAB case
		for(i = 0; i < size; i++)
			A[i] = (unsigned char)(i*gap -128.0 + 0.5);
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = A[j];
					*temp++ = A[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(XYZ_INDEX): //XYZ case
		//this is unsupported
		for(i = 0; i < size; i++)
			A[i] = (unsigned char)(i*gap -128.0 + 0.5);
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(CMYK_INDEX): //CMYK case		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
					for(l = 0; l < size; l++)
					{
						*temp++ = r[i];
						*temp++ = r[j];
						*temp++ = r[k];
						*temp++ = r[l];
						for(m = 4; m < packsize; m++)
							temp++;
					}
		break;
		
	}	
	
}

//unsigned char version
Iccindex::Iccindex(long type, unsigned short* table, long size, long packsize)
{
	long i, j, k, l, m;
	unsigned short r[256], A[256];
	double gap;	
	unsigned short *temp;
	
	gap = 65535.0/(size - 1);
	for(i = 0; i < size; i++){
		r[i] = (unsigned short)(i*gap + 0.5);
	}
	
	temp = table;
	switch(type){
		case(GRAYSCALE_INDEX): //GRAYSCALE
		for(i = 0; i < size; i++){
			*temp++ = r[i];
			for(m = 1; m < packsize; m++)
			temp++;
		}
		break;		
		
		case(RGB_INDEX): //RGB case
		case(CMY_INDEX):
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;

		case(LAB_INDEX): //LAB case
		for(i = 0; i < size; i++)
			A[i] = (unsigned short)(i*gap - 32768.0 + 0.5);
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = A[j];
					*temp++ = A[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(XYZ_INDEX): //XYZ case
		//this is unsupported
		for(i = 0; i < size; i++)
			A[i] = (unsigned short)(i*gap - 32768.0 + 0.5);
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(CMYK_INDEX): //CMYK case		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
					for(l = 0; l < size; l++)
					{
						*temp++ = r[i];
						*temp++ = r[j];
						*temp++ = r[k];
						*temp++ = r[l];
						for(m = 4; m < packsize; m++)
							temp++;
					}
		break;
		
	}	
	
}

Iccindex::Iccindex(long type, double* table, long size, long packsize)
{
	long i, j, k, l, m;
	double r[256], A[256];
	double gap;	
	double *temp;
	
	gap = 255.0/(size - 1);
	for(i = 0; i < size; i++){
		r[i] = i*gap;
	}

	temp = table;
	switch(type){
		case(GRAYSCALE_INDEX): //GRAYSCALE
		for(i = 0; i < size; i++){
			*temp++ = r[i];
			for(m = 1; m < packsize; m++)
			temp++;
		}
		break;		
		
		case(RGB_INDEX): //RGB case
		case(CMY_INDEX):
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;

		case(LAB_INDEX): //LAB case
		gap = 100.0/(size - 1);
		for(i = 0; i < size; i++){
			r[i] = i*gap;
		}
		gap = 255.0/(size - 1);
		for(i = 0; i < size; i++)
			A[i] = i*gap - 128.0;
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = A[j];
					*temp++ = A[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(XYZ_INDEX): //XYZ case
		//this is unsupported
		for(i = 0; i < size; i++)
			A[i] = (unsigned short)(i*gap - 32768.0 + 0.5);
		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
				{
					*temp++ = r[i];
					*temp++ = r[j];
					*temp++ = r[k];
					for(m = 3; m < packsize; m++)
						temp++;
				}
		break;
		
		case(CMYK_INDEX): //CMYK case		
		for(i = 0; i < size; i++)
			for(j = 0; j < size; j++)
				for(k = 0; k < size; k++)
					for(l = 0; l < size; l++)
					{
						*temp++ = r[i];
						*temp++ = r[j];
						*temp++ = r[k];
						*temp++ = r[l];
						for(m = 4; m < packsize; m++)
							temp++;
					}
		break;
		
	}	
	
}
