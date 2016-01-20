/*
// CGM.CPP
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/CGM.CPP 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CGM.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     12/08/97 2:52p Jayn
// Text, arcs, pie charts, improved transform, etc.
// 
// 2     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
//    Rev 1.0   14 Aug 1997 15:18:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:32   Fred
// Initial revision.
// 
//    Rev 1.6   15 Jul 1997 09:33:46   Jay
// Support for larger images.
// 
//    Rev 1.5   12 Jun 1997 09:08:24   Jay
// Fixed subfile in init from gcs->pSourceDevice.
// 
//    Rev 1.4   23 May 1997 16:13:52   Jay
// Support for nFX winding
// 
//    Rev 1.3   31 Mar 1997 17:11:42   Jay
// Support for source device during init().
// 
//    Rev 1.2   01 Nov 1996 17:11:38   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   25 Aug 1996 12:59:20   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:47:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:12   FRED
// Initial revision.
// 
//    Rev 2.3   07 Jul 1995 16:10:54   JAY
// Misc changes.
// 
//    Rev 2.2   14 Jun 1995 16:46:30   JAY
// Now handles cgms which don't have a size (for example, 0-byte files).
// 
//    Rev 2.1   05 Jun 1995 11:31:02   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:34   JAY
// Initial revision.
// 
//    Rev 1.10   30 Nov 1994 16:33:48   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.9   13 Jul 1994 11:26:58   JAY
// GCS can now be NULL to init.
// 
//    Rev 1.8   07 Jul 1994 11:30:42   JAY
// Line width and edge width now have defaults during size.
// 
//    Rev 1.7   09 Jun 1994 11:03:18   JAY
// Now defaults vdc_type (as it should!).
// 
//    Rev 1.6   23 May 1994 08:39:54   JAY
// Added debugging messages (commented out) for sizing code.
// 
//    Rev 1.5   11 May 1994 16:33:34   JAY
// Made some debugging messages more usable.
// 
//    Rev 1.4   08 Mar 1994 12:27:58   JAY
// Changed some printf's to od's. (They remain commented out most of the time.)
// 
//    Rev 1.3   22 Feb 1994 15:02:32   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.2   25 Jan 1994 15:18:42   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:05:56   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:06   JAY
// 
*/

#include "stdafx.h"

#define DEBUG_CGMx
#define DUMP_CGMx

#define	WIDTH_SCALE			4
#define	SHOW_COLORSx

#include	<stdlib.h>					// for __max()
#include	<math.h>

#include "pmgobj.h"
#include "grafrec.h"
#include "grafobj.h"
#include "cgm.h"
#include "utils.h"

/*
// Read the CGM helper info.
*/

ERRORCODE CGMHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the CGM helper info.
*/

ERRORCODE CGMHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the CGM helper info.
*/

ST_DEV_IO_SIZE CGMHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Set the current cgm color.
*/

VOID CLOCAL CGMUpdateState::cgm_to_outline_color(CGM_COLOR color, OUTLINE_COLOR *ocolor)
{
/* Scale if necessary. */

	if (color_scale[0])
	{
		color.red = scale_number(color.red-color_origin.red,
													255,
													color_extent.red);
	}

	if (color_scale[1])
	{
		color.green = scale_number(color.green-color_origin.green,
													255,
													color_extent.green);
	}

	if (color_scale[2])
	{
		color.blue = scale_number(color.blue-color_origin.blue,
													255,
													color_extent.blue);
	}

	ocolor->red = color.red;
	ocolor->green = color.green;
	ocolor->blue = color.blue;
}

#ifdef DUMP_CGM
PRIVATE char *
class_names[] =
{
	"Delimiter",
	"Metafile Descriptor",
	"Picture Descriptor",
	"Control",
	"Graphical Primitive",
	"Attribute",
	"Escape",
	"External",
	"Reserved"
};

/*
// Strings for class ids.
*/

PRIVATE char *
delimiter_names[] =
{
	"No-op",
	"Begin Metafile",
	"End Metafile",
	"Begin Picture",
	"Begin Picture Body",
	"End Picture"
},
*metafile_names[] =
{
	"0",
	"Version",
	"Description",
	"VDC type",
	"Integer precision",
	"Real precision",
	"Index precision",
	"Color precision",
	"Color index precision",
	"Maximum color index",
 	"Color value extent",
	"Metafile element list",
	"Metafile defaults replacement",
	"Font list",
	"Character set list",
	"Character coding announcer"
},
*picture_names[] =
{
	"0",
	"Scaling mode",
	"Color selection mode",
	"Line width spec.",
	"Marker size spec. mode",
	"Edge width spec. mode",
	"VDC extent",
	"Background color"
},
*control_names[] =
{
	"0",
	"VDC integer precision",
	"VDC real precision",
	"Auxiliary color",
	"Transparency",
	"Clip rectangle",
	"Clip indicator"
},
*primitive_names[] =
{
	"0",
	"Polyline",
	"Disjoint polyline",
	"Polymarker",
	"Text",
	"Restricted text",
	"Append text",
	"Polygon",
	"Polygon set",
	"Cell array",
	"Generalized drawing primitive",
	"Rectangle",
	"Circle",
	"Circular arc 3 point",
	"Circular arc 3 point close",
	"Circular arc center",
	"Circular arc center close",
	"Ellipse",
	"Elliptical arc",
	"Elliptical arc close"
},
*attribute_names[] =
{
	"0",
	"Line bundle index",
	"Line type",
	"Line width",
	"Line color",
	"Marker bundle index",
	"Marker type",
	"Marker size",
	"Marker color",
	"Text bundle index",
	"Text font index",
	"Text precision",
	"Character expansion factor",
	"Character spacing",
	"Text color",
	"Character height",
	"Character orientation",
	"Text path",
	"Text alignment",
	"Character set index",
	"Alternate character set index",
	"Fill bundle index",
	"Interior style",
	"Fill color",
	"Hatch index",
	"Pattern index",
	"Edge bundle index",
	"Edge type",
	"Edge width",
	"Edge color",
	"Edge visibility",
	"Fill reference point",
	"Pattern table",
	"Pattern size",
	"Color table",
	"Aspect source flags"
},
*escape_names[] =
{
	"0",
	"Escape"
},
*external_names[] =
{
	"0",
	"Message",
	"Application data"
};

