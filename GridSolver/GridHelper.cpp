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
    scanf("%d", &this->nRows); // ignored
    scanf("%d", &this->nColumns); // ignored
    scanf("%d", &this->nMaxIndex);

	int bufSize = this->nMaxIndex * this->nMaxIndex;
	this->_points.reserve(bufSize);
	this->_points.insert(this->_points.begin(), bufSize, 0);

	this->_usedPairs.reserve(bufSize);
	this->_usedPairs.insert(this->_usedPairs.begin(), bufSize, 0);

	this->_localSearchCnt = 0;
	this->_localUsedPairs.reserve(bufSize);
	this->_localUsedPairs.insert(this->_localUsedPairs.begin(), bufSize, 0);

	this->_localSearchCnt2 = 0;
	this->_localSearchConsidered.reserve(this->nRows * this->nColumns);
	this->_localSearchConsidered.insert(this->_localSearchConsidered.begin(), (this->nRows * this->nColumns), 0);
	
	this->_pairList.reserve(this->nMaxIndex);
	this->_pairList.insert(this->_pairList.begin(), this->nMaxIndex, vector<int>());

	this->_localPointSum.reserve(this->nMaxIndex);
	this->_localPointSum.insert(this->_localPointSum.begin(), this->nMaxIndex, 0);
	
	this->_adjacentCells.reserve(this->nRows * this->nColumns);
	this->_adjacentCells.insert(this->_adjacentCells.begin(), this->nRows * this->nColumns, vector<int>());
	for (int i=0; i<this->nRows; ++i) {
		for (int k=0; k<this->nColumns; ++k) {
			int idx = i * this->nColumns + k;
			if ( i == 0 ) {
				if ( k == 0 ) {	// LT
					this->_adjacentCells[idx].push_back( idx + 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
					this->_adjacentCells[idx].push_back( idx + this->nColumns + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// RT
					this->_adjacentCells[idx].push_back( idx - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
				}
				else {	// T
					this->_adjacentCells[idx].push_back( idx - 1 );
					this->_adjacentCells[idx].push_back( idx + 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
					this->_adjacentCells[idx].push_back( idx + this->nColumns + 1 );
				}
			}
			else if ( i == this->nRows - 1 ) {
				if ( k == 0 ) {	// LB
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacentCells[idx].push_back( idx + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// RB
					this->_adjacentCells[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - 1 );
				}
				else {	// B
					this->_adjacentCells[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacentCells[idx].push_back( idx - 1 );
					this->_adjacentCells[idx].push_back( idx + 1 );
				}
			}
			else {
				if ( k == 0 ) { // L
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacentCells[idx].push_back( idx + 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
					this->_adjacentCells[idx].push_back( idx + this->nColumns + 1 );
				}
				else if ( k == this->nColumns - 1 ) {	// R
					this->_adjacentCells[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
				}
				else {	// C
					this->_adjacentCells[idx].push_back( idx - this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx - this->nColumns );
					this->_adjacentCells[idx].push_back( idx - this->nColumns + 1 );
					this->_adjacentCells[idx].push_back( idx - 1 );
					this->_adjacentCells[idx].push_back( idx + 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns - 1 );
					this->_adjacentCells[idx].push_back( idx + this->nColumns );
					this->_adjacentCells[idx].push_back( idx + this->nColumns + 1 );
				}
			}
		}
	}
	
	this->values.reserve(this->nMaxIndex);
	this->values.insert(this->values.begin(), this->nMaxIndex, 0);

    // read in point values
    while (true) {
        int i, j;

        scanf("%d", &i);
        if (i<0) break;
        scanf("%d", &j);
        scanf("%d", &this->_points[ IDX(i, j) ]);

		this->values[i] = i;
		this->values[j] = j;

		this->_pairList[i].push_back(j);
		this->_pairList[j].push_back(i);
    }
	this->values.erase( remove_if(this->values.begin(), this->values.end(), bind1st(equal_to<int>(), 0)), this->values.end() );
	std::sort(values.begin(), values.end());
}

long long GridHelper::scoreGrid(GA::Solution::Ptr pSol)
{
	memset(&this->_usedPairs[0], 0, sizeof(this->_usedPairs[0]) * this->_usedPairs.size());

    long long score = 0;

	int size = pSol->genotype.size();
	for (int i=0; i<size; ++i) {
		int c = pSol->genotype[i];

		vector<int>& adjacent = this->_adjacentCells[i];
		int adjSize = adjacent.size();

		for (int k=0; k<adjSize; ++k) {
			int r = pSol->genotype[ adjacent[k] ];

			this->_usedPairs[ IDX(c, r) ] += 1;
			if ( this->_usedPairs[ IDX(c, r) ] == 1)
				score += this->_points[ IDX(c, r) ];
		}
	}
	
    return score;
}

void GridHelper::localOptimization(GA::Solution::Ptr pSol) {
	if ( this->_localSearchCnt > 2100000000 ) {
		this->_localSearchCnt = 0;
		memset( &this->_localUsedPairs[0], 0, sizeof(this->_localUsedPairs[0]) * this->_localUsedPairs.size() );
	}

	long long cost;
	do {
		cost = pSol->cost;
		pSol->cost = this->_localOptimization(pSol);
	} while (pSol->cost > cost);
}

long long GridHelper::_localOptimization(GA::Solution::Ptr pSol) {
	++this->_localSearchCnt2;

	int idx = rand() % pSol->genotype.size();

	queue<int> q;
	q.push( idx );
	this->_localSearchConsidered[ idx ] = this->_localSearchCnt2;

	while ( !q.empty() ) {
		idx = q.front();

		vector<int>& adjacent = this->_adjacentCells[idx];
		int size = adjacent.size();

		int oldVal = pSol->genotype[idx];
		for (int i=0; i<size; ++i) {
			int v = pSol->genotype[ adjacent[i] ];
			this->_usedPairs[ IDX( oldVal, v) ] -= 1;
			this->_usedPairs[ IDX( v, oldVal) ] -= 1;
		}
		
		int val = this->_findBestMatch(pSol, idx);
		for (int i=0; i<size; ++i) {
			int v = pSol->genotype[ adjacent[i] ];
			this->_usedPairs[ IDX( val, v) ] += 1;
			this->_usedPairs[ IDX( v, val) ] += 1;

			if ( this->_localSearchConsidered[ adjacent[i] ] < this->_localSearchCnt2 ) {
				q.push( adjacent[i] );
				this->_localSearchConsidered[ adjacent[i] ] = this->_localSearchCnt2;
			}
		}

		pSol->genotype[idx] = val;
		q.pop();
	}
	return this->scoreGrid(pSol);
}

int GridHelper::_findBestMatch(GA::Solution::Ptr pSol, int idx) {
	++this->_localSearchCnt;

	vector<int>& adjacent = this->_adjacentCells[idx];
	int size = adjacent.size();
	for (int i=0; i<size; ++i) {
		int va = pSol->genotype[ adjacent[i] ];

		vector<int>& wl = this->_pairList[ va ];
		int kSize = wl.size();
		for ( int k=0; k<kSize; ++k ) {
			int vc = wl[k];
			if (this->_usedPairs[ IDX(vc, va) ] == 0 && this->_localUsedPairs[ IDX(vc, va) ] < this->_localSearchCnt) {
				if ( vc != va )
					this->_localPointSum[ vc ] += ( this->_points[ IDX(va, vc) ] + this->_points[ IDX(vc, va) ] );
				else
					this->_localPointSum[ vc ] += this->_points[ IDX(vc, va) ];

				this->_localUsedPairs[ IDX(vc, va) ] = this->_localSearchCnt;
			}
		}
	}

	int maxIdx = 0;
	long long maxSum = 0;
	for (int i=0; i<this->nMaxIndex; ++i) {
		if (this->_localPointSum[i] != 0) {
			if (this->_localPointSum[i] > maxSum) {
				maxSum = this->_localPointSum[i];
				maxIdx = i;
			}
			this->_localPointSum[i] = 0;
		}
	}

	return maxIdx;
}

void GridHelper::setPairTable(GA::Solution::Ptr pSol) {
	int size = this->nMaxIndex * this->nMaxIndex;
	pSol->pairs.reserve(size);
	pSol->pairs.insert(pSol->pairs.begin(), size, 0);

	
	int ssize = pSol->genotype.size();
	for (int i=0; i<ssize; ++i) {
		int c = pSol->genotype[i];

		vector<int>& adjacent = this->_adjacentCells[i];
		int adjSize = adjacent.size();

		for (int k=0; k<adjSize; ++k) {
			int r = pSol->genotype[ adjacent[k] ];
			if (this->_points[ IDX(c, r) ] > 0)
				pSol->pairs[ IDX(c, r)  ] = 1;
				//pSol->pairs[ IDX(c, r)  ] += 1;
		}
	}
}
