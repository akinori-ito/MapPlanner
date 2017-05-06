/*
 * MapPlanner.cpp
 * Provided as a part of MapPlanner Software
 * Author: Akinori Ito
 *            aito@fw.ipsj.or.jp
 * 2017.5.7
 * See LICENCE.txt for license of this software.
 */

#include "stdafx.h"
#include "MapPlane.h"
#include "PathFinder.h"
#include "PPM.h"
#include <iostream>
#include <fstream>

int main()
{
	// �e�X�g
	PPM *org;
	try {
		org = PPM::create("map.ppm");
	}
	catch (const char *mesg) {
		std::cout << mesg << std::endl;
		exit(1);
	}
	MapPlane map(*org);
	MapPlane *newmap = map.mask_darker(3, 200);
	PPM *ppm = newmap->toPPM();
	ppm->write("dark.ppm");

	// ����T��
	try {
		PathFinder pf(newmap);
		std::vector<PathFinder::Position> res = pf.find(414, 616, 349, 88); //pf.find(414, 616, 412, 332);
		for (int i = 0; i < res.size(); i++)
			std::cout << "(" << res.at(i).x << "," << res.at(i).y << ")" << std::endl;
		for (int i = 0; i < res.size(); i++) {
			ppm->point(res[i].x, res[i].y) = PPM::RED;
		}
		ppm->write("Result.ppm");
	}
	catch (char *err) {
		std::cerr << err << std::endl;
	}

    return 0;
}

