/*  eve_stuff.c	
		The eve dongle protection stuff 
		
	Portions (c) Copyright 1993 Monaco SYstems Inc.
	By James Vogh 
*/
	

//#include "eveitf.h"

#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <Quickdraw.h>
#include <Dialogs.h>
#include <stdio.h>
#include <string.h>

//#include "colorfix.h"
#include "eve_stuff_2&3.h"
#include "eve3demo.h"
#include "eveitf.h"
//#define NO_EVE

//#define FAST_EVE_3

McoDongleType dongletype = (McoDongleType)-1;
short	Gimagecode = -1;
McoUse	GDongleUsage = (McoUse)-1;
// The Handle used by eve
// Must be removed before exit from plug
Handle PktHandle = NULL;








/* scramble  scrambles the contents of an array  
        array   the array whose contents are to be scrambled
        num     the number of elements in the array */

void scramble(char array[],int num,long s1)
{

        int     i,swap;
        char	t;

for(i=0; i<num; i++)
        {
        swap=(int)rand1(0,num,&s1);
        t=array[i];
        array[i]=array[swap];
        array[swap]=t;
        }
}

/* Unscramble an array */

void unscramble(char array[], int num,long s1)
{

	int	i,swaps[100];
	char	t;
	
for (i=0; i<num; i++)	swaps[i] = rand1(0,num,&s1);
for (i=num-1; i>=0; i--)
	{
	t = array[i];
	array[i] = array[swaps[i]];
	array[swaps[i]] = t;
	}
}

/* scramble  scrambles a number
        num     the number to be scrambled
		s1		the seedto the random number generater */

long scramble_num(unsigned short num,long s1)
{

return  num ^ (long)rand1(0,1000000,&s1);

}

/* Unscramble an array */

unsigned short unscramble_num(long num,long s1)
{

return( (unsigned short) num ^ (long)rand1(0,1000000,&s1));	

}


//Send the passwords to the dongle

void set_passwords(int level)
{
	char	s1[20];
	char	s2[21];
	char	s3[22];
	
	strcpy(s1,PASSWD1);
	strcpy(s2,PASSWD2);
	strcpy(s3,PASSWD3);
	
	if (level > 0) unscramble(s1,strlen(s1),SCRAMKEY);
	if (level > 1) unscramble(s2,strlen(s2),SCRAMKEY);
	if (level > 2) unscramble(s3,strlen(s3),SCRAMKEY);

	if (level > 0) EVEEnable(s1);
	if (level > 1) EVEEnable(s2);
	if (level > 2) EVEEnable(s3);
}	


/* Reset an eve dongle */

/* Reset an eve dongle */

char GoodReset( void )
{
	short int rc;
	
	rc = EVEReset();
	
	if (rc == 0) return(TRUE);	 
	else	 return(FALSE);
}


//Wait just 1 second
void just_wait_a_sec(void)
{
int i;
unsigned long	sec1,sec2;

GetDateTime(&sec1);
for (i=0; i<100000; i++)
	{
	GetDateTime(&sec2);
	if (sec1!=sec2) return;
	}
}

// open the eve 3 dongle
McoStatus open_e3(void)
{
	unsigned short		DevID;
	int					i;
	unsigned long 		SN;
	unsigned short int 	result;

PktHandle = RBEHANDLE();
DevID = unscramble_num(E3_DEVID,E3_DEVID_S1);
for (i=0; i<MAX_RETRIES; i++)
	{
	result = RBEFINDFIRST(DevID, &SN, PktHandle);
	if (result == E3_SUCCESS) 
		{
		dongletype = EVE_3;
		return MCO_SUCCESS;
		}
	}
return MCO_BADEVE;
}

// get the next dongle
int get_next_e3(void)
{
unsigned short		DevID;
unsigned long 		SN;

DevID = unscramble_num(E3_DEVID,E3_DEVID_S1);
if  (RBEFINDNEXT(&SN, PktHandle))
	return RBEFINDFIRST(DevID, &SN, PktHandle);
return 0;
}


// close the eve 3 dongle
void close_eve(void)
{
if (PktHandle != NULL) DisposeHandle(PktHandle);
PktHandle = NULL;
}


