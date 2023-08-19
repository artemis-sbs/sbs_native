#pragma once 
#include <cmath>

struct HSV
{
	double H;
	double S;
	double V;
};

struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

struct HSL
{
	int H;
	float S;
	float L;
};


struct RGB HSVToRGB(struct HSV hsv);
float HueToRGB(float v1, float v2, float vH);
struct RGB HSLToRGB(struct HSL hsl);