/*
// info about the classes.
*/

PRIVATE struct
{
	SHORT	last_id;
	CHAR	**names;
} info[] =
{
	{ CGM_DELIMITER_ELEMENTS, delimiter_names },
	{ CGM_METAFILE_ELEMENTS, metafile_names },
	{ CGM_PICTURE_ELEMENTS, picture_names },
	{ CGM_CONTROL_ELEMENTS, control_names },
	{ CGM_PRIMITIVE_ELEMENTS, primitive_names },
	{ CGM_ATTRIBUTE_ELEMENTS, attribute_names },
	{ CGM_ESCAPE_ELEMENTS, escape_names },
	{ CGM_EXTERNAL_ELEMENTS, external_names },
};
#endif

/*
// Read a short from the file.
// CGM files have integers swapped (from the IBM perspective), so we
// swap them in this routine.
// 'precision' is the number of bytes to read for this integer and must never
// be larger than 4.
*/

BOOL CLOCAL CGMUpdateState::read_cgm_short(LPSHORT s, SHORT precision)
{
	LPBYTE p;

/*
// We do something goofy here.
// We only want 2 bytes, so we only take the top two.
*/

#ifndef FIX_CGMS
	if ((p = file.fast_access(precision)) == NULL)
#endif
	{
		BYTE buffer[4];

		if (file.read(buffer, precision) != ERRORCODE_None)
		{
			return FALSE;
		}
		if (precision == 1)
		{
			*s = (SHORT)buffer[0];
		}
		else
		{
			*s = (SHORT)buffer[1] + (((SHORT)buffer[0]) << 8);
		}
	}
#ifndef FIX_CGMS
	else
	{
		if (precision == 1)
		{
			*s = (SHORT)p[0];
		}
		else
		{
			*s = (SHORT)p[1] + (((SHORT)p[0]) << 8);
		}
	}
#endif

	return TRUE;
}

/*
// Read a precision value.
*/

VOID CLOCAL CGMUpdateState::read_precision(LPSHORT s)
{
	SHORT i;

	read_integer(&i);

	if ((i /= 8) > 0 && i <= 4)
	{
		*s = i;
	}
/* Else leave it as it is. */
}

/*
// Read a string.
*/

#ifdef DUMP_CGMfoo
PRIVATE CHAR * CLOCAL
read_string(SHORT length)
{
	CHAR buffer[256];
	UCHAR slen;

/* Read the length. */

	file.read((UCHAR near *)&slen, 1);

/* Read all the characters. */

	file.read(buffer, (SHORT)slen);
	buffer[slen] = '\0';

	return buffer;
}

/*
// Print a string.
*/

PRIVATE VOID CLOCAL
print_string(SHORT length)
{
	od("\"%s\"\r\n", (LPCSTR)read_string(length));
}
#endif

/*
// Read a CGM triple color.
*/

VOID CLOCAL CGMUpdateState::read_cgm_triple_color(CGM_COLOR far *color)
{
	CGM_COLOR c;

	read_cgm_short(&c.red, color_precision);
	c.red &= 0x00FF;
	read_cgm_short(&c.green, color_precision);
	c.green &= 0x00FF;
	read_cgm_short(&c.blue, color_precision);
	c.blue &= 0x00FF;

	*color = c;
}

/*
// Read a color.
*/

VOID CLOCAL CGMUpdateState::read_cgm_color(OUTLINE_COLOR *ocolor)
{
	CGM_COLOR color;

	if (color_selection_mode == COLOR_SELECTION_INDEXED)
	{
		SHORT index;

	/* Read the index. */

		read_cgm_short(&index, color_index_precision);

	/* Validate the index. */

		if (index < 0)
		{
			index = 0;
		}
		if (index > max_color_index)
		{
			index = max_color_index;
		}

	/* Plug the color in as desired. */

		if (palette == NULL)
		{
			color.red = color.green = color.blue = 0;
		}
		else
		{
			color = palette[index];
		}
#ifdef SHOW_COLORS
		if (debug_flags & 1024)
		{
			printf("%d : (%d, %d, %d)\n", index, *color);
		}
#endif
	}
	else
	{
	/* Must be DIRECT */
		read_cgm_triple_color(&color);
	}

	cgm_to_outline_color(color, ocolor);
}

/*
// Read a point.
*/

VOID CLOCAL CGMUpdateState::read_vdc_point(CGM_POINT *point)
{
	read_vdc(&point->x);
	read_vdc(&point->y);
}

/*
// Skip an element.
*/

BOOL CLOCAL CGMUpdateState::parser_ignore(SHORT id, SHORT length)
{
	return TRUE;
}

