#include "StdAfx.h"
#include "PaperTemplates.h"
#include "AGDoc.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

// New products
#ifdef NOTUSED
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"CD Stomper 98173",	"DVD Jewel case Insert",		0,		0,		1,	2,	0.53125,0.609375,7.1875,4.984375,7.1875,5.515625,0,
"CD Stomper 98173",	"DVD Jewel case Insert",		0,		0,		1,	2,	0.53125,5.59375,7.1875,	0.53125,7.1875,	0.53125,0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		1,	1,	0.493,	0.615,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		1,	1,	0.493,	5.739,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		2,	1,	5.376,	1.313,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		2,	1,	0.494,	5.691,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		1,	2,	5.888,	1.01,	1.606,	1.606,	1.606,	2.25,	0,
"CD Stomper 98102",	"White Matte CD Labels",		0,		0,		1,	2,	1.006,	6.134,	1.606,	1.606,	1.606,	2.25,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		1,	1,	0.493,	0.615,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		1,	1,	0.493,	5.739,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		2,	1,	5.376,	1.313,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		2,	1,	0.494,	5.691,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		1,	2,	5.888,	1.01,	1.606,	1.606,	1.606,	2.25,	0,
"CD Stomper 98110",	"White Glossy CD Labels",		0,		0,		1,	2,	1.006,	6.134,	1.606,	1.606,	1.606,	2.25,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		1,	1,	0.493,	0.615,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		1,	1,	0.493,	5.739,	4.646,	4.646,	4.646,	4.646,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		2,	1,	5.376,	1.313,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		2,	1,	0.494,	5.691,	0.276,	3.996,	2.354,	3.996,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		1,	2,	5.888,	1.01,	1.606,	1.606,	1.606,	2.25,	0,
"CD Stomper 98111",	"Starter Kit CD Labels",		0,		0,		1,	2,	1.006,	6.134,	1.606,	1.606,	1.606,	2.25,	0,
"53284",		"Holiday Greeting Cards",			0,		0,		1,	3,	0.375,	0.25,	7.75,	3.25,	7.75,	3.438,	0,
"8871",			"Ink Jet Business Card",			0,		0,		2,	5,	0.75,	0.5,	3.5,	2.0,	3.5,	2.0,	0,
"8876",			"Ink Jet Business Card - Ivory",	0,		0,		2,	5,	0.75,	0.5,	3.5,	2.0,	3.5,	2.0,	0,
#endif NOTUSED

static const PaperTemplate g_HalfFoldCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"Half-Fold Card",					0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3251",	"Feather Edge Greeting Card",		0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3260",	"Ivory Embossed Half-Fold Card",	0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3265",	"White Half-Fold Card",				0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3269",	"Glossy Photo Quality Half-Fold Card",0,	0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3297",	"Half-Fold Card",					0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3375",	"Confetti Heavyweight Half-Fold Card",0,	0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3378",	"Textured Heavyweight Half-Fold Card",0,	0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Avery 3382",	"Metallic Printing Effect Card",	0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"Fellowes",		"Half-Fold Card",					0,		0,		1,	2,	0,		0,		L2H,	L2W,	L2H,	L2W,	1,
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
};

static const PaperTemplate g_HalfFoldCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"Half-Fold A5 Card",				0,		0,		1,	2,	0,		0,		A5H,	A5W,	A5H,	A5W,	1,
"Avery C2351",	"A5 Greeting Card",					0,		0,		1,	2,	0,		0,		A5H,	A5W,	A5H,	A5W,	1,
"Avery C32289",	"A5 Greeting Card",					0,		0,		1,	2,	0,		0,		A5H,	A5W,	A5H,	A5W,	1,
"Avery C9351",	"Photo Quality Gloss A5 Greeting Card",0,	0,		1,	2,	0,		0,		A5H,	A5W,	A5H,	A5W,	1,
"",/*Generic*/	"A4 Paper",							0,		0,		1,	1,	0,		0,		A4W,	A4H,	A4W,	A4H,	0,
};

static const PaperTemplate g_QuarterFoldCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"Quarter-Fold Card",				0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3259",	"Ivory Embossed Note Card",			0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3266",	"White Quarter-Fold Card",			0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3268",	"White Note Card",					0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3379",	"Textured Heavyweight Note Card",	0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 8315",	"Ink Jet Note Card",				0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 8317",	"Embossed Ink Jet Note Card - Ivory",0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Fellowes",		"Quarter-Fold Card",				0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
};

static const PaperTemplate g_QuarterFoldCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"Quarter-Fold A6 Card",				0,		0,		2,	2,	0,		0,		A6W,	A6H,	A6W,	A6H,	0,
"Avery C2342",	"Embossed cards A6 (inside)",		0,		0,		2,	2,	0,		0,		A6W,	A6H,	A6W,	A6H,	0,
"Avery C2352",	"A6 Greeting Card",					0,		0,		2,	2,	0,		0,		A6W,	A6H,	A6W,	A6H,	0,
"Avery C32287",	"Greeting Cards",					0,		0,		2,	2,	0,		0,		A6W,	A6H,	A6W,	A6H,	0,
"",/*Generic*/	"A4 Paper",							0,		0,		1,	1,	0,		0,		A4W,	A4H,	A4W,	A4H,	0,
};

