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
	long long _localOptimization(GA::Solution::Ptr pSol);
	int _findBestMatch(GA::Solution::Ptr pSol, int idx);

public:
	int nRows, nColumns, nMaxIndex;
	std::vector<int> values;

private:
	std::vector<int> _points;
	std::vector<int> _usedPairs;

	std::vector< std::vector<int> > _pairList;
	std::vector< std::vector<int> > _adjacentCells;

	int _localSearchCnt;
	std::vector<int> _localUsedPairs;
	std::map<int, long long> _localPointSum;
};