/*
// Parse a delimiter descriptor element for sizing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_delimiter(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_BEGIN_METAFILE:
		{
			UCHAR slen;

			file.read(&slen, sizeof(slen));

			if (slen != 255 && slen >= 3)
			{
				BYTE buffer[3];

				// Read the graphic title.
				file.read(buffer, sizeof(buffer));
				if (buffer[0] == 'n'
					 && buffer[1] == 'F'
					 && buffer[2] == 'X')
				{
					// nFX CGM file. Use WINDING mode.
					m_nFillMode = WINDING;
               TRACE("nFX graphic!\n");
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse a picture descriptor element for drawing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_picture(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_COLOR_SELECTION_MODE:
		{
			read_enum(&color_selection_mode);
			break;
		}
		case CGM_LINE_WIDTH_MODE:
		{
			read_enum(&line_width_mode);
			break;
		}
		case CGM_EDGE_WIDTH_MODE:
		{
			read_enum(&edge_width_mode);
			break;
		}
		case CGM_MARKER_SIZE_MODE:
		{
			read_enum(&marker_size_mode);
			break;
		}
		case CGM_VDC_EXTENT:
		{
			CGM_POINT p0, p1;

#ifdef FIX_CGMS
			ST_DEV_POSITION Pos;

			file.tell(&Pos);
#endif

			read_vdc_point(&p0);
			read_vdc_point(&p1);

//			printf("extent: %d, %d to %d, %d\n", p0, p1);

			if (p0.y < p1.y)
			{
				reverse_y_transform = !reverse_y_transform;
			}
			else
			{
				SHORT tmp = p0.y;
				p0.y = p1.y;
				p1.y = tmp;
			}

			if (p0.x > p1.x)
			{
				SHORT tmp = p0.x;
				p0.x = p1.x;
				p1.x = tmp;
				reverse_x_transform = !reverse_x_transform;
			}

#if 0
			TRACE("Ext: %d, %d, %d, %d (rev: %d, %d); min: %d, %d; max: %d, %d\n",
					p0, p1,
					reverse_x_transform, reverse_y_transform,
					metafile_min, metafile_max);
#endif

		/* Use the discovered size if we have them. */

			if (metafile_min.x < metafile_max.x
					&& metafile_min.y < metafile_max.y)
			{
			/* Use the discovered bounds. */
//				od("use discovered bounds...\r\n");

#ifdef FIX_CGMS
				ST_DEV_POSITION EndPos;
				file.tell(&EndPos);

				ASSERT(vdc_integer_precision == 2);

				CGM_POINT vdcp0, vdcp1;
				if (reverse_x_transform)
				{
					vdcp0.x = metafile_max.x;
					vdcp1.x = metafile_min.x;
				}
				else
				{
					vdcp0.x = metafile_min.x;
					vdcp1.x = metafile_max.x;
				}
				if (!reverse_y_transform)
				{
					vdcp0.y = metafile_max.y;
					vdcp1.y = metafile_min.y;
				}
				else
				{
					vdcp0.y = metafile_min.y;
					vdcp1.y = metafile_max.y;
				}
			/* We want to write! */
				
				TRACE("Fix the CGM!\n");
				file.seek(Pos, ST_DEV_SEEK_SET);
				// Shorts are written reversed.
				file.write(((char*)&vdcp0.x)+1, 1);
				file.write(((char*)&vdcp0.x)+0, 1);
				file.write(((char*)&vdcp0.y)+1, 1);
				file.write(((char*)&vdcp0.y)+0, 1);

				file.write(((char*)&vdcp1.x)+1, 1);
				file.write(((char*)&vdcp1.x)+0, 1);
				file.write(((char*)&vdcp1.y)+1, 1);
				file.write(((char*)&vdcp1.y)+0, 1);
#endif
				p0.x = metafile_min.x;
				p0.y = metafile_min.y;
				p1.x = metafile_max.x;
				p1.y = metafile_max.y;
			}
			else
			{
				LONG delta;

//				od("use actual extent...\r\n");

				if ((delta = (USHORT)p1.x - (USHORT)p0.x) > 32767)
				{
					p1.x = (SHORT)scale_pcoord(p1.x, 32767L, delta);
					p0.x = (SHORT)scale_pcoord(p0.x, 32767L, delta);
				}

				if ((delta = (USHORT)p1.y - (USHORT)p0.y) > 32767)
				{
					p1.y = (SHORT)scale_pcoord(p1.y, 32767L, delta);
					p0.y = (SHORT)scale_pcoord(p0.y, 32767L, delta);
				}
			}

			set_source_extent(p0, p1);

//			od("used VDC extent: %d, %d to %d, %d\r\n", p0, p1);

			break;
		}
		case CGM_BACKGROUND_COLOR:
		{
			CGM_COLOR color;
			OUTLINE_COLOR ocolor;

			read_cgm_triple_color(&color);

			if (palette != NULL)
			{
				palette[0] = color;
			}

			cgm_to_outline_color(color, &ocolor);
			set_background_color(ocolor);
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse a picture descriptor element for sizing.
*/

BOOL CLOCAL CGMUpdateState::SIZE_picture(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_VDC_EXTENT:
		{
			CGM_POINT p0, p1;

			read_vdc_point(&p0);
			read_vdc_point(&p1);

			size_origin.x = p0.x;
			size_origin.y = p0.y;
			int x = p1.x - p0.x;
			int y = p1.y - p0.y;
			if (x < 0)
				x = -x;
			if (y < 0)
				y = -y;
			size_extent_x = x;
			size_extent_y = y;

		/* Stop now if not proof. */

			break;
		}
		case CGM_BACKGROUND_COLOR:
		{
		/* Skip this command. */
			break;
		}
		default:
		{
			return DRAW_picture(id, length);
		}
	}
	return TRUE;
}

