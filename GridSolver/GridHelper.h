#pragma once

#include "GA\GA.h"
#include <vector>

class GridHelper {
public:
	GridHelper() {
		_points = NULL;
		_used = NULL;
	};
	virtual ~GridHelper() {
		this->cleanup();
	};

	void readPoints();
	int scoreGrid(GA::Solution::Ptr pSol);
	
protected:
	int getPoints(GA::Solution::Ptr pSol, int x1, int y1, int x2, int y2);

	virtual void cleanup() {
		if (_points) {
			for (int i=0; i<nn; i++)
				delete[] _points[i];
			delete[] _points;
			_points = NULL;
		}

		if (_used) {
			for (int i=0; i<nn; i++)
				delete[] _used[i];
			delete[] _used;
			_used = NULL;
		}
	}

private:
	int **_points;
	bool **_used;

public:
	int rows, columns, nn;
	std::vector<int> values;
};