static const PaperTemplate g_NoteCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"HP",			"Note Cards",						6.0,	8.0,	1,	2,	0,		0,		6.0,	4.0,	6.0,	4.0,	1,
};

static const PaperTemplate g_NoteCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"HP",			"Note Cards",						6.0,	8.0,	1,	2,	0,		0,		6.0,	4.0,	6.0,	4.0,	1,
};

static const PaperTemplate g_BusinessCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 8371",	"Ink Jet Business Cards",			0,		0,		2,	5,	0.75,	0.5,	3.5,	2.0,	3.5,	2.0,	1,
"Avery 5371",	"Laser Business Cards",				0,		0,		2,	5,	0.75,	0.5,	3.5,	2.0,	3.5,	2.0,	1,
"Fellowes",		"Business Cards",					0,		0,		2,	5,	0.75,	0.5,	3.5,	2.0,	3.5,	2.0,	1,
};

static const	PaperTemplate g_AveryPostersUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 2701",	"White Posters",					0,		0,		1,	1,	0.375,	0.375,	L1W,	L1H,	8.5,	11,		0,
"Avery 2715",	"Yellow Posters",					0,		0,		1,	1,	0.375,	0.375,	L1W,	L1H,	8.5,	11,		0,
};

static const PaperTemplate g_AveryPostersEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 2701",	"White Posters",					0,		0,		1,	1,	0.375,	0.375,	8.5,	11,		8.5,	11,		0,
"Avery 2715",	"Yellow Posters",					0,		0,		1,	1,	0.375,	0.375,	8.5,	11,		8.5,	11,		0,
};

static const PaperTemplate g_BusinessCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
//"WHSmith",	"Business Cards",					0,		0,		2,	5,	m(16),	m(10),	m(86.5),m(54),	m(90),	m(55.5),1,
"Avery C32010", "Business Cards",					0,		0,		2,	5,	m(15),	m(13.5),m(85),	m(54),	m(95),	m(54),	1,
"Avery C32011", "Quick & Clean Business Cards laser",0,		0,		2,	5,	m(15),	m(13.5),m(85),	m(54),	m(95),	m(54),	1,
"Avery C32015", "Quick & Clean Business Cards matte",0,		0,		2,	4,	m(17),	m(31.5),m(85),	m(54),	m(91),	m(60),	1,
"Avery C32016", "Quick & Clean Business Cards laser",0,		0,		2,	5,	m(15),	m(13.5),m(85),	m(54),	m(95),	m(54),	1,
"Avery C32028", "Quick & Clean Business Cards glossy",0,	0,		2,	4,	m(17),	m(31.5),m(85),	m(54),	m(91),	m(60),	1,

//"Avery C2354","Business Cards",					0,		0,		2,	4,	m(),	m(),	m(80.4),m(50.8),m(),	m(),	1,
};

static const PaperTemplate g_PostCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 3263",	"White Postcards",					0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3377",	"Confetti Heavyweight Postcards",	0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3380",	"Textured Heavyweight Postcards",	0,		0,		2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 8387",	"Ink Jet Postcards",				0,		0,	 	2,	2,	0,		0,		L4W,	L4H,	L4W,	L4H,	0,
"Avery 3611",	"Kid's Postcards"/*USED*/,			0,		0,		1,	2,	1.25,	1.25,	6.0,	4.0,	6.0,	4.5,	1,
"Avery 5389",	"Index Cards/Postcards"/*USED*/,	0,		0,		1,	2,	1.25,	1.5,	6.0,	4.0,	6.0,	4.0,	1,
"Avery 8389",	"Glossy Photo Quality Postcards"/*USED*/,0,	0,		1,	2,	1.25,	1.25,	6.0,	4.0,	6.0,	4.5,	1,
};

static const PaperTemplate g_PostCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery C32252",	"Postcards",						0,		0,		2,	2,	0,		0,		A6W,	A6H,	A6W,	A6H,	0,
"Avery C2341",	"Embossed cards A5 (cover)",		0,		0,		1,	1,	m(20),	m(168.5),m(170),m(108.5),m(170),m(108.5),1,
"Avery C2341",	"Embossed cards A5 (inside)",		0,		0,		1,	1,	0,		A5W,	A5H,	A5W,	A5H,	A5W,	1,
"Avery C2353",	"Postcards",						0,		0,		1,	2,	m(31.75),m(31.75),m(146.5),m(110.4),m(146.5),m(123),1,
"Avery C9353",	"Photo Quality Gloss Postcards",	0,		0,		1,	2,	m(31.75),m(31.75),m(146.5),m(110.4),m(146.5),m(123),1,
"Avery C2342",	"Embossed cards A6 (cover)",		0,		0,		1,	2,	m(119),	m(14),	m(77),	m(120.5),m(77),	A6H,	0,
"Avery C32286",	"Greeting Cards",					0,		0,		1,	3,	0,		0,		A5H,	m(99.1),A5H,	m(99.1),1,
};

