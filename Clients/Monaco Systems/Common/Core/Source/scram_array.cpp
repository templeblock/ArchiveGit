// Scramble and unscamble the order of items in an array
// scram_array.c

#include "assort.h"
#include "scram_array.h"
#include "monacoprint.h"

long int rand1(long int max,int *seed)
{
return((long int)(max*(float)ran1(seed)));
}


void scramble2(int *array,int num)
{

        int     i,swap,t;
        
		int		seed;
		
seed = -SCRAMBLE_SEED;

for(i=0; i<num; i++)
        {
        swap=(int)rand1(num,&seed);
        t=array[i];
        array[i]=array[swap];
        array[swap]=t;
        }
}

void scramble_array(double *array,int size,int elements)
{
int *index;
double	*temp;
int i,j,m,n;

index = new int[size];
temp = new double[size*elements];

for (i=0; i<size; i++) index[i] = i;
scramble2(index,size);

m = 0;
for (i=0; i<size; i++)
	for (j=0; j<elements; j++)
		{
		temp[m] = array[m];
		m++;
		}

for (i=0; i<size; i++)
	{
	m = i*elements;
	n = index[i]*elements;
	for (j=0; j<elements; j++)
		{
		array[n] = temp[m];
		m++;
		n++;
		}
	}
delete index;
delete temp;
}

void unscramble_array(double *array,int size,int elements)
{
int *index;
double	*temp;
int i,j,m,n;

index = new int[size];
temp = new double[size*elements];

for (i=0; i<size; i++) index[i] = i;
scramble2(index,size);

m = 0;
for (i=0; i<size; i++)
	for (j=0; j<elements; j++)
		{
		temp[m] = array[m];
		m++;
		}

for (i=0; i<size; i++)
	{
	m = i*elements;
	n = index[i]*elements;
	for (j=0; j<elements; j++)
		{
		array[m] = temp[n];
		m++;
		n++;
		}
	}
delete index;
delete temp;
}


void scramble_array(unsigned char *array,int size,int elements)
{
int *index;
unsigned char	*temp;
int i,j,m,n;

index = new int[size];
temp = new unsigned char[size*elements];

for (i=0; i<size; i++) index[i] = i;
scramble2(index,size);

m = 0;
for (i=0; i<size; i++)
	for (j=0; j<elements; j++)
		{
		temp[m] = array[m];
		m++;
		}

for (i=0; i<size; i++)
	{
	m = i*elements;
	n = index[i]*elements;
	for (j=0; j<elements; j++)
		{
		array[n] = temp[m];
		m++;
		n++;
		}
	}
delete index;
delete temp;
}

void unscramble_array(unsigned char *array,int size,int elements)
{
int *index;
unsigned char	*temp;
int i,j,m,n;

index = new int[size];
temp = new unsigned char[size*elements];

for (i=0; i<size; i++) index[i] = i;
scramble2(index,size);

m = 0;
for (i=0; i<size; i++)
	for (j=0; j<elements; j++)
		{
		temp[m] = array[m];
		m++;
		}

for (i=0; i<size; i++)
	{
	m = i*elements;
	n = index[i]*elements;
	for (j=0; j<elements; j++)
		{
		array[m] = temp[n];
		m++;
		n++;
		}
	}
delete index;
delete temp;
}