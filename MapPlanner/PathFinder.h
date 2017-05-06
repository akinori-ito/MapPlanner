/*
 * PathFinder.cpp
 * Provided as a part of MapPlanner Software
 * Author: Akinori Ito
 *            aito@fw.ipsj.or.jp
 * 2017.5.7
 * See LICENCE.txt for license of this software.
 */
#pragma once
#include <vector>
#include "MapPlane.h"

class PositionTrace {
public:
	int x;
	int y;
	double score;
	double hscore; //score+heuristic
	PositionTrace *prev;
	PositionTrace() {
		x = -1;
		y = -1;
		prev = nullptr;
		score = 0;
		hscore = 0;
	}
	PositionTrace(PositionTrace& pt) {
		x = pt.x;
		y = pt.y;
		prev = pt.prev;
		score = pt.score;
		hscore = pt.hscore;
	}
	PositionTrace(int _x, int _y, double s, double h, PositionTrace *p) {
		x = _x;
		y = _y;
		prev = p;
		score = s;
		hscore = s + h;
	}
	void make_invalid() {
		x = -1;
		y = -1;
	}
	bool is_invalid() {
		return x < 0 || y < 0;
	}
};

class PathFinder
{
public:

	MapPlane *map;
	class Position {
	public:
		int x;
		int y;
		Position() { x = 0; y = 0; }
		Position(int _x, int _y) { x = _x; y = _y; }
	};

	PathFinder(MapPlane *m);
	~PathFinder();
	std::vector<Position>& find(int x1, int y1, int x2, int y2);
};

bool operator< (const PositionTrace &p, const PositionTrace &q); 
bool operator> (const PositionTrace &p, const PositionTrace &q);
bool operator== (const PositionTrace &p, const PositionTrace &q);