McoDongleType getdongletype(void)
{
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID;
	int i;

if (dongletype != -1) return dongletype;

#ifdef FAST_EVE_3
return EVE_3;
#endif

if (PktHandle == NULL) 
	{
	PktHandle = RBEHANDLE();
	if (open_e3() == MCO_SUCCESS)
		{
		dongletype = EVE_3;
		return EVE_3;
		}
	}
// if PktHandle is not null and dongletype is not -1 then an error has occured
else return NO_DONGLE;
for (i=0; i<MAX_RETRIES; i++)
	{
	result =  EVEReset();
	result = EVEStatus();
	if (result == 0) 
		{
		dongletype = EVE_2;
		return EVE_2;
		}
	}
return NO_DONGLE;
}

McoStatus McoDealerDial(void);

// Check to see if the dongle is a dealer dongle
// If it is display a large annoying dialog

void CheckDealer(void)
{
unsigned short int 	result;
unsigned short		data1;
//McoDealerDial();
result = RBEREAD(DEALERCODE_E3, &data1, L1_E3, PktHandle);

if (data1 & 1) 
	{
	result = RBEREAD(160+DEALERRAM_E3, &data1, L1_E3, PktHandle);
	if ((data1<=0) || (data1>=7)) 
		{
		McoDealerDial();
		result = RBEWRITE(168+DEALERRAM_E3,1,L1_E3,PktHandle);
		}
	}
}

// Check to see if the dongle is a dealer dongle
// If it is display a large annoying dialog

void IncDealerCnt(void)
{
	McoStatus			state;
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID;
	unsigned short		data1;

state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{
	result = RBEREAD(DEALERCODE_E3, &data1, L1_E3, PktHandle);
	
	if (data1 & 1) 
		{
		result = RBEREAD(160+DEALERRAM_E3, &data1, L1_E3, PktHandle);
		if (data1+DEALERINC_E3	>=7)
			{
			McoDealerDial();
			result = RBEWRITE(168+DEALERRAM_E3,1,L1_E3,PktHandle);
			}
		else result = RBEWRITE(168+DEALERRAM_E3,data1 + DEALERINC_E3,L1_E3,PktHandle);
		}
	}
}

// Switch the dongle types to the "new" type

void SwitchDongle_e3(void)
{
unsigned short int 	result,Passwd;
unsigned short		data1;

result = RBEREAD(IMAGECODE_E3_OLD, &data1, L1_E3, PktHandle);
if (data1 & 3)
	{
	Passwd = unscramble_num(E3_PASSWD,E3_PASSWD_S1);
	result = RBEWRITE(IMAGECODE_E3_OLD+32, 0, Passwd, PktHandle);
	result = RBEWRITE(IMAGECODE_E3+32, data1 | 0x1C, Passwd, PktHandle);
	}
}

// Get the usage level of the dongle. Either Super, unlimited, limited, almost gone, 
//		gone, or bad eve.

