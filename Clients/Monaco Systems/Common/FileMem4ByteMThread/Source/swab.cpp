#include "swab.h"

Swab::Swab()
{
}

Swab::~Swab()
{
}

void Swab::setSwab(Boolean s)
{
	_swab = s;
}

Boolean Swab::getSwab(void)
{
	return _swab;
}

void Swab::swabShort(u_int16 *s, u_int32 num)
{
	u_int32	i;
	char temp;
	char *head;
	char *test = (char*)s;
	
	for(i = 0; i < num; i++)
	{
		head = (char*)(s++);
		temp = head[0];
		head[0] = head[1];
		head[1] = temp;
	}
}

void Swab::swabLong(u_int32 *l, u_int32 num)
{
	u_int32	i;
	char temp;
	char *head;
	char *test = (char*)l;

	for(i = 0; i < num; i++)
	{
		head = (char*)(l++);
		temp = head[0];
		head[0] = head[3];
		head[3] = temp;
		temp = head[1];
		head[1] = head[2];
		head[2] = temp;
	}
}

