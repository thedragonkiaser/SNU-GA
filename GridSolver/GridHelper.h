#pragma once

#include "GA/GA.h"
#include <vector>
#include <map>
#include <hash_set>
#include <utility>

class GridHelper {
public:
	GridHelper() {};
	virtual ~GridHelper() {};

	void readPoints();
	long long scoreGrid(GA::Solution::Ptr pSol);

	void localOptimization(GA::Solution::Ptr pSol);
	
protected:
	int _getPoints(GA::Solution::Ptr pSol, int x1, int y1, int x2, int y2);

	long long _localOptimization(GA::Solution::Ptr pSol);
	int _findBestMatch(GA::Solution::Ptr pSol, int idx);

public:
	int nRows, nColumns, nMaxIndex;
	std::vector<int> values;

private:
	std::vector<int> _points;
	std::vector<int> _used;
	
	std::vector< std::vector<int> > _pointList;
	std::vector< std::vector<int> > _adjacent;
	std::hash_set<int> _usedLocal;
	std::map<int, long long> _sumLocal;
};