/*
// Parse a metafile descriptor element for drawing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_metafile(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_VDC_TYPE:
		{
			SHORT t;

			read_enum(&t);
			vdc_type = t;

			if (vdc_type != 0)
			{
				return FALSE;				/* Stop now! */
			}
			break;
		}
		case CGM_INTEGER_PRECISION:
		{
			read_precision(&integer_precision);
			break;
		}
		case CGM_INDEX_PRECISION:
		{
			read_precision(&index_precision);
			break;
		}
		case CGM_COLOR_PRECISION:
		{
			read_precision(&color_precision);
			break;
		}
		case CGM_COLOR_INDEX:
		{
			read_precision(&color_index_precision);
			break;
		}
		case CGM_MAXIMUM_COLOR_INDEX:
		{
			SHORT i;

			read_cgm_short(&i, color_index_precision);
			max_color_index = i;

#ifdef DUMP_CGM
			od("[%d]\r\n", max_color_index);
#endif
			break;
		}
		case CGM_COLOR_VALUE_EXTENT:
		{
			CGM_COLOR c0, c1;
		/* Read the two values. */

			read_cgm_triple_color(&c0);
			read_cgm_triple_color(&c1);

		/* Convert to origin/extent from color/color. */

			color_extent.red = c1.red - (color_origin.red = c0.red);
			color_extent.green = c1.green - (color_origin.green = c0.green);
			color_extent.blue = c1.blue - (color_origin.blue = c0.blue);

		/* Set flags about whether we need to scale or not. */

			color_scale[0] = (color_origin.red != 0 || color_extent.red != 255);
			color_scale[1] = (color_origin.green != 0 || color_extent.green != 255);
			color_scale[2] = (color_origin.blue != 0 || color_extent.blue != 255);

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse a control element for drawing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_control(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_VDC_INTEGER_PRECISION:
		{
			read_precision(&vdc_integer_precision);
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse a graphics primitive for drawing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_primitive(SHORT id, SHORT length)
{
/* Reset in preparation for drawing a primitive. */

	switch (id)
	{
		case CGM_RECTANGLE:
		{
			CGM_POINT p0, p1;
		/*
		// Read the two end points.
		*/

			read_vdc_point(&p0);
			read_vdc_point(&p1);

		/*
		// Process it as a rectangle.
		*/

			draw_rectangle(p0, p1);

			break;
		}
		case CGM_POLYLINE:
		{
		/* Compute the number of points. */

			if ((length /= (vdc_integer_precision*2)) == 0)
			{
			/* Fairly degenerate. */
				break;
			}

			draw_polyline((SHORT)length);

			break;
		}
		case CGM_POLYGON:
		{
			if ((length /= (vdc_integer_precision*2)) == 0)
			{
			/* Fairly degenerate. */
				break;
			}

			draw_polygon((SHORT)length);

			break;
		}
		case CGM_ELLIPSE:
		{
			CGM_POINT center, cd1, cd2;
			read_vdc_point(&center);				/* Read the center. */
			read_vdc_point(&cd1);					/* Endpoint of conjugate diameter 1. */
			read_vdc_point(&cd2);					/* Endpoint of conjugate diameter 2. */

			if (cd1.x == center.x && cd2.y == center.y)
			{
			/* Normal orientation. */
				draw_ellipse(center, cd2.x - center.x, cd1.y - center.y);
			}
			else if (cd2.x == center.x && cd1.y == center.y)
			{
			/* Normal orientation. */
				draw_ellipse(center, cd1.x - center.x, cd2.y - center.y);
			}
			else
			{
#ifdef DUMP_CGM
				od("*** BAD ELLIPSE ***\r\n");
#endif
			}
			break;
		}
		case CGM_CIRCLE:
		{
			CGM_POINT center;
			SHORT radius;

		/*
		//				p0.x		top		p1.x
		//
		//		p0.y								p0.y
		//
		//
		//		left								right
		//
		//
		//		p1.y								p1.y
		//
		//				p0.x		 bottom	p1.x
		*/

			read_vdc_point(&center);
			read_vdc(&radius);

			draw_ellipse(center, radius, radius);

			break;
		}
		case CGM_CIRC_ARC_3PT:
		case CGM_CIRC_ARC_3PT_CLOSE:
		{
			CGM_POINT ps, pi, pe;
			SHORT closure = -1;

		/* Read the parameters. */

			read_vdc_point(&ps);				/* Read starting point. */
			read_vdc_point(&pi);				/* Read intermediate point. */
			read_vdc_point(&pe);				/* Read ending point. */
			if (id == CGM_CIRC_ARC_3PT_CLOSE)
			{
				read_enum(&closure);
			}

			draw_elliptical_arc_3pt(ps, pi, pe, closure);

			break;
		}
		case CGM_CIRC_ARC_CENTER:
		case CGM_CIRC_ARC_CENTER_CLOSE:
		{
			CGM_POINT center, start_delta, end_delta;
			SHORT radius;
			SHORT closure = -1;
			double start_angle, end_angle;

		/* Read the parameters. */

			read_vdc_point(&center);
			read_vdc_point(&start_delta);		/* stored as dx, dy */
			read_vdc_point(&end_delta);		/* stored as dx, dy */
			read_vdc(&radius);
			if (id == CGM_CIRC_ARC_CENTER_CLOSE)
			{
				read_enum(&closure);
			}

			/* Compute the angles. */

			start_angle = angle_from_vector(start_delta.x, start_delta.y);
			end_angle = angle_from_vector(end_delta.x, end_delta.y);

		/* Add the arc. */

			draw_elliptical_arc(center, (double)radius, (double)radius, start_angle, end_angle, closure);

			break;
		}
		case CGM_ELLI_ARC:
		case CGM_ELLI_ARC_CLOSE:
		{
			CGM_POINT center, cd1, cd2, start_delta, end_delta;
			SHORT closure = -1;
			SHORT rx, ry;
			double start_angle, end_angle;

		/* Read the parameters. */

			read_vdc_point(&center);
			read_vdc_point(&cd1);
			read_vdc_point(&cd2);
			read_vdc_point(&start_delta);		/* stored as dx, dy */
			read_vdc_point(&end_delta);		/* stored as dx, dy */

			if (id == CGM_ELLI_ARC_CLOSE)
			{
				read_enum(&closure);
			}

		/* Compute the start and end angles. */

			start_angle = angle_from_vector(start_delta.x, start_delta.y);
			end_angle = angle_from_vector(end_delta.x, end_delta.y);

			if (cd1.x == center.x && cd2.y == center.y)
			{
			/* Normal orientation. */
				rx = cd2.x - center.x;
				ry = cd1.y - center.y;
			}
			else if (cd2.x == center.x && cd1.y == center.y)
			{
			/* Normal orientation. */
				rx = cd1.x - center.x;
				ry = cd2.y - center.y;
			}
			else
			{
#ifdef DUMP_CGM
				od("*** BAD ELLIPSE ***\r\n");
#endif
				break;
			}

		/* Add the arc. */

			draw_elliptical_arc(center, (double)rx, (double)ry, start_angle, end_angle, closure);

			break;
		}
#if 0
		case CGM_DISJOINT_POLYLINE:
#endif
		case CGM_POLYMARKER:
		{
		/* Compute how many points we have. */

			length /= (vdc_integer_precision*2);

			draw_polymarker((SHORT)length);

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse an attribute for drawing.
*/

BOOL CLOCAL CGMUpdateState::DRAW_attribute(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_LINE_TYPE:
		{
			SHORT type;

			read_index(&type);
			set_line_type(type);
			break;
		}
		case CGM_LINE_WIDTH:
		{
			SHORT width;

			read_vdc(&width);
			set_line_width(width);
			break;
		}
		case CGM_LINE_COLOR:
		{
			OUTLINE_COLOR color;

			read_cgm_color(&color);
			set_line_color(color);
			break;
		}
		case CGM_MARKER_TYPE:
		{
			SHORT type;

			read_index(&type);

			if (type >= MARKER_RESERVED)
			{
			/* Map it onto a good range. */
				type = MARKER_DOT
						+ ((type - MARKER_RESERVED) % (MARKER_RESERVED-MARKER_DOT));
			}
			set_marker_type(type);
			break;
		}
		case CGM_MARKER_SIZE:
		{
			SHORT size;

			read_vdc(&size);
			set_marker_size(size);
			break;
		}
		case CGM_MARKER_COLOR:
		{
			OUTLINE_COLOR color;

			read_cgm_color(&color);
			set_marker_color(color);
			break;
		}
		case CGM_INTERIOR_STYLE:
		{
			SHORT style;

			read_enum(&style);
			set_interior_style(style);
			break;
		}
		case CGM_FILL_COLOR:
		{
			OUTLINE_COLOR color;

			read_cgm_color(&color);
			set_fill_color(color);
			break;
		}
		case CGM_HATCH_INDEX:
		{
			SHORT index;

			read_index(&index);
			set_hatch_index(index);
			break;
		}
#if 0
		case CGM_TEXT_COLOR:
		{
			CGM_COLOR color;

			read_cgm_color(&color);
			set_text_color(color);
			break;
		}
#endif
		case CGM_EDGE_TYPE:
		{
			SHORT type;

			read_index(&type);
			set_edge_type(type);
			break;
		}
		case CGM_EDGE_WIDTH:
		{
			SHORT width;

			read_vdc(&width);
			set_edge_width(width);
			break;
		}
		case CGM_EDGE_COLOR:
		{
			OUTLINE_COLOR color;

			read_cgm_color(&color);
			set_edge_color(color);
			break;
		}
		case CGM_EDGE_VISIBILITY:
		{
			SHORT vis;

			read_enum(&vis);
			set_edge_visibility(vis);
			break;
		}
#if 0
		case CGM_FILL_REFERENCE_POINT:
		{
			read_vdc_point(&fill_reference);
			break;
		}
		case CGM_LINE_BUNDLE:
		case CGM_MARKER_BUNDLE:
		case CGM_TEXT_BUNDLE:
		case CGM_TEXT_FONT:
		case CGM_FILL_BUNDLE:
		case CGM_PATTERN_INDEX:
		case CGM_EDGE_BUNDLE:
#endif
		case CGM_COLOR_TABLE:
		{
			SHORT index;
			SHORT colors;
			CGM_COLOR far *color;

			if (palette == NULL)
			{
			/* Skip it. */
				break;
			}

		/* Read the starting color index */

			read_cgm_short(&index, color_index_precision);

		/* Compute how many colors are (technically) left. */ 

			colors = (length - color_index_precision)/(3*color_precision);

#ifdef DUMP_CGM
			od("{from %d, %d colors}\r\n", index, colors);
#endif

			color = palette + index;
			while (colors--)
			{
				if (index <= max_color_index)
				{
				/* Read a valid color. */
					read_cgm_triple_color(color);
#ifdef SHOW_COLORS
					if (debug_flags & 1024)
					{
						printf("%d: (%d, %d, %d)\n", index, *color);
					}
#endif
					index++;
					color++;
				}
				else
				{
					break;
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Bump the metafile extent to include the points passed.
// It's assumed that x0 <= x1 and y0 <= y1.
*/

VOID CLOCAL CGMUpdateState::bump_metafile_extent(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT width)
{
#if 0
	if (debug_flags & 1024)
	{
		printf("bump_extent to include (%d, %d)(%d, %d)\n", x0, y0, x1, y1);
	}
#endif

/* Handle the metafile bounds. */

	long pwidth = (width + 1)/2;

	long px0 = (long)x0 - pwidth;
	long py0 = (long)y0 - pwidth;
	long px1 = (long)x1 + pwidth;
	long py1 = (long)y1 + pwidth;

	if (px0 < -32768
			|| py0 < -32768
			|| px1 > 32767
			|| py1 > 32767)
	{
		m_fTooBig = TRUE;
	}
	else
	{
		if (metafile_min.x > (SHORT)px0)
		{
			metafile_min.x = (SHORT)px0;
		}
		if (metafile_min.y > (SHORT)py0)
		{
			metafile_min.y = (SHORT)py0;
		}
		if (metafile_max.x < (SHORT)px1)
		{
			metafile_max.x = (SHORT)px1;
		}
		if (metafile_max.y < (SHORT)py1)
		{
			metafile_max.y = (SHORT)py1;
		}
	}

/* Now handle the extended bounds. */

	width *= WIDTH_SCALE;

	px0 = (long)x0 - width;
	py0 = (long)y0 - width;
	px1 = (long)x1 + width;
	py1 = (long)y1 + width;

	if (px0 < -32768
			|| py0 < -32768
			|| px1 > 32767
			|| py1 > 32767)
	{
		m_fTooBig = TRUE;
	}
	else
	{
		if (extended_min.x > (SHORT)px0)
		{
			extended_min.x = (SHORT)px0;
		}
		if (extended_min.y > (SHORT)py0)
		{
			extended_min.y = (SHORT)py0;
		}
		if (extended_max.x < (SHORT)px1)
		{
			extended_max.x = (SHORT)px1;
		}
		if (extended_max.y < (SHORT)py1)
		{
			extended_max.y = (SHORT)py1;
		}
	}
}

/*
// Size a rectangle.
*/

VOID CLOCAL CGMUpdateState::size_rectangle(CGM_POINT center, SHORT xr, SHORT yr, SHORT width)
{
	bump_metafile_extent(center.x - xr, center.y - yr,
								center.x + xr, center.y + yr, width);
}

/*
// Size an ellipse given the control points.
*/

VOID CLOCAL CGMUpdateState::size_ellipse_control(CGM_POINT center, CGM_POINT cd1, CGM_POINT cd2, SHORT width)
{
	SHORT xr, yr;

	if (cd1.x == center.x && cd2.y == center.y)
	{
		xr = cd2.x - center.x;
		yr = cd1.y - center.y;
	}
	else if (cd2.x == center.x && cd1.y == center.y)
	{
		xr = cd1.x - center.x;
		yr = cd2.y - center.y;
	}
	else
	{
		xr = yr = 0;

#ifdef DUMP_CGM
		od("*** BAD ELLIPSE ***\r\n");
#endif
	}
	if (xr < 0)
	{
		xr = -xr;
	}
	if (yr < 0)
	{
		yr = -yr;
	}
	size_rectangle(center, xr, yr, width);
}

/*
// Parse a graphics primitive for sizing.
*/

BOOL CLOCAL CGMUpdateState::SIZE_primitive(SHORT id, SHORT length)
{
	SHORT width;

	if (!proof)
	{
	/* Hit a primitive. Need to stop since we want quick draft output. */
		return FALSE;
	}

	width = get_edge_width();

	switch (id)
	{
		case CGM_POLYLINE:
		{
			width = get_line_width();
		/* Fall through to... */
		}
		case CGM_RECTANGLE:
		case CGM_POLYGON:
		{
			CGM_POINT p0;

//			od("size poly/rect (%d)\r\n", width);

			length /= (vdc_integer_precision*2);

			while (length--)
			{
				read_vdc_point(&p0);

				bump_metafile_extent(p0.x, p0.y, p0.x, p0.y, width);
			}
			break;
		}
		case CGM_ELLIPSE:
		{
//			od("size ellipse (%d)\r\n", width);

			CGM_POINT center, cd1, cd2;

			read_vdc_point(&center);				/* Read the center. */
			read_vdc_point(&cd1);					/* Endpoint of conjugate diameter 1. */
			read_vdc_point(&cd2);					/* Endpoint of conjugate diameter 2. */

			size_ellipse_control(center, cd1, cd2, width);

			break;
		}
		case CGM_CIRCLE:
		{
			CGM_POINT center;
			SHORT radius;

//			od("size circle (%d)\r\n", width);

		/*
		//				p0.x		top		p1.x
		//
		//		p0.y								p0.y
		//
		//
		//		left								right
		//
		//
		//		p1.y								p1.y
		//
		//				p0.x		 bottom	p1.x
		*/

			read_vdc_point(&center);
			read_vdc(&radius);

			size_rectangle(center, radius, radius, width);

			break;
		}
		case CGM_CIRC_ARC_3PT:
		case CGM_CIRC_ARC_3PT_CLOSE:
		{
			CGM_POINT ps, pi, pe;
			SHORT closure;

//			od("size circ arc 3pt\r\n");

		/* Read the parameters. */

			read_vdc_point(&ps);				/* Read starting point. */
			read_vdc_point(&pi);				/* Read intermediate point. */
			read_vdc_point(&pe);				/* Read ending point. */

			if (id == CGM_CIRC_ARC_3PT_CLOSE)
			{
				read_enum(&closure);
			}
			else
			{
				width = get_line_width();
			}

		/* Check for any generate cases. */

			if ((ps.x == pi.x && pi.x == pe.x)
						|| (ps.y == pi.y && pi.y == pe.y))
			{
				bump_metafile_extent(ps.x, ps.y, ps.x, ps.y, width);
				bump_metafile_extent(pe.x, pe.y, pe.x, pe.y, width);
			}
			else
			{
				CGM_POINT center;
				SHORT radius;

			/*
			// Compute the center of the circle.
			// This routine is based on calculating the center y coordinate.
			// Since the calculation is symmetric, we just reverse everything for
			// the x calculation.
 			*/

				center_from_3pt(ps.y, ps.x, pi.y, pi.x, pe.y, pe.x, &center.x);
				center_from_3pt(ps.x, ps.y, pi.x, pi.y, pe.x, pe.y, &center.y);

			/* Compute the radius. */

				radius = (SHORT)ceil(distance(ps, center)) + width;

				size_rectangle(center, 0, 0, radius);
			}

			break;
		}
		case CGM_CIRC_ARC_CENTER:
		case CGM_CIRC_ARC_CENTER_CLOSE:
		{
			CGM_POINT center, start_delta, end_delta;
			SHORT radius;
			SHORT closure;

//			od("size circ arc center\r\n");

		/* Read the parameters. */

			read_vdc_point(&center);
			read_vdc_point(&start_delta);		/* stored as dx, dy */
			read_vdc_point(&end_delta);		/* stored as dx, dy */
			read_vdc(&radius);

			if (id == CGM_CIRC_ARC_CENTER_CLOSE)
			{
				read_enum(&closure);
			}
			else
			{
				width = get_line_width();
			}

			radius += width;

			size_rectangle(center, 0, 0, radius);

			break;
		}
		case CGM_ELLI_ARC:
		case CGM_ELLI_ARC_CLOSE:
		{
			CGM_POINT center, cd1, cd2, start_delta, end_delta;
			SHORT closure;

//			od("size elli arc\r\n");

		/* Read the parameters. */

			read_vdc_point(&center);
			read_vdc_point(&cd1);
			read_vdc_point(&cd2);
			read_vdc_point(&start_delta);		/* stored as dx, dy */
			read_vdc_point(&end_delta);		/* stored as dx, dy */

			if (id == CGM_ELLI_ARC_CLOSE)
			{
				read_enum(&closure);
			}
			else
			{
				width = get_line_width();
			}

			size_ellipse_control(center, cd1, cd2, width);

			break;
		}
#if 0
		case CGM_DISJOINT_POLYLINE:
#endif
		case CGM_POLYMARKER:
		{
			CGM_POINT center;
			SHORT width;

			width = source_scale(2);

		/* Compute how many points we have. */

			length /= (vdc_integer_precision*2);

			width += get_marker_size()/2;

			while (length--)
			{
			/* Read the center of the marker. */

				read_vdc_point(&center);

				size_rectangle(center, 0, 0, width);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// Parse an attribute for sizing.
*/

BOOL CLOCAL CGMUpdateState::SIZE_attribute(SHORT id, SHORT length)
{
	switch (id)
	{
		case CGM_LINE_WIDTH:
		case CGM_MARKER_SIZE:
		case CGM_EDGE_WIDTH:
		case CGM_EDGE_VISIBILITY:
		{
			return DRAW_attribute(id, length);
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

/*
// CGM parsing table.
*/

CGMUpdateState::CGM_PARSER BASED_CODE cgm_parser[][CGM_RESERVED-CGM_DELIMITER] =
{
	// CGM size mode.
	{
		CGMUpdateState::parser_ignore,					/* DELIMITER */
		CGMUpdateState::DRAW_metafile,					/* METAFILE */
		CGMUpdateState::SIZE_picture,						/* PICTURE */
		CGMUpdateState::DRAW_control,						/* CONTROL */
		CGMUpdateState::SIZE_primitive,					/* PRIMITIVE */
		CGMUpdateState::SIZE_attribute,					/* ATTRIBUTE */
		CGMUpdateState::parser_ignore,					/* ESCAPE */
		CGMUpdateState::parser_ignore						/* EXTERNAL */
	},

	// CGM draw mode.
	{
		CGMUpdateState::DRAW_delimiter,     			/* DELIMITER */
		CGMUpdateState::DRAW_metafile,					/* METAFILE */
		CGMUpdateState::DRAW_picture,						/* PICTURE */
		CGMUpdateState::DRAW_control,						/* CONTROL */
		CGMUpdateState::DRAW_primitive,					/* PRIMITIVE */
		CGMUpdateState::DRAW_attribute,					/* ATTRIBUTE */
		CGMUpdateState::parser_ignore,					/* ESCAPE */
		CGMUpdateState::parser_ignore						/* EXTERNAL */
	}
};

/*
// Process the next cgm record.
// There are different possible modes, so we handle them via the above table.
*/

BOOL CGMUpdateState::parse_next_command(BOOL *primitive)
{
	SHORT command;
	LONG curpos;
	SHORT Class, id, length, l;
	BOOL result;
	ST_DEV_POSITION pos;

/* Read the next command. Align to word boundary as required. */

	file.tell(&pos);
	if (pos & 1)
	{
		file.seek(1L, ST_DEV_SEEK_CUR);
	}

	if (!read_cgm_short(&command, 2))
	{
		return FALSE;
	}

/* Parse the command. */

	Class = (command >> 12) & 0x0F;
	id = (command >> 5) & 0x7F;
	length = command & 0x1F;

	if (Class > CGM_RESERVED)
	{
#ifdef DUMP_CGM
		od("Class: %s (%d),", (LPCSTR)class_names[CGM_RESERVED], Class);
#endif
		Class = CGM_RESERVED;
	}
	else
	{
#ifdef DUMP_CGM
		od("Class: %s,", (LPCSTR)class_names[Class]);
		if (id >= info[Class].last_id)
		{
			od(" id: %d, ", id);
		}
		else
		{
			od(" id: %s, ", (LPCSTR)(info[Class].names[id]));
		}
#endif
	}

	if (length == 0x1f)
	{
		length = 0;

#ifdef DUMP_CGM
		od("(LONG)");
#endif

		do
		{
			if (!read_cgm_short(&l, 2))
			{
				break;
			}
			if (l < 0)
			{
			/* High bit is set. Extension! */
#ifdef DUMP_CGM
				od("(EXTENSION)");
#endif
				file.seek((ST_DEV_POSITION)(l & (SHORT)0x7FFF), ST_DEV_SEEK_CUR);
			}
			length += l & (SHORT)0x7FFF;
		} while (l < 0);
	}
	else
	{
		l = length;
	}
#ifdef DUMP_CGM
	od("length: %d\r\n", length);
#endif

	file.tell(&curpos);

/*
// The following switch used to be jump into a table.
// But, I couldn't figure out a way to put object methods into a table,
// so I did it this way.
*/

	result = TRUE;

	if (Class >= CGM_DELIMITER && Class < CGM_RESERVED)
	{
		if (!(this->*cgm_parser[parse_mode][Class])(id, length))
		{
		/* Stopped for one reason or another. */
			return FALSE;
		}

	}
	file.seek(curpos + (LONG)length, ST_DEV_SEEK_SET);

	*primitive = (Class == CGM_PRIMITIVE);
	return TRUE;
}

ERRORCODE CGMUpdateState::init(GraphicPtr graphic)
{
	BOOL primitive;
	CGMHelperPtr helper = (CGMHelperPtr)graphic->my_little_helper();
	ERRORCODE error;

/* Try to open the file. */

	if ((error = open_outline()) != ERRORCODE_None)
	{
		return error;
	}

/* No discovered name. */

	discovered_name = NULL;

	metafile_min.x = \
	metafile_min.y = \
	extended_min.x = \
	extended_min.y = 0x7FFF;

	metafile_max.x = \
	metafile_max.y = \
	extended_max.x = \
	extended_max.y = -0x7FFF;

	m_fTooBig = FALSE;

/* Parse the file with the intention of sizing. */

#ifndef FIX_CGMS
	file.set_read_buffer(2048);
#endif

	set_defaults();

	while (parse_next_command(&primitive))
		;

	close_outline();

	if (vdc_type != 0
		 || size_extent_x == 0
		 || size_extent_y == 0)
	{
		return ERRORCODE_IllegalType;
	}

   if ((long)metafile_max.x - (long)metafile_min.x > 32767
         || (long)metafile_max.y - (long)metafile_min.y > 32767
         || (long)extended_max.x - (long)extended_min.x > 32767
         || (long)extended_max.y - (long)extended_min.y > 32767)
   {
      m_fTooBig = TRUE;
   }

	if (m_fTooBig)
	{
		// Reset these values. Use the stated VDC.
		metafile_min.x = \
		metafile_min.y = \
		extended_min.x = \
		extended_min.y = 0x7FFF;

		metafile_max.x = \
		metafile_max.y = \
		extended_max.x = \
		extended_max.y = -0x7FFF;
	}
/*
// Setup the colors.
*/

	helper->record.number_of_colors =
				color_selection_mode == COLOR_SELECTION_INDEXED
						? max_color_index+1
						: -1;

/*
// Plug in our discovered bounds.
// If this is not a proof sizing, these values will not be set to anything
// other than as initialized above.
*/

	helper->record.metafile_min = metafile_min;
	helper->record.metafile_max = metafile_max;

#if 0
	od("init_cgm: meta (%d, %d)(%d, %d); actual (%d, %d)(%d, %d); ext (%d, %d, %d, %d)\r\n",
					metafile_min, metafile_max,
					size_origin.x,
 					size_origin.y,
					size_origin.x+size_extent_x,
					size_origin.y+size_extent_y,
					extended_min, extended_max);
#endif

/* Set our extent. */

	if (metafile_min.x >= metafile_max.x
			|| metafile_min.y >= metafile_max.y)
	{
//		printf("USE EXTENT\n");
		extended_max.x = \
		extended_min.x = \
		extended_max.y = \
 		extended_min.y = 0;
	}
	else
	{
//		printf("COMPUTE EXTENT\n");

		size_extent_x = metafile_max.x-metafile_min.x;
		size_extent_y = metafile_max.y-metafile_min.y;

		extended_min.x -= metafile_min.x;
		extended_max.x -= metafile_min.x;
 		extended_min.y -= metafile_min.y;
		extended_max.y -= metafile_min.y;
	}

	helper->record.extended_min = extended_min;
	helper->record.extended_max = extended_max;

	graphic->record.x_size = size_extent_x;
	graphic->record.y_size = size_extent_y;

/* Fit the file into a 2"x2" square. */

	graphic->record.x_resolution = \
	graphic->record.y_resolution = \
 			__max(size_extent_x, size_extent_y)/2;

#if 0
	printf("x: %d, y:%d, xres:%d, yres:%d\n",
						graphic->record.x_size, graphic->record.y_size,
						graphic->record.x_resolution, graphic->record.y_resolution);
#endif

	graphic->record.storage = GRAPHIC_STORAGE_FILE;

/* Handle the name. */

	ASSERT(discovered_name == NULL);

	return ERRORCODE_None;
}

/*
// Initialize a CGM graphic.
*/

ERRORCODE CGMHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	CGMUpdateState ustate(graphic,
								 CGM_PARSE_SIZE,
								 gcs == NULL
										? TRUE
										: gcs->proof);

	// Handle the based device if necessary.
	if (gcs != NULL && gcs->pSourceDevice != NULL)
	{
      StorageFile& File = ustate.GetFile();
      File.clear_name();
		File.set_based_device(gcs->pSourceDevice);
      File.set_subfile();
	}

/* Do the initialization. */

	return ustate.init(graphic);
}

/*
// The constructor for the CGM object.
*/

CGMUpdateState::CGMUpdateState(GraphicPtr data, CGM_PARSE_MODE mode, BOOL prf)
			: OutlineUpdateState(data), palette(NULL), parse_mode(mode), proof(prf)
{
	CGMHelperPtr helper = (CGMHelperPtr)data->my_little_helper();

/* Allocate the CGM palette if not sizing. */

	if (mode == CGM_PARSE_DRAW)
	{
	/* Try to allocate the CGM palette. */

		SHORT colors;

	/* Allocate the palette if we need it. */

		if ((colors = helper->record.number_of_colors) <= 0)
		{
		/* Direct color. Only keep background color. */
			colors = 1;						/* Only background color. */
		}

	//	printf("new palette...");

		TRY
			palette = new CGM_COLOR[colors];
		END_TRY

		if (palette == NULL)
		{
		/* Can't continue! */
//			continue_error(-1, "Not enough memory to paint CGM file.");
		}
	}

/* Plug in the min and max in case we found them. */

	metafile_min.x = helper->record.metafile_min.x;
	metafile_min.y = helper->record.metafile_min.y;
	metafile_max.x = helper->record.metafile_max.x;
	metafile_max.y = helper->record.metafile_max.y;
}

/*
// The destructor for the CGM object.
*/

CGMUpdateState::~CGMUpdateState()
{
	delete [] palette;
}

/*
// Open the outline file.
// If the palette failed earlier, just return error now.
*/

ERRORCODE CGMUpdateState::open_outline(VOID)
{
/* See if we failed somewhere in the constructor. */

	if (parse_mode == CGM_PARSE_DRAW && palette == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Open as normal. */

	return OutlineUpdateState::open_outline();
}

/*
// Read the next point.
// This is used by polygon and polygon code.
*/

BOOL CGMUpdateState::read_next_point(OUTLINE_POINT *p)
{
	read_vdc_point(p);
	return TRUE;
}

VOID CGMUpdateState::set_defaults(VOID)
{
	if (parse_mode == CGM_PARSE_SIZE)
	{
		set_line_width(1);
		set_edge_width(1);
	}
	else
	{
	/* Pass it on. */
		OutlineUpdateState::set_defaults();
	}

/* Color selection defaults to indexed. */

	color_selection_mode = COLOR_SELECTION_INDEXED;

/* Set the default vdc type. */

	vdc_type = 0;

/* Set the precision defaults. */

	vdc_integer_precision =
		integer_precision =
		index_precision = 2;			/* 16 bits */

	color_precision =
		color_index_precision = 1;	/* 8 bits */

/* Set the color extent defaults. */

	max_color_index = 63;

	color_scale[0] =
		color_scale[1] =
		color_scale[2] = FALSE;

	if (palette != NULL)
	{
	/* Background color is white. */
		palette[0].red =
			palette[0].green =
			palette[0].blue = 255;
	}
	size_extent_x = size_extent_y = 0;
}
