#include "Replacement.h"
#include "../lib/Utililty.h"
#include <algorithm>
#include <set>

namespace GA {
	//////////////////////////// ReplacementOp ////////////////////////////
	ReplacementOp* ReplacementOp::create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-R", 0) );
		switch (mode) {
			case Worst:			return new ReplaceWorst(cmdLine);
			case Parent:		return new ReplaceParent(cmdLine);
			case WorstParent:	return new ReplaceWorstParent(cmdLine);
			case Crowding:		return new ReplaceCrowd(cmdLine);
		}
		return NULL;
	}

	void ReplacementOp::replace(Solution::Vector& offsprings, vector<Solution::Pair>& parents, Solution::Vector& population) {
		int nSize = (int)offsprings.size();
		for (int i=0; i<nSize; ++i) {
			Solution::Ptr pOffspring(offsprings[i]);
			this->_remove(pOffspring, parents[i], population);

			Solution::Vector::iterator it = find_if(population.begin(), population.end(), bind1st(SolutionPtrGreater(), pOffspring));
			population.insert(it, pOffspring);
		}
	}

	//////////////////////////// ReplaceWorst ////////////////////////////
	void ReplaceWorst::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		population.pop_back();
	}

	//////////////////////////// ReplaceParent ////////////////////////////
	void ReplaceParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),	bind1st(equal_to<Solution::Ptr>(), parents.first));
		population.erase(it);
	}

	//////////////////////////// ReplaceWorstParent ////////////////////////////
	void ReplaceWorstParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Ptr parent( *(parents.first) < *(parents.second) ? parents.first : parents.second );
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),	bind1st(equal_to<Solution::Ptr>(), parent));
		population.erase(it);
	}

	//////////////////////////// ReplaceCrowding ////////////////////////////
	ReplaceCrowd::ReplaceCrowd(CCmdLine& cmdLine) : ReplacementOp() {
		this->_nCrowdSize = Utility::strTo<int>( cmdLine.GetArgument("-R", 1) );
	}
	void ReplaceCrowd::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		int minDiff = -1;
		int solIdx = -1;
		set<int> indice;
		while(indice.size() != this->_nCrowdSize)
			indice.insert(rand() % population.size());

		set<int>::iterator it = indice.begin();
		for (; it != indice.end(); ++it) {
			int idx = *it;
			int diff = pOffspring->getDistance(population[idx]);
			if (minDiff == -1 || diff < minDiff) {
				minDiff = diff;
				solIdx = idx;
			}
		}
		population.erase(population.begin() + solIdx);
	}
}
