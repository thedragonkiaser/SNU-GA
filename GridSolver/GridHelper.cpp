#include "GridHelper.h"

#include <string.h>
#include <cstdio>

void GridHelper::readPoints()
{
    // allowable numbers from 0 to n-1
    scanf("%d", &xx); // ignored
    scanf("%d", &yy); // ignored
    scanf("%d", &nn);

	this->cleanup();
	// create and zero matrix
    _points = new int *[nn];
    for (int i=0; i<nn; i++)
		_points[i] = new int [nn];
	memset(_points, 0, sizeof(_points));

    _used = new bool *[nn];
    for (int i=0; i<=nn; i++)
		_used[i] = new bool [nn];

    // read in point values
    while (true) {
        int i, j;

        scanf("%d", &i);
        if (i<0) break;
        scanf("%d", &j);
        scanf("%d", &_points[i][j]);
    }
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

int GridHelper::getPoints(GA::Solution::Ptr pSol, int x1, int y1, int x2, int y2)
{
    // not previously counted pair
	int xy1 = pSol->get(x1, y1);
	int xy2 = pSol->get(x2, y2);

    if (!_used[xy1][xy2]) {
		if (_points[xy1][xy2]) {
			_used[xy1][xy2] = true;
			return _points[xy1][xy2];
        }
	}
	return 0;
}
