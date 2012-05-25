#include "GridHelper.h"

#include <string.h>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <iterator>
#include <queue>

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

//	this->_usedLocal.reserve(size);
//	this->_usedLocal.insert(this->_usedLocal.begin(), size, 0);

	this->_pointList.reserve(this->nMaxIndex);
	this->_pointList.insert(this->_pointList.begin(), this->nMaxIndex, vector<int>());

//	this->_sumLocal.reserve(this->nMaxIndex);
//	this->_sumLocal.insert(this->_sumLocal.begin(), this->nMaxIndex, 0);

	this->_adjacent.reserve(this->nRows * this->nColumns);
	this->_adjacent.insert(this->_adjacent.begin(), this->nRows * this->nColumns, vector<int>());
	for (int i=0; i<this->nRows; ++i) {
		for (int k=0; k<this->nColumns; ++k) {
			int idx = i * this->nColumns + k;
			if ( i == 0 ) {
				if ( k == 0 ) {	// LT
					this->_adjacent[idx].push_back( idx + 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
					this->_adjacent[idx].push_back( idx + this->nColumns + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// RT
					this->_adjacent[idx].push_back( idx - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
				}
				else {	// T
					this->_adjacent[idx].push_back( idx - 1 );
					this->_adjacent[idx].push_back( idx + 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
					this->_adjacent[idx].push_back( idx + this->nColumns + 1 );
				}
			}
			else if ( i == this->nRows - 1 ) {
				if ( k == 0 ) {	// LB
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacent[idx].push_back( idx + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// RB
					this->_adjacent[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - 1 );
				}
				else {	// B
					this->_adjacent[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacent[idx].push_back( idx - 1 );
					this->_adjacent[idx].push_back( idx + 1 );
				}
			}
			else {
				if ( k == 0 ) { // L
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacent[idx].push_back( idx + 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
					this->_adjacent[idx].push_back( idx + this->nColumns + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// R
					this->_adjacent[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
				}
				else {	// C
					this->_adjacent[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx - this->nColumns );
					this->_adjacent[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacent[idx].push_back( idx - 1 );
					this->_adjacent[idx].push_back( idx + 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacent[idx].push_back( idx + this->nColumns );
					this->_adjacent[idx].push_back( idx + this->nColumns + 1 );
				}
			}
		}
	}
	
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
		this->_pointList[j].push_back(i);
    }
	this->values.erase( remove_if(this->values.begin(), this->values.end(), bind1st(equal_to<int>(), 0)), this->values.end() );
	sort(values.begin(), values.end());
}

long long GridHelper::scoreGrid(GA::Solution::Ptr pSol)
{
	fill(this->_used.begin(), this->_used.end(), 0);

    long long score = 0;

	int size = pSol->genotype.size();
	for (int i=0; i<size; ++i) {
		vector<int>& adjacent = this->_adjacent[i];
		int adjSize = adjacent.size();

		int c = pSol->genotype[i];
		for (int k=0; k<adjSize; ++k) {
			int r = pSol->genotype[adjacent[k]];
			this->_used[ IDX(c, r) ] += 1;
			if ( this->_used[ IDX(c, r) ] == 1 )
				score += this->_points[ IDX(c, r) ];
		}
	}
	
    return score;
}

int GridHelper::_getPoints(GA::Solution::Ptr pSol, int cx, int cy, int rx, int ry)
{
	if ((rx<0) || (rx>=pSol->width) || (ry<0) || (ry>=pSol->height))
        return 0;

    // not previously counted pair
	int c = pSol->get(cx, cy);
	int r = pSol->get(rx, ry);

	this->_used[ IDX(c, r) ] += 1;
    if ( this->_used[ IDX(c, r) ] == 1 ) {
		if (this->_points[ IDX(c, r) ])
			return this->_points[ IDX(c, r) ];
	}
	return 0;
}

void GridHelper::localOptimization(GA::Solution::Ptr pSol) {
	long long cost;
	do {
		cost = pSol->cost;
		pSol->cost = this->_localOptimization(pSol);
	} while (pSol->cost > cost);
}

long long GridHelper::_localOptimization(GA::Solution::Ptr pSol) {
	vector<int> considered;
	considered.reserve(pSol->genotype.size());
	considered.insert(considered.begin(), pSol->genotype.size(), 0);

	int idx = rand() % pSol->genotype.size();

	queue<int> q;
	q.push( idx );
	while ( !q.empty() ) {
		idx = q.front();
		considered[ idx ] = 1;

		vector<int>& adjacent = this->_adjacent[idx];
		int size = adjacent.size();

		int oldVal = pSol->genotype[idx];
		for (int i=0; i<size; ++i) {
			int v = pSol->genotype[ adjacent[i] ];
			this->_used[ IDX( oldVal, v) ] -= 1;
			this->_used[ IDX( v, oldVal) ] -= 1;
		}
		
		int val = this->_findBestMatch(pSol, idx);
		for (int i=0; i<size; ++i) {
			int v = pSol->genotype[ adjacent[i] ];
			this->_used[ IDX( val, v) ] += 1;
			this->_used[ IDX( v, val) ] += 1;

			if ( considered[ adjacent[i] ] == 0 ) {
				q.push( adjacent[i] );
				considered[ adjacent[i] ] = 1;
			}
		}

		pSol->genotype[idx] = val;
		q.pop();
	}
	return this->scoreGrid(pSol);
}

bool map_comp(map<int, long long>::value_type &i1, map<int, long long>::value_type &i2)
{
	return i1.second<i2.second;
}

int GridHelper::_findBestMatch(GA::Solution::Ptr pSol, int idx) {
	this->_usedLocal.clear();
	this->_sumLocal.clear();

	vector<int>& adjacent = this->_adjacent[idx];
	int size = adjacent.size();
	for (int i=0; i<size; ++i) {
		int& va = pSol->genotype[ adjacent[i] ];

		vector<int>& wl = this->_pointList[ va ];
		int kSize = wl.size();
		for ( int k=0; k<kSize; ++k ) {
			int& vc = wl[k];
			if (this->_used[ IDX(vc, va) ] != 0)
				continue;

			int key = vc * 1000 + va;
			hash_set<int>::iterator it = this->_usedLocal.find( key );
			if (it != this->_usedLocal.end() ) {
				this->_sumLocal[ vc ] += this->_points[ IDX(vc, va) ];
				if ( vc != va )
					this->_sumLocal[ vc ] += this->_points[ IDX(vc, va) ];
				this->_usedLocal.insert( key );
			}
		}
	}

	map<int, long long>::iterator it = max_element(this->_sumLocal.begin(), this->_sumLocal.end(), map_comp);
	return it->first;
}