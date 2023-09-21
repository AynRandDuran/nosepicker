#ifndef __MATH_UTILS__
#define __MATH_UTILS__
#include "structs.h"
#include <math.h>

#define RAD_2_DEG(a) (float)a * (180.0f/M_PI)
#define DEG_2_RAD(a) (float)a * (M_PI/180.0f)

FPoint rotate(float theta, FPoint* vec);
float magnitude(FPoint* vec);
FPoint normalized(FPoint* vec);
float dot(const FPoint* left, const FPoint* right);
FPoint mult(FPoint* vec, float mag);
void mult_left(FPoint* vec, float mag);
FPoint fp_add(FPoint* left, FPoint* right);
void fp_add_left(FPoint* left, FPoint* right);
FPoint fp_sub(FPoint* left, FPoint* right);
void fp_sub_left(FPoint* left, FPoint* right);

#endif //__MATH_UTILS__

