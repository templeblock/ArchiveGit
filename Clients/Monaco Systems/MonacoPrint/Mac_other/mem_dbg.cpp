/* debuging code for new and delete */

#include <new.h>
#include <stdlib.h>
#include <Memory.h>
#include "mcomem.h"
#include "mcostat.h"
#include "errordial.h"
#include <stdio.h>
#include "mem_dbg.h"

// initial values

#define NEW_FILL_I	0xAE
#define HAND_FILL_I	0xAB
#define PTR_FILL_I	0xAC

// disposed values

#define NEW_FILL_D	0xBE
#define HAND_FILL_D	0xBB
#define PTR_FILL_D	0xBC

long current_level;


long max_news = 0;
long num_new = 0;

void **dbg_new_ptrs = 0L;
size_t *dbg_new_sizes = 0L;
long *dbg_new_counts = 0L;
long *dbg_new_levels = 0L;


long max_ptrs = 0;
long num_ptrs = 0;

void **dbg_ptr_ptrs = 0L;
long *dbg_ptr_sizes = 0L;
long *dbg_ptr_counts = 0L;
long *dbg_ptr_levels = 0L;

long max_hand = 0;
long num_hand = 0;

void **dbg_hand_ptrs = 0L;
long *dbg_hand_sizes = 0L;
long *dbg_hand_counts = 0L;
long *dbg_hand_levels = 0L;


void setup_debug(long MAX_NEWS,long MAX_PTR, long MAX_HANDLE)
{
int i;

current_level = -1;

dbg_ptr_ptrs = (void**)McoMalloc(MAX_PTR*sizeof(void*));
for (i=0; i<MAX_PTR; i++) dbg_ptr_ptrs[i] = 0L;
dbg_ptr_sizes = (long*)McoMalloc(MAX_PTR*sizeof(long));
dbg_ptr_counts = (long*)McoMalloc(MAX_PTR*sizeof(long));
dbg_ptr_levels = (long*)McoMalloc(MAX_PTR*sizeof(long));
max_ptrs = MAX_PTR;
num_ptrs = 0;


dbg_new_ptrs = (void**)McoMalloc(MAX_NEWS*sizeof(void*));
for (i=0; i<MAX_NEWS; i++) dbg_new_ptrs[i] = 0L;
dbg_new_sizes = (size_t*)McoMalloc(MAX_NEWS*sizeof(size_t));
dbg_new_counts = (long*)McoMalloc(MAX_NEWS*sizeof(long));
dbg_new_levels = (long*)McoMalloc(MAX_NEWS*sizeof(long));
max_news = MAX_NEWS;
num_new = 0;


dbg_hand_ptrs = (void**)McoMalloc(MAX_HANDLE*sizeof(void*));
for (i=0; i<MAX_HANDLE; i++) dbg_hand_ptrs[i] = 0L;
dbg_hand_sizes = (long*)McoMalloc(MAX_HANDLE*sizeof(long));
dbg_hand_counts = (long*)McoMalloc(MAX_HANDLE*sizeof(long));
dbg_hand_levels = (long*)McoMalloc(MAX_HANDLE*sizeof(long));
max_hand = MAX_HANDLE;
num_hand = 0;

current_level = 0;

}

void clear_debug(void)
{
current_level = -1;

if (dbg_new_ptrs) McoFree(dbg_new_ptrs);
dbg_new_ptrs = 0L;
if (dbg_new_sizes) McoFree(dbg_new_sizes);
dbg_new_sizes = 0L;
if (dbg_new_counts) McoFree(dbg_new_counts);
dbg_new_counts = 0L;
if (dbg_new_levels) McoFree(dbg_new_levels);
dbg_new_levels = 0L;


if (dbg_hand_ptrs) McoFree(dbg_hand_ptrs);
dbg_hand_ptrs = 0L;
if (dbg_hand_sizes) McoFree(dbg_hand_sizes);
dbg_hand_sizes = 0L;
if (dbg_hand_counts) McoFree(dbg_hand_counts);
dbg_hand_counts = 0L;
if (dbg_hand_levels) McoFree(dbg_hand_levels);
dbg_hand_levels = 0L;

if (dbg_ptr_ptrs) McoFree(dbg_ptr_ptrs);
dbg_ptr_ptrs = 0L;
if (dbg_ptr_sizes) McoFree(dbg_ptr_sizes);
dbg_ptr_sizes = 0L;
if (dbg_ptr_counts) McoFree(dbg_ptr_counts);
dbg_ptr_counts = 0L;
if (dbg_ptr_levels) McoFree(dbg_ptr_levels);
dbg_ptr_levels = 0L;


}