static const PaperTemplate g_EnvelopesUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/ 	"Quarter-Fold Card Envelope",		0,		0,		1,	1,	0,		0,		5.75,	4.375,	5.75,	4.375,	0,
"",/*Generic*/ 	"Half-Fold Card Envelope",			0,		0,		1,	1,	0,		0,		8.875,	5.5,	8.875,	5.5,	0,
"",/*Generic*/ 	"Monarch Envelope", 				0,		0,		1,	1,	0,		0,		7.5,	3.875,	7.5,	3.875,	0,
"",/*Generic*/ 	"Personal Envelope",				0,		0,		1,	1,	0,		0,		6.5,	3.625,	6.5,	3.625,	0,
"",/*Generic*/ 	"#9 Envelope",						0,		0,		1,	1,	0,		0,		8.875,	3.875,	8.875,	3.875,	0,
"",/*Generic*/ 	"#10 Envelope", 					0,		0,		1,	1,	0,		0,		9.5,	4.125,	9.5,	4.125,	0,
"",/*Generic*/ 	"#11 Envelope", 					0,		0,		1,	1,	0,		0,		10.375, 4.5,	10.375, 4.5,	0,
"",/*Generic*/ 	"#12 Envelope", 					0,		0,		1,	1,	0,		0,		11.0,	4.5,	11.0,	4.5,	0,
"",/*Generic*/ 	"#14 Envelope", 					0,		0,		1,	1,	0,		0,		11.5,	5.0,	11.5,	5.0,	0,
"",			 	"Note Card Envelope",				0,		0,		1,	1,	0,		0,		6.25,	4.25,	6.25,	4.25,	0,
"",			 	"4x8 Photo Card Envelope",			0,		0,		1,	1,	0,		0,		8.25,	4.25,	8.25,	4.25,	0,
};

static const PaperTemplate g_EnvelopesEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/ 	"DL Envelope",						0,		0,		1,	1,	0,		0,		DLW,	DLH,	DLW,	DLH,	0,
"",/*Generic*/ 	"C5 Envelope",						0,		0,		1,	1,	0,		0,		C5W,	C5H,	C5W,	C5H,	0,
"",/*Generic*/ 	"C6 Envelope",						0,		0,		1,	1,	0,		0,		C6W,	C6H,	C6W,	C6H,	0,
"",/*Generic*/ 	"C65 Envelope", 					0,		0,		1,	1,	0,		0,		C65W,	C65H,	C65W,	C65H,	0,
};

static const PaperTemplate g_IronOnsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 3271",	"T-Shirt Transfer Sheets",	 		0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 3275",	"Ink Jet T-Shirt Transfer Sheets",	0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 3279",	"Dark T-Shirt Transfer Sheets",		0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 8938",	"Light T-shirt Transfer Sheets",	0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Fellowes",		"Iron-on Transfer Sheets",	 		0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
};

static const PaperTemplate g_IronOnsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"A4 Paper",							0,		0,		1,	1,	0,		0,		A4W,	A4H,	A4W,	A4H,	0,
};

static const PaperTemplate g_FullSheetUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 3276",	"Ink Jet Window Decals",			0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 3383",	"Sticker Project Paper",			0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 8165",	"Ink Jet Full Sheet",				0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 8255",	"Ink Jet Label for Color Printing",	0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 3270",	"Magnet Sheets",					0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Fellowes",		"Magnet Sheets",					0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
};

static const PaperTemplate g_FullSheetEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"A4 Paper",							0,		0,		1,	1,	0,		0,		A4W,	A4H,	A4W,	A4H,	0,
};

static const PaperTemplate g_BrochuresUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
};

static const PaperTemplate g_BrochuresEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"A4 Paper",							0,		0,		1,	1,	0,		0,		A4W,	A4H,	A4W,	A4H,	0,
};

static const PaperTemplate g_TrifoldsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	3,	0,		0,		L1W,	L1H/3,	L1W,	L1H/3,	1,
};

static const PaperTemplate g_TrifoldsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"A4 Paper",							0,		0,		1,	3,	0,		0,		A4W,	A4H/3,	A4W,	A4H/3,	1,
};

static const PaperTemplate g_HolidayCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",				"4x8 Photo Cards",					8.0,	4.0,	1,	1,	0,		0,		8.0,	4.0,	8.5,	4.0,	0,
"",/*Generic*/	"8.5 by 11 inch Paper",				0,		0,		1,	3,	0,		0,		L1W,	L1H/3,	L1W,	L1H/3,	1,
};

static const PaperTemplate g_HolidayCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",				"4x8 Photo Cards",					8.0,	4.0,	1,	1,	0,		0,		8.0,	4.0,	8.5,	4.0,	0,
"",/*Generic*/	"A4 Paper",							0,		0,		1,	3,	0,		0,		A4W,	A4H/3,	A4W,	A4H/3,	1,
};

static const PaperTemplate g_PhotoCardsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",				"Photo Cards",						6.0,	4.0,	1,	1,	0,		0,		6.0,	4.0,	6.0,	4.0,	0,
};

static const PaperTemplate g_PhotoCardsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",				"Photo Cards",						6.0,	4.0,	1,	1,	0,		0,		6.0,	4.0,	6.0,	4.0,	0,
};

static const PaperTemplate g_CDLabelsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
// 4.764" (121mm) sized product
"",/*Generic*/	"CD/DVD Labels",					0,		0,		1,	2,	m(47.62),m(15.87),m(121),m(121),m(121),	m(127),	0,

