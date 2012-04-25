#include <algorithm>
#include <assert.h>
#include <iterator>

#include "GA.h"
#include "Selection.h"
#include "Crossover.h"
#include "Mutation.h"
#include "Replacement.h"

namespace GA {
	// Constructors
	Solution::Solution()
		: width(0), height(0), cost(0)
	{}

	int zero() {return 0;}

	Solution::Solution(int w, int h)
		: width(w), height(h), cost(0) {
		genotype.reserve( width * height );
		generate_n(back_inserter(genotype), width * height, zero);
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
	Solution& Solution::operator =(const Solution& rhs) {
		copy(rhs.genotype.begin(), rhs.genotype.end(), this->genotype.begin());
		this->cost = rhs.cost;
		return *this;
	}
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

	Solution::Pair GAHelper::select(Solution::Vector& population, Solution::Pair& parents) {
		return this->_selector->select(population, parents);
	}

	Solution::Ptr GAHelper::crossover(Solution::Pair& parents, Solution::Ptr pSolution) {
		return this->_crossover->crossover(parents, pSolution);
	}
	bool GAHelper::mutate(Solution::Ptr pSolution, int upperBound) {
		return this->_mutator->mutate(pSolution, upperBound);
	}

	void GAHelper::replace(Solution::Vector& offsprings, vector<Solution::Pair>& parents, Solution::Vector& population) {
		this->_replacer->replace(offsprings, parents, population);
	}

	void GAHelper::generateFitness(Solution::Vector& population) {
		this->_selector->generateFitness(population);
	}
}