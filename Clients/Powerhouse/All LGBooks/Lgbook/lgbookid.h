#ifndef _LGBOOKID_H_
#define _LGBOOKID_H_

// Scene Table id
#define IDR_SCENETABLE	1000

// Scene Type constants
#define ST_NORMAL			0
#define ST_NOREADTOME		1
#define ST_NOREAD_NOPREV	3
#define ST_NOPREVIOUS		2
#define ST_NOPLAY			4

// Accelerator table id
#define IDR_ACCEL			1

// Common Dialog id's (100-119)
#define IDD_MAINMENU		100   
#define IDD_SPANMAINMENU	300 
#define IDD_OPTIONS			101		// from story
#define IDD_OPTIONS2		102		// from main menu
#define IDD_SPANOPTIONS		301		// from story
#define IDD_SPANOPTIONS2	302		// from main menu
#define IDD_ACTIVITY		103		// from story

#define IDD_SPANACTIVITY	303		// from story

#define IDD_SOUNDBOX 		105		// from story
#define IDD_SPANSOUNDBOX 	305		// from story

#define IDD_COLORBOOK		107
#define IDD_SPANCOLORBOOK	307
#define IDD_CREDITS			108
#define IDD_LANGUAGE		109
#define IDD_SPANLANGUAGE	309
#define IDD_DEMO			110
#define IDD_SPANDEMO		310
#define IDD_BELONGS			111
#define IDD_MUSIC			112
#define IDD_SOUNDS			113
#define IDD_SINGALONG		114
#define IDD_SPANMUSIC		312
#define IDD_SPANSOUNDS		313
#define IDD_SPANSINGALONG	314
#define IDD_THEATRE1		115
#define IDD_THEATRE2		116
#define IDD_THEATRE3		117
#define IDD_EXITSTORY		118
#define IDD_SPANEXITSTORY	318
#define IDD_POWERSCREEN		119
#define IDD_POKEYSCREEN		120
#define IDD_DEMO1			121
#define IDD_SPANDEMO1		321
#define IDD_DEMO2			123
#define IDD_SPANDEMO2		323
#define IDD_DEMO3			124
#define IDD_SPANDEMO3		324
#define IDD_DEMO4			125
#define IDD_SPANDEMO4		325
#define IDD_DEMO5			126
#define IDD_SPANDEMO5		326
#define IDD_DEMO6			127
#define IDD_SPANDEMO6		327
#define IDD_DEMO7			128
#define IDD_SPANDEMO7		328

// Title Specific Dialog id's (130-199)

// Spanish offset - requires all dialogs to be in range (1-199)
#define IDD_SPANISHOFFSET	200

// Cursor id's (201-150)
#define ID_POINTER		201
#define ID_NULLCURSOR	202

// Title Specific Cursor id's (251-300)

// Strings id's (301-400)
#define IDS_SECTION		301
#define IDS_CDNAME		302

// Title Specific String id's (401-500)

// Control id's (501-700)
#define IDC_TOON		501
#define IDC_NEXT		502
#define IDC_PREVIOUS	503
#define IDC_READ		504
#define IDC_QUIT		505
#define IDC_OPTIONS		506
#define IDC_ACTIVITIES	507
#define IDC_MAINMENU	508
#define IDC_BACK		509
#define IDC_EDIT		510
#define IDC_GOTOSCENE	511
#define IDC_LEVEL1		512
#define IDC_LEVEL2		513
#define IDC_LEVEL3		514
#define IDC_SCENEUP		515
#define IDC_SCENEDOWN	516
#define IDC_LANGUAGE	517
#define IDC_CREDITS		518
#define IDC_BOOKMARK	519
#define IDC_DEMO		520
#define IDC_SKIPNEXT	521
#define IDC_OPTIONS2	522
#define IDC_SOUNDBOX	523
#define IDC_SOUNDBOX2	524
#define IDC_PREVPANEL	525
#define IDC_NEXTPANEL	526
#define IDC_PRINT		527
#define IDC_PLAYBELONGS 528
#define IDC_HELP		529

