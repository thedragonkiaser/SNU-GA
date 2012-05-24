#pragma once

#include "GA/GA.h"
#include <vector>
#include <utility>

class GridHelper {
public:
	GridHelper() {};
	virtual ~GridHelper() {};

	void readPoints();
	long long scoreGrid(GA::Solution::Ptr pSol);
	
protected:
	int getPoints(GA::Solution::Ptr pSol, int x1, int y1, int x2, int y2);

public:
	int nRows, nColumns, nMaxIndex;
	std::vector<int> values;

private:
	std::vector<int> _points;
	std::vector<int> _used;

	std::vector< std::vector<int> > _pointList;
};


