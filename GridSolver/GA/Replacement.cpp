#include "Replacement.h"
#include "../lib/Utililty.h"
#include <algorithm>

namespace GA {
	//////////////////////////// ReplacementOp ////////////////////////////
	ReplacementOp* ReplacementOp::create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-R", 0) );
		switch (mode) {
			case Worst:			return new ReplaceWorst(cmdLine);
			case Parent:		return new ReplaceParent(cmdLine);
			case WorstParent:	return new ReplaceWorstParent(cmdLine);
			case Crowding:		return NULL;
		}
		return NULL;
	}

	void ReplacementOp::replace(Solution::Vector& offsprings, vector<Solution::Pair>& parents, Solution::Vector& population) {
		int nSize = (int)offsprings.size();
		for (int i=0; i<nSize; ++i) {
			Solution::Ptr pOffspring(offsprings[i]);
			this->_remove(pOffspring, parents[i], population);

			Solution::Vector::iterator it = population.begin();
			for (; it != population.end(); ++it) {
				if (*pOffspring > **it)
					break;
			}
			population.insert(it, pOffspring);
		}
	}

	//////////////////////////// ReplaceWorst ////////////////////////////
	void ReplaceWorst::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		population.pop_back();
	}

	//////////////////////////// ReplaceParent ////////////////////////////
	void ReplaceParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),
			bind1st(equal_to<Solution::Ptr>(), parents.first));
		population.erase(it);
	}

	//////////////////////////// ReplaceWorstParent ////////////////////////////
	void ReplaceWorstParent::_remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) {
		Solution::Ptr parent( *(parents.first) < *(parents.second) ? parents.first : parents.second );
		Solution::Vector::iterator it = remove_if(population.begin(), population.end(),
			bind1st(equal_to<Solution::Ptr>(), parent));
		population.erase(it);
	}
}
