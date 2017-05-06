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

