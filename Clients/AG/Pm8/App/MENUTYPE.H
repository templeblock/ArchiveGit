/*
// Menu type definitions
//
// A menu is a "user-defined" resource of type BARMENU.
// Each entry in the menu has the following fields (all must be present):
//
//	Type ID  Style"Text\0"
//
// "Type" is one of the menu entry types defined below.
// "ID" is the identifier for the entry (whatever number is meaningful).
// "Style" is some combination of the Windows style flags. The style value must
// be 32 bits. If you use the standard Windows style flags, they're LONG by
// default. Otherwise, put an 'L' on it (like 0L, not 0).
// "Text" is the text to show in the menu for the entry.
//		THE '\0' MUST BE EXPLICITLY PUT IN.
//
// The menu must be terminated with a single value of type BARMENU_END. Only
// the type field is necessary, and any other fields will be ignored.
*/

/*
// Our user type for menus.
// It just needs to be some number > 255
*/

#define RT_BARMENU		1000
#define BARMENU			RT_BARMENU LOADONCALL DISCARDABLE

/*
// Menu entry types.
*/

#define BM_END				-1
#define BM_BUTTON			0
#define BM_EDIT			1
#define BM_STATIC			2
#define BM_LISTBOX		3
#define BM_SCROLLBAR		4
#define BM_COMBOBOX		5