// Activity id's
#define IDC_ACTIVITY1	531
#define IDC_ACTIVITY2	532
#define IDC_ACTIVITY3	533
#define IDC_ACTIVITY4	534
#define IDC_ACTIVITY5	535
#define IDC_ACTIVITY6	536
#define IDC_ACTIVITY7	537
#define IDC_ACTIVITY8	538
#define IDC_ACTIVITY9	539
#define IDC_ACTIVITY10	540

#define IDC_LANGUAGE1	551
#define IDC_LANGUAGE2	552
#define IDC_LANGUAGE3	553
#define IDC_LANGUAGE4	554
#define IDC_LANGUAGE5	555

#define IDC_MUSIC		560
#define IDC_SOUNDS		561
#define IDC_SINGALONG	562
#define IDC_THEATRE1	563
#define IDC_THEATRE2	564
#define IDC_THEATRE3	565

#define IDC_EXITSTORY	570

#define IDC_DEMO1		580
#define IDC_DEMO2		581
#define IDC_DEMO3		582
#define IDC_DEMO4		583
#define IDC_DEMO5		584
#define IDC_DEMO6		585
#define IDC_DEMO7		586

#define IDC_SCENE1		601
#define IDC_SCENE2		602
#define IDC_SCENE3		603
#define IDC_SCENE4		604
#define IDC_SCENE5		605
#define IDC_SCENE6		606
#define IDC_SCENE7		607
#define IDC_SCENE8		608
#define IDC_SCENE9		609
#define IDC_SCENE10		610
#define IDC_SCENE11		611
#define IDC_SCENE12		612
#define IDC_SCENE13		613
#define IDC_SCENE14		614
#define IDC_SCENE15		615
#define IDC_SCENE16		616
#define IDC_SCENE17		617
#define IDC_SCENE18		618
#define IDC_SCENE19		619
#define IDC_SCENE20		620
#define IDC_SCENE21		621
#define IDC_SCENE22		622
#define IDC_SCENE23		623
#define IDC_SCENE24		624
#define IDC_SCENE25		625
#define IDC_SCENE26		626
#define IDC_SCENE27		627
#define IDC_SCENE28		628
#define IDC_SCENE29		629
#define IDC_SCENE30		630
#define IDC_SCENE31		631
#define IDC_SCENE32		632
#define IDC_SCENE33		633
#define IDC_SCENE34		634
#define IDC_SCENE35		635

#define IDC_SELECTSCENE1		651
#define IDC_SELECTSCENE2		652
#define IDC_SELECTSCENE3		653
#define IDC_SELECTSCENE4		654
#define IDC_SELECTSCENE5		655
#define IDC_SELECTSCENE6		656
#define IDC_SELECTSCENE7		657
#define IDC_SELECTSCENE8		658
#define IDC_SELECTSCENE9		659
#define IDC_SELECTSCENE10		660
#define IDC_SELECTSCENE11		661
#define IDC_SELECTSCENE12		662
#define IDC_SELECTSCENE13		663
#define IDC_SELECTSCENE14		664
#define IDC_SELECTSCENE15		665
#define IDC_SELECTSCENE16		666
#define IDC_SELECTSCENE17		667
#define IDC_SELECTSCENE18		668
#define IDC_SELECTSCENE19		669
#define IDC_SELECTSCENE20		670
#define IDC_SELECTSCENE21		671
#define IDC_SELECTSCENE22		672
#define IDC_SELECTSCENE23		673
#define IDC_SELECTSCENE24		674
#define IDC_SELECTSCENE25		675
#define IDC_SELECTSCENE26		676
#define IDC_SELECTSCENE27		677
#define IDC_SELECTSCENE28		678
#define IDC_SELECTSCENE29		679
#define IDC_SELECTSCENE30		680
#define IDC_SELECTSCENE31		681
#define IDC_SELECTSCENE32		682
#define IDC_SELECTSCENE33		683
#define IDC_SELECTSCENE34		684
#define IDC_SELECTSCENE35		685


// Title Specific Control id's (701-800)

#endif // _LGBOOKID_H_
