#include "icclut8.h"
#include "NewInterpolate.h"
#include "iccindex.h"

McoStatus Icclut8::combine(CMLut16Type *luttag, unsigned char *data)
{
	long pixelsize, datasize, i, j;
	unsigned char *in[4], *lut, *out[4];
	unsigned char *start, *index;
	long	indexsize, packsize;
		
	datasize = 1; 
	for(i = 0; i < luttag->inputChannels; i++)
		datasize *= luttag->gridPoints;			
	pixelsize = datasize;				
	datasize *= luttag->outputChannels;

	for(i = 0; i < luttag->inputChannels; i++)
		in[i] = (unsigned char*)(data + 256*i);
		
	for(i = 0; i < luttag->outputChannels; i++)	
		out[i] = (unsigned char*)(data + luttag->inputChannels*256 + datasize + 256*i);
	
	lut = (unsigned char*)(data + luttag->inputChannels*256);
	
	indexsize = 1;
	for(i = 0; i < luttag->inputChannels; i++)
	 	indexsize *= luttag->gridPoints;
	if(	luttag->inputChannels >= luttag->outputChannels){
		packsize = luttag->inputChannels;
		indexsize *= luttag->inputChannels;
	}	
	else{
		packsize = luttag->outputChannels;	
		indexsize *= luttag->outputChannels;
	}
	
	index = new unsigned char[indexsize];	
	if(!index)	return MCO_MEM_ALLOC_ERROR;	
		
	switch(luttag->inputChannels){
		case(3):
			switch(luttag->outputChannels){
				case(3):
				{Iccindex index3(RGB_INDEX, index, luttag->gridPoints, 3);}
				break;
				
				case(4):
				{Iccindex index3(RGB_INDEX, index, luttag->gridPoints, 4);}
				break;
			}	
			break;
			
		case(4):
			switch(luttag->outputChannels){
				case(3):
				{Iccindex index4(CMYK_INDEX, index, luttag->gridPoints, 4);}
				break;
				
				case(4):
				{Iccindex index4(CMYK_INDEX, index, luttag->gridPoints, 4);}
				break;
			}	
			break;
				
		default:
			delete []index;
			return MCO_FAILURE;
	}
								
	//input 
	start = index;
	for( i = 0; i < pixelsize; i++){
		for(j = 0; j < luttag->inputChannels; j++)
			start[j] = in[j][start[j]];
		start += packsize;
	}
	
	//lut
	switch(luttag->inputChannels){
		case(3):
			{Linterp3 inter3(lut, luttag->outputChannels, luttag->gridPoints, index, 
				packsize, pixelsize, index);}
		break;
		
		case(4):
			{Linterp4 linter4(lut, luttag->outputChannels, luttag->gridPoints, index, 
				packsize, pixelsize, index);}
		break;	
	
		default:
			delete []index;
			return MCO_FAILURE;
	}	

	//output
	for(i = 0; i < pixelsize; i++){
		start = index + i*luttag->outputChannels;
		for(j = 0; j < luttag->outputChannels; j++)
			start[j] = out[j][start[j]];
	}

	//copy to the data
	memcpy((char*)data, (char*)index, datasize);
	delete []index;		

	return MCO_SUCCESS;
}
