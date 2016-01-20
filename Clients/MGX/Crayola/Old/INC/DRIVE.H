// Used in drive.c
#define DRIVE_UNIQUE    (DRIVE_REMOVABLE+DRIVE_FIXED+DRIVE_REMOTE)
#ifndef WIN32
#define DRIVE_CDROM     (DRIVE_UNIQUE+1)
#define DRIVE_RAM       (DRIVE_UNIQUE+2)
#endif