void fill_mem(size_t size, void *ptr,char fill)
{
long i;
char *ptrc;

ptrc  = (char*)ptr;
for (i=0; i<size; i++)
	{
	ptrc[i] = fill;
	}
}


void debug_new(size_t size,void *ptr)
{
long i;


if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_new_ptrs) return;
if (!dbg_new_sizes) return;
if (!dbg_new_counts) return;
if (!dbg_new_levels) return;
for (i=0; i<max_news; i++) if (!dbg_new_ptrs[i]) 
	{	
	dbg_new_ptrs[i] = ptr;
	dbg_new_sizes[i] = size;
	dbg_new_counts[i] = num_new;
	dbg_new_levels[i] = current_level;
	if ((size == 340) || (num_new == 57) || (num_new == 57) || (num_new == 57))
		{
		i = 0;
		}
	num_new++;

	break;
	}
if (i == max_news) McoErrorAlert(MCO_MAX_MEMORY);
fill_mem(size,ptr,NEW_FILL_I);
}

void debug_delete(void *ptr)
{
long i;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_new_ptrs) return;
for (i=0; i<max_news; i++) if (dbg_new_ptrs[i] == ptr)
	{
	fill_mem(dbg_new_sizes[i],ptr,NEW_FILL_D);
	dbg_new_ptrs[i] = 0L;
	dbg_new_sizes[i] = 0;
	dbg_new_counts[i] = 0;
	dbg_new_levels[i] = 0;
	break;
	}
if (i == max_news) McoErrorAlert(MCO_INVALID_PTR);
}


void debug_newptr(long size,void *ptr)
{
long i;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_ptr_ptrs) return;
if (!dbg_ptr_sizes) return;
if (!dbg_ptr_counts) return;
if (!dbg_ptr_levels) return;
for (i=0; i<max_ptrs; i++) if (!dbg_ptr_ptrs[i]) 
	{
	dbg_ptr_ptrs[i] = ptr;
	dbg_ptr_sizes[i] = size;
	dbg_ptr_counts[i] = num_ptrs;
	dbg_ptr_levels[i] = current_level;
	if ((num_ptrs == 3587) || (num_ptrs == 3588))
		{
		i = 0;
		}
	num_ptrs++;
	break;
	}
if (i == max_news) McoErrorAlert(MCO_MAX_MEMORY);
fill_mem(size,ptr,PTR_FILL_I);
}

void debug_freeptr(void *ptr)
{
long i;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_ptr_ptrs) return;
for (i=0; i<max_ptrs; i++) if (dbg_ptr_ptrs[i] == ptr)
	{
	fill_mem(dbg_ptr_sizes[i],ptr,PTR_FILL_D);
	dbg_ptr_ptrs[i] = 0L;
	dbg_ptr_sizes[i] = 0;
	dbg_ptr_counts[i] = 0;
	dbg_ptr_levels[i] = 0;
	break;
	}
if (i == max_ptrs) McoErrorAlert(MCO_INVALID_PTR);
}

void debug_newhand(long size,void *ptr)
{
long i;
Handle h;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_hand_ptrs) return;
if (!dbg_hand_sizes) return;
if (!dbg_hand_counts) return;
if (!dbg_hand_levels) return;
for (i=0; i<max_ptrs; i++) if (!dbg_hand_ptrs[i]) 
	{
	dbg_hand_ptrs[i] = ptr;
	dbg_hand_sizes[i] = size;
	dbg_hand_counts[i] = num_hand;
	dbg_hand_levels[i] = current_level;
	if ((num_hand == 24) || (num_hand == 25))
		{
		i = 0;
		}
	num_hand++;
	break;
	}
