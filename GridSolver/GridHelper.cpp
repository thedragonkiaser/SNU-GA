#include "GridHelper.h"

#include <string.h>
#include <cstdio>
#include <algorithm>
#include <functional>

using namespace std;

void GridHelper::readPoints()
{
    // allowable numbers from 0 to n-1
    scanf("%d", &this->rows); // ignored
    scanf("%d", &this->columns); // ignored
    scanf("%d", &this->nn);

	// create and zero matrix
	int size = this->nn * this->nn;

	this->_points.clear();
	this->_points.reserve(size);
    this->_points.insert(this->_points.begin(), size, 0);

	this->_used.clear();
	this->_used.reserve(size);
	this->_used.insert(this->_used.begin(), size, 0);

	this->values.clear();
	this->values.reserve(this->nn);
	this->values.insert(this->values.begin(), this->nn, 0);

    // read in point values
    while (true) {
        int i, j;

        scanf("%d", &i);
        if (i<0) break;
        scanf("%d", &j);
        scanf("%d", &this->_points[j * this->nn + i]);

		this->values[i] = i;
		this->values[j] = j;
    }
	this->values.erase( remove_if(this->values.begin(), this->values.end(), bind1st(equal_to<int>(), 0)), this->values.end() );
	sort(values.begin(), values.end());
}

int GridHelper::scoreGrid(GA::Solution::Ptr pSol)
{
	fill(this->_used.begin(), this->_used.end(), 0);

    int score = 0;
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

    if (!this->_used[r * this->nn + c]) {
		if (this->_points[r * this->nn + c]) {
			this->_used[r * this->nn + c] = true;
			return this->_points[r * this->nn + c];
        }
	}
	return 0;
}