// 4.9375" (125.42mm) sized product
"HP",			"CD/DVD Labels"/*USED*/,			0,		0,		1,	2,	m(45.24),m(13.5),m(125.42),m(125.42),m(125.42),m(127),0,
"Avery 8692",	"Ink Jet CD/DVD Labels"/*USED*/,	0,		0,		1,	2,	m(45.24),m(13.5),m(125.42),m(125.42),m(125.42),m(127),0, // copy
"Avery 8831",	"Ink Jet CD/DVD Labels"/*USED*/,	0,		0,		1,	2,	m(45.24),m(13.5),m(125.42),m(125.42),m(125.42),m(127),0, // copy

// Original product definitions
//"",/*Generic*/"CD/DVD Labels",					0,		0,		1,	2,	1.875,	.625,	4.75,	4.75,	4.75,	5.00,	0,
//"HP",			"CD/DVD Labels"/*USED*/,			0,		0,		1,	2,	1.78125,.53125,	4.9375,	4.9375,	4.9375,	5.00,	0,
//"Avery 8692",	"Ink Jet CD/DVD Labels",			0,		0,		1,	2,	1.9375,	0.6875,	4.625,	4.625,	4.625,	5.00,	0,
//"Avery 8831",	"Ink Jet CD/DVD Labels",			0,		0,		1,	2,	1.9375,	0.6875,	4.625,	4.625,	4.625,	5.00,	0,
};

static const PaperTemplate g_CDLabelsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
// 4.764" (121mm) sized product
"Avery C9660",	"CD/DVD PQ Label",					0,		0,		1,	2,	m(44.5),m(19.5),m(121),	m(121),	m(121),	m(137),	0,
"Avery J8676",	"CD/DVD Labels",					0,		0,		1,	2,	m(44.5),m(19.5),m(121),	m(121),	m(121),	m(137),	0,
"Avery L7676",	"CD/DVD Labels, B+W",				0,		0,		1,	2,	m(44.5),m(19.5),m(121),	m(121),	m(121),	m(137),	0,
"DECAdry",		"CD/DVD Labels",					0,		0,		1,	2,	m(44.5),m(26.5),m(121),	m(121),	m(121),	m(123),	0,

// 4.9375" (125.42mm) sized product
"HP",			"CD/DVD Labels"/*USED*/,			0,		0,		1,	2,	m(42.3),m(22.3),m(125.42),m(125.42),m(125.42),m(127),0,

// Original product definitions
//"Avery C9660","CD/DVD PQ Label",					0,		0,		1,	2,	m(46.5),m(21.5),m(117),	m(117),	m(117),	m(137),	0,
//"Avery J8676","CD/DVD Labels",					0,		0,		1,	2,	m(46.5),m(21.5),m(117),	m(117),	m(117),	m(137),	0,
//"Avery L7676","CD/DVD Labels, B+W",				0,		0,		1,	2,	m(46.5),m(21.5),m(117),	m(117),	m(117),	m(137),	0,
//"DECAdry",	"CD/DVD Labels",					0,		0,		1,	2,	m(46.5),m(28.5),m(117),	m(117),	m(117),	m(123),	0,
//"HP",			"CD/DVD Labels"/*USED*/,			0,		0,		1,	2,	1.78125,.53125,	4.9375,	4.9375,	4.9375,	5.00,	0,
};

static const PaperTemplate g_CDInsertsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"CD/DVD Jewel Case Insert",			0,		0,		1,	2,	.75,	1.875,	4.75,	4.75,	4.75,	4.75,	1,

// Original product definitions
//"Avery 8831",	"Ink Jet CD/DVD Jewel Case Cover",	0,		0,		1,	1,	1.875,	5.625,	4.75,	4.75,	4.75,	4.75,	0,
//"Avery 8831",	"Ink Jet CD/DVD Jewel Case Tray",	0,		0,		1,	1,	1.5625,	0.625,	5.375,	4.625,	5.375,	4.625,	0,
};

static const PaperTemplate g_CDInsertsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"",/*Generic*/	"CD/DVD Jewel Case Insert",			0,		0,		1,	2,	.75,	1.875,	4.75,	4.75,	4.75,	4.75,	1,

// Original product definitions
//"Avery J8435","CD Case Insert Cover & Tray",		0,		0,		1,	1,	m(59.5),m(24),	m(121),	m(121),	m(121),	m(121),	0,
//"Avery J8435","CD Case Insert Cover & Tray",		0,		0,		1,	1,	m(29.5),m(155),	m(151),	m(118),	m(151),	m(118),	1,
};

