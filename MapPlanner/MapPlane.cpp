#include "stdafx.h"
#include "MapPlane.h"
#include <string>

MapPlane::MapPlane(int w, int h)
{
	width = w;
	height = h;
	threshold = -1;
	data = new uint8_t[w*h];

}

MapPlane::MapPlane(MapPlane& map)
{
	width = map.width;
	height = map.height;
	threshold = map.threshold;
	size_t size = width*height;
	data = new uint8_t[size];
	memcpy((void*)data, (void*)map.data, size);
}

MapPlane::MapPlane(MapPlane* map)
{
	width = map->width;
	height = map->height;
	threshold = map->threshold;
	size_t size = width*height;
	data = new uint8_t[size];
	memcpy((void*)data, (void*)map->data, size);
}

MapPlane::MapPlane(PPM& ppm)
{
	width = ppm.Width();
	height = ppm.Height();
	threshold = 128;
	size_t size = width*height;
	data = new uint8_t[size];
	for (int i = 0; i < width; i++)
		for (int j = 0; j < width; j++)
			set(i, j, ppm.point(i, j).pix[0]);
}

MapPlane::~MapPlane()
{
	delete[] data;
}

uint8_t MapPlane::get(int w, int h) 
{
	if (w < 0 || width <= w || h < 0 || height <= h) {
		throw "MapPlane: subscript out of range";
	}
	return data[h*width + w];
}

void MapPlane::set(int w, int h, uint8_t value)
{
	if (w < 0 || width <= w || h < 0 || height <= h) {
		throw "MapPlane: subscript out of range";
	}
	data[h*width + w] = value;
}

void MapPlane::mask_bright1(int w, int h, int length, uint8_t thres, MapPlane *map)
{
	if (get(w, h) <= thres)
		return;
	int wb = w - length;
	int we = w + length;
	int hb = h - length;
	int he = h + length;
	if (wb < 0) wb = 0;
	if (hb < 0) hb = 0;
	if (we >= width) we = width - 1;
	if (he >= height) he = height - 1;
	for (int i = hb; i <= he; i++) {
		for (int j = wb; j <= we; j++) {
			map->set(j, i, 255);
		}
	}
}

void MapPlane::mask_dark1(int w, int h, int length, uint8_t thres, MapPlane *map)
{
	if (get(w, h) > thres)
		return;
	int wb = w - length;
	int we = w + length;
	int hb = h - length;
	int he = h + length;
	if (wb < 0) wb = 0;
	if (hb < 0) hb = 0;
	if (we >= width) we = width - 1;
	if (he >= height) he = height - 1;
	for (int i = hb; i <= he; i++) {
		for (int j = wb; j <= we; j++) {
			map->set(j, i, 0);
		}
	}

}

MapPlane* MapPlane::mask_brighter(int length, uint8_t thres)
{
	MapPlane* map = new MapPlane(this);
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
			mask_bright1(w, h, length, thres, map);
	map->threshold = thres;
	return map;
}

MapPlane* MapPlane::mask_darker(int length, uint8_t thres)
{
	MapPlane* map = new MapPlane(this);
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
			mask_dark1(w, h, length, thres, map);
	map->threshold = thres;
	return map;
}

PPM* MapPlane::toPPM()
{
	PPM *ppm = new PPM(width, height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			for (int p = 0; p < 3; p++)
				ppm->point(i, j).pix[p] = get(i, j);
		}
	}
	return ppm;
}