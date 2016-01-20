#include "think_bugs.h"

//only support positive number
double modf_mod(double total, double * integral)
{

	*integral = (double)((long)total);
	return (total-(*integral));
}

//string length must be less than 255, otherwise it will crash
StringPtr ctopstr(char *str)
{
	unsigned char len;
	short	i;
	
	len = strlen(str);
	for( i = len - 1; i >= 0; i--)
		*(str+i+1) = *(str+i);
		
	*str = len;
	
	return (StringPtr)str;
}		

//string length must be less than 255, otherwise it will crash
char* ptocstr(StringPtr str)
{
	unsigned char len, i;
	
	len = *((unsigned char*)str);
	
	for( i = 0; i < len; i++)
		*(str+i) = *(str+i+1);

	*(str+len) = '\0';

	return (char*)str;	
}