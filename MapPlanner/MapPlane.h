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
	int threshold; //�u���邢�v�u�Â��v�𕪂���臒l�B臒l���ݒ�Ȃ�-1

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
	// ���ׂĂ̖��邢�_�ɑ΂���2*length+1�̐����`����ݍ��񂾃}�b�v��Ԃ��B
	MapPlane* mask_brighter(int length, uint8_t thres);
	// ���ׂĂ̈Â��_�ɑ΂���2*length+1�̐����`����ݍ��񂾃}�b�v��Ԃ��B
	MapPlane* mask_darker(int length, uint8_t thres);

	PPM *toPPM();
};


