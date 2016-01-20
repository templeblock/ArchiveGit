#ifndef PATCH_LAYOUT_H
#define PATCH_LAYOUT_H


typedef struct PatchLayout{
	// 1 = strips, 0 = rectangle
	int		strips;
	// number of strips/sheets
	int		sheets;
	// number or rows
	int		rows;
	// number of columns
	int   	columns;
	// starting position
	//    0			1
	//
	//	  2			3
	int		starting;
	// direction  0 = horz, 1 = vert
	int 	direction;
	//width in mm
	double	width;
	//height in mm
	double height;
}PatchLayout;

#endif //PATCH_LAYOUT_H