McoUse get_usage_level_e3(int *count)
{
	McoStatus			state;
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID;
	unsigned short		data1,data2,data3,data4;
	int	i,j;
	McoUse	use = P_NoEve;

#ifdef NO_EVE
return  P_SuperUse;
#endif

*count = 0;

if (GDongleUsage == -1)
	{
	state = MCO_SUCCESS;
	if (PktHandle == NULL) state = open_e3();
	if (state == MCO_SUCCESS) 
		{
		for (i=0; i<MAX_RETRIES; i++)
			{
			// Convert to the new dongle type
			//SwitchDongle_e3();
			//See if dealer dialog should be show
			CheckDealer();
			result = RBEREAD(IMAGECODE_E3, &data1, L1_E3, PktHandle);
			result = RBEREAD( MONACOCOL_VER_E3,&data2, L1_E3, PktHandle);

		//	if (data1 & PRODUCT_CODE)
		//		{
				result = RBEREAD(LOCK1_E3, &data1, L1_E3, PktHandle);
				result = RBEREAD(LOCK2_E3, &data2, data1, PktHandle);
				if (data1 != L2_E3) data1 = 0;
				if ((data2>=0) && (data2<32)) result = RBEREAD(data2, &data3, data1, PktHandle);
				else data3 = -1;
				if (data1 == 0) data1 = -1;
				if (data3 == data1)
					{
					result = RBEREAD(COUNTREG2_E3, &data2, data1, PktHandle);
					result = RBEREAD(COUNTREG2_E3, &data4, data1, PktHandle);
					if (data2 != data4)
						{
						for (j=0; j<MAX_RETRIES; j++)
							{
							just_wait_a_sec();
							result = RBEREAD(COUNTREG2_E3, &data2, data1, PktHandle);
							just_wait_a_sec();
							result = RBEREAD(COUNTREG2_E3, &data4, data1, PktHandle);
							if (data2 == data4) break;
							}
						}
					*count = data2;
					if (data2 > NORMCOUNT) use = P_BadCount;
					if (data2 == SUPERCOUNT) use = P_SuperUse;
					if (data2 == NORMCOUNT) use = P_UnlimitUse;
					if (data2 <= LIMITCOUNT_MAX) use = P_LimitUse;
					if (data2 <= ALMOSTCOUNT) use = P_AlmostNoUse;
					if (data2 == 0) use = P_NoUse;
					}
				else use = P_BadEve;
		//		}
		//	else 
		//		{
		//		use = P_NoEve;
		//		}
			if (use>P_NoUse) 
				{
				GDongleUsage = use;
				return use;
				}
			just_wait_a_sec();
			}
		}
	else return P_NoEve;
	}
else
	{
	// if PktHandle is null and GDongleUse is not -1 then an error has occured
	if (PktHandle == NULL) return P_NoEve;

	CheckDealer();
	for (i=0; i<MAX_RETRIES; i++)
		{
		result = RBEREAD(COUNTREG2_E3, &data2, data1, PktHandle);
		if (result == E3_SUCCESS) 
			{
			if (data2 > NORMCOUNT) use = P_BadCount;
			if (data2 == SUPERCOUNT) use = P_SuperUse;
			if (data2 == NORMCOUNT) use = P_UnlimitUse;
			if (data2 <= LIMITCOUNT_MAX) use = P_LimitUse;
			if (data2 <= ALMOSTCOUNT) use = P_AlmostNoUse;
			if (data2 == 0) use = P_NoUse;
			GDongleUsage = use;
			*count = data2;
			return use;
			}
		just_wait_a_sec();
		}
	}
return use; 
}

// Switch to the new dongle type

void SwitchDongle_e2(void)
{
short val1,val2,val3;

val1 = EVEChallenge(LOCK1_E2,L1_E2);
val2 = EVEChallenge(LOCK2_E2,val1);
val3 = EVEReadGPR(val2);
if (val3 == val1) 
	{
	set_passwords(3);
	EVESetLock(LOCK1_E2,L1_E2_NEW,L2_E2);
	EVEWriteGPR(IMAGECODE_E2,0x1F);
	return;
	}
GoodReset();
set_passwords(1);	

val1 = EVEChallenge(LOCK1_E2,FS_L1_E2);
val2 = EVEChallenge(LOCK2_E2,val1);
val3 = EVEReadGPR(val2);
// An "FS" dongle
if (val3 == val1)
	{
	set_passwords(3);
	// Set to normal
	EVEWriteGPR(LOCKREG_E2,L2_E2);
	EVESetLock(LOCK1_E2,L1_E2_NEW,L2_E2);
	EVESetLock(LOCK2_E2,L2_E2,LOCKREG_E2);
	// Turn off batch
	EVEWriteGPR(IMAGECODE_E2,0x19);
	}
}

// Get the usage level of the dongle. Either Super, unlimited, limited, almost gone, 
//		gone, or bad eve

