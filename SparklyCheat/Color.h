#pragma once
typedef unsigned char byte;

class Color
{
public:
	byte r = 0, g = 0, b = 0, a = 255;

	byte* rgba()
	{
		static byte output[4];
		output[0] = r, output[1] = g, output[2] = b, output[3] = a;
		return output;
	}

	Color(byte red, byte green, byte blue, byte alpha = 255)
	{
		r = red, g = green, b = blue, a = alpha;
	}
	Color(byte value, byte alpha = 255)
	{
		r = g = b = value, a = alpha;
	}
};