////////////////////////////////////////////////////////////////////////////////////////////////
//	Hide encryped data into a table															  //
//  codetable.c																				  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////

//#include "eve_stuff_2&3.h"
//#include "basewin.h"
#include "assort.h"
#include "codetable.h"
#include "mcotypes.h"

#define ENCRYPT_STR "\p© MonacoSys"

// get the bit from the data that is specified by bitnum
int getBit(unsigned char *data,int bitnum)
{
int i,j;

i = bitnum % 8;
i = 1 << i;
j = bitnum / 8;

return (data[j] & i) >> (bitnum % 8);
}


// set the random number generator seed by looking at an array of numbers
int getSeed(int *a,int num)
{
int i,seed = 0;

for (i=0; i<num; i++)
	{
	seed = seed ^ a[i];
	}
seed = -seed;
return seed;
}

// set the bit accordong to the message and the random number
void setBit(unsigned char *data,unsigned char *mess,int *count,long *seed)
{
int bit;

bit = rand1(0,100000,seed);
bit = bit >> 4;
bit = bit & 1;
bit = bit ^ getBit(mess,*count);
*data = (*data & 0xfe) | bit;
*count = *count +1;
}

// set the bit accordong to the message and the random number
void setBit(unsigned short *data,unsigned char *mess,int *count,long *seed)
{
int bit;

bit = rand1(0,100000,seed);
bit = bit >> 4;
bit = bit & 1;
bit = bit ^ getBit(mess,*count);
*data = (*data & 0xfe) | bit;
*count = *count +1;
}

// place the code into the table
void codeTable(unsigned char *table)
{
int i;
int seedData[99];
long	seed = 0;
int index;
int count = 0;
Str255 estr = ENCRYPT_STR;

//if (isPressed(0x3a)) return;

// get the seed data

index = (16+16*33)*4;
for (i=0; i<33; i++)
	{
	seedData[i*3] = table[index+i*33*33*4];		// cyan
	seedData[i*3+1] = table[index+i*33*33*4+1];	// magenta
	seedData[i*3+2] = table[index+i*33*33*4+2];	// yellow
	}
	
seed = getSeed(seedData,99);

// write twelve bytes of data

// a = -128, b = -128
index = (0+0*33)*4;
for (i=0; i<16; i++)
	{
	setBit(&table[index+i*33*33*4],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+1],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+2],estr,&count,&seed);
	}
	
// a = 128, b = 128	
index = (32+32*33)*4;
for (i=0; i<16; i++)
	{
	setBit(&table[index+i*33*33*4],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+1],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+2],estr,&count,&seed);
	}
}

// place the code into the table
void codeTable(unsigned short *table)
{
int i;
int seedData[99];
long	seed = 0;
int index;
int count = 0;
Str255 estr = ENCRYPT_STR;

//if (isPressed(0x3a)) return;

// get the seed data

index = (16+16*33)*4;
for (i=0; i<33; i++)
	{
	seedData[i*3] = table[index+i*33*33*4];		// cyan
	seedData[i*3+1] = table[index+i*33*33*4+1];	// magenta
	seedData[i*3+2] = table[index+i*33*33*4+2];	// yellow
	}
	
seed = getSeed(seedData,99);

// write twelve bytes of data

// a = -128, b = -128
index = (0+0*33)*4;
for (i=0; i<16; i++)
	{
	setBit(&table[index+i*33*33*4],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+1],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+2],estr,&count,&seed);
	}
	
// a = 128, b = 128	
index = (32+32*33)*4;
for (i=0; i<16; i++)
	{
	setBit(&table[index+i*33*33*4],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+1],estr,&count,&seed);
	setBit(&table[index+i*33*33*4+2],estr,&count,&seed);
	}
}
