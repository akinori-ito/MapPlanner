/*
 * PPM.cpp
 * Provided as a part of MapPlanner Software
 * Author: Akinori Ito
 *            aito@fw.ipsj.or.jp
 * 2017.5.7
 * See LICENCE.txt for license of this software.
 */
#include "stdafx.h"
#include "PPM.h"
#include <stdio.h>
#include <cstring>
#include <direct.h>


const PPM::pixel PPM::BLACK(0, 0, 0);
const PPM::pixel PPM::RED( 255,0,0 );
const PPM::pixel PPM::GREEN( 0,255,0 );
const PPM::pixel PPM::YELLOW( 255,255,0 );
const PPM::pixel PPM::BLUE( 0,0,255 );
const PPM::pixel PPM::MAGENTA( 255,0,255 );
const PPM::pixel PPM::CYAN( 0,255,255 );
const PPM::pixel PPM::WHITE( 255,255,255 );


PPM::PPM(int w, int h)
{
	width = w;
	height = h;
	size = width*height;
	format = FORMAT_PPM;
	data = new pixel[size];
}

PPM::PPM(PPM& ppm)
{
	size = ppm.size;
	width = ppm.width;
	height = ppm.height;
	format = ppm.format;
	memcpy(data, ppm.data, size);
}

PPM::~PPM()
{
	delete[] data;
}

static char *
MyPPM_readline(char *buf, int size, FILE *f)
{
	for (;;) {
		if (fgets(buf, size, f) == NULL)
			return NULL;
		if (buf[0] != '#')
			return buf;
	}
}


PPM* PPM::create(const char *filename)
{
	PPM *ppm;
	char buffer[MyPPM_BSIZE];
	FILE *f;
	int x, y, i;
	errno_t err;

	if ((err = fopen_s(&f,filename, "r")) != 0) {
//		char errbuffer[1024];
//		strerror_s(errbuffer, err);
		throw "Cannot open file";
	}
	ppm = new PPM();
	/* read type */
	if (MyPPM_readline(buffer, MyPPM_BSIZE, f) == NULL)
		return NULL;
	if (strcmp(buffer, "P2\n") == 0)
		ppm->format = FORMAT_PGM;
	else if (strcmp(buffer, "P3\n") == 0)
		ppm->format = FORMAT_PPM;
	else if (strcmp(buffer, "P5\n") == 0)
		ppm->format = FORMAT_PGMRAW;
	else if (strcmp(buffer, "P6\n") == 0)
		ppm->format = FORMAT_PPMRAW;
	else {
		delete ppm;
		throw "PPM format mismatch\n";
	}
	/* read width&height */
	for (;;) {
		if (MyPPM_readline(buffer, MyPPM_BSIZE, f) == NULL)
			throw "PPM read error (width&height)";
		if (buffer[0] == '\n' || buffer[0] == '#')
			continue;
		break;
	}
	if (sscanf_s(buffer, "%d%d", &ppm->width, &ppm->height) != 2) {
		throw "PPM size bad\n";
	}
	ppm->size = ppm->width*ppm->height;
	/* read depth */
	if (MyPPM_readline(buffer, MyPPM_BSIZE, f) == NULL)
		throw "PPM read error (depth)";
	if (strcmp(buffer, "255\n") != 0) {
		throw "PPM depth unsupported\n";
	}
	ppm->data = new pixel[ppm->width*ppm->height];
	for (y = 0; y < ppm->height; y++) {
		for (x = 0; x < ppm->width; x++) {
			int pix;
			switch (ppm->format) {
			case FORMAT_PGM:
				fscanf_s(f, "%d", &pix);
				for (i = 0; i < 3; i++)
					ppm->point(x, y).pix[i] = pix;
				break;
			case FORMAT_PPM:
				for (i = 0; i < 3; i++) {
					fscanf_s(f, "%d", &pix);
					ppm->point(x, y).pix[i] = pix;
				}
				break;
			case FORMAT_PGMRAW:
				pix = fgetc(f);
				for (i = 0; i < 3; i++)
					ppm->point(x, y).pix[i] = pix;
				break;
			case FORMAT_PPMRAW:
				for (i = 0; i < 3; i++) {
					pix = fgetc(f);
					ppm->point(x, y).pix[i] = pix;
				}
				break;
			}
		}
	}
	fclose(f);
	return ppm;
}

void PPM::write(const char *filename)
{
	FILE *f;
	int x, y, i;

	if (fopen_s(&f,filename, "w"), f == NULL)
		throw "PPM::write(): cannot open file";
	fprintf(f, "P%d\n", format);
	fprintf(f, "%d %d\n", width, height);
	fprintf(f, "255\n");
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			switch (format) {
			case FORMAT_PGM:
				fprintf(f, "%d\n", point(x, y).pix[0]);
				break;
			case FORMAT_PPM:
				for (i = 0; i < 3; i++) {
					fprintf(f, "%d\n", point(x, y).pix[i]);
				}
				break;
			case FORMAT_PGMRAW:
				fputc(point(x, y).pix[0], f);
				break;
			case FORMAT_PPMRAW:
				for (i = 0; i < 3; i++) {
					fputc(point(x, y).pix[i], f);
				}
				break;
			}
		}
	}
	fclose(f);
}

void PPM::drawLine(int x1, int y1, int x2, int y2, PPM::pixel pix)
{
	if (x1 == x2) {
		int y;
		if (y1 > y2) {
			y = y1; y1 = y2; y2 = y;
		}
		for (y = y1; y <= y2; y++) {
			point(x1, y) = pix;
		}
	}
	else {
		int x, y;
		if (x1 > x2) {
			x = x1; x1 = x2; x2 = x;
			y = y1; y1 = y2; y2 = y;
		}
		int diff = x2 - x1;
		double a = (double)(y2 - y1) / diff;
		for (int i = 0; i <= diff; i++) {
			double y = a*i + y1;
			point(x1 + i, (int)y) = pix;
		}

	}
}