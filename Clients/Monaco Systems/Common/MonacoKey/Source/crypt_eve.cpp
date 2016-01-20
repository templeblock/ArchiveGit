/**********************************************************************/
/* crypt_eve.c														  */
/* encrypt and decrypt the passwords to remotly program the dongle    */
/* James Vogh														  */
/**********************************************************************/

//#include "eve_stuff_2&3.h"
#if defined(_WIN32)
#include <time.h>

#else

#endif
#include "crypt_eve.h"



#define TIME_DEC 2840227200

int	monaco_date()
{
unsigned long long_time;

#if defined(_WIN32)
time( (long*)&long_time );
#else
GetDateTime( &long_time);
#endif

long_time -= TIME_DEC;
long_time = long_time/86400;
return long_time;
}

/* The random number generator from NRC */

#define         PRM1      259200
#define         IA1     7141
#define         IC1     54773
#define         RM1     (1.0/PRM1)
#define         PRM2      134456
#define         IA2     8121
#define         IC2     28411
#define         RM2     (1.0/PRM2)
#define         M3      243000
#define         IA3     4561
#define         IC3     51349


double   ran1(long *idum)
{
        static  long    ix1,ix2,ix3;
        static  double   r[98];
        double           temp;
        static  int     iff=0;
        int             j;
        
if (*idum < 0 || iff==0) 
        {
        iff=1;
        ix1=(IC1-(*idum)) % PRM1;
        ix1=(IA1*ix1+IC1) % PRM1;
        ix2=ix1 % PRM2;
        ix1=(IA1*ix1+IC1) % PRM1;
        ix3=ix1 % M3;
        for (j=0; j<97; j++)
                {
                ix1=(IA1*ix1+IC1) % PRM1;
                ix2=(IA2*ix2+IC2) % PRM2;
                r[j]=(ix1+ix2*RM2)*RM1;
                }
        *idum=1;
        }
ix1=(IA1*ix1+IC1) % PRM1;
ix2=(IA2*ix2+IC2) % PRM2;
ix3=(IA3*ix3+IC3) % M3;
j=((97*ix3)/M3);
temp=r[j];
r[j]=(ix1+ix2*RM2)*RM1;
return(temp);
}

long rand1(long min,long max,long *s1)
{
return((long )(0.999*ran1(s1)*(double)(max-min))+min);
}

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


