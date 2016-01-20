//////////////////////////////////////////////////////////////////////////
// 
//   condata.h
//   structure for passing data to generate a profile
//
//////////////////////////////////////////////////////////////////////////

#ifndef IN_CONDATA
#define IN_CONDATA

typedef struct ConData{
	short 	tabletype;			// 1 = photoshp, 2 = ICC, 3 = CRD, 4 = RGB, 5 = ICC CMYK, 6 = ICC TEK
	short 	separation_type;	//GCR == 0, UCR == 1
	short 	black_type; 		//None==1,Light==2,Medium==3,Heavy==4 or Maximum==5
	long 	total_ink_limit;	// from 0 to 400
	long 	black_ink_limit;	// from 0 to 100
	long	saturation;			// from 0 to 100, 100 is max saturation
	long	col_balance;		// 0 = neutral, 100 = paper color
	long	col_bal_cut; 		// defines where to cuttoff the color balnce function, 0== cuttoff at L = 0, 100 = cuttoff at L = 95
	long	L_linear;			// 0 = linear, 100 = compressed
	short	ink_linerize;		// lineraize ink L
	short	ink_neutralize; 	// neutralize ink sim for paper color
	short	ink_highlight;		// lock the highlight point (L goes from minL to 100)
	short	simulate;			//simulate or not, if == 1, yes
	short	smooth;				//amount of smoothing to apply
	short	rev_table_size;		//reverse table size, large(1) = 17x17x17x17, small(0) = 9x9x9x9
	short	smooth_sim;			//amount of smoothing to apply to simulation
	short	K_only;				// If 1 the set 0,0,0,100 in CMYK table to 0,0,0,100
} ConData;	


#endif