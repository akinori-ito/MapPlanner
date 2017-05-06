#pragma once
#include <cstdint>
#include "PPM.h"

class MapPlane
{
	uint8_t *data;
	void mask_bright1(int w, int h, int length, uint8_t thres, MapPlane *map);
	void mask_dark1(int w, int h, int length, uint8_t thres, MapPlane *map);
public:
	int width;
	int height;
	int threshold; //「明るい」「暗い」を分ける閾値。閾値未設定なら-1

	MapPlane(int w, int h);
	MapPlane(MapPlane& map);
	MapPlane(MapPlane* map);
	MapPlane(PPM& ppm);
	~MapPlane();
	uint8_t get(int w, int h);
	void set(int w, int h, uint8_t value);
	bool is_bright(int w, int h, uint8_t value) {
		if (w < 0 || width <= w || h < 0 || height <= h)
			return false;
		return get(w, h) > value;
	}
	bool is_bright(int w, int h) {
		return is_bright(w, h, threshold);
	}
	// すべての明るい点に対して2*length+1の正方形を畳み込んだマップを返す。
	MapPlane* mask_brighter(int length, uint8_t thres);
	// すべての暗い点に対して2*length+1の正方形を畳み込んだマップを返す。
	MapPlane* mask_darker(int length, uint8_t thres);

	PPM *toPPM();
};


