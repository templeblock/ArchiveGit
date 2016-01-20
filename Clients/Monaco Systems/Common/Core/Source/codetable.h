////////////////////////////////////////////////////////////////////////////////////////////////
//	Hide encryped data into a table															  //
//  codetable.h																				  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////

// get the bit from the data that is specified by bitnum
int getBit(unsigned char *data,int bitnum);

// set the random number generator seed by looking at an array of numbers
int getSeed(int *a,int num);

// set the bit accordong to the message and the random number
void setBit(unsigned char *data,unsigned char *mess,int *count,int *seed);
void setBit(unsigned short *data,unsigned char *mess,int *count,int *seed);

// place the code into the table
void codeTable(unsigned char *table);
void codeTable(unsigned short *table);