McoUse get_usage_level_e2(int *count)
{
	short sresult;
	short val1,val2,val3,val4,data1;
	McoUse	use;
	int i,j;

// Eve 2 is not supported
return P_NoUse;

#ifdef NO_EVE
return  P_SuperUse;
#endif
	
for (i=0; i<MAX_RETRIES; i++)
	{	

	GoodReset();
	sresult = EVEStatus();

	if (sresult == 0)
		{
		set_passwords(1);	
		SwitchDongle_e2();
		data1 = EVEReadGPR(IMAGECODE_E2);
		if (data1 & PRODUCT_CODE)
			{
			val1 = EVEChallenge(LOCK1_E2,L1_E2_NEW);
			val2 = EVEChallenge(LOCK2_E2,val1);
			val3 = EVEReadGPR(val2);
			if (val3 == val1)
				{
				val1 = EVEReadGPR(COUNTREG_E2);
				val4 = EVEReadGPR(COUNTREG_E2);
				if (val1 != val4)
					{
					for (j=0; j<MAX_RETRIES; j++)
						{
						just_wait_a_sec();
						val1 = EVEReadGPR(COUNTREG_E2);
						just_wait_a_sec();
						val4 = EVEReadGPR(COUNTREG_E2);
						if (val1 == val4) break;
						}
					}
				*count = val1;
				if (val1 > NORMCOUNT) use = P_BadCount;
				if (val1 == SUPERCOUNT) use = P_SuperUse;
				if (val1 == NORMCOUNT) use = P_UnlimitUse;
				if (val1 <= LIMITCOUNT_MAX) use = P_LimitUse;
				if (val1 <= ALMOSTCOUNT) use = P_AlmostNoUse;
				if (val1 == 0) use = P_NoUse;
				}
			else use = P_BadEve;
			}
		else use = P_BadEve;
		}
	else 
		{
		use = P_NoEve;
		}
	GoodReset();
	if (use>P_NoUse) return use;
	just_wait_a_sec();
	}
return use;
}

McoUse get_usage_level(int *count)
{
McoDongleType dType;

dType = getdongletype();
switch (dType) {
	case EVE_3: return get_usage_level_e3(count);
	case EVE_2: return get_usage_level_e2(count);
	}
return P_NoEve;
}

// Decrement the register in the dongle which is used as a counter
// For Eve 3


McoStatus decrement_count_e3(void)
{

	//Handle PktHandle;
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID,Passwd;
	int	i,c;
	unsigned short		val1,val2,data1 = 0;
	McoStatus 			state;
		
#ifdef NO_EVE
return MCO_SUCCESS;
#endif

state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{
	DevID = unscramble_num(E3_DEVID,E3_DEVID_S1);
	Passwd = unscramble_num(E3_PASSWD,E3_PASSWD_S1);
	result = RBEREAD(COUNTREG_E3, &val1, data1, PktHandle);
	result = RBEREAD(COUNTREG_E3, &val2, data1, PktHandle);
	c = 0;
	while ((val1 != val2) && (c<MAX_RETRIES))
		{
		just_wait_a_sec();
		result = RBEREAD(COUNTREG_E3, &val1, data1, PktHandle);
		result = RBEREAD(COUNTREG_E3, &val2, data1, PktHandle);
		c++;
		}
	if (val1 != val2) return MCO_FAILURE;
	if ((val1 >0) && (val1 <NORMCOUNT))	
		{
		result = RBEWRITE(COUNTREG_E3+32, (val1-1), Passwd, PktHandle);
		result = RBEREAD(COUNTREG_E3, &val2, data1, PktHandle);
		c = 0;
		while ((val1-1 != val2) && (c<MAX_RETRIES))
			{
			just_wait_a_sec();
			result = RBEWRITE(COUNTREG_E3+32, (unsigned short)(val1-1), Passwd, PktHandle);
			result = RBEREAD(COUNTREG_E3, &val2, data1, PktHandle);
			c++;
			} 	
		if (val1-1 != val2) return MCO_FAILURE;		
		}
	return MCO_SUCCESS;
	}
return state;
}	


// Decrement the register in the dongle which is used as a counter
// For Eve 2

