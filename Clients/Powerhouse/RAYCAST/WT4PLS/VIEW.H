/* view.h */

typedef struct {
     fixed x, y, height;
     fixed angle;
     fixed arc;
     fixed view_plane_size;
     fixed eye_distance;
} View;

extern View *new_view(fixed arc);
