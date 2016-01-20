/********************************************************************/
/*  These functions are easily transferable to other programs       */
/*  June 16,1992                                                    */
/*  James Vogh                                                      */
/********************************************************************/

#define IN_ASSORT
#include "james_v.h"
#include <string.h>
#include "mcomem.h"

int s1,s2;

/* Don't ask, just leave it in here 

int stupid(the_samp)
struct  samp_data *the_samp;
{
 printf("%d\n",the_samp->data_s); 
return(the_samp->data_s);
}
*/

/*  generate a Gaussian kernal */

void generate_gaus_kernal(double *kern,int num,double b)
{
        int i;
        double temp;

for (i=0; i<num; i++)
        {
        temp=(double)(num/2);
        kern[i]=exp(-((double)i-temp)*((double)i-temp)*b/(temp*temp));
        }
}

void generate_decay_curve(double *kern,int num,double a,double b,double c)
{
        int i;
        
printf("%e  %e  %e\n",a,b,c);
for (i=0; i<num; i++)
        {
        kern[i]=c*exp(b*(i+a));
        }
}


/* create an array of size size */

long get_mem(unsigned long size)
{
        long add;

/* printf("%d\n",size);  */
//add =(long) new int [size]; 
add=(long)McoMalloc(size);
if (add>0) return(add);
else 
        {
        printf("Memory allocation error, may be out of memory\n");
        exit(0);
        }
return (0);
}

void free_mem(void *p)
{
McoFree(p);
}

double *get_array(int size)    
{
        double *temp;

temp = (double*)get_mem(sizeof(double)*size);
return(temp);
}

long *get_l_array(int size)    
{
        long *temp;

temp = (long*)get_mem(sizeof(long)*size);
return(temp);
}

/*
void print_loc(int i)
{
        int x,y,l;

y=i/(i3*i4);
x=(i%(i3*i4))/i3;
l=(i%(i3*i4))%i3;
printf("%d  %d  %d\n",x,y,l);
}


void get_loc(int i,int *x,int *y,int *l)
{
*y=i/(i3*i4);
*x=(i%(i3*i4))/i3;
*l=(i%(i3*i4))%i3;
}
*/

void convolve(double *in,double *out,double *kern,int s1,int s2)
{
        register int i,j,k;

/*  The output variable is cleared below */
k=s2>>1;
for (i=0; i<s1; i++) out[i]=0;
for (i=0; i<s1; i++)
        {
        if (in[i]>0.0) 
                {
                for(j=0; j<s2; j++)
                        {
                        if ((j-k+i>=0) && (j-k+i<s1))
                                {
                                out[j-k+i] += in[i]*kern[j];
                                }
                        }
                }
        }
}


void error(char *c)
{
printf("WORKOVER ---- %s\n",c);
exit(0);
}


void print_array(double *f,int n)
{
        int i;
for (i=0; i<n; i++)
        {
        printf("%e  ",f[i]);
        }
printf("\n");
}

double convert_from_space(double *map,int num_l)
{
        int i,max_i;
        double   max;

max=-1e20;
max_i=-1;
for (i=0; i<num_l; i++)
        {
        if (map[i]>max) 
                {
                /* if (map[i]>0.0) printf("%f\n",map[i]); */
                max=map[i];
                max_i=i;
                }
        }
return((double)max_i/num_l);
}

int fround(double f)
{
int     t;

t=(int)f;
if (f-(double)t>=0.5) t++;
return(t);
}


void convert_to_space(double *map,int num_l,double value,double max_map)
{  
        int  i;

for (i=0; i<num_l; i++) map[i]=0;
if ((value>=0) && (value<1.0))
map[fround(value*num_l)]=max_map;
}

void add_to_space(double *map,int num_l,double value,double max_map)
{  
        int  i;

if ((value>=0) && (value<1.0))
map[(int)(value*num_l)] += max_map;
}



#define MAX_RAN_INT 2147483647

double ran0()
{
/*      double temp;

temp=(double)rand()/(double)MAX_RAN_INT;
 printf("%f\n",temp); */
return((double)rand()/(double)MAX_RAN_INT);
}

#define         M1      259200
#define         IA1     7141
#define         IC1     54773
#define         RM1     (1.0/M1)
#define         M2      134456
#define         IA2     8121
#define         IC2     28411
#define         RM2     (1.0/M2)
#define         M3      243000
#define         IA3     4561
#define         IC3     51349