static const PaperTemplate g_LabelsUS[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery 8160", 	"Ink Jet Address",					0,		0,		3,	10, 0.188,	0.5,	2.625,	1.0,	2.75,	1.0,	1,
"Avery 8162", 	"Ink Jet Address"/*USED*/,			0,		0,		2,	7,	0.156,	0.833,	4.0,	1.333,	4.188,	1.333,	1,
"Avery 8163", 	"Ink Jet Address - Shipping"/*USED*/,0,		0,		2,	5,	0.156,	0.5,	4.0,	2.0,	4.188,	2.0,	1,
"Avery 8164",	"Ink Jet Address - Shipping"/*USED*/,0,		0,		2,	3,	0.156,	0.5,	4.0,	3.333,	4.188,	3.333,	1,
"Avery 8165",	"Ink Jet Full Sheet"/*USED*/,		0,		0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 8167",	"Ink Jet Return Address"/*USED*/,	0,		0,		4,	20,	0.3,	0.5,	1.75,	0.5,	2.05,	0.5,	1,
"Avery 8250",	"Ink Jet Label for Color Printing",	0,		0,		3,	10,	0.1875,	0.5,	2.625,	1.0,	2.75,	1.0,	1,
"Avery 8253", 	"Ink Jet for Color Printing"/*USED*/,0,		0,		2,	5,	0.156,	0.5,	4.0,	2.0,	4.188,	2.0,	1,
"Avery 8254",	"Ink Jet Label for Color Printing"/*USED*/,0,0,		2,	3,	0.156,	0.5,	4.0,	3.333,	4.188,	3.333,	1,
"Avery 8255",	"Ink Jet Label for Color Printing"/*USED*/,0,0,		1,	1,	0,		0,		L1W,	L1H,	L1W,	L1H,	0,
"Avery 8257",	"Ink Jet Return Address for Color Printing",0,0,	3,	10,	0.375,	0.625,	2.25,	0.75,	2.75,	1.0,	1,
"Avery 8660", 	"Clear Address",					0,		0,		3,	10, 0.188,	0.5,	2.625,	1.0,	2.75,	1.0,	1,
"Avery 8662", 	"Clear Address"/*USED*/,			0,		0,		2,	7,	0.156,	0.833,	4.0,	1.333,	4.188,	1.333,	1,
"Avery 8663", 	"Clear Address - Shipping"/*USED*/,	0,		0,		2,	5,	0.156,	0.5,	4.0,	2.0,	4.188,	2.0,	1,

"Avery L7654-F","Video Face"/*USED*/,				0,		0,		2,	6,	1.084,	0.376,	3.0,	m(46.6),3.1,	m(46.6),1,
"Avery L7654-S","Video Spine"/*USED*/,				0,		0,		1,	16, 1.284,	0.542,	5.7,	0.667,	8.2677,	0.667,	1,
//"Avery 5199", "Video Cassette Face",				0,		0,		2,	5,	1.069,	0.917,	3.063,	m(46.6),3.3,	m(46.6),1,
//"Avery 5199", "Video Cassette Spine", 			0,		0,		1,	15, 1.344,	0.5,	5.813,	0.667,	5.813,	0.667,	1,

"",/*Generic*/	"Round Sticker Sheet"/*USED*/,		0,		0,		3,	3,	0.313,	1.25,	2.5,	2.5,	2.688,	3.0,	0,
"",/*Generic*/	"Square Sticker Sheet"/*USED*/,		0,		0,		3,	3,	0.313,	1.25,	2.5,	2.5,	2.688,	3.0,	0,
"Fellowes", 	"Square Magnet Sheet"/*USED*/,		0,		0,		3,	3,	0.313,	1.25,	2.5,	2.5,	2.688,	3.0,	0,
"Fellowes", 	"Round Magnet Sheet"/*USED*/, 		0,		0,		3,	3,	0.313,	1.25,	2.5,	2.5,	2.688,	3.0,	0,
};

static const PaperTemplate g_LabelsEU[] = {
//Product		Name								CustomW	CustomH	Col	Row	LeftMar	TopMar	Width	Height	Width+	Height+ ReverseOrientation
"Avery J8159", "Address Label",						0,		0,		3,	8,	m(6.45),0.543,	m(64),	m(33.9),2.62,	m(33.9),1,
"Avery J8160", "Address Label",						0,		0,		3,	7,	m(7.2),	0.625,	m(63.5),m(38.1),2.6,	m(38.1),1,
"Avery J8161", "Address Label",						0,		0,		3,	6,	m(7.2),	0.376,	m(63.5),m(46.6),2.6,	m(46.6),1,
"Avery J8162", "Address Label",						0,		0,		2,	8,	m(4.67),0.541,	m(99.1),m(33.9),4.0,	m(33.9),1,
"Avery J8163", "Address Label",						0,		0,		2,	7,	m(4.67),0.625,	m(99.1),m(38.1),4.0,	m(38.1),1,
"Avery J8164", "Address Label",						0,		0,		3,	4,	m(7.2),	0.208,	m(63.5),m(72),	2.6,	m(72),	0,
"Avery J8165", "Parcel Label",						0,		0,		2,	4,	m(4.67),0.542,	m(99.1),m(67.7),4.0,	m(67.7),1,
"Avery J8166", "Parcel Label",						0,		0,		2,	3,	m(4.67),0.375,	m(99.1),m(93.1),4.0,	m(93.1),1,
"Avery J8167", "Shipping Label", 					0,		0,		1,	1,	m(5.2),	m(4.7),	m(199.5),m(289),m(199.5),m(289),0,
"Avery J8168", "Shipping Label", 					0,		0,		1,	2,	m(4.7),	0.224,	m(199.5),m(143.5),m(199.5),m(143.5),1,
"Avery J8169", "Parcel Label",						0,		0,		2,	2,	0.182,	0.402,	m(99.1),m(139),	4.002,	m(139),	0,
"Avery J8173", "Address Label", 					0,		0,		2,	5,	m(4.67),0.265,	m(99.1),m(57),	4.0,	m(57),	1,
"Avery J8766", "3.5'' Diskette - Face Only",		0,		0,		2,	5,	0.917,	0.757,	2.756,	2.047,	3.673,	2.047,	1,
"Avery J8560", "Clear Address Labels",				0,		0,		3,	7,	m(7.2),	0.625,	m(63.5),m(38.1),2.6,	m(38.1),1,
"Avery J8562", "Clear Address Labels",				0,		0,		2,	8,	m(4.67),0.541,	m(99.1),m(33.9),4.0,	m(33.9),1,
"Avery J8563", "Clear Address Labels",				0,		0,		2,	7,	m(4.67),0.625,	m(99.1),m(38.1),4.0,	m(38.1),1,
"Avery J8565", "Clear Parcel Labels", 				0,		0,		2,	4,	m(4.67),0.542,	m(99.1),m(67.7),4.0,	m(67.7),1,
};

