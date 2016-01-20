	title	EMS Programmer's Toolkit

.radix 16
.model medium
.data
	even
emsname	db	'EMMXXXX0'
.code
	even
;************************************************************************
;*	unsigned char far ems_installed()				*
;*				/* Determine if EMS installed */	*
;*									*
;*		input(s) & output(s):	None				*
;*									*
;*		return code:	1 = EMS Driver Installed		*
;*				0 = No EMS Driver Installed		*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_installed

_ems_installed proc far

	push	bp		;C lang entry sequence
	mov	bp,sp
	push	di
	push	si
	push	ds
	mov	ax,3567		;Get EMS Interrupt Handler Vector
	int	21		;Call MS-DOS
	mov	di,0A		;ES:DI (000A) points to name of driver
;	mov	ax,cs
;	mov	ds,ax
	lea	si,emsname	;DS:SI points to expected EMS driver name
	mov	cx,8		;Length of expected EMS driver name
	mov	ax,1		;Assume Good return code
	cld			;Foward search
	rep	cmpsb		;Perform actual search
	je	emsinstalledend	;Found expected value
	mov	al,0		;Error return code
emsinstalledend:
	pop	ds
	pop	si
	pop	di
	pop	bp
	ret

_ems_installed endp
	page

;************************************************************************
;*	unsigned char far ems_gstatus()	/* Get Current EMS Status */	*
;*									*
;*		input(s) & output(s):	None				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_gstatus

_ems_gstatus proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ah,40		;"Get Status" function code
	int	67		;Call EMS Interrupt Handler
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_gstatus endp


