/* debuging code for new and delete */

#ifndef IN_MEM_DBG
#define IN_MEM_DBG

void setup_debug(long MAX_NEWS,long MAX_PTRS,long MAX_HANDS);
void clear_debug(void);
void debug_new(unsigned long size,void *ptr);
void debug_delete(void *ptr);
void debug_newptr(long size,void *ptr);
void debug_freeptr(void *ptr);
void debug_newhand(long size,void *ptr);
void debug_freehand(void *ptr);
void debug_checkhand(void *ptr);
void test_debug(void);
void dump_debug(long level);
void inc_level(void);
void dec_level(void);

#endif