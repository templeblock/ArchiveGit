/****************************************************************************
* Return the square root of a long value				   													*
****************************************************************************/
inline long long_sqrt(long v)
{
int				i;
unsigned long	result,tmp;
unsigned long	low,high;

if (v <= 1L) return((unsigned)v);

low = v;
high = 0L;
result = 0;

for (i = 0 ; i < 16 ; i++)
{
  result += result;
  high = (high << 2) | ((low >> 30) & 0x3);
  low <<= 2;
  tmp = result + result + 1;
  if (high >= tmp)
	{
		result++;
		high -= tmp;
	}
}

if (v - (result * result) >= (result - 1))
    result++;

return(result);
}