double   ran1(int *idum)
{
        static  long    ix1,ix2,ix3;
        static  double   r[98];
        double           temp;
        static  int     iff=0;
        int             j;
        
if (*idum < 0 || iff==0) 
        {
        iff=1;
        ix1=(IC1-(*idum)) % M1;
        ix1=(IA1*ix1+IC1) % M1;
        ix2=ix1 % M2;
        ix1=(IA1*ix1+IC1) % M1;
        ix3=ix1 % M3;
        for (j=0; j<97; j++)
                {
                ix1=(IA1*ix1+IC1) % M1;
                ix2=(IA2*ix2+IC2) % M2;
                r[j]=(ix1+ix2*RM2)*RM1;
                }
        *idum=1;
        }
ix1=(IA1*ix1+IC1) % M1;
ix2=(IA2*ix2+IC2) % M2;
ix3=(IA3*ix3+IC3) % M3;
j=((97*ix3)/M3);
if (j>96 || j<0) error("Random number error");
temp=r[j];
r[j]=(ix1+ix2*RM2)*RM1;
return(temp);
}


double gasdev(int *idum)
{
        static int iset=0;
        static double gset;
        double fac,r,v1,v2;

        if  (iset == 0) {
                do {
                        v1=2.0*ran1(idum)-1.0;
                        v2=2.0*ran1(idum)-1.0;
                        r=v1*v1+v2*v2;
                } while (r >= 1.0);
                fac=sqrt(-2.0*log(r)/r);
                gset=v1*fac;
                iset=1;
                return v2*fac;
        } else {
                iset=0;
                return gset;
        }
}

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
        ix1=(IC1-(*idum)) % M1;
        ix1=(IA1*ix1+IC1) % M1;
        ix2=ix1 % M2;
        ix1=(IA1*ix1+IC1) % M1;
        ix3=ix1 % M3;
        for (j=0; j<97; j++)
                {
                ix1=(IA1*ix1+IC1) % M1;
                ix2=(IA2*ix2+IC2) % M2;
                r[j]=(ix1+ix2*RM2)*RM1;
                }
        *idum=1;
        }
ix1=(IA1*ix1+IC1) % M1;
ix2=(IA2*ix2+IC2) % M2;
ix3=(IA3*ix3+IC3) % M3;
j=((97*ix3)/M3);
temp=r[j];
r[j]=(ix1+ix2*RM2)*RM1;
return(temp);
}


long int rand1(long int min,long int max)
{
return((long int)(0.999*ran1(&s1)*(double)(max-min))+min);
}


long rand1(long min,long max,long *s1)
{
return((long )(0.999*ran1(s1)*(double)(max-min))+min);
}

int     days[12] ={31,28,31,30,31,30,31,31,30,31,30,31};


/* convert a date in year month day format to a year + percentage of year format */

int decimal_date2(char *date,double *d_date)
{
        int     year,month,day,num_days,i;
        char    y[4],m[4],d[4];
                int     total_days = 365;

y[0]=date[0];
y[1]=date[1];
y[2]=date[6];
m[0]=date[2];
m[1]=date[3];
m[2]=date[6];
d[0]=date[4];
d[1]=date[5];
d[2]=date[6];

sscanf(y,"%d",&year);
sscanf(m,"%d",&month);
sscanf(d,"%d",&day);


if (!(year % 4)) 
        {
        days[1]++;
        total_days=366;
        }
num_days=0;
for (i=0; i<month-1; i++)
        {
        num_days += days[i];
        }
num_days += day;

*d_date=1900.0+(double)year+(double)num_days/(double)total_days;
/* printf("%d %d %d %d %d %f      %s\n",year,month,day,num_days,total_days,d_date,date); */
return (0);
}

void report_error(int *num_correct,int *num_in_test,int num_types)
{
        int i;

for (i=0; i<num_types; i++)
        {
        printf("Set #%d, # Correct %d out of %d, %f percent correct\n",
                        i+1,num_correct[i],num_in_test[i],
                        (double)num_correct[i]/(double)num_in_test[i]*100);
        }
}




/*  erms calculates the root mean squared error 
        output  The output of a network
        desired The desired output   */

double erms(double *output,double *desired,int num_pat)
{
        double sum,temp;
        int i;

sum=0;
for(i=0; i<num_pat; i++)
        {
        sum+=(desired[i]-output[i])*(desired[i]-output[i]);
        //cout << "Desired " << desired[i] << " Output " << output[i] << '\n';
        }
temp=sqrt(sum);
return(temp);
}


/* scramble  scrambles the contents of an array  
        array   the array whose contents are to be scrambled
        num     the number of elements in the array */

void scramble(int *array,int num)
{

        int     i,swap,t;

for(i=0; i<num; i++)
        {
        swap=(int)rand1(0,num);
        t=array[i];
        array[i]=array[swap];
        array[swap]=t;
        }
}

/* Count the number of bits */

int num_bits(int n)
{
        int     c;
c=0;
while (n>0)
        {
        n = n/2;
        c++;
        }
return(c-1);
}

/* The following is a simple FFT function it will at least make this 
   code more portable.  The code was adapted fro NR in C 
*/

