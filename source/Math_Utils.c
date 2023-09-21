#include "Math_Utils.h"
#include <math.h>

FPoint rotate(float theta, FPoint* vec)
{
	return (FPoint){vec->x * cos(theta) - vec->y * sin(theta),
									vec->x * sin(theta) + vec->y * cos(theta)};
}

// root of sum of squared components
float magnitude(FPoint* vec)
{
	return sqrtf(pow(vec->x,2) + pow(vec->y, 2));
}

FPoint normalized(FPoint* vec)
{
	float mag = magnitude(vec);
	return (FPoint){vec->x/mag, vec->y/mag};
}

float dot(const FPoint* left, const FPoint* right)
{
	return (left->x * right->x) + (left->y * right->y);
}

FPoint mult(FPoint* vec, float mag)
{
	return (FPoint){vec->x * mag, vec->y * mag};
}

void mult_left(FPoint* vec, float mag)
{
	vec->x *= mag; vec->y *= mag;
}

FPoint fp_add(FPoint* left, FPoint* right)
{
	return (FPoint){left->x + right->x, left->y + right->y};
}

void fp_add_left(FPoint* left, FPoint* right)
{
	left->x += right->x;
	left->y += right->y;
}

FPoint fp_sub(FPoint* left, FPoint* right)
{
	return (FPoint){left->x - right->x, left->y - right->y};
}

void fp_sub_left(FPoint* left, FPoint* right)
{
	left->x -= right->x;
	left->y -= right->y;
}

