const long FIX = 8;
const long ARC = 3600;

extern long cs[ARC], sn[ARC];

//
// Interp[] is 512x256 table
// the result of
//
//		x * i;
//		x : [-255...255], i : [1...255]
//
// is stored in [x][i*256]
//
extern short *Interp;

extern void BuildTrigTables();
extern void BuildInterpolationTable();


