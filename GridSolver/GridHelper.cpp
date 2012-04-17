#include "GridHelper.h"

#include <string.h>
#include <cstdio>
#include <algorithm>
#include <functional>

using namespace std;

void GridHelper::readPoints()
{
    // allowable numbers from 0 to n-1
    scanf("%d", &rows); // ignored
    scanf("%d", &columns); // ignored
    scanf("%d", &nn);

	this->cleanup();
	// create and zero matrix
    _points = new int *[nn];
    for (int i=0; i<nn; i++)
		_points[i] = new int [nn];
	memset(_points, 0, sizeof(_points));

    _used = new bool *[nn];
    for (int i=0; i<nn; i++)
		_used[i] = new bool [nn];

	values.clear();
	values.reserve(nn);
	values.insert(values.begin(), nn, 0);

    // read in point values
    while (true) {
        int i, j;

        scanf("%d", &i);
        if (i<0) break;
        scanf("%d", &j);
        scanf("%d", &_points[i][j]);

		values[i] = i;
		values[j] = j;
    }
	values.erase( remove_if(values.begin(), values.end(), bind1st(equal_to<int>(), 0)), values.end() );
}

int GridHelper::scoreGrid(GA::Solution::Ptr pSol)
{
    int score;
	memset(_used, false, sizeof(_used));

    score = 0;
    for (int x=0; x<pSol->width; x++) {
        for (int y=0; y<pSol->height; y++) {
            // score the pairs in the neighborhood of (x, y)
            score += getPoints(pSol, x, y, x+1, y-1);
            score += getPoints(pSol, x, y, x+1, y);
            score += getPoints(pSol, x, y, x+1, y+1);
            score += getPoints(pSol, x, y, x, y-1);
            score += getPoints(pSol, x, y, x, y+1);
            score += getPoints(pSol, x, y, x-1, y-1);
            score += getPoints(pSol, x, y, x-1, y);
            score += getPoints(pSol, x, y, x-1, y+1);
        }
    }
	
    return score;
}

int GridHelper::getPoints(GA::Solution::Ptr pSol, int cx, int cy, int rx, int ry)
{
	if ((rx<0) || (rx>=pSol->width) || (ry<0) || (ry>=pSol->height)) {
        return 0;
    } 

    // not previously counted pair
	int c = pSol->get(cx, cy);
	int r = pSol->get(rx, ry);

    if (!_used[c][r]) {
		if (_points[c][r]) {
			_used[c][r] = true;
			return _points[c][r];
        }
	}
	return 0;
}
