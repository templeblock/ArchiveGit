#include "mcostat.h"
#include "icctypes.h"
#include "setupctype.h"

McoStatus setUpCType(Ctype *CT,int numbits_icc)
{
int i,j;
char *dev_con,*con_dev;
short *dev_con_s,*con_dev_s;

	for (i=0; i< 6; i++) 
		{
		CT->input_tables[i] = 0L;
		CT->output_tables[i] = 0L;
		}
	
	if (numbits_icc == 8)
		{
		for(i = 0; i <CT->input_channels; i++)
			{
			dev_con = new char[CT->input_entries];
			if (!dev_con) return MCO_MEM_ALLOC_ERROR;
			for(j = 0; j < CT->input_entries; j++){
				dev_con[j] = j;
				}	
			CT->input_tables[i] = dev_con;	
			}
			
		for(i = 0; i < CT->output_channels; i++)
			{
			con_dev = new char[CT->output_entries];
			if (!con_dev) return MCO_MEM_ALLOC_ERROR;
			for(j = 0; j < CT->output_entries; j++){
				con_dev[j] = j;
				}	
			CT->output_tables[i] = con_dev;	
			}
		}
	else
		{
		for(i = 0; i <CT->input_channels; i++)
			{
			dev_con_s = new short[CT->input_entries];
			if (!dev_con_s) return MCO_MEM_ALLOC_ERROR;
			for(j = 0; j < CT->input_entries; j++){
				dev_con_s[j] = 65535*((double)j/((double)CT->input_entries-1.0));
				}	
			CT->input_tables[i] = (char*)dev_con_s;	
			}
			
		for(i = 0; i < CT->output_channels; i++)
			{
			con_dev_s = new short[CT->output_entries];
			if (!con_dev_s) return MCO_MEM_ALLOC_ERROR;
			for(j = 0; j < CT->output_entries; j++){
				con_dev_s[j] = 65535*((double)j/((double)CT->output_entries-1.0));;
				}	
			CT->output_tables[i] = (char*)con_dev_s;	
			}
		}	

return MCO_SUCCESS;	
}


void cleanupCType(Ctype *CT)
{
int i;

for (i=0; i< 6; i++) 
	{
	if (CT->input_tables[i]) delete CT->input_tables[i];
	if (CT->output_tables[i]) delete CT->output_tables[i];
	}
}

