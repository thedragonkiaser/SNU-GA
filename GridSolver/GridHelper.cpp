#include "GridHelper.h"

#include <string.h>
#include <cstdio>
#include <algorithm>
#include <functional>

using namespace std;

#define IDX(i, j)	j * this->nMaxIndex + i

void GridHelper::readPoints()
{
    // allowable numbers from 0 to n-1
    scanf_s("%d", &this->nRows, sizeof(int)); // ignored
    scanf_s("%d", &this->nColumns, sizeof(int)); // ignored
    scanf_s("%d", &this->nMaxIndex, sizeof(int));

	// create and zero matrix
	int size = this->nMaxIndex * this->nMaxIndex;

	this->_points.reserve(size);
    this->_points.insert(this->_points.begin(), size, 0);
	
	this->_used.reserve(size);
	this->_used.insert(this->_used.begin(), size, 0);

	this->_pointList.reserve(this->nMaxIndex);
	this->_pointList.insert(this->_pointList.begin(), this->nMaxIndex, vector<int>());
	
	this->values.reserve(this->nMaxIndex);
	this->values.insert(this->values.begin(), this->nMaxIndex, 0);

    // read in point values
    while (true) {
        int i, j;

        scanf_s("%d", &i, sizeof(int));
        if (i<0) break;
        scanf_s("%d", &j, sizeof(int));
        scanf_s("%d", &this->_points[ IDX(i, j) ], sizeof(int));

		this->values[i] = i;
		this->values[j] = j;

		this->_pointList[i].push_back(j);
    }
	this->values.erase( remove_if(this->values.begin(), this->values.end(), bind1st(equal_to<int>(), 0)), this->values.end() );
	sort(values.begin(), values.end());
}

long long GridHelper::scoreGrid(GA::Solution::Ptr pSol)
{
	fill(this->_used.begin(), this->_used.end(), 0);

    long long score = 0;
    for (int x=0; x<pSol->width; x++) {
        for (int y=0; y<pSol->height; y++) {
            // score the pairs in the neighborhood of (x, y)
            score += this->getPoints(pSol, x, y, x+1, y-1);
            score += this->getPoints(pSol, x, y, x+1, y);
            score += this->getPoints(pSol, x, y, x+1, y+1);
            score += this->getPoints(pSol, x, y, x, y-1);
            score += this->getPoints(pSol, x, y, x, y+1);
            score += this->getPoints(pSol, x, y, x-1, y-1);
            score += this->getPoints(pSol, x, y, x-1, y);
            score += this->getPoints(pSol, x, y, x-1, y+1);
        }
    }
	
    return score;
}

int GridHelper::getPoints(GA::Solution::Ptr pSol, int cx, int cy, int rx, int ry)
{
	if ((rx<0) || (rx>=pSol->width) || (ry<0) || (ry>=pSol->height))
        return 0;

    // not previously counted pair
	int c = pSol->get(cx, cy);
	int r = pSol->get(rx, ry);

    if (!this->_used[r * this->nMaxIndex + c]) {
		if (this->_points[r * this->nMaxIndex + c]) {
			this->_used[r * this->nMaxIndex + c] = true;
			return this->_points[r * this->nMaxIndex + c];
        }
	}
	return 0;
}
