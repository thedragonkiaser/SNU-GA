#pragma once

#include "GA\GA.h"
#include <vector>

class GridHelper {
public:
	GridHelper() {};
	virtual ~GridHelper() {};

	void readPoints();
	int scoreGrid(GA::Solution::Ptr pSol);
	
protected:
	int getPoints(GA::Solution::Ptr pSol, int x1, int y1, int x2, int y2);

public:
	int rows, columns, nn;
	std::vector<int> values;

private:
	std::vector<int> _points;
	std::vector<int> _used;
};


