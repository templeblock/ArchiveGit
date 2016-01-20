/*
// WPG.CPP
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/WPG.CPP 1     3/03/99 6:14p Gbeddow $
//
// $Log: /PM8/App/WPG.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 4     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     12/08/97 2:52p Jayn
// Text, arcs, pie charts, improved transform, etc.
// 
// 2     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
//    Rev 1.0   14 Aug 1997 15:27:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:10   Fred
// Initial revision.
// 
//    Rev 1.1   25 Aug 1996 12:59:38   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:44:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:44   FRED
// Initial revision.
// 
//    Rev 2.2   07 Jul 1995 16:10:42   JAY
// Misc changes.
// 
//    Rev 2.1   05 Jun 1995 11:30:52   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:18   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:34:12   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:33:14   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   14 Jun 1994 16:47:50   JAY
// WPG 2.0 handling is now commented out. It didn't really work anyway.
// 
//    Rev 1.1   05 Jan 1994 08:06:52   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:22   JAY
// 
*/

#include "stdafx.h"

#include	<string.h>
#include	<math.h>

#include "pmgobj.h"
#include "grafrec.h"
#include "grafobj.h"
#include "wpg.h"
#include "utils.h"

/*
// Read the WPG helper info.
*/

ERRORCODE WPGHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the WPG helper info.
*/

ERRORCODE WPGHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the WPG helper info.
*/

ST_DEV_IO_SIZE WPGHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Extract a color from a color table.
*/

OUTLINE_COLOR WPGUpdateState::rgb_from_color_index(BYTE index)
{
	OUTLINE_COLOR color;

	color.red = palette[index][0];
	color.green = palette[index][1];
	color.blue = palette[index][2];

	return color;
}

/*
// Do nothing.
*/

BOOL WLOCAL WPGUpdateState::WPG_null_processor(BYTE *record_type, LONG length)
{
	return TRUE;
}

BOOL WLOCAL WPGUpdateState::WPG_process_end(BYTE *record_type, LONG length)
{
/* Do not continue. */
	return FALSE;
}

/*
// Process a version 1 Start record.
*/

BOOL WLOCAL WPGUpdateState::WPG_process_1_start(BYTE *record_type, LONG length)
{
	WPG_1_START record;

/* Read the record. */

	if (file.read((UCHAR *)&record, sizeof(record)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Plug in the header info. */

	width = record.width;
	height = record.height;
	x_resolution =
		y_resolution = WPU;			/* Always */

	if (painting)
	{
		OUTLINE_POINT p0, p1;

	/* Tell the outline rasterizer how big our source extent is. */

		p0.x = 0;
 		p0.y = 0;
		p1.x = width;
		p1.y = height;
		set_source_extent(p0, p1);

		reverse_y_transform = !reverse_y_transform;
	}

	return TRUE;
}

/*
// Process a version 1 fill attributes record.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_fill_attributes(BYTE *record_type, LONG length)
{
	struct
	{
		BYTE pattern;
		BYTE color;
	} p;

	if (file.read((UCHAR *)&p, sizeof(p)) != ERRORCODE_None)
	{
		return FALSE;
	}

	switch (p.pattern)
	{
		case 0:
		{
		/* Hollow. */
			set_interior_style(0);
			break;
		}
		case 1:
		{
		/* Solid. */
			set_interior_style(1);
			break;
		}
		default:
		{
			set_interior_style(1);				/* Solid for now. */
			break;
		}
	}

/* Set the color. */

	set_fill_color(rgb_from_color_index(p.color));

	return TRUE;
}

/*
// Process a version 1 line attributes record.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_line_attributes(BYTE *record_type, LONG length)
{
	struct
	{
		BYTE style;
		BYTE color;
		WORD width;
	} p;
	OUTLINE_COLOR color;

	if (file.read(&p, sizeof(p)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Set the styles. */

	if (p.style == 0)
	{
		set_edge_visibility(0);
	}
	else
	{
		set_edge_visibility(1);
	}

/* Set the colors. */

	color = rgb_from_color_index(p.color);
	set_line_color(color);
	set_edge_color(color);

/* Set the widths. */

	set_line_width(p.width);
	set_edge_width(p.width);

	return TRUE;
}