#define NR_SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void four1(double *data,int nn,int isign)
{
        int n,mmax,m,j,istep,i;
        double wtemp,wr,wpr,wpi,wi,theta;
        double tempr,tempi;

        n=nn << 1;
        j=1;
        for (i=1;i<n;i+=2) {
                if (j > i) {
                        NR_SWAP(data[j],data[i]);
                        NR_SWAP(data[j+1],data[i+1]);
                }
                m=n >> 1;
                while (m >= 2 && j > m) {
                        j -= m;
                        m >>= 1;
                }
                j += m;
        }
        mmax=2;
        while (n > mmax) {
                istep=2*mmax;
                theta=6.28318530717959/(isign*mmax);
                wtemp=sin(0.5*theta);
                wpr = -2.0*wtemp*wtemp;
                wpi=sin(theta);
                wr=1.0;
                wi=0.0;
                for (m=1;m<mmax;m+=2) {
                        for (i=m;i<=n;i+=istep) {
                                j=i+mmax;
                                tempr=wr*data[j]-wi*data[j+1];
                                tempi=wr*data[j+1]+wi*data[j];
                                data[j]=data[i]-tempr;
                                data[j+1]=data[i+1]-tempi;
                                data[i] += tempr;
                                data[i+1] += tempi;
                        }
                        wr=(wtemp=wr)*wpr-wi*wpi+wr;
                        wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        }
}

#undef NR_SWAP


void strcpy2(char *c, char *b)
{
int i;

for (i=0; i<=strlen(b); i++)
	c[i]=b[i];
}

void get_block(char *c,char *c2,int &n)
{
	int i;

i=0;
while ((n<=strlen(c)) && (c[n]!=' '))
	c2[i++]=c[n++];
c2[i]='\0';
while ((n<=strlen(c)) && (c[n]==' ')) n++;
}

char *skip_blocks(char *c,int n)
{ 
	int i,v;

v=0;
for (i=0; i<n; i++)
	{
	while ((v<=strlen(c)) && (c[v]!=' ')) v++;
	while ((v<=strlen(c)) && (c[v]==' ')) v++;	
	if (v>strlen(c)) return(0L);
//	cout << &c[v];
	}
return (&c[v]);
}

int strcmp2(char *c, char *d)
{
int a,b,i;

a = strlen(c);
b = strlen(d);

if (a != b) return 1;

for (i=0; i<a; i++) if (c[i] != d[i]) return 1;
return 0;
}

int hasLetters(char *j)
{
int l,i;

l = strlen(j);

for (i=0; i<l; i++)
	{
	if (((j[i] >= 'a') && (j[i] <= 'z')) || ((j[i] >= 'A') && (j[i] <= 'Z'))) 
		return 1;
	}
return 0;
}

char* sscanf_n(char *c,int start,int n, double *p)
{
	int i,j;
	char *c2,*c3 = 0L;
	char *c4;

c3 = new char[strlen(c)];

c2=skip_blocks(c,start);
c4 = c2;
if (c2 == 0L) goto bail;
j=0;
for (i=0; i<n; i++)
	{
	get_block(c2,c3,j);
	if (c3[0] == 0) {c4 = 0; goto bail;}
	sscanf(c3,"%lf",&p[i]);
	}
c2=skip_blocks(c,start+n);
if (c2 == 0L) goto bail;
strcpy(c3,c2);
strcpy(c,c3);
delete c3;
return 0L;
bail:
c[0] = 0;
if (c3) delete c3;
return c4;
}


char* sscanf_n_i(char *c,int start,int n, int *p)
{
	int i,j;
	char *c2,*c3 = 0L;
	char *c4;


c3 = new char[strlen(c)];

c2=skip_blocks(c,start);
c4 = c2;
if (c2 == 0L) goto bail;
j=0;
for (i=0; i<n; i++)
	{
	get_block(c2,c3,j);
	if (c3[0] == 0) {c4 = 0; goto bail;}
	sscanf(c3,"%d",&p[i]);
	}
c2=skip_blocks(c,start+n);
if (c2 == 0L) goto bail;
strcpy(c3,c2);
strcpy(c,c3);
delete c3;
return 0L;
bail:
c[0] = 0;
if (c3) delete c3;
return c4;
}

char* sscanf_n_li(char *c,int start,int n, long *p)
{
	int i,j;
	char *c2,*c3 = 0L;
	char *c4;


c3 = new char[strlen(c)];

c2=skip_blocks(c,start);
c4 = c2;
if (c2 == 0L) goto bail;
j=0;
for (i=0; i<n; i++)
	{
	get_block(c2,c3,j);
	if (c3[0] == 0) {c4 = 0; goto bail;}
	sscanf(c3,"%ld",&p[i]);
	}
c2=skip_blocks(c,start+n);
if (c2 == 0L) goto bail;
strcpy(c3,c2);
strcpy(c,c3);
delete c3;
return 0L;
bail:
c[0] = 0;
if (c3) delete c3;
return c4;
}

