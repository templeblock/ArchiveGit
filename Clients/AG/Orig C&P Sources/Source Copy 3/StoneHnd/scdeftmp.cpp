#define MANUAL_INST
#define ANSI_CLASS_INST

#ifdef MANUAL_INST

#define DEFINE_TEMPLATES
#include "scparagr.h"
#include "scspcrec.h"
#include "scpubobj.h"
#undef DEFINE_TEMPLATE

#ifdef _WINDOWS
#pragma warning ( disable : 4660 )	 // duplicate template definitions
#endif

#ifdef ANSI_CLASS_INST
	template class scSizeableArray< char* >;
	template class scSizeableArrayD< stPara >;
	template class scSizeableArrayD< scKeyRecord >;
	template class scSizeableArray< UCS2 >;
	template class scSizeableArrayD< scSpecLocation >;
	template class scSizeableArrayD< scSpecRecord >;
	template class scSizeableArrayD< RefCountPtr< stSpec > >;
#endif // ANSI_CLASS_INST

#endif // MANUAL_INST


