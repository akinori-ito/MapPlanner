/*
 * PPM.cpp
 * Provided as a part of MapPlanner Software
 * Author: Akinori Ito
 *            aito@fw.ipsj.or.jp
 * 2017.5.7
 * See LICENCE.txt for license of this software.
 */
#pragma once
#include <cstdint>
class PPM
{
	static const int FORMAT_PBM = 1;
	static const int FORMAT_PGM = 2;
	static const int FORMAT_PPM = 3;
	static const int FORMAT_PBMRAW = 4;
	static const int FORMAT_PGMRAW = 5;
	static const int FORMAT_PPMRAW = 6;

	int size;
	int width;
	int height;
	int format;
	class pixel {
	public:
		uint8_t pix[3];
		pixel() {
			pix[0] = pix[1] = pix[2] = 0;
		}
		pixel(uint8_t R, uint8_t G, uint8_t B) {
			pix[0] = R;
			pix[1] = G;
			pix[2] = B;
		}
	} *data;
	static const int MyPPM_BSIZE = 1024;
public:

	static const pixel BLACK;
	static const pixel RED;
	static const pixel GREEN;
	static const pixel YELLOW;
	static const pixel BLUE;
	static const pixel MAGENTA;
	static const pixel CYAN;
	static const pixel WHITE;

	PPM() {
		size = width = height = 0;
		format = FORMAT_PPM;
	}
	PPM(int width, int height);
	PPM(PPM& ppm);
	~PPM();
	pixel& point(int x, int y) {
		return data[y*width + x];
	}
	int Width() {
		return width;
	}
	int Height() {
		return height;
	}
	static PPM* create(const char *filename);
	void write(const char *filename);
};