;************************************************************************
;*	unsigned char far ems_version(version)	/* Get EMS Version */	*
;*		unsigned char *version;					*
;*									*
;*		input(s):	None					*
;*									*
;*		output(s):	BCD form of EMS version number		*
;*				(8-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_version

_ems_version proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ah,46		;"Get Version" function code
	int	67		;Call EMS Interrupt Handler
	les	bx,[bp+6]	;Load ES:BX with pointer to "version"
	mov	es:[bx],al	;Move version number from AL to "version"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_version endp
	page

;************************************************************************
;*	unsigned char fa ems_pfba(pfba)				*
;*				/* Get EMS Page Frame Base Address */	*
;*		unsigned char **pfba;					*
;*									*
;*		input(s):	None					*
;*									*
;*		output(s):	segment:offset of EMS Page Frame Base	*
;*				(segment:offset 32 bit pair)		*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_pfba

_ems_pfba proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ah,41		;"Get Page Frame Address" function code
	int	67		;Call EMS Interrupt Handler
	mov	dx,bx		;Save segment #
	les	bx,[bp+6]	;Load ES:BX with pointer to "pfba"
	mov	es:[bx+2],dx	;Move segment # to "pfba"
	sub	dx,dx
	mov	es:[bx],dx	;Move offset # to "pfba"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_pfba endp
	page

;************************************************************************
;*	unsigned char far ems_gpc(tpc,upc)				*
;*			/* Get EMS Total & Unallocated Page Counts */	*
;*		unsigned short far *tpc, *upc;				*
;*									*
;*		input(s):	None					*
;*									*
;*		output(s):	Counts of total & unallocated pages	*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_gpc

_ems_gpc proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ah,42		;"Get Unallocated Page Count" function code
	int	67		;Call EMS Interrupt Handler
	push	bx
	les	bx,[bp+6]	;Load ES:BX with pointer to "tpc"
	mov	es:[bx],dx	;Move total page count to "tpc"
	les	bx,[bp+10d]	;Load ES:BX with pointer to "upc"
	pop	es:[bx]		;Move unallocated page count to "upc"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_gpc endp
	page

;************************************************************************
;*	unsigned char far ems_allocate(pages,handle)			*
;*					/* Allocate EMS Pages */	*
;*		unsigned short pages;					*
;*		unsigned short far *handle;				*
;*									*
;*		input(s):	Count of pages to be allocated		*
;*				(16-bit unsigned)			*
;*									*
;*		output(s):	EMS handle				*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_allocate

_ems_allocate proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	bx,[bp+6]	;Get calling program's page count
	mov	ah,43		;"Allocate Pages" function code
	int	67		;Call EMS Interrupt Handler
	les	bx,[bp+8]	;Load ES:BX with pointer to "handle"
	mov	es:[bx],dx	;Move EMS handle to "handle"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_allocate endp
	page

;************************************************************************
;*	unsigned char far ems_deallocate(handle)			*
;*					/* DeAllocate EMS Pages */	*
;*		unsigned short handle;					*
;*									*
;*		input(s):	Number of EMS handle to be deallocated	*
;*				(16-bit unsigned)			*
;*									*
;*		output(s):	- none -				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_deallocate

_ems_deallocate proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	dx,[bp+6]	;Get calling program's EMS handle
	mov	ah,45		;"Deallocate Pages" function code
	int	67		;Call EMS Interrupt Handler
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_deallocate endp
	page

;************************************************************************
;*	unsigned char far ems_map(handle,logical_page,physical_page)	*
;*						/* Map EMS Pages */	*
;*		unsigned short handle, logical_page, physical_page;	*
;*									*
;*		input(s):	Number of EMS handle to be mapped	*
;*				(16-bit unsigned)			*
;*				Logical page within handle to map	*
;*				(16-bit unsigned in range of		*
;*					  0 to (alloc_amt - 1))		*
;*				Physical page to map to			*
;*				(16-bit unsigned in range of 0 - 3)	*
;*									*
;*		output(s):	- none -				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_map

_ems_map proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	dx,[bp+6]	;Get calling program's EMS handle
	mov	bx,[bp+8]	;Get logical page number
	mov	al,[bp+10d]	;Get low byte of physical page number
	mov	ah,44		;"Map Pages" function code
	int	67		;Call EMS Interrupt Handler
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_map endp
	page

;************************************************************************
;*	unsigned char far ems_gahc(active_handle_count)			*
;*				/* Get Count of Active EMS Handles */	*
;*		unsigned short *active_handle_count;			*
;*									*
;*		input(s):	- none -				*
;*									*
;*		output(s):	Count of active EMS handles		*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_gahc

_ems_gahc proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ah,4bh		;"Get Active Handle Count" function code
	int	67		;Call EMS Interrupt Handler
	mov	dx,bx		;Save value of active handle count
	les	bx,[bp+6]	;Load ES:BX with pointer to "active_handle_ct"
	mov	es:[bx],dx	;Move active handle count to "active_handle_ct"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_gahc endp
	page

;************************************************************************
;*	unsigned char far ems_ghpc(handle,pages)			*
;*			/* Get Page Count of Active EMS Handle */	*
;*		unsigned short handle;					*
;*		unsigned short far *pages;				*
;*									*
;*		input(s):	EMS handle				*
;*				(16-bit unsigned)			*
;*									*
;*		output(s):	Count of pages allocated to handle	*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_ghpc

_ems_ghpc proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	dx,[bp+6]	;Get calling program's EMS handle
	mov	ah,4c		;"Get Page Count for Handle" function code
	int	67		;Call EMS Interrupt Handler
	mov	dx,bx		;Save value of page count
	les	bx,[bp+8]	;Load ES:BX with pointer to "pages"
	mov	es:[bx],dx	;Move page count to "pages"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_ghpc endp
	page

;************************************************************************
;*	unsigned char far ems_gahpc(handles,handle_page_count_array)	*
;*			/* Get Page Count of All Active EMS Handles */	*
;*		unsigned short far *handle_page_count_array;		*
;*		unsigned short far *handles;				*
;*									*
;*		input(s):	- none -				*
;*									*
;*		output(s):	Array of handle / page count pairs	*
;*				(16-bit unsigned, 16-bit unsigned)	*
;*				Count of active EMS handles		*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_gahpc

_ems_gahpc proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	push	di		;Save DI
	les	di,[bp+10d]	;Load ES:DI with pointer to "handle_page_count_array"
	mov	ah,4dh		;"Get Page Count for All Handles" function code
	int	67		;Call EMS Interrupt Handler
	mov	dx,bx		;Save value of handle count
	les	bx,[bp+6]	;Load ES:BX with pointer to "handles"
	mov	es:[bx],dx	;Move active handle count to "handles"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	di		;Restore DI
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_gahpc endp
	page

;************************************************************************
;*	unsigned far char ems_size_pmap(pmap_size)			*
;*					/* Get Size of EMS Page Map */	*
;*		unsigned short far *pmap_size;				*
;*									*
;*		input(s):	- none -				*
;*									*
;*		output(s):	Size of EMS page map			*
;*				(16-bit unsigned)			*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_size_pmap

_ems_size_pmap proc far

	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	ax,4e03		;"Get/Set Page Map" / "Get Size" function code
	int	67		;Call EMS Interrupt Handler
	les	bx,[bp+6]	;Load ES:BX with pointer to "pmap_size"
	mov	dl,al		;Put page map size into work register
	sub	dh,dh
	mov	es:[bx],dx	;Move EMS page map size to "pmap_size"
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_size_pmap endp

	page
;************************************************************************
;*	unsigned char far ems_get_pmap(page_map)/* Get EMS Page Map */	*
;*		unsigned char far *page_map;				*
;*									*
;*		input(s):	- none -				*
;*									*
;*		output(s):	EMS page map				*
;*				(array of 8-bit unsigned values)	*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_get_pmap

_ems_get_pmap proc far

	mov	ax,4e00		;"Get Page Map" function code
	jmp	short getset

_ems_get_pmap endp


;************************************************************************
;*	unsigned char far ems_getset_pmap(get_page_map,set_page_map)	*
;*					/* Get & Set EMS Page Map */	*
;*		unsigned char far *get_page_map, *set_page_map;		*
;*									*
;*		input(s):	EMS page map				*
;*				(array of 8-bit unsigned values)	*
;*									*
;*		output(s):	EMS page map				*
;*				(array of 8-bit unsigned values)	*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_getset_pmap

_ems_getset_pmap proc far

	mov	ax,4e02		;"Get-&-Set Page Map" function code
getset:	push	bp		;C lang entry sequence
	mov	bp,sp
	push	di
	push	si
	push	ds
	les	di,[bp+6]	;Load ES:DI with pointer to "get_page_map"
	lds	si,[bp+10d]	;Load DS:SI with pointer to "set_page_map"
	int	67		;Call EMS Interrupt Handler
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	ds
	pop	si
	pop	di
	pop	bp
	ret

_ems_getset_pmap endp


;************************************************************************
;*	unsigned char far ems_set_pmap(page_map)/* Set EMS Page Map */	*
;*		unsigned char far *page_map;				*
;*									*
;*		input(s):	EMS page map				*
;*				(array of 8-bit unsigned values)	*
;*									*
;*		output(s):	- none -				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_set_pmap

_ems_set_pmap proc far

	mov	ax,4e01		;"Set Page Map" function code
	jmp	short getset

_ems_set_pmap endp
	page

;************************************************************************
;*	unsigned char far ems_save_page_map(handle)			*
;*					/* Save EMS Page Map */		*
;*		unsigned short handle;					*
;*									*
;*		input(s):	Number of EMS handle of program		*
;*				(16-bit unsigned)			*
;*									*
;*		output(s):	- none -				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_save_page_map

_ems_save_page_map proc far

	mov	ah,47d		;"EMS Save Page Map" function code
savres:	push	bp		;Entry sequence - save old BP
	mov	bp,sp		;Set stack framepointer
	mov	dx,[bp+6]	;Get calling program's EMS handle
	int	67		;Call EMS Interrupt Handler
	mov	al,ah		;Put return code in low-order byte of AX
	sub	ah,ah		;Official wasteful cleanup!
	pop	bp		;Exit sequence - restore old BP
	ret

_ems_save_page_map endp


;************************************************************************
;*	unsigned char far ems_restore_page_map(handle)			*
;*					/* Restore EMS Page Map */	*
;*		unsigned short handle;					*
;*									*
;*		input(s):	Number of EMS handle of program		*
;*				(16-bit unsigned)			*
;*									*
;*		output(s):	- none -				*
;*									*
;*		return code:	EMS 3.2 Current Status Return Code	*
;*				(8-bit unsigned)			*
;************************************************************************

	public	_ems_restore_page_map

_ems_restore_page_map proc far

	mov	ah,48d		;"EMS Restore Page Map" function code
	jmp	short savres

_ems_restore_page_map endp

end