static const int g_HalfFoldCardsSizeUS		= sizeof(g_HalfFoldCardsUS)/sizeof(PaperTemplate);
static const int g_HalfFoldCardsSizeEU		= sizeof(g_HalfFoldCardsEU)/sizeof(PaperTemplate);
static const int g_QuarterFoldCardsSizeUS	= sizeof(g_QuarterFoldCardsUS)/sizeof(PaperTemplate);
static const int g_QuarterFoldCardsSizeEU	= sizeof(g_QuarterFoldCardsEU)/sizeof(PaperTemplate);
static const int g_NoteCardsSizeUS			= sizeof(g_NoteCardsUS)/sizeof(PaperTemplate);
static const int g_NoteCardsSizeEU			= sizeof(g_NoteCardsEU)/sizeof(PaperTemplate);
static const int g_BusinessCardsSizeUS		= sizeof(g_BusinessCardsUS)/sizeof(PaperTemplate);
static const int g_BusinessCardsSizeEU		= sizeof(g_BusinessCardsEU)/sizeof(PaperTemplate);
static const int g_PostCardsSizeUS			= sizeof(g_PostCardsUS)/sizeof(PaperTemplate);
static const int g_PostCardsSizeEU			= sizeof(g_PostCardsEU)/sizeof(PaperTemplate);
static const int g_EnvelopesSizeUS			= sizeof(g_EnvelopesUS)/sizeof(PaperTemplate);
static const int g_EnvelopesSizeEU			= sizeof(g_EnvelopesEU)/sizeof(PaperTemplate);
static const int g_IronOnsSizeUS			= sizeof(g_IronOnsUS)/sizeof(PaperTemplate);
static const int g_IronOnsSizeEU			= sizeof(g_IronOnsEU)/sizeof(PaperTemplate);
static const int g_FullSheetSizeUS			= sizeof(g_FullSheetUS)/sizeof(PaperTemplate);
static const int g_FullSheetSizeEU			= sizeof(g_FullSheetEU)/sizeof(PaperTemplate);
static const int g_BrochuresSizeUS			= sizeof(g_BrochuresUS)/sizeof(PaperTemplate);
static const int g_BrochuresSizeEU			= sizeof(g_BrochuresEU)/sizeof(PaperTemplate);
static const int g_TrifoldsSizeUS			= sizeof(g_TrifoldsUS)/sizeof(PaperTemplate);
static const int g_TrifoldsSizeEU			= sizeof(g_TrifoldsEU)/sizeof(PaperTemplate);
static const int g_HolidayCardsSizeUS		= sizeof(g_HolidayCardsUS)/sizeof(PaperTemplate);
static const int g_HolidayCardsSizeEU		= sizeof(g_HolidayCardsEU)/sizeof(PaperTemplate);
static const int g_PhotoCardsSizeUS			= sizeof(g_PhotoCardsUS)/sizeof(PaperTemplate);
static const int g_PhotoCardsSizeEU			= sizeof(g_PhotoCardsEU)/sizeof(PaperTemplate);
static const int g_CDLabelsSizeUS			= sizeof(g_CDLabelsUS)/sizeof(PaperTemplate);
static const int g_CDLabelsSizeEU			= sizeof(g_CDLabelsEU)/sizeof(PaperTemplate);
static const int g_CDInsertsSizeUS			= sizeof(g_CDInsertsUS)/sizeof(PaperTemplate);
static const int g_CDInsertsSizeEU			= sizeof(g_CDInsertsEU)/sizeof(PaperTemplate);
static const int g_LabelsSizeUS				= sizeof(g_LabelsUS)/sizeof(PaperTemplate);
static const int g_LabelsSizeEU				= sizeof(g_LabelsEU)/sizeof(PaperTemplate);
static const int g_AveryPostersSizeUS		= sizeof(g_AveryPostersUS)/sizeof(PaperTemplate);
static const int g_AveryPostersSizeEU		= sizeof(g_AveryPostersEU)/sizeof(PaperTemplate);

