#ifndef _INTERPOLATE_H
#define _INTERPOLATE_H

class Linterp1{
private:
protected:
public:

Linterp1(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp);
	
//do nothing
~Linterp1(void){};
	
};	

class Linterp1r{
private:
protected:
public:

Linterp1r(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp);
	
~Linterp1r(void){};	
};

class Linterp3{
private:
protected:
public:

//table:char, src:char, dest:char
Linterp3(unsigned char *table, long tablep, long gridp, 
	unsigned char *src, long srcp, long srcsize, unsigned char *dest);

Linterp3(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest);
	
//table:char, src:char, dest:short
Linterp3(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);

//table:short, src:char, dest:char
//this only works for ICC RGB to LAB
Linterp3(unsigned short *table, long tablep, long gridp, 
	unsigned char *src,long srcp, long srcsize, unsigned char *dest, long nouse);

//this works for other case
Linterp3(unsigned short *table, long tablep, long gridp, 
	unsigned char *src,long srcp, long srcsize, unsigned char *dest);

//not been test yet
//table:short, src:char, dest:short
Linterp3(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);

//this might be obsolete	
Linterp3(double *nouse, unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest);

//do nothing
~Linterp3(void){};

};

class Linterp4{
private:
protected:
public:

Linterp4(unsigned char *table, long tablep, long gridp, 
	unsigned char *src, long srcp, long srcsize, unsigned char *dest);

Linterp4(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest);

//not been test yet
Linterp4(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);
	
//not been test yet
Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);
	
Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest);

//do nothing
~Linterp4(void){};
};


#endif