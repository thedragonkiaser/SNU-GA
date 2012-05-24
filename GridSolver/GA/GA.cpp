#include <algorithm>
#include <assert.h>

#include "GA.h"
#include "Selection.h"
#include "Crossover.h"
#include "Mutation.h"
#include "Replacement.h"
#include "../GridHelper.h"

namespace GA {
	// Constructors
	Solution::Solution()
		: width(0), height(0), cost(0)
	{}

	Solution::Solution(int w, int h)
		: width(w), height(h), cost(0) {
		genotype.reserve( width * height );
	}

	// Methods
	int Solution::get(int x, int y) {
		assert(y * width + x < (int)this->genotype.size());
		return this->genotype[y * width + x];
	}

	void Solution::set(int x, int y, int val) {
		assert(y * width + x < (int)this->genotype.size());
		this->genotype[y * width + x] = val;
	}

	int Solution::getDistance(const Solution& sol) {
		// Hamming Distance
		int dist=0;
		int size = this->genotype.size();
		for (int i=0; i<size; ++i) {
			if (this->genotype[i] != sol.genotype[i])
				++dist;
		}
		return dist;
	}

	int Solution::getDistance(Ptr pSol) {
		return this->getDistance(*pSol);
	}

	// Operators
	bool Solution::operator ==(const Solution& other) const {
		return equal(this->genotype.begin(), this->genotype.end(), other.genotype.begin());
    }
	bool Solution::operator ==(Ptr p) const {
		return *this == *p;
    }
	bool Solution::operator <(const Solution& r) const {
		return this->cost < r.cost;
    }
	bool Solution::operator <(Ptr p) const {
		return *this < *p;
    }

	GAHelper::GAHelper(CCmdLine& cmdLine) {
		this->_selector = SelectionOp::create(cmdLine);
		this->_crossover = CrossoverOp::create(cmdLine);
		this->_mutator = MutationOp::create(cmdLine);
		this->_replacer = ReplacementOp::create(cmdLine);
	}

	Solution::Pair GAHelper::select(Solution::Vector& population) {
		return this->_selector->select(population);
	}

	Solution::Ptr GAHelper::crossover(Solution::Pair& parents) {
		return this->_crossover->crossover(parents);
	}
	bool GAHelper::mutate(Solution::Ptr pSolution, GridHelper *pHelper) {
		return this->_mutator->mutate(pSolution, pHelper);
	}

	void GAHelper::replace(Solution::Vector& offsprings, vector<Solution::Pair>& parents, Solution::Vector& population) {
		this->_replacer->replace(offsprings, parents, population);
	}

	void GAHelper::generateFitness(Solution::Vector& population) {
		this->_selector->generateFitness(population);
	}
}