McoStatus decrement_count_e2(void)
{
	short sresult;
	short val1,val2;
	int 	i,c;
	
#ifdef NO_EVE
return MCO_SUCCESS;
#endif

for (i=0; i<MAX_RETRIES; i++)
	{
	GoodReset();
	sresult = EVEStatus();
	
	if (sresult == 0)
		{
		set_passwords(3);
		val1 = EVEReadGPR(COUNTREG_E2);
		val2 = EVEReadGPR(COUNTREG_E2);
		c = 0;
		while ((val1 != val2) && (c<MAX_RETRIES))
			{
			just_wait_a_sec();
			val1 = EVEReadGPR(COUNTREG_E2);
			val2 = EVEReadGPR(COUNTREG_E2);
			c++;
			}
		if (val1 != val2) return MCO_FAILURE;
		if ((val1 >0) && (val1 <NORMCOUNT))	
			{
			EVEWriteGPR(COUNTREG_E2,val1-1);
			val2 = EVEReadGPR(COUNTREG_E2);
			c = 0;
			while ((val1-1 != val2) && (c<MAX_RETRIES))
				{
				just_wait_a_sec();
				EVEWriteGPR(COUNTREG_E2,val1-1);
				val2 = EVEReadGPR(COUNTREG_E2);
				c++;
				} 	
			if (val1-1 != val2) return MCO_FAILURE;		
			}
		GoodReset();
		return MCO_SUCCESS;
		}
	just_wait_a_sec();
	}
if (sresult != 0) return MCO_FAILURE;
return MCO_SUCCESS;
}	
	

McoStatus decrement_count(void)
{
McoDongleType dType;

dType = getdongletype();
switch (dType) {
	case EVE_3: return decrement_count_e3();
	case EVE_2: return decrement_count_e2();
	}
return MCO_FAILURE;
}


// Decrement the register in the dongle which is used as a counter
// For Eve 3


McoStatus decrement_count_print(void)
{

	//Handle PktHandle;
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID,Passwd;
	int	i,c;
	unsigned short		val1,val2,data1 = 0;
	McoStatus 			state;
		
#ifdef NO_EVE
return MCO_SUCCESS;
#endif

state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{
	DevID = unscramble_num(E3_DEVID,E3_DEVID_S1);
	Passwd = unscramble_num(E3_PASSWD,E3_PASSWD_S1);
	result = RBEREAD(COUNTREG2_E3, &val1, data1, PktHandle);
	result = RBEREAD(COUNTREG2_E3, &val2, data1, PktHandle);
	c = 0;
	while ((val1 != val2) && (c<MAX_RETRIES))
		{
		just_wait_a_sec();
		result = RBEREAD(COUNTREG2_E3, &val1, data1, PktHandle);
		result = RBEREAD(COUNTREG2_E3, &val2, data1, PktHandle);
		c++;
		}
	if (val1 != val2) return MCO_FAILURE;
	if ((val1 >0) && (val1 <NORMCOUNT))	
		{
		result = RBEWRITE(COUNTREG2_E3+32, (val1-1), Passwd, PktHandle);
		result = RBEREAD(COUNTREG2_E3, &val2, data1, PktHandle);
		c = 0;
		while ((val1-1 != val2) && (c<MAX_RETRIES))
			{
			just_wait_a_sec();
			result = RBEWRITE(COUNTREG2_E3+32, (unsigned short)(val1-1), Passwd, PktHandle);
			result = RBEREAD(COUNTREG2_E3, &val2, data1, PktHandle);
			c++;
			} 	
		if (val1-1 != val2) return MCO_FAILURE;		
		}
	return MCO_SUCCESS;
	}
return state;
}	


/*  The following works with the serial number of the EVE dongle */

// Get the serial number for an e3

long get_internal_sn_e3(void)
{
	unsigned long	SN;
	Handle	PktHandle;
	unsigned short DevID;
	unsigned short int 	result;
	McoStatus state;

state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{
	result = RBEFINDFIRST(DevID, &SN, PktHandle);
	return (long)SN;
	}
return P_BadCount;
}

// Gets the eve 2 serial number

long get_internal_sn_e2(void)
{
int sresult;

GoodReset();
sresult = EVEStatus();
if (sresult == 0) return EVEReadGPR(SERREG_E2);
return -1;
}


long get_internal_sn(void)
{
McoDongleType dType;

dType = getdongletype();
switch (dType) {
	case EVE_3: return get_internal_sn_e3();
	case EVE_2: return get_internal_sn_e2();
	}
return -1;
}


// Get the access code given the serial number

long get_access_code(long int_num)
{
long 	temp;

temp = int_num;

temp -= INT_NUM_DEC;
temp = -temp;
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
return  rand1(0,2000000000,&temp);
}

// Return the monaco data 

int	monaco_date()
{
unsigned long time;

GetDateTime( &time);

time -= TIME_DEC;
time = time/86400;
return time;
}

// Get the access code when the date is considered

