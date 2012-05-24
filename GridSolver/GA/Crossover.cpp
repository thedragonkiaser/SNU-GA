#include "../lib/Utililty.h"
#include "Crossover.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <cmath>

using namespace std;

namespace GA {
	CrossoverOp* CrossoverOp::create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-X", 0) );

		switch (mode) {
		case BlockUniform:	return new BlockUniformCrossover(cmdLine);
		case Geographic:	return new GeographicCrossover(cmdLine);
		}

		return NULL;
	}

	BlockUniformCrossover::BlockUniformCrossover(CCmdLine& cmdLine)
		: CrossoverOp()
		, _nRowCuts(0)
		, _nColCuts(0) {
		this->_nRowCuts = Utility::strTo<int>( cmdLine.GetArgument("-X", 1) );
		this->_nColCuts = Utility::strTo<int>( cmdLine.GetArgument("-X", 2) );
	}

	Solution::Ptr CrossoverOp::crossover(Solution::Pair& parents) {
		Solution::Ptr p = make_shared<Solution>(parents.first->width, parents.first->height);
		this->_crossover(parents, p);
		return p;
	}
	
	int randomBinary() { return rand() % 2; }
	int zero() {return 0;}

	void BlockUniformCrossover::_crossover(Solution::Pair& parents, Solution::Ptr p) {
		Solution::Ptr parentArray[2] = {parents.first, parents.second};
		generate_n(back_inserter(p->genotype), parents.first->genotype.size(), zero);

		int nRowCuts = this->_nRowCuts;
		if (nRowCuts == 0) nRowCuts = rand() % p->height;
		if (nRowCuts >= p->height) nRowCuts = p->height - 1;
		
		int nColCuts = this->_nColCuts;
		if (nColCuts == 0) nColCuts = rand() % p->width;
		if (nColCuts >= p->width) nColCuts = p->width - 1;

		int nBlockCnt = (nRowCuts+1) * (nColCuts+1);
		vector<int> blocks(nBlockCnt);
		generate(blocks.begin(), blocks.end(), randomBinary);

		set<int> rowCuts;
		while(rowCuts.size() != nRowCuts)
			rowCuts.insert(rand() % (p->height-1) + 1);
		rowCuts.insert(p->height);

		set<int> colCuts;
		while(colCuts.size() != nColCuts)
			colCuts.insert(rand() % (p->width-1) + 1);
		colCuts.insert(p->width);

		int i = 0;
		int from = 0;
		set<int>::iterator it = colCuts.begin();
		for (;it != colCuts.end(); ++it) {
			for (int y=0; y<p->height; ++y) {
				for (int x=from; x<*it; ++x)
					p->set(x, y, i);
			}
			from = *it;
			++i;
		}

		it = rowCuts.begin();
		from = *it;
		it++;
		i = 1;
		for (;it != rowCuts.end(); ++it) {
			for (int y=from; y<*it; ++y) {
				for (int x=0; x<p->width; ++x)
					p->set(x, y, p->get(x, y) + i * (nColCuts + 1));
			}
			from = *it;
			++i;
		}

		for (int i=0; i<p->genotype.size(); ++i)
			p->genotype[i] = parentArray[ blocks[ p->genotype[i] ] ]->genotype[i];
	}
	
	GeographicCrossover::GeographicCrossover(CCmdLine& cmdLine)
		: CrossoverOp()
		, _nCuts(0) {
		this->_nCuts = Utility::strTo<int>( cmdLine.GetArgument("-X", 1) );
	}

	void GeographicCrossover::_crossover(Solution::Pair& parents, Solution::Ptr p) {
		Solution::Ptr parentArray[2] = {parents.first, parents.second};
		int size = parents.first->genotype.size();
		generate_n(back_inserter(p->genotype), size, zero);

		static int MARKER[2] = {HOR, VER};
		static int aDir[DIR][4] = {
			{1, p->width, -1, -p->width},
			{-1, p->width, 1, -p->width},
			{-1, -p->width, 1, p->width},
			{1, -p->width, -1, p->width},
			{0, p->width, 0 , -p->width},
			{1, 0, -1, 0} };

		int w = p->width;
		int h = p->height;
		
		vector<int> cuts(size, 0);
		if (this->_nCuts == 0)
			this->_nCuts = sqrt( sqrt((double)size) );

		for (int n=0; n<this->_nCuts; ++n) {
			int s, t;
			int nDir = rand() % DIR;
			switch (nDir) {
			case TR:
			case TL:
			case TB:
				s = rand() % (w - 1);
				cuts[s] = VER;
				break;
			case BL:
			case BR:
				s = rand() % (w - 1) + w * (h - 2);
				cuts[s + w] = VER;
				break;
			case LR:
				s = (rand() % (h - 1)) * w;
				cuts[s] = HOR;
				break;
			}
			switch (nDir) {
			case TR:
			case BR:
			case LR:
				t = (rand() % (h - 1)) * w + w - 2;
				cuts[t + 1] = HOR;
				break;
			case TL:
			case BL:
				t = (rand() % (h - 1)) * w;
				cuts[t] = HOR;
				break;
			case TB:
				t = rand() % (w - 1) + w * (h - 2);
				cuts[t + w] = VER;
				break;
			}
	
			while (true) {
				int sX = s % w; int sY = s / w;
				int tX = t % w; int tY = t / w;
				if (sX == tX) {
					int base = (sY > tY) ? t : s;
					int iter = abs(tY - sY);
					for (int i=0; i<iter; i += w)
						cuts[base + i + w] |= VER;
					break;
				}
				else if (sY == tY) {
					int base = (sX > tX) ? t : s;
					int iter = abs(tX - sX);
					for (int i=0; i<iter; ++i)
						cuts[base + i + 1] |= HOR;
					break;
				}

				// try move s
				int coin = rand() % 2;
				int nMove = aDir[nDir][coin];
				bool bMoveS = true;
				if (coin == 0) {
					bMoveS &= !(sX == 0 && nMove < 0);
					bMoveS &= !(sX == w - 1 && nMove > 0);
				}
				else {
					bMoveS &= !(sY == 0 && nMove < 0 );
					bMoveS &= !(sY == h - 1 && nMove > 0);
				}

				if (bMoveS) {
					if (nMove > 0) {
						s += nMove;
						cuts[s] |= MARKER[coin];
					}
					else {
						cuts[s] |= MARKER[coin];
						s += nMove;
					}
				}

				// try move t
				coin = rand() % 2;
				nMove = aDir[nDir][coin + 2];
				bool bMoveT = true;
				if (coin == 0) {
					bMoveT &= !(tX == 0 && nMove < 0 );
					bMoveT &= !(tX == w - 1 && nMove > 0);
				}
				else {
					bMoveT &= !(tY == 0 && nMove < 0 );
					bMoveT &= !(tY == h - 1 && nMove > 0);
				}

				if (bMoveT) {
					if (nMove > 0) {
						t += nMove;
						cuts[t] |= MARKER[coin];
					}
					else {
						cuts[t] |= MARKER[coin];
						t += nMove;
					}
				}

				if (!bMoveS && !bMoveT)
					break;
			}
		}

		for (int i=1; i<p->width; ++i)
			p->genotype[i] = (cuts[i-1] & VER) ? 1 - p->genotype[i-1] : p->genotype[i-1];

		int k = p->width;
		for (int i=k; i<size; ++i)
			p->genotype[i] = (cuts[i-k] & HOR) ? 1 - p->genotype[i-k] : p->genotype[i-k];

		for (int i=0; i<size; ++i)
			p->genotype[i] = parentArray[ p->genotype[i] ]->genotype[i];
	}
}
