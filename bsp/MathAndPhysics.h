#define G			9.80665f
#define PI 		3.1415926f
#define RadtoDeg    57.324841f
#define DegtoRad    0.0174533f
#define SQRU(Sq)        (((float)Sq)*((float)Sq))
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LOWPASSF(now, pass, fac) pass*(1-fac) + fac*now
float invSqrt(float x);