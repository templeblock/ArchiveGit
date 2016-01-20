/* slice.h -- Functions to draw wall and floor slices.
**
** Chris Laurel, March 1994
*/


#define TEST_ASM 1

static inline void draw_wall_slice(Pixel *start, Pixel *end,
				   unsigned char *tex_base, fixed tex_y,
				   fixed tex_dy, int fb_width, int tex_width);
