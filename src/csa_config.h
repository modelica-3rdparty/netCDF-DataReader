#ifndef CSA_CONFIG_H
#define CSA_CONFIG_H

#ifdef _WIN32
#ifndef isnan
#define isnan _isnan
static unsigned _int64 lNaN = ((unsigned _int64) 1 << 63) - 1;
#define NaN (*(double*)&lNaN)
#endif
#define copysign _copysign
#define hypot _hypot
#define rint (int)
#define M_PI 3.14159265358979323846
#endif

#ifndef NaN
#define NaN (0.0/0.0)
#endif

#endif /* CSA_CONFIG_H */
