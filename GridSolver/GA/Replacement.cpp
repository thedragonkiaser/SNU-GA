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

			Solution::Vector::iterator it = find_if(population.begin(), population.end(), bind1st(SolutionPtrEqual(), pOffspring));
			if (it != population.end())
				continue;

			bool bReplace = this->_remove(pOffspring, parents[i], population);
			if (bReplace) {
				Solution::Vector::iterator it = find_if(population.begin(), population.end(), bind1st(SolutionPtrGreater(), pOffspring));
				population.insert(it, pOffspring);
			}
		}
	}

	//////////////////////////// ReplaceWorst ////////////////////////////
	bool ReplaceWorst::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		population.pop_back();
		return true;
	}

	//////////////////////////// ReplaceParent ////////////////////////////
	bool ReplaceParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {			
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),	bind1st(equal_to<Solution::Ptr>(), parents.first));
		population.erase(it);
		return true;
	}

	//////////////////////////// ReplaceWorstParent ////////////////////////////
	bool ReplaceWorstParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Ptr parent( *(parents.first) < *(parents.second) ? parents.first : parents.second );
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),	bind1st(equal_to<Solution::Ptr>(), parent));
		population.erase(it);
		return true;
	}

	//////////////////////////// ReplaceCrowding ////////////////////////////
	ReplaceCrowd::ReplaceCrowd(CCmdLine& cmdLine) : ReplacementOp() {
		this->_nCrowdSize = Utility::strTo<int>( cmdLine.GetArgument("-R", 1) );
		this->_bReplaceOnlyBetter = Utility::strTo<int>( cmdLine.GetArgument("-R", 2) ) != 0;
	}

	bool ReplaceCrowd::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		int minDiff = -1;
		int solIdx = -1;

		if (population.size() == this->_nCrowdSize) {
			int size = population.size();
			for (int i=0; i < size; ++i) {
				int diff = pOffspring->getDistance(population[i]);
				if (minDiff == -1 || diff < minDiff) {
					minDiff = diff;
					solIdx = i;
				}
			}
		}
		else {
			set<int> indice;
			while(indice.size() != this->_nCrowdSize)
				indice.insert(rand() % (population.size() - 1) + 1);

			set<int>::iterator it = indice.begin();
			for (; it != indice.end(); ++it) {
				int idx = *it;
				int diff = pOffspring->getDistance(population[idx]);
				if (minDiff == -1 || diff < minDiff) {
					minDiff = diff;
					solIdx = idx;
				}
			}
		}

		if (this->_bReplaceOnlyBetter) {
			if ( pOffspring->cost > population[solIdx]->cost )
				population.erase(population.begin() + solIdx);
			else
				return false;
		}
		else 
			population.erase(population.begin() + solIdx);

		return true;
	}
}