//////////////////////////////////////////////////////////////////////
bool CPaperTemplates::SizeMatch(const SIZE& Size, SIZE& DeltaSize, const PaperTemplate* pTemplate)
{
	if (!Size.cx && !Size.cy)
		return true;

	bool bPortrait = (Size.cx <= Size.cy);
	bool bPaperPortrait = (pTemplate->fWidth <= pTemplate->fHeight);

	int iPaperWidth = 0;
	int iPaperHeight = 0;
	if (bPaperPortrait == bPortrait)
	{
		iPaperWidth  = INCHES(pTemplate->fWidth);
		iPaperHeight = INCHES(pTemplate->fHeight);
	}
	else
	{
		iPaperWidth  = INCHES(pTemplate->fHeight);
		iPaperHeight = INCHES(pTemplate->fWidth);
	}

	DeltaSize.cx = iPaperWidth - Size.cx;
	DeltaSize.cy = iPaperHeight - Size.cy;

	// Is it an exact size match?
	return (abs(DeltaSize.cx) <= 2) && (abs(DeltaSize.cy) <= 2);
}

//////////////////////////////////////////////////////////////////////
int CPaperTemplates::FindSizeMatch(int iAGDocType, SIZE& Size)
{
	// Never adjust these document types since they can have arbitrary sizes
	if (iAGDocType == DOC_BANNER)
		return -1;

	const PaperTemplate* pTemplateList = NULL;
	int nTemplates = GetTemplate(iAGDocType, 0/*iTemplate*/, &pTemplateList);
	if (!pTemplateList || !nTemplates)
		return -1;

	SIZE SmallestDeltaSize = {999999999,999999999};
	int iClosest = 0;
	for (int i = 0; i < nTemplates; i++)
	{
		SIZE DeltaSize = {0,0};
		const PaperTemplate* pTemplate = &pTemplateList[i];
		if (SizeMatch(Size, DeltaSize, pTemplate))
			return i;

		if (abs(DeltaSize.cx) + abs(DeltaSize.cy) < abs(SmallestDeltaSize.cx) + abs(SmallestDeltaSize.cy))
		{
			SmallestDeltaSize = DeltaSize;
			iClosest = i;
		}
	}

	// The size has to be adjusted
	Size.cx += SmallestDeltaSize.cx;
	Size.cy += SmallestDeltaSize.cy;
	return iClosest;
}

//////////////////////////////////////////////////////////////////////
bool CPaperTemplates::AdjustSize(int iAGDocType, SIZE& Size)
{
	// Never adjust these document types since they can have arbitrary sizes
	if (iAGDocType == DOC_BANNER || iAGDocType == DOC_AVERYPOSTER)
		return false;

	const PaperTemplate* pTemplateList = NULL;
	int nTemplates = GetTemplate(iAGDocType, 0/*iTemplate*/, &pTemplateList);
	if (!pTemplateList || !nTemplates)
		return false;

	SIZE SmallestDeltaSize = {999999999,999999999};
	for (int i = 0; i < nTemplates; i++)
	{
		SIZE DeltaSize = {0,0};
		const PaperTemplate* pTemplate = &pTemplateList[i];
		if (SizeMatch(Size, DeltaSize, pTemplate))
			return false;

		if (abs(DeltaSize.cx) + abs(DeltaSize.cy) < abs(SmallestDeltaSize.cx) + abs(SmallestDeltaSize.cy))
			SmallestDeltaSize = DeltaSize;
	}

	// The size has to be adjusted
	Size.cx += SmallestDeltaSize.cx;
	Size.cy += SmallestDeltaSize.cy;
	return true;
}

//////////////////////////////////////////////////////////////////////
void CPaperTemplates::StuffCombo(int iAGDocType, int iTemplateSelected, const PaperTemplate** pRetTemplate, HWND hCombo, int iWidth, int iHeight, bool bIgnoreCompetitors)
{
	::SendMessage(hCombo, CB_RESETCONTENT, 0, 0);

	const PaperTemplate* pTemplateList = NULL;
	int nTemplates = GetTemplate(iAGDocType, 0/*iTemplate*/, &pTemplateList);
	if (!pTemplateList || !nTemplates)
		return;

	if (iTemplateSelected < 0 || iTemplateSelected >= nTemplates)
		iTemplateSelected = 0;

	for (int i = 0; i < nTemplates; i++)
	{
		SIZE Size = {iWidth, iHeight};
		SIZE DeltaSize = {0,0};
		const PaperTemplate* pTemplate = &pTemplateList[i];
		if (iAGDocType != DOC_AVERYPOSTER)
		{
			if (!SizeMatch(Size, DeltaSize, pTemplate))
				continue;
		}

		CString strString = pTemplate->pProduct;
		if (!strString.IsEmpty())
			strString += ' ';
		strString += pTemplate->pName;

		if (bIgnoreCompetitors && strString.Find("CD/DVD") >= 0 && strString.Find("Avery") >= 0)
			continue;
			
		int iIndex = ::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)strString);
		::SendMessage(hCombo, CB_SETITEMDATA, iIndex, i);
		if (i == iTemplateSelected)
			::SendMessage(hCombo, CB_SETCURSEL, iIndex, 0);
	}

	int iIndex = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);
	if (iIndex < 0)
	{
		iIndex = 0;
		::SendMessage(hCombo, CB_SETCURSEL, iIndex, 0);
	}

	if (pRetTemplate)
	{
		int iItem = ::SendMessage(hCombo, CB_GETITEMDATA, iIndex, 0);
		if (iItem >= 0 && iItem < nTemplates)
			*pRetTemplate = &pTemplateList[iItem];
	}
}

