/*
 * PathFinder.cpp
 * Provided as a part of MapPlanner Software
 * Author: Akinori Ito
 *            aito@fw.ipsj.or.jp
 * 2017.5.7
 * See LICENCE.txt for license of this software.
 */
#include "stdafx.h"
#include "PathFinder.h"
#include <queue>
#include <functional>
#include <math.h>
#include <iostream>

PathFinder::PathFinder(MapPlane *m)
{
	map = m;
}


PathFinder::~PathFinder()
{
}

static double dist(double x1, double y1, double x2, double y2)
{
	double xd = x1 - x2;
	double yd = y1 - y2;
	return sqrt(xd*xd + yd*yd);
}

bool operator< (const PositionTrace &p, const PositionTrace &q) {
	return p.hscore < q.hscore;
}
bool operator> (const PositionTrace &p, const PositionTrace &q) {
	return p.hscore > q.hscore;
}
bool operator== (const PositionTrace &p, const PositionTrace &q) {
	return p.hscore == q.hscore;
}

static bool pushmap(MapPlane *map,
	std::priority_queue<PositionTrace, std::vector<PositionTrace>, std::greater<PositionTrace>>& queue,
	int xdist, int ydist, int x, int y, double score_incr, PositionTrace *p)
{
	if (map->is_bright(x, y)) {
		queue.push(PositionTrace(x, y, p->score + score_incr, dist(x, y, xdist, ydist), p));
		map->set(x, y, 0);
		return true;
	}
	return false;
}


std::vector<PathFinder::Position>& PathFinder::find(int x1, int y1, int x2, int y2)
{
	if (!map->is_bright(x1, y1)) {
		throw "The origin is out of the movable area";
	}
	if (!map->is_bright(x2, y2)) {
		throw "The destination is out of the movable area";
	}
	std::priority_queue<PositionTrace, std::vector<PositionTrace>, std::greater<PositionTrace>> queue;
	const double sqrt2 = sqrt(2.0);

	// find from the destination to the origin
	queue.push(PositionTrace(x2, y2, 0, 0, nullptr));
	map->set(x2, y2, 0);

	PositionTrace pt;
	PositionTrace *pp = nullptr;
	std::vector<PositionTrace*> travarsed;
	// search start
	while (!queue.empty()) {
		pt = queue.top();
		queue.pop();
		int x = pt.x;
		int y = pt.y;
		pp = new PositionTrace(pt);
		if (x == x1 && y == y1)
			break;
		bool pushed = false;
		pushed |= pushmap(map, queue, x1, y1, x - 1, y - 1, sqrt2, pp);
		pushed |= pushmap(map, queue, x1, y1, x,     y - 1, 1.0,   pp);
		pushed |= pushmap(map, queue, x1, y1, x + 1, y - 1, sqrt2, pp);
		pushed |= pushmap(map, queue, x1, y1, x - 1, y,     1.0,   pp);
		pushed |= pushmap(map, queue, x1, y1, x + 1, y,     1.0,   pp);
		pushed |= pushmap(map, queue, x1, y1, x - 1, y + 1, sqrt2, pp);
		pushed |= pushmap(map, queue, x1, y1, x,     y + 1, 1.0,   pp);
		pushed |= pushmap(map, queue, x1, y1, x - 1, y + 1, sqrt2, pp);
		pt.make_invalid();
		if (pushed)
			travarsed.push_back(pp);
		else
			delete pp;
	}
	if (pt.is_invalid())
		throw "No path";

	// traceback
	std::vector<Position>* res = new std::vector<Position>();
	PositionTrace *ppt = pp;
	while (ppt != nullptr) {
//		std::cout << ppt->x << " , " << ppt->y << "   " << ppt->score <<  std::endl;
		res->push_back(Position(ppt->x, ppt->y));
		ppt = ppt->prev;
	}
	// remove dynamic data
	for (int i = 0; i < travarsed.size(); i++)
		delete travarsed[i];
	return *res;
}

//
// Direction index:
//   0  1  2
//   3     4
//   5  6  7
//
// direction() returns the direction index from (x0,y0) and (x1,y1)
// where the center point is (x0,y0) and the other point is (x1,y1), and
// |x0-x1| <= 1 and |y0-y1| <= 1.
//
static int dirindex[8][2] = {
	{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {1,0}, {-1,1}, {0,1}, {1,1}
};

static int direction(PathFinder::Position p0, PathFinder::Position p1)
{
	int xdiff = p1.x - p0.x;
	int ydiff = p1.y - p0.y;
	for (int i = 0; i < 8; i++) {
		if (xdiff == dirindex[i][0] && ydiff == dirindex[i][1])
			return i;
	}
	// falling to here means that (x1,y1) is not an 8-neighbor of (x0,y0)
	return -1;
}

std::vector<PathFinder::Position>& PathFinder::makeLines(std::vector<PathFinder::Position>& pos)
{
	std::vector<PathFinder::Position> *res = new std::vector<PathFinder::Position>();
	int current_direction = -1;
	if (pos.size() == 0)
		return *res;

	for (int i = 1; i < pos.size()-1; i++) {
		int dir = direction(pos[i - 1], pos[i]);
		if (dir != current_direction) {
			current_direction = dir;
			res->push_back(pos[i - 1]);
		}
	}
	res->push_back(pos[pos.size() - 1]);
	return *res;
}