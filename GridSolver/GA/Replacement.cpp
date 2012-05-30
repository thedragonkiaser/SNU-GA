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
			case Strugle:		return new ReplaceStrugle(cmdLine);
		}
		return NULL;
	}

	void ReplacementOp::replace(Solution::Vector& offsprings, vector<Solution::Pair>& parents, Solution::Vector& population) {
		int nSize = (int)offsprings.size();
		vector<int> replace;
		replace.reserve(nSize);
		for (int i=0; i<nSize; ++i) {
			Solution::Ptr pOffspring(offsprings[i]);

			bool bReplace = false;
			Solution::Vector::iterator it = find_if(population.begin(), population.end(), bind1st(SolutionPtrEqual(), pOffspring));
			if (it == population.end()) 
				bReplace = this->_remove(pOffspring, parents[i], population);
			replace.push_back(bReplace ? 1 : 0);
		}

		for (int i=0; i<nSize; ++i) {
			Solution::Ptr pOffspring(offsprings[i]);
			if (replace[i]) {
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
		if (it == population.end())
			it = remove_if(population.begin(), population.end(), bind1st(equal_to<Solution::Ptr>(), parents.second));

		if (it == population.end())
			population.pop_back();
		else
			population.erase(it);
		return true;
	}

	//////////////////////////// ReplaceWorstParent ////////////////////////////
	bool ReplaceWorstParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Ptr parent( *(parents.first) < *(parents.second) ? parents.first : parents.second );
		Solution::Ptr another( *(parents.first) < *(parents.second) ? parents.second : parents.first );

		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),	bind1st(equal_to<Solution::Ptr>(), parent));

		if (it == population.end() && *population.front() != *another)
			it = remove_if(population.begin(), population.end(), bind1st(equal_to<Solution::Ptr>(), another));

		if (it != population.end())
			population.erase(it);
		else
			population.pop_back();
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

	//////////////////////////// ReplaceStrugle ////////////////////////////
	bool ReplaceStrugle::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		int minDiff = -1;
		int solIdx = -1;

		int size = population.size();
		for (int i=0; i<size; ++i) {
			int diff = pOffspring->getDistance(population[i]);
			if (minDiff == -1 || diff < minDiff) {
				minDiff = diff;
				solIdx = i;
			}
		}
		
		if ( pOffspring->cost > population[solIdx]->cost )
			population.erase(population.begin() + solIdx);
		else
			return false;

		return true;
	}
}
