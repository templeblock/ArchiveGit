
// Initialize ray caster for a ray
void InitRayCaster(int oMaj, int oMin);

// Client function handles shading, texturing, whatever
typedef void (*SectProc)(int projZ, int surfaceZ, int surfaceX, int sectFrac, int minorDirection);

// Cast the ray, calling the 'sect...()' functions at intersections
void CastRay(int major, int minor, int dmin, int steps, SectProc sectMin, SectProc sectMaj);
// Cast the ray, calling the 'sect...()' functions at major axis intersections
void CastRayMajor(int major, int minor, int dmin, int steps, SectProc sectMaj);

