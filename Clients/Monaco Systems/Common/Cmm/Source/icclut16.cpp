#include "icclut16.h"
#include "NewInterpolate.h"
#include "iccindex.h"

McoStatus Icclut16::combine(CMLut16Type *luttag, unsigned char *data)
{
	long pixelsize, datasize, i;
	unsigned short *in[4], *lut, *out[4];
	unsigned short *index;
	long	indexsize, packsize;
			
	datasize = 1; 
	for(i = 0; i < luttag->inputChannels; i++)
		datasize *= luttag->gridPoints;
	pixelsize = datasize;				
	datasize *= luttag->outputChannels*2;

	for(i = 0; i < luttag->inputChannels; i++)
		in[i] = (unsigned short*)(data + luttag->inputTableEntries*2*i);
		
	for(i = 0; i < luttag->outputChannels; i++) 	
		out[i] = (unsigned short*)(data + luttag->inputChannels*luttag->inputTableEntries*2 + datasize +
									luttag->outputTableEntries*2*i);
	
	lut = (unsigned short*)(data + luttag->inputChannels*luttag->inputTableEntries*2);
	
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
	
	index = new unsigned short[indexsize];	
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
	for( i = 0; i < luttag->inputChannels; i++)
		Linterp1 linterp1(in[i], 1, luttag->inputTableEntries, index+i, packsize, 
							pixelsize, index+i, packsize);

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
	for( i = 0; i < luttag->outputChannels; i++)
		Linterp1 linterp1(out[i], 1, luttag->outputTableEntries, index+i, luttag->outputChannels, 
							pixelsize, index+i, luttag->outputChannels);

/*
//begin of a test		

#include "fortestonly.h"
#include "clmem.h"

	McoHandle testH;
	testH = McoNewHandle(64*8*3);
	if(!testH)	return MCO_MEM_ALLOC_ERROR;	

	unsigned char *test;
	
	test = (unsigned char*)McoLockHandle(testH);
	
	for(i = 0; i < 512*3; i++){
		test[i] = (unsigned char)(index[i] >> 8);
	}
	
	for(i = 0; i < 512; i++){
		j = i*3;
		test[j+1] += 128;
		test[j+2] += 128;
	}
		
	saveLABTiff(testH, 64, 8);
//end of a test
*/
				
	//copy to the data
	memcpy((char*)data, (char*)index, datasize);
	delete []index;		

	return MCO_SUCCESS;		
}

