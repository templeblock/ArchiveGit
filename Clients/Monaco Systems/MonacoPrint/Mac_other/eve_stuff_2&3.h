/*	eve_stuff_2&3.h										*/
/*		The definitions used by the eve code			*/
/*		Copyright 1993 Monaco Systems Inc.				*/
/*		James Vogh										*/

#ifndef IN_EVE_STUFF
#define IN_EVE_STUFF


#include "mcostat.h"

#define	MONACO_COLOR_VER_2	20
#define MONACO_SELECT_VER_1	10
#define MONACO_PRINT_VER_1	10
//#define MONACO_PRINT_VER_1_LIGHT	10
//#define MONACO_PRINT_VER_1_MEDIUM	40
//#define MONACO_PRINT_VER_1_PRO		70
#define	MONACO_VIEW_VER_1			10

// These passwords are scrambled in order to protect their security
// Use unscramble() to unscramble the passwords
                    
#define E3_DEVID	303792
#define E3_DEVID_S1	-342
#define E3_PASSWD	195246
#define E3_PASSWD_S1 -3852

// The eve 2 passwords

#define PASSWD1		"pfSEWoVhSePTrTpC"
#define PASSWD2		"jnbPOptAORzRfuGr"
#define PASSWD3		"pTDkZMzafOKQPaur"

// The seed used for the random number generator which scrambles and unscrambles passwords

#define SCRAMKEY	-7563

// The allocation of the registers and locks for eve 3

#define COUNTREG_E3		4
#define	LOCK1_E3		0
#define LOCK2_E3		2
#define LOCKREG_E3		6
#define SERREG_E3		8
#define TIMEREG_E3		10
#define IMAGECODE_E3_OLD	12
#define IMAGECODE_E3	14
#define DEALERCODE_E3		16
#define DEALERRAM_E3	2
#define MONACOCOL_VER_E3	18		// The latest version of MC supported
#define MONACOPRINT_VER_E3	20		// The latest version of MP supported
#define MONACOVIEW_VER_E3	24		// The latest version of MP supported
#define COUNTREG2_E3			26


#ifdef PETER_BATCH
#define DEALERINC_E3	3
#else
#define DEALERINC_E3	1
#endif



#define L1_E3			657
#define L1_E3_NEW		743			
#define L2_E3			925

// The allocation of the registers and locks for eve 2


#define COUNTREG_E2	1
#define	LOCK1_E2		1
#define LOCK2_E2		2
#define LOCKREG_E2		2
#define SERREG_E2		3
#define TIMEREG_E2		4
#define IMAGECODE_E2	5

#define L1_E2			657
#define L1_E2_NEW		753
#define L2_E2			32


#define FS_L1_E2			847
#define FS_L2_E2			18

#ifdef PETER_BATCH
#define PRODUCT_CODE 2
#else
#define PRODUCT_CODE 1
#endif


#define	SUPERCOUNT	2000 
#define NORMCOUNT   1500
#define LIMITCOUNT_MAX 1499
#define LIMITCOUNT  25
#define ALMOSTCOUNT 10

// The maximum number of retries before get_usage gives up

#ifndef MAX_RETRIES
#define MAX_RETRIES  5
#endif

typedef enum {
	NO_DONGLE = 0,
	EVE_2,
	EVE_3
	} McoDongleType;


typedef enum {
	P_NoEve = 0,
	P_BadEve,
	P_NoUse ,
	P_AlmostNoUse,
	P_LimitUse,
	P_UnlimitUse,
	P_SuperUse,
	P_BadCount
	} McoUse;
	
	
#define SER_NUM_DEC	999888
#define SER_NUM_DISP 34
#define INT_NUM_DEC  4
#define INT_NUM_DISP 72
#define INT_NUM_DEC_2 9
#define CLICK_NUM_DEC 45
#define CLICK_NUM_DISP 43
#define CLICK_AC_MOD	567854

#define INT_NUM_DEC_3 56
#define CODE_NUM_DEC	36
#define CODE_NUM_DISP	542
#define CODE_AC_MOD	765456

// Time decrement is the number of days since Jan 1 1904 to Jan 1 1994 times 86400

#define TIME_DEC 2840227200

long rand1(long min,long max,long *s1);
void scramble(char array[],int num,long s1);
void unscramble(char array[], int num,long s1);
long get_internal_sn(void);
long scramble_num(unsigned short num,long s1);
unsigned short unscramble_num(long num,long s1);
char GoodReset( void );
void set_passwords(int level);
void close_eve(void);
McoUse get_usage_level(int *count);
McoStatus decrement_count(void);
McoStatus decrement_count_print(void);
long get_internal_serial_num(long ext_num);
long get_access_code(long int_num,long mod);
int	monaco_date();
int check_date(void);
long get_internal_sn(void);
short get_image_code(void);
McoDongleType getdongletype(void);
short get_image_code_from_ac(long acc_code_3, long int_num);
void just_wait_a_sec(void);
int get_num_clicks(long acc_code_2, long int_num);

long get_access_code_2(long int_num, long num_clicks);
long get_access_code_3(long int_num, long image_code);

void IncDealerCnt(void);
Boolean is_monaco_print(void);

#endif