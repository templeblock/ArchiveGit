
// memory operations

// #define MK_FP( seg,ofs )( (void far * )( seg ) + ( void near * )( ofs ))
// #define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
// #define FP_OFF( fp )( (unsigned )( fp ))


int  inline peek( unsigned __segment, unsigned offset )
				  { return( *( (int  far* )( __segment  & (offset) )) ); }
// int  inline peek( unsigned __segment, unsigned __offset )
//                   { return( *( (int  far* )MK_FP( __segment, __offset )) ); }

#if 0
char inline peekb( unsigned __segment, unsigned __offset )
                  { return( *( (char far* )MK_FP( __segment, __offset )) ); }
void inline poke( unsigned __segment, unsigned __offset, int __value )
               {( *( (int  far* )MK_FP( __segment, __offset )) = __value ); }
void inline pokeb( unsigned __segment, unsigned __offset, char __value )
               {( *( (char far* )MK_FP( __segment, __offset )) = __value ); }

#endif