/*
// Process a version 1 marker attributes record.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_marker_attributes(BYTE *record_type, LONG length)
{
	struct
	{
		BYTE style;
		BYTE color;
		WORD width;
	} p;

	static MARKER_TYPE BASED_CODE wpg_marker_translate[] =
	{
		MARKER_RESERVED,				/* No markers. */
		MARKER_DOT,						/* . */
		MARKER_PLUS,					/* + */
		MARKER_ASTERISK,				/* * */
		MARKER_CIRCLE,					/* circle */
		MARKER_RESERVED,				/* square */
		MARKER_RESERVED,				/* triangle */
		MARKER_RESERVED,				/* upside-down triangle */
		MARKER_RESERVED,				/* diamond */
		MARKER_CROSS					/* X */
	};

	if (file.read((UCHAR *)&p, sizeof(p)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Set the type. */

	set_marker_type(wpg_marker_translate[p.style]);

/* Set the color. */

	set_marker_color(rgb_from_color_index(p.color));

/* Set the size. */

	set_marker_size(p.width);

	return TRUE;
}

/*
// Process a version 1 polymarker record.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_polymarker(BYTE *record_type, LONG length)
{
	SHORT count;

	if (file.read((UCHAR *)&count, sizeof(count)) != ERRORCODE_None)
	{
		return FALSE;
	}

	draw_polymarker(count);
	return TRUE;
}

/*
// Process a version 1 line.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_line(BYTE *record_type, LONG length)
{
	draw_polyline(2);
	return TRUE;
}

/*
// Process a version 1 polyline.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_polyline(BYTE *record_type, LONG length)
{
	SHORT count;

	if (file.read((UCHAR *)&count, sizeof(count)) != ERRORCODE_None)
	{
		return FALSE;
	}
	draw_polyline(count);
	return TRUE;
}

/*
// Process a version 1 rectangle.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_rectangle(BYTE *record_type, LONG length)
{
	struct
	{
		SHORT x;
		SHORT y;
		SHORT width;
		SHORT height;
	} p;

	OUTLINE_POINT p0, p1;

	if (file.read((UCHAR *)&p, sizeof(p)) != ERRORCODE_None)
	{
		return FALSE;
	}

	p0.x = p.x;
	p0.y = p.y - p.height;
	p1.x = p.x + p.width;
	p1.y = p.y;
	draw_rectangle(p0, p1);
	return TRUE;
}

/*
// Process a version 1 polygon.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_polygon(BYTE *record_type, LONG length)
{
	SHORT count;

	if (file.read((UCHAR *)&count, sizeof(count)) != ERRORCODE_None)
	{
		return FALSE;
	}
	draw_polygon(count);
	return TRUE;
}

/*
// Process a version 1 ellipse.
*/

BOOL WLOCAL WPGUpdateState::WPG_1_ellipse(BYTE *record_type, LONG length)
{
	struct
	{
		SHORT center_x;
		SHORT center_y;
		SHORT major_radius;
		SHORT minor_radius;
		SHORT rotation_angle;
		SHORT start_angle;
		SHORT end_angle;
		SHORT flags;
	} p;
	OUTLINE_POINT center;

	if (file.read((UCHAR *)&p, sizeof(p)) != ERRORCODE_None)
	{
		return FALSE;
	}

	double rotation_angle = DEG2RAD(p.rotation_angle);

	center.x = p.center_x;
	center.y = p.center_y;

	if (p.start_angle == 0 && p.end_angle == 360)
	{
	/* Do a fast ellipse/circle. */

		if (rotation_angle != 0)
		{
			od("Rotated ellipse (%d)!\r\n", p.rotation_angle);
		}
		draw_ellipse(center, p.major_radius, p.minor_radius, rotation_angle);
	}
	else
	{
		od("Elliptical arc!\r\n");
		draw_elliptical_arc(center,
 						(double)p.major_radius, (double)p.minor_radius,
						DEG2RAD(p.start_angle), DEG2RAD(p.end_angle),
						(p.flags & 1)
									? 0				/* Connect to center */
									: (p.flags & 2)
										? 1			/* Chord closure */
										: -1			/* No closure */
//						,rotation_angle
						);
	}
	return TRUE;
}

/*
// Process a version 1 color map/
*/

BOOL WLOCAL WPGUpdateState::WPG_1_colormap(BYTE *record_type, LONG length)
{
	struct
	{
		WORD	starting_index;
		WORD	number_of_colors;
	} p;
	if (palette != NULL)
	{
	/* Read the record. */
		if (file.read((UCHAR *)&p, sizeof(p)) != ERRORCODE_None)
		{
			return FALSE;
		}

	/* Read the entries here. */

		if (file.read(palette+p.starting_index,
								p.number_of_colors*sizeof(RGBTriple)) != ERRORCODE_None)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
// The version 1 processor table.
*/

WPGUpdateState::WPG_1_PROCESSOR far WPGUpdateState::version_1_processor[] =
{
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Bad op */
	{ WPGUpdateState::WPG_1_fill_attributes, FALSE },			/* FillAttributes */
	{ WPGUpdateState::WPG_1_line_attributes, FALSE },			/* LineAttributes */
	{ WPGUpdateState::WPG_1_marker_attributes, FALSE },		/* MarkerAttributes */
	{ WPGUpdateState::WPG_1_polymarker, TRUE },					/* PolyMarker */
	{ WPGUpdateState::WPG_1_line, TRUE },							/* Line */
	{ WPGUpdateState::WPG_1_polyline, TRUE },					/* PolyLine */
	{ WPGUpdateState::WPG_1_rectangle, TRUE },					/* Rectangle */
	{ WPGUpdateState::WPG_1_polygon, TRUE },						/* Polygon */
	{ WPGUpdateState::WPG_1_ellipse, TRUE },						/* Ellipse */
	{ WPGUpdateState::WPG_null_processor, TRUE },				/* Curve (unused) */
	{ WPGUpdateState::WPG_null_processor, TRUE },				/* Bitmap */
	{ WPGUpdateState::WPG_null_processor, TRUE },				/* GText */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* GTextAttributes */
	{ WPGUpdateState::WPG_1_colormap, FALSE },					/* ColorMap */
	{ WPGUpdateState::WPG_process_1_start, FALSE },			/* StartWPGData */
	{ WPGUpdateState::WPG_process_end, FALSE },					/* EndWPGData */
	{ WPGUpdateState::WPG_null_processor, FALSE }				/* Postscript data */
};

/*
// Parse a version 1 command.
*/

BOOL WLOCAL WPGUpdateState::parse_1_command(BOOL *primitive)
{
	BYTE record_header[1];
	LONG length;
 	ST_DEV_POSITION curpos;

//	printf("parse next record (%d)...", version);

/* Read the header byte(s) */

	if (file.read(record_header, 1) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Read the record length. */

	if (!read_record_length(&length))
	{
		return FALSE;
	}

/* Process the record depending on the version number. */

	file.tell(&curpos);

	if (record_header[0] < WPG1RT_COUNT)
	{
//		printf("(%d)", record_header[0]);
		if (!(this->*version_1_processor[record_header[0]].parser)(record_header, length))
		{
			return FALSE;
		}
		*primitive = version_1_processor[record_header[0]].primitive;
	}

/* Advance past this record. */

	file.seek(curpos+length, ST_DEV_SEEK_SET);

//	printf("\n");
	return TRUE;
}

#ifdef WPG2

/*
// Process a version 2 Start record.
*/

BOOL WLOCAL WPGUpdateState::WPG_process_2_start(BYTE *record_type, LONG length)
{
	WPG_2_START record;
//	OUTLINE_POINT p0, p1, dummy;
	OUTLINE_POINT p0, p1, p2, p3;

/* Read the record. */

	if (file.read((UCHAR *)&record, sizeof(record)) != ERRORCODE_None)
	{
		return FALSE;
	}

	fixed_coordinates = !(record.coordinate_precision == 0);

/* Skip the view extent. */

	read_next_point(&p2);
	read_next_point(&p3);

/* Read the source extent. */

	read_next_point(&p0);
	read_next_point(&p1);

	width = p1.x - p0.x;
	height = p1.y - p0.y;

	od("W: %d, H: %d, (W: %d, H: %d)\r\n",
			width, height,
			p3.x - p2.x, p3.y - p2.y);

	x_resolution = record.x_resolution;
	y_resolution = record.y_resolution;

	if (painting)
	{
		p1.x = width;
		p1.y = height;

	/* Tell the outline rasterizer how big our source extent is. */

		set_source_extent(p0, p1);

		reverse_y_transform = !reverse_y_transform;
	}
//	printf("[%d, %d, %d, %d]\n", p0, p1);

	return TRUE;
}

/*
// Handle version 2 characterization flags.
*/

BOOL WLOCAL WPGUpdateState::handle_cflags(VOID)
{
/* Read the flags. */

	if (file.read(&characterization_flags, sizeof(characterization_flags)) != ERRORCODE_None)
	{
		return FALSE;
	}

/*
// Set some typical settings.
*/

	if (IS_FILLED(characterization_flags))
	{
	/* Filled. */
		set_interior_style(1);
	}
	else
	{
	/* Hollow. */
		set_interior_style(0);
	}

	if (IS_FRAMED(characterization_flags))
	{
	/* Framed. */
		set_edge_visibility(1);
	}
	else
	{
	/* Not framed. */
		set_edge_visibility(0);
	}

/*
// See if there are any extra values to read.
*/

	if (HAS_EXTRA_PARAMS(characterization_flags))
	{
//		printf("{ %x }", characterization_flags);
	/*
	// Read the values.
	*/

	/* Edit lock flags. */

		if (HAS_EDIT_LOCK_FLAGS(characterization_flags))
		{
			file.read(&edit_lock_flags, sizeof(edit_lock_flags));
		}

	/* Object id. */

		if (HAS_OBJECT_ID(characterization_flags))
		{
			file.read(&object_id, sizeof(object_id));
		}

	/* If we have a transformation, set the default one. */

		if (HAS_TRANSFORM(characterization_flags))
		{
			angle = 0;
			scale_x = scale_y = 0x00010000;
			skew_x = skew_y = 0;
			trans_x_integer = trans_y_integer = 0;
			trans_x_fraction = trans_y_fraction = 0;
			taper_x = taper_y = 0;

		/* Rotation angle. */

			if (HAS_ANGLE(characterization_flags))
			{
				file.read(&angle, sizeof(angle));
			}

		/* Scale transformation elements. */

			if (HAS_SCALE_XFORM(characterization_flags))
			{
				file.read(&scale_x, sizeof(scale_x) + sizeof(scale_y));
			}

		/* Skew transformation elements. */

			if (HAS_SKEW_XFORM(characterization_flags))
			{
				file.read(&skew_x, sizeof(skew_x) + sizeof(skew_y));
			}

		/* Translation transformation elements. */

			if (HAS_TRANS_XFORM(characterization_flags))
			{
				file.read(&trans_x_fraction,
	 							sizeof(trans_x_fraction)
	 							+ sizeof(trans_x_integer)
	 							+ sizeof(trans_y_fraction)
	 							+ sizeof(trans_y_integer));
			}

		/* Taper transformation elements. */

			if (HAS_TAPER_XFORM(characterization_flags))
			{
				file.read(&taper_x, sizeof(taper_x) + sizeof(taper_y));
			}
#if 0
			od("[a: %ld; s: %08lx, %08lx; k: %08lx, %08lx; T: %08lx%04x, %08lx%04x; P: %08lx, %08lx]\r\n",
				angle,
				scale_x, scale_y,
				skew_x, skew_y,
				trans_x_integer, trans_x_fraction,
				trans_y_integer, trans_y_fraction,
				taper_x, taper_y);
#endif
		}
	}

	return TRUE;
}

/*
// Read a color.
// The color may be double-precision.
*/

OUTLINE_COLOR WLOCAL WPGUpdateState::WPG_2_read_color(BOOL dp)
{
	OUTLINE_COLOR color;

	if (dp)
	{
	/* "double-precision" which means 'shorts' */
		file.read(&color, sizeof(color));
		file.seek(2, ST_DEV_SEEK_CUR);		/* Skip transparency */
	}
	else
	{
		RGBTriple triple;

		file.read(&triple, sizeof(triple));
		color.red = triple[0];
		color.green = triple[1];
		color.blue = triple[2];
		file.seek(1, ST_DEV_SEEK_SET);		/* Skip transparency */
	}
	return color;
}

/*
// Process a version 2 pen color record.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_pen_fore_color(BYTE *record_type, LONG length)
{
	OUTLINE_COLOR color;

	color = WPG_2_read_color(record_type[1] == WPG2RT_DP_Pen_Fore_Color);

	set_edge_color(color);
	set_line_color(color);
	return TRUE;
}

/*
// Process a version 2 brush color record.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_brush_fore_color(BYTE *record_type, LONG length)
{
	BYTE gradient_type;

	file.read(&gradient_type, sizeof(gradient_type));

	if (gradient_type == 0)
	{
	/* Not a gradient. */
		OUTLINE_COLOR color;

		color = WPG_2_read_color(record_type[1] == WPG2RT_DP_Brush_Fore_Color);

		set_fill_color(color);
	}
	else
	{
		SHORT number_of_fore;
		OUTLINE_COLOR color;
		BOOL dp = record_type[1] == WPG2RT_DP_Brush_Fore_Color;

		file.read(&number_of_fore, sizeof(number_of_fore));

//		printf("(Gradient %d)", number_of_fore);

		color = WPG_2_read_color(dp);

		while (--number_of_fore)
		{
			OUTLINE_COLOR c2;

			c2 = WPG_2_read_color(dp);

			color.red = midpoint(color.red, c2.red);
			color.green = midpoint(color.green, c2.green);
			color.blue = midpoint(color.blue, c2.blue);
		}

		set_fill_color(color);
	}
	return TRUE;
}

/*
// Process a version 2 polyline record.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_polyline(BYTE *record_type, LONG length)
{
	SHORT count;

/* Handle the characterization flags. */

	if (!handle_cflags())
	{
		return FALSE;
	}

/* Read the number of vertices. */

	if (file.read(&count, sizeof(count)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Now process it correctly. */

	if (IS_CLOSED(characterization_flags))
	{
		draw_polygon(count);
	}
	else
	{
		draw_polyline(count);
	}

	return TRUE;
}

/*
// Process a version 2 polycurve record.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_polycurve(BYTE *record_type, LONG length)
{
	SHORT count, closure;

/* Handle the characterization flags. */

	if (!handle_cflags())
	{
		return FALSE;
	}

/* Read the number of control point triplets. */

	if (file.read((UCHAR *)&count, sizeof(count)) != ERRORCODE_None)
	{
		return FALSE;
	}

	if (IS_CLOSED(characterization_flags))
	{
		closure = 0;			/* Normal closure. */
	}
	else if (IS_FILLED(characterization_flags))
	{
		closure = 1;			/* Chord closure */
	}
	else
	{
		closure = -1;			/* Not closed. */
	}

	draw_polycurve_triples(count, closure);

	return TRUE;
}

/*
// Process a version 2 rectangle.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_rectangle(BYTE *record_type, LONG length)
{
	OUTLINE_POINT p0, p1;

/* Handle the characterization flags. */

	if (!handle_cflags())
	{
		return FALSE;
	}

	read_next_point(&p0);
	read_next_point(&p1);

	draw_rectangle(p0, p1);
	return TRUE;
}

/*
// Process a version 2 polyline record.
*/

BOOL WLOCAL WPGUpdateState::WPG_2_arc(BYTE *record_type, LONG length)
{
	OUTLINE_POINT center, radius, start_delta, end_delta;
	SHORT closure;
	double start_angle, end_angle, x_radius, y_radius;

/* Handle the characterization flags. */

	if (!handle_cflags())
	{
		return FALSE;
	}

/* Read the parameters. */

	read_next_point(&center);
	read_next_point(&radius);
	read_next_point(&start_delta);		/* stored as dx, dy */
	read_next_point(&end_delta);			/* stored as dx, dy */
	file.read(&closure, sizeof(closure));

	x_radius = (double)radius.x;
	y_radius = (double)radius.y;

/* Compute the angles. */

	start_angle = angle_from_vector(start_delta.x, start_delta.y);
	end_angle = angle_from_vector(end_delta.x, end_delta.y);

#if 0
	printf("\nf: %x, ctr: %d, %d; rad: %d, %d; ang: %f, %f, cls: %x",
				characterization_flags,
				center,
				radius,
				start_angle, end_angle,
				closure);
#endif

	if (HAS_TRANSFORM(characterization_flags))
	{
		SHORT trans_x, trans_y;
		double x_scale, y_scale;

		x_scale = ((double)scale_x)/65536.0;
		y_scale = ((double)scale_y)/65536.0;

//		printf("\n(( %d, %d -> ", center);

		center.x = (SHORT)(x_scale*center.x);
		center.y = (SHORT)(y_scale*center.y);
		x_radius *= x_scale;
		y_radius *= y_scale;

//		printf("%d, %d -> ", center);

		trans_x = (SHORT)trans_x_integer;
		if (trans_x_fraction & 0x8000)
		{
			trans_x++;
		}

		trans_y = (SHORT)trans_y_integer;
		if (trans_y_fraction & 0x8000)
		{
			trans_y++;
		}

		center.x += trans_x;
		center.y += trans_y;
//		printf("%d, %d )) ", center);
	}

/* Add the arc. */

	if (IS_CLOSED(characterization_flags))
	{
		closure = (closure & 1)
							? 0			/* Wedge */
							: (closure & 2)
								? 1			/* Chord */
								: -1;			/* None */
	}
	else
	{
		if (IS_FILLED(characterization_flags))
		{
			closure = 1;
		}
		else
		{
			closure = -1;
		}
	}

	if (closure != -1 && start_angle == end_angle)
	{
		closure = 1;			/* Simple chord closure. Don't connect to center. */
	}
	
	draw_elliptical_arc(center,
				x_radius, y_radius,
				start_angle, end_angle, closure);
	return TRUE;
}

BOOL WLOCAL WPGUpdateState::WPG_2_pen_size(BYTE *record_type, LONG length)
{

	SHORT fixed_save = fixed_coordinates;
	OUTLINE_POINT size;
	SHORT width;

/* This is kind of a cheat... */

	fixed_coordinates = (record_type[1] == WPG2RT_DP_Pen_Size);
	read_next_point(&size);
	fixed_coordinates = fixed_save;

#if 0
	if (size.x != size.y)
	{
		printf("\nSIZE NOT SQUARE (%d, %d)\n", size.x, size.y);
	}
#endif
	width = midpoint(size.x, size.y);

	set_line_width(width);
	set_edge_width(width);

	return TRUE;
}

/*
// Version 2 processors.
*/

WPGUpdateState::WPG_2_PROCESSOR WPGUpdateState::version_2_processor[] =
{
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Bad op */
	{ WPGUpdateState::WPG_process_2_start, FALSE },			/* Start_WPG */
	{ WPGUpdateState::WPG_process_end, FALSE },					/* End_WPG */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Form_Settings */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Ruler_Settings */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Grid_Settings */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Layer */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Reserved 7 */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Pen_Style_Def */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Pattern_Def */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Comment */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Color_Transfer */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Color_Palette */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* DP_Color_Palette */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Bitmap_Data */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Text_Data */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Chart_Style */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Chart_Data */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Object_Image */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Reserved 19 */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Reserved 20 */
	{ WPGUpdateState::WPG_2_polyline, TRUE },					/* Polyline */
	{ WPGUpdateState::WPG_null_processor, TRUE },				/* Polyspline */
	{ WPGUpdateState::WPG_2_polycurve, TRUE },					/* Polycurve */
	{ WPGUpdateState::WPG_2_rectangle, TRUE },					/* Rectangle */
//	{ WPGUpdateState::WPG_2_arc, TRUE },							/* Arc */
	{ WPGUpdateState::WPG_2_arc, FALSE },							/* Arc */
	{ WPGUpdateState::WPG_null_processor, TRUE },				/* Compound_Polygon */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Bitmap */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Text_Line */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Text_Block */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Text_Path */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Chart */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Group */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Object_Capsule */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Font_Settings */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Line_Cap_Def */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Line_Join_Def */
	{ WPGUpdateState::WPG_2_pen_fore_color, FALSE },			/* Pen_Fore_Color */
	{ WPGUpdateState::WPG_2_pen_fore_color, FALSE },			/* DP_Pen_Fore_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Pen_Back_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* DP_Pen_Back_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Pen_Style */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Pen_Pattern */
	{ WPGUpdateState::WPG_2_pen_size, FALSE },					/* Pen_Size */
	{ WPGUpdateState::WPG_2_pen_size, FALSE },					/* DP_Pen_Size */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Line_Cap */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Line_Join */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Brush_Gradient */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* DP_Brush_Gradient */
	{ WPGUpdateState::WPG_2_brush_fore_color, FALSE },		/* Brush_Fore_Color */
	{ WPGUpdateState::WPG_2_brush_fore_color, FALSE },		/* DP_Brush_Fore_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* Brush_Back_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE },				/* DP_Brush_Back_Color */
	{ WPGUpdateState::WPG_null_processor, FALSE }				/* Brush_Pattern */
};
/*
// Parse a version 2 command.
*/

BOOL WLOCAL WPGUpdateState::parse_2_command(BOOL *primitive)
{
	struct
	{
		BYTE Class;
		BYTE type;
	} record_header;
	LONG extensions, length;
 	ST_DEV_POSITION curpos;

//	printf("parse next record (%d)...", version);

/* Read the header byte(s) */

	if (file.read(&record_header, sizeof(record_header)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Read the extension count. */

	if (!read_record_length(&extensions))
	{
		return FALSE;
	}

/* Read the record length. */

	if (!read_record_length(&length))
	{
		return FALSE;
	}

/* Process the record depending on the version number. */

	file.tell(&curpos);

	if (record_header.type < WPG2RT_COUNT)
	{
//		printf("('%s' %d, %d; %ld, %ld)", record_name[record_header.type],
//						record_header.Class, record_header.type, extensions, length);
//		if (!version_2_processor[record_header.type].primitive)
		{
			if (!(this->*version_2_processor[record_header.type].parser)((BYTE *)&record_header, length))
			{
				return FALSE;
			}
		}
		*primitive = version_2_processor[record_header.type].primitive;
	}

/* Advance past this record. */

	file.seek(curpos+length, ST_DEV_SEEK_SET);

//	printf("\n");
	return TRUE;
}

#endif

#if 0
LPSTR record_name[] =
{
"Bad op",
"Start_WPG",
"End_WPG",
"Form_Settings",
"Ruler_Settings",
"Grid_Settings",
"Layer",
"Reserved 7",
"Pen_Style_Def",
"Pattern_Def",
"Comment",
"Color_Transfer",
"Color_Palette",
"DP_Color_Palette",
"Bitmap_Data",
"Text_Data",
"Chart_Style",
"Chart_Data",
"Object_Image",
"Reserved 19",
"Reserved 20",
"Polyline",
"Polyspline",
"Polycurve",
"Rectangle",
"Arc",
"Compound_Polygon",
"Bitmap",
"Text_Line",
"Text_Block",
"Text_Path",
"Chart",
"Group",
"Object_Capsule",
"Font_Settings",
"Line_Cap_Def",
"Line_Join_Def",
"Pen_Fore_Color",
"DP_Pen_Fore_Color",
"Pen_Back_Color",
"DP_Pen_Back_Color",
"Pen_Style",
"Pen_Pattern",
"Pen_Size",
"DP_Pen_Size",
"Line_Cap",
"Line_Join",
"Brush_Gradient",
"DP_Brush_Gradient",
"Brush_Fore_Color",
"DP_Brush_Fore_Color",
"Brush_Back_Color",
"DP_Brush_Back_Color",
"Brush_Pattern"
};
#endif

/*
// Read WPG length.
// Read from 1 to 5 bytes for a length.
*/

BOOL WPGUpdateState::read_record_length(LONG *length)
{
	BYTE blength;

/* Read the length byte. */

	if (file.read(&blength, sizeof(blength)) != ERRORCODE_None)
	{
		return FALSE;
	}

/* See if this is the length or not. */

	if (blength == 0xff)
	{
		WORD wlength;
	/*
 	// At least larger than byte.
	// Read the next word.
 	*/
		if (file.read(&wlength, sizeof(wlength)) != ERRORCODE_None)
		{
			return FALSE;
		}

	/*
 	// See if this is the whole length.
 	*/
		if (wlength & 0x8000)
		{
			WORD wlow;
		/*
 		// Not the whole length!
		// We have the high word. Read the low word.
 		*/
			if (file.read(&wlow, sizeof(wlow)) != ERRORCODE_None)
			{
				return FALSE;
			}
			*length = (LONG)wlow | ((LONG)(wlength & 0x7FFF)<<16);
		}
		else
		{
		/* This is it. */
			*length = (LONG)wlength;
		}
	}
	else
	{
	/* This is the whole length. */
		*length = (LONG)blength;
	}

	return TRUE;
}

/*
// Parse the next WPG command.
*/

BOOL WPGUpdateState::parse_next_command(BOOL *primitive)
{
	switch (version)
	{
		case 1:
		{
			return parse_1_command(primitive);
		}
#ifdef WPG2
		case 2:
		{
			return parse_2_command(primitive);
		}
#endif
		default:
		{
			return FALSE;
		}
	}
}

/*
// Read a WPG header.
*/

ERRORCODE WPGUpdateState::read_wpg_header(WP_HEADER *header)
{
	static CHAR BASED_CODE file_id[4] = { -1, 'W', 'P', 'C' };
	ERRORCODE error;

/* Read the header. */

	if ((error = file.read(header, sizeof(*header))) != ERRORCODE_None)
	{
		return error;
	}

/* Make sure we have a Word Perfect file. */

	if (memcmp(header->WP_file_id, file_id, 4) != 0
				|| header->file_type != WPFT_WPG
#ifdef WPG2
				|| (version = header->version_major) > 2)
#else
	/* We don't try to support version 2.0 WPGs yet. */
				|| (version = header->version_major) > 1)
#endif
	{
		return ERRORCODE_IllegalType;
	}

/* Seek to the data start. */

	return file.seek(header->data_start, ST_DEV_SEEK_SET);
}

ERRORCODE WPGUpdateState::read_wpg_info(VOID)
{
	WP_HEADER header;
	ERRORCODE error;

/* Read the header. */

	if ((error = read_wpg_header(&header)) == ERRORCODE_None)
	{
		BOOL primitive;
	/*
	// Now we must find the WPG start record to know how big we are.
	// It's supposed to be the first record, so read the first record.
	*/

		width = -1;

		if (!parse_next_command(&primitive) || width == -1)
		{
			return ERRORCODE_IllegalType;
		}

	/* We got values. Plug them in. */

//		printf("Got w: %d, h:%d, xr: %d, yr: %d\n",
//	 			width, height, x_resolution, y_resolution);
	}
	return error;
}

/*
// Set defaults for a WPG reading session.
*/

VOID WPGUpdateState::set_defaults(VOID)
{
	if (painting)
	{
		OutlineUpdateState::set_defaults();

	/* "default position" */

		WP_HEADER header;

		read_wpg_header(&header);
	}
}

/*
// Read the next point from the file.
*/

BOOL WPGUpdateState::read_next_point(OUTLINE_POINT *point)
{
	if (fixed_coordinates)
	{
		CFixed p[2];

		if (file.read((UCHAR *)p, sizeof(p)) != ERRORCODE_None)
		{
			return FALSE;
		}
//		p[0] += 0x8000;
//		p[1] += 0x8000;
		point->x = FixedInteger(p[0]);
		point->y = FixedInteger(p[1]);
		return TRUE;
	}
	else
	{
		return file.read(point, sizeof(OUTLINE_POINT)) == ERRORCODE_None;
	}
}

/*
// Open the outline file.
// If the palette failed earlier, just return error now.
*/

ERRORCODE WPGUpdateState::open_outline(VOID)
{
/* See if we failed somewhere in the constructor. */

	if (painting && palette == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Open as normal. */

	return OutlineUpdateState::open_outline();
}

/*
// Initialize a WPG object's graphic data structure.
*/

ERRORCODE WPGUpdateState::init(GraphicPtr graphic)
{
	ERRORCODE error;

	if ((error = open_outline()) != ERRORCODE_None)
	{
		return error;
	}

/* Try to open the file. */

#ifndef FIX_CGMS
	file.set_read_buffer(2048);
#endif

	if ((error = read_wpg_info()) == ERRORCODE_None)
	{
	/* Colors are OK. */

		graphic->record.x_size = width;
		graphic->record.y_size = height;
		graphic->record.x_resolution = x_resolution;
		graphic->record.y_resolution = y_resolution;

		graphic->record.storage = GRAPHIC_STORAGE_FILE;
	}

	close_outline();

	return error;
}

/*
// The constructor for a WPGUpdateState.
*/

WPGUpdateState::WPGUpdateState(GraphicPtr data, BOOL p)
			: OutlineUpdateState(data), painting(p), palette(NULL), fixed_coordinates(FALSE)
{
	if (painting)
	{
		static RGBTriple BASED_CODE default_palette[] =
		{
			{ 0, 0, 0},				/* 0 */
			{ 0, 0, 170},			/* 1 */
			{ 0, 170, 0},			/* 2 */
			{ 0, 170, 170},		/* 3 */
			{ 170, 0, 0},			/* 4 */
			{ 170, 0, 170},		/* 5 */
			{ 170, 85, 0},			/* 6 */
			{ 170, 170, 170},		/* 7 */
			{ 85, 85, 85},			/* 8 */
			{ 85, 85, 255},		/* 9 */
			{ 85, 255, 85},		/* A */
			{ 85, 255, 255},		/* B */
			{ 255, 85, 85},		/* C */
			{ 255, 85, 255},		/* D */
			{ 255, 255, 85},		/* E */
			{ 255, 255, 255}		/* F */
		};

		palette = NULL;
		TRY
			palette = new RGBTriple[256];
		END_TRY
		if (palette == NULL)
		{
		/* Can't continue! */
//			continue_error(-1, "Not enough memory to paint WPG file.");
		}
		else
		{
			memcpy(palette, default_palette, sizeof(default_palette));
		}
	}
}

/*
// The destructor for a WPGUpdateState.
*/

WPGUpdateState::~WPGUpdateState()
{
	delete [] palette;
}

/*
// Initialize a WPG graphic.
*/

ERRORCODE WPGHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	WPGUpdateState ustate(graphic, FALSE);

/* Do the initialization. */

	return ustate.init(graphic);
}