void encrypt(long sn,unsigned long &p1,unsigned long &p2,
	unsigned char codes1,unsigned char codes2,unsigned char count1,unsigned char count2)
{
int	i;
long temp,date;
unsigned long rn;
unsigned char t1,t2,t3,t4,t5;
unsigned char scram[256];

temp = sn;
temp = temp ^ N1;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
if ((count1 < 254) && (count1 > 0) && (count2 <254) && (count2 > 0)) date = monaco_date();
else date = 54364324;
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
t1 = scram[codes1];
for (i=0; i<P1+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
t2 = scram[codes2];
temp = sn;
temp = temp ^ N1 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
t3 = scram[count1];
for (i=0; i<P1+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
t4 = scram[count2];
temp = sn;
temp = temp ^ M1;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
p1 = ((unsigned long)t1<<24) + ((unsigned long)t3<<16) + ((unsigned long)t2<<8) + (unsigned long)t4;
rn = rand1(0,2147483647,&temp);
p1 = p1 ^ rn;

temp = sn;
temp = temp ^ N2;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
if ((count1 < 254) && (count1 > 0) && (count2 <254) && (count2 > 0)) date = monaco_date();
else date = 54364324;
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
t1 = scram[count1];
for (i=0; i<P2+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
t2 = scram[codes1];
temp = sn;
temp = temp ^ N2 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
t3 = scram[count2];
for (i=0; i<P2+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
t4 = scram[codes2];
temp = sn;
temp = temp ^ M2;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
p2 = ((unsigned long)t1<<24) + ((unsigned long)t3<<16) + ((unsigned long)t2<<8) + (unsigned long)t4;
rn = rand1(0,2147483647,&temp);
p2 = p2 ^ rn;
}

void unscramble2(unsigned char *scram, unsigned char *unscram,long num)
{

	int	i;
	
for (i=0; i<num; i++)
	{
	unscram[scram[i]] = i;
	}
}

long decrypt(long sn, unsigned long p1,unsigned long p2,
	unsigned char &codes1,unsigned char &codes2,unsigned char &count1,unsigned char &count2)
{
int i;
//long sn;
long rn1,rn2,rn3,rn4,rn5;
long temp,date;
long t1,t2,t3,t4,t5;
unsigned long tp,rn;
unsigned char scram[256],scram1[256],scram2[256],scram3[256],scram4[256];

//sn = get_internal_sn();
//sn = 56;
//sn = 5243846;

temp = sn;
temp = temp ^ N1;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
date = monaco_date();
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
unscramble2(scram,scram1,256);
for (i=0; i<P1+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram2,256);
temp = sn;
temp = temp ^ N1 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
unscramble2(scram,scram3,256);
for (i=0; i<P1+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram4,256);
temp = sn;
temp = temp ^ M1;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
rn = rand1(0,2147483647,&temp);
tp = p1 ^ rn;
t1 = tp>>24;
t3 = (tp - (t1<<24)) >>16;
t2 = (tp - (t1<<24) - (t3<<16))>>8;
t4 = (tp - (t1<<24) - (t3<<16) - (t2<<8));
t1 = scram1[t1];
t2 = scram2[t2];
t3 = scram3[t3];
t4 = scram4[t4];

//if (t1 != t2) goto no_date;
//if (t3 != t4) goto no_date;
codes1 = t1;
codes2 = t2;
count1 = t3;
count2 = t4;

temp = sn;
temp = temp ^ N2;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
date = monaco_date();
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
unscramble2(scram,scram1,256);
for (i=0; i<P2+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram2,256);
temp = sn;
temp = temp ^ N2 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
unscramble2(scram,scram3,256);
for (i=0; i<P2+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram4,256);
temp = sn;
temp = temp ^ M2;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
rn = rand1(0,2147483647,&temp);
tp = p2 ^ rn;
t1 = tp>>24;
t3 = (tp - (t1<<24)) >>16;
t2 = (tp - (t1<<24) - (t3<<16))>>8;
t4 = (tp - (t1<<24) - (t3<<16) - (t2<<8));
t1 = scram1[t1];
t2 = scram2[t2];
t3 = scram3[t3];
t4 = scram4[t4];

if (t1 != count1) goto no_date;
if (t3 != count2) goto no_date;
if (t2 != codes1) goto no_date;
if (t4 != codes2) goto no_date;
return 0;

no_date:

//sn = get_internal_sn();
//sn = 56;
//sn = 5243846;

temp = sn;
temp = temp ^ N1;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
date = 54364324;
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
unscramble2(scram,scram1,256);
for (i=0; i<P1+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram2,256);
temp = sn;
temp = temp ^ N1 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
unscramble2(scram,scram3,256);
for (i=0; i<P1+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram4,256);
temp = sn;
temp = temp ^ M1;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
rn = rand1(0,2147483647,&temp);
tp = p1 ^ rn;
t1 = tp>>24;
t3 = (tp - (t1<<24)) >>16;
t2 = (tp - (t1<<24) - (t3<<16))>>8;
t4 = (tp - (t1<<24) - (t3<<16) - (t2<<8));
t1 = scram1[t1];
t2 = scram2[t2];
t3 = scram3[t3];
t4 = scram4[t4];

codes1 = t1;
codes2 = t2;
count1 = t3;
count2 = t4;

temp = sn;
temp = temp ^ N2;
if (temp >0) temp = -temp;
if (temp == 0) temp = -6453;
date = 54364324;
for (i=0; i<256; i++) scram[i] = i;
scramble((char*)scram,256,temp);
unscramble2(scram,scram1,256);
for (i=0; i<P2+3;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram2,256);
temp = sn;
temp = temp ^ N2 ^ date;
if (temp >0) temp = -temp;
if (temp == 0) temp = -8345;
scramble((char*)scram,256,temp);
unscramble2(scram,scram3,256);
for (i=0; i<P2+1;  i++) t5 = rand1(0,256,&temp);
scramble((char*)scram,256,temp);
unscramble2(scram,scram4,256);
temp = sn;
temp = temp ^ M2;
if (temp > 0) temp = -temp;
if (temp == 0) temp = -346;
rn = rand1(0,2147483647,&temp);
tp = p2 ^ rn;
t1 = tp>>24;
t3 = (tp - (t1<<24)) >>16;
t2 = (tp - (t1<<24) - (t3<<16))>>8;
t4 = (tp - (t1<<24) - (t3<<16) - (t2<<8));
t1 = scram1[t1];
t2 = scram2[t2];
t3 = scram3[t3];
t4 = scram4[t4];

if (t1 != count1) return 0;
if (t3 != count2) return 0;
if (t2 != codes1) return 0;
if (t4 != codes2) return 0;

return 1;

}