int get_dated_access_code(int int_num)
{
long 	temp;

temp = int_num;

temp -= INT_NUM_DEC;
temp = -temp;
temp += monaco_date();
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
return  rand1(0,32000,&temp);
}

// Check the dongle date against the internal date

int check_date_e3(void)
{
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID,Passwd;
	int	i,j;
	unsigned short		val1,val4,data1 = 0;
	McoStatus 			state;


state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{

	PktHandle = RBEHANDLE();
	if (PktHandle == NULL) return MCO_FAILURE;

	DevID = unscramble_num(E3_DEVID,E3_DEVID_S1);
	Passwd = unscramble_num(E3_PASSWD,E3_PASSWD_S1);

	val1 = 32000;
	result = RBEREAD(COUNTREG_E3, &val1, data1, PktHandle);
	result = RBEREAD(COUNTREG_E3, &val4, data1, PktHandle);
	if (val1 != val4)
		{
		for (j=0; j<MAX_RETRIES; j++)
			{
			just_wait_a_sec();
			result = RBEREAD(COUNTREG_E3, &val1, data1, PktHandle);
			just_wait_a_sec();
			result = RBEREAD(COUNTREG_E3, &val4, data1, PktHandle);
			if (val1 == val4) break;				
			}
		}
	}
if (monaco_date() > val1) return 1; 
return 0;
}

// Check the dongle date against the internal date

int check_date_e2(void)
{
int val1,val4;
int i,j;
short sresult;
val1 = 32000;
for (i=0; i<MAX_RETRIES; i++)
	{	

	GoodReset();
	sresult = EVEStatus();
	
	if (sresult == 0)
		{
		set_passwords(1);	
		val1 = EVEReadGPR(TIMEREG_E2);
		val4 = EVEReadGPR(TIMEREG_E2);
		if (val1 != val4)
			{
			for (j=0; j<MAX_RETRIES; j++)
				{
				just_wait_a_sec();
				val1 = EVEReadGPR(TIMEREG_E2);
				just_wait_a_sec();
				val4 = EVEReadGPR(TIMEREG_E2);
				if (val1 == val4) break;				
				}
			}
		}
	}
if (monaco_date() > val1) return 1; 
return 0;
}

int check_date(void)
{
McoDongleType dType;

dType = getdongletype();
switch (dType) {
	case EVE_3: return check_date_e3();
	case EVE_2: return check_date_e2();
	}
return MCO_FAILURE;
}

	
// Get the second code that has the number of clicks encodded within it
	
long get_access_code_2(int int_num, int num_clicks)
{
long 	temp,temp2;

temp = int_num;

temp -= INT_NUM_DEC_2;
temp = -temp;
temp += monaco_date();
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
temp2 = rand1(0,10000000,&temp);
temp = 0;
temp -= CLICK_NUM_DEC;
temp = -temp;
temp += num_clicks;
if (temp>0) temp = -temp;
if (temp == 0) temp -= CLICK_NUM_DISP;
return (rand1(0,10000000,&temp) + temp2);
}

// Return the number of clicks encodded into the second access code

int get_num_clicks(long acc_code_2, int int_num)
{
long 	temp,temp2;
int 	i;


temp = int_num;

temp -= INT_NUM_DEC_2;
temp = -temp;
temp += monaco_date();
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
temp2 = rand1(0,10000000,&temp);

for (i=0; i<NORMCOUNT; i++)
	{
	temp = 0;
	temp -= CLICK_NUM_DEC;
	temp = -temp;
	temp += i;
	if (temp>0) temp = -temp;
	if (temp == 0) temp -= CLICK_NUM_DISP;
	if (rand1(0,10000000,&temp) + temp2 == acc_code_2 ) return i;
	}
return -1;
}

// Get the third code that has the image type code encodded within it
	
long get_access_code_3(int int_num, int image_code)
{
long 	temp,temp2;

temp = int_num;

temp -= INT_NUM_DEC_3;
temp = -temp;
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
temp2 = rand1(0,10000000,&temp);
temp = 0;
temp -= CODE_NUM_DEC;
temp = -temp;
temp += image_code;
if (temp>0) temp = -temp;
if (temp == 0) temp -= CODE_NUM_DISP;
return (rand1(0,10000000,&temp) + temp2);
}