if (i == max_hand) McoErrorAlert(MCO_MAX_MEMORY);
h = (Handle)ptr;
HLock((Handle)h);
fill_mem(size,*h,HAND_FILL_I);
HUnlock((Handle)h);
}

void debug_freehand(void *ptr)
{
long i;
Handle h;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_hand_ptrs) return;
for (i=0; i<max_hand; i++) if (dbg_hand_ptrs[i] == ptr)
	{
	h = (Handle)ptr;
	HLock((Handle)h);
	fill_mem(dbg_hand_sizes[i],*h,HAND_FILL_D);
	HUnlock((Handle)h);
	dbg_hand_ptrs[i] = 0L;
	dbg_hand_sizes[i] = 0;
	dbg_hand_counts[i] = 0;
	dbg_hand_levels[i] = 0;
	break;
	}
if (i == max_hand) McoErrorAlert(MCO_INVALID_PTR);
}

void debug_checkhand(void *ptr)
{
long i;

if (current_level < 0) return;

if (!ptr) McoErrorAlert(MCO_INVALID_PTR);
if (!dbg_hand_ptrs) return;
for (i=0; i<max_hand; i++) if (dbg_hand_ptrs[i] == ptr)
	{
	break;
	}
if (i == max_hand) McoErrorAlert(MCO_INVALID_PTR);
}

// check to see if info is still valid
void test_debug(void)
{
int i;

for (i=0; i<max_news; i++) if (dbg_new_ptrs[i])
	{
	if ((i > num_new) || (dbg_new_levels[i] < 0) || (dbg_new_levels[i] > 10000)) McoErrorAlert(MCO_INVALID_PTR);
	}
	
for (i=0; i<max_ptrs; i++) if (dbg_ptr_ptrs[i])
	{
	if ((i > num_ptrs) || (dbg_ptr_levels[i] < 0) || (dbg_ptr_levels[i] > 10000)) McoErrorAlert(MCO_INVALID_PTR);
	}
for (i=0; i<max_hand; i++) if (dbg_hand_ptrs[i])
	{
	if ((i > num_hand) || (dbg_hand_levels[i] < 0) || (dbg_hand_levels[i] > 10000)) McoErrorAlert(MCO_INVALID_PTR);
	}	
	
	
}



void dump_debug(long level)
{
long i;
FILE *fs;

fs = fopen("memory_debug","w");
if (!fs) return;

for (i=0; i<max_news; i++) if ((dbg_new_ptrs[i]) && (dbg_new_levels[i] >= level))
	{
	fprintf(fs,"New Leak at ptr %ld, size %ld, count %ld, level %ld\n",
			(long)dbg_new_ptrs[i],(long)dbg_new_sizes[i],dbg_new_counts[i],dbg_new_levels[i]);
	}
	
for (i=0; i<max_ptrs; i++) if ((dbg_ptr_ptrs[i]) && (dbg_ptr_levels[i] >= level))
	{
	fprintf(fs,"Ptr Leak at ptr %ld, size %ld, count %ld, level %ld\n",
			(long)dbg_ptr_ptrs[i],(long)dbg_ptr_sizes[i],dbg_ptr_counts[i],dbg_ptr_levels[i]);
	}
for (i=0; i<max_hand; i++) if ((dbg_hand_ptrs[i]) && (dbg_hand_levels[i] >= level))
	{
	fprintf(fs,"Hand Leak at ptr %ld, size %ld, count %ld, level %ld\n",
			(long)dbg_hand_ptrs[i],(long)dbg_hand_sizes[i],dbg_hand_counts[i],dbg_hand_levels[i]);
	}	
fclose(fs);	
	
}

void inc_level(void)
{
current_level++;
}

void dec_level(void)
{
current_level--;
}