//////////////////////////////////////////////////////////////////////
int CPaperTemplates::GetTemplate(int iAGDocType, int iTemplate, const PaperTemplate** pRetTemplate)
{
	AGDOCTYPE DocType = (AGDOCTYPE)iAGDocType;

	const PaperTemplate* pTemplate = NULL;
	int nTemplates = 0;
	bool bMetric = CAGDocSheetSize::GetMetric();

	switch (DocType)
	{
		case DOC_BANNER:
		case DOC_FULLSHEET:
		{
			pTemplate = (!bMetric ? &g_FullSheetUS[0] : &g_FullSheetEU[0]);
			nTemplates = (!bMetric ? g_FullSheetSizeUS : g_FullSheetSizeEU);
			break;
		}
		case DOC_BROCHURE:
		{
			pTemplate = (!bMetric ? &g_BrochuresUS[0] : &g_BrochuresEU[0]);
			nTemplates = (!bMetric ? g_BrochuresSizeUS : g_BrochuresSizeEU);
			break;
		}
		case DOC_BUSINESSCARD:
		case DOC_GIFTNAMECARD:
		{
			pTemplate = (!bMetric ? &g_BusinessCardsUS[0] : &g_BusinessCardsEU[0]);
			nTemplates = (!bMetric ? g_BusinessCardsSizeUS : g_BusinessCardsSizeEU);
			break;
		}
		case DOC_CDBOOKLET:
		{
			pTemplate = (!bMetric ? &g_CDInsertsUS[0] : &g_CDInsertsEU[0]);
			nTemplates = (!bMetric ? g_CDInsertsSizeUS : g_CDInsertsSizeEU);
			break;
		}
		case DOC_CDLABEL:
		{
			pTemplate = (!bMetric ? &g_CDLabelsUS[0] : &g_CDLabelsEU[0]);
			nTemplates = (!bMetric ? g_CDLabelsSizeUS : g_CDLabelsSizeEU);
			break;
		}
		case DOC_ENVELOPE:
		{
			pTemplate = (!bMetric ? &g_EnvelopesUS[0] : &g_EnvelopesEU[0]);
			nTemplates = (!bMetric ? g_EnvelopesSizeUS : g_EnvelopesSizeEU);
			break;
		}
		case DOC_HALFCARD:
		{
			pTemplate = (!bMetric ? &g_HalfFoldCardsUS[0] : &g_HalfFoldCardsEU[0]);
			nTemplates = (!bMetric ? g_HalfFoldCardsSizeUS : g_HalfFoldCardsSizeEU);
			break;
		}
		case DOC_HOLIDAYCARD:
		{
			pTemplate = (!bMetric ? &g_HolidayCardsUS[0] : &g_HolidayCardsEU[0]);
			nTemplates = (!bMetric ? g_HolidayCardsSizeUS : g_HolidayCardsSizeEU);
			break;
		}
		case DOC_IRONON:
		{
			pTemplate = (!bMetric ? &g_IronOnsUS[0] : &g_IronOnsEU[0]);
			nTemplates = (!bMetric ? g_IronOnsSizeUS : g_IronOnsSizeEU);
			break;
		}
		case DOC_LABEL:
		{
			pTemplate = (!bMetric ? &g_LabelsUS[0] : &g_LabelsEU[0]);
			nTemplates = (!bMetric ? g_LabelsSizeUS : g_LabelsSizeEU);
			break;
		}
		case DOC_NOTECARD:
		{
			pTemplate = (!bMetric ? &g_NoteCardsUS[0] : &g_NoteCardsEU[0]);
			nTemplates = (!bMetric ? g_NoteCardsSizeUS : g_NoteCardsSizeEU);
			break;
		}
		case DOC_PHOTOCARD:
		{
			pTemplate = (!bMetric ? &g_PhotoCardsUS[0] : &g_PhotoCardsEU[0]);
			nTemplates = (!bMetric ? g_PhotoCardsSizeUS : g_PhotoCardsSizeEU);
			break;
		}
		case DOC_POSTCARD:
		{
			pTemplate = (!bMetric ? &g_PostCardsUS[0] : &g_PostCardsEU[0]);
			nTemplates = (!bMetric ? g_PostCardsSizeUS : g_PostCardsSizeEU);
			break;
		}
		case DOC_QUARTERCARD:
		{
			pTemplate = (!bMetric ? &g_QuarterFoldCardsUS[0] : &g_QuarterFoldCardsEU[0]);
			nTemplates = (!bMetric ? g_QuarterFoldCardsSizeUS : g_QuarterFoldCardsSizeEU);
			break;
		}
		case DOC_TRIFOLD:
		{
			pTemplate = (!bMetric ? &g_TrifoldsUS[0] : &g_TrifoldsEU[0]);
			nTemplates = (!bMetric ? g_TrifoldsSizeUS : g_TrifoldsSizeEU);
			break;
		}
		case DOC_AVERYPOSTER:
		{
			pTemplate = (!bMetric ? &g_AveryPostersUS[0] : &g_AveryPostersEU[0]);
			nTemplates = (!bMetric ? g_AveryPostersSizeUS : g_AveryPostersSizeEU);
			break;
		}
	}

	if (iTemplate < 0 || iTemplate >= nTemplates)
		iTemplate = 0;

	if (pRetTemplate)
		*pRetTemplate = (pTemplate + iTemplate);

	return nTemplates;
}