// Return the number image code encodded into the third access code

int get_image_code_from_ac(long acc_code_3, int int_num)
{
long 	temp,temp2;
int 	i;


temp = int_num;

temp -= INT_NUM_DEC_3;
temp = -temp;
if (temp>0) temp = -temp;
if (temp == 0) temp -= INT_NUM_DISP;
temp2 = rand1(0,10000000,&temp);

for (i=0; i<NORMCOUNT; i++)
	{
	temp = 0;
	temp -= CODE_NUM_DEC;
	temp = -temp;
	temp += i;
	if (temp>0) temp = -temp;
	if (temp == 0) temp -= CODE_NUM_DISP;
	if (rand1(0,10000000,&temp) + temp2 == acc_code_3 ) return i;
	}
return -1;
}

short get_image_code_e3(void)
{
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID,Passwd;
	int	i,j;
	unsigned short		val1,val4,data1 = 0;
	McoStatus			state;

state = MCO_SUCCESS;
if (PktHandle == NULL) state = open_e3();
if (state == MCO_SUCCESS) 
	{
	result = RBEREAD(IMAGECODE_E3, &val1, data1, PktHandle);
	result = RBEREAD(IMAGECODE_E3, &val4, data1, PktHandle);
	if (val1 != val4)
		{
		for (j=0; j<MAX_RETRIES; j++)
			{
			just_wait_a_sec();
			result = RBEREAD(IMAGECODE_E3, &val1, data1, PktHandle);
			just_wait_a_sec();
			result = RBEREAD(IMAGECODE_E3, &val4, data1, PktHandle);
			if (val1 == val4) break;				
			}
		}
	}
return val1;
}



// Check the dongle date against the internal date

short get_image_code_e2(void)
{
int val1,val4;
int i,j;
short sresult;

val1 = 32000;
for (i=0; i<MAX_RETRIES; i++)
	{	

	GoodReset();
	sresult = EVEStatus();
	
	if (sresult == 0)
		{
		set_passwords(1);	
		val1 = EVEReadGPR(IMAGECODE_E2);
		val4 = EVEReadGPR(IMAGECODE_E2);
		if (val1 != val4)
			{
			for (j=0; j<MAX_RETRIES; j++)
				{
				just_wait_a_sec();
				val1 = EVEReadGPR(IMAGECODE_E2);
				just_wait_a_sec();
				val4 = EVEReadGPR(IMAGECODE_E2);
				if (val1 == val4) break;				
				}
			}
		}
	}
return val1;
}


short get_image_code(void)
{
McoDongleType dType;

#ifdef NO_EVE
return 0xFFFF;
#endif

if (Gimagecode > -1) return Gimagecode;

dType = getdongletype();
switch (dType) {
	case EVE_3: Gimagecode = get_image_code_e3();
		return Gimagecode;
	case EVE_2: Gimagecode = get_image_code_e2();
		return Gimagecode;
	}
return 0;
}

Boolean is_monaco_print(void)
{
	McoStatus			state;
	unsigned long 		SN;
	unsigned short int 	result;
	unsigned short		DevID;
	unsigned short		data1,data2,data3,data4;
	int	i,j;
	McoUse	use;

if (PktHandle == NULL) state = open_e3();
for (i=0; i<MAX_RETRIES; i++)
	{
	result = RBEREAD( MONACOPRINT_VER_E3,&data2, L1_E3, PktHandle);
	if (data2 == MONACO_PRINT_VER_1)
		{
		result = RBEREAD(LOCK1_E3, &data1, L1_E3, PktHandle);
		result = RBEREAD(LOCK2_E3, &data2, data1, PktHandle);
		if ((data2>=0) && (data2<32)) result = RBEREAD(data2, &data3, data1, PktHandle);
		else data3 = -1;
		if (data3 == data1) return TRUE;				
		just_wait_a_sec();
		}
	get_next_e3();
	}
return FALSE;
}



/*
short convertCodeToPeter(short imageCode)
{
short JPT[] = JamesToPeterTypes;
short c,i;

c = 0;
for (i=0; i<3; i++)
	{
	if (imageCode & (1 << i)) c += 1 << JPT[i];
	}
return c;
}
*/