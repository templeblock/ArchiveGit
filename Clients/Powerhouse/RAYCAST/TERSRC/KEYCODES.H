//			Key Bindings


//	 ALT- or META- keys

#define UP_ARROW_KEY		72
#define DN_ARROW_KEY		80
#define LF_ARROW_KEY		75
#define RT_ARROW_KEY		77

#define CTRL_RT_ARROW_KEY 	116
#define CTRL_LF_ARROW_KEY 	115
#define CTRL_HOME_KEY		119
#define CTRL_END_KEY		117

#define INSERT_KEY			82
#define DELETE_KEY			83
#define HOME_KEY			71
#define END_KEY				79

#define ESC_KEY				27

#define	PG_UP_KEY			73
#define PG_DN_KEY			81

#define F_KEY(x)			(58+x)
#define ALT_F_KEY(x)		(103+x)
#define CTRL_F_KEY(x)		(93+x)
#define ALT_NUM_KEY(x)		(119+x)
#define ALT_QROW_KEY(x) 	(16+x)
#define ALT_AROW_KEY(x) 	(30+x)
#define ALT_ZROW_KEY(x) 	(44+x)

#define	QUIT_KEY			ALT_QROW_KEY(0)	// ALT-Q
#define CLOSE_KEY			ALT_QROW_KEY(1) // ALT-W

#define COPY_KEY        	ALT_QROW_KEY(8) // ALT-O
#define CUT_KEY				ALT_ZROW_KEY(2)	// ALT-C
#define PASTE_KEY			ALT_QROW_KEY(9)	// ALT-P
#define CLEAR_KEY			ALT_AROW_KEY(2)	// ALT-D

extern int AltCodes[];
#define AltCode(c) AltCodes[toupper(c) - 'A']
#define ALT(c) AltCodes[(c)-'A']
