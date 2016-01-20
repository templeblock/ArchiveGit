#ifndef	MONITOR_H
#define	MONITOR_H

typedef struct Monitor{
	double			setup[11];	//add one for scaling factor
/*
	double			gamma;
	double			whitex;
	double			whitey;
	double			redx;
	double 			redy;
	double			greenx;
	double			greeny;
	double			bluex;
	double			bluey;
	double			lightnessscale;
	double			tolerance;
*/
}Monitor;


typedef struct PhotoMonitor {
	short			version;
	short			gamma;
	short			whiteX;
	short			whiteY;
	short			RedX;
	short			RedY;
	short			GreenX;
	short			GreenY;
	short			BlueX;
	short			BlueY;
}PhotoMonitor;  

#endif