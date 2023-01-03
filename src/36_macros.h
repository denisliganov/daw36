
#pragma once




#define LIMIT(x, min, max) (x = (x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Make fourCC code
#define MAKE_FOURCC(a,b,c,d) ((((long)a) << 24) | (((long)b) << 16) | (((long)c) << 8) | (((long)d) << 0))


