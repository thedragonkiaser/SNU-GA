#include "Mutation.h"
#include "../lib/Utililty.h"


namespace GA {
	MutationOp* MutationOp::create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-M", 0) );
		switch (mode) {
		case Uniform:		return new UniformMutation(cmdLine);
		case NonUniform:	return NULL;
		}
		return NULL;
	}

	MutationOp::MutationOp(CCmdLine& cmdLine) {
		_threshold = (int)(Utility::strTo<float>( cmdLine.GetArgument("-M", 1) ) * 10000);
	}

	bool MutationOp::mutate(Solution::Ptr pSolution, vector<int>& geneSet, float progressRatio) {
		bool bMutated = false;
		vector<int>::iterator it = pSolution->genotype.begin();
		for(; it != pSolution->genotype.end(); ++it) {
			if (rand()%10000 < this->_threshold) {
				*it = this->_mutate(*it, geneSet, progressRatio);
				bMutated = true;
			}
		}
		return bMutated;
	}

	int UniformMutation::_mutate(int gene, vector<int>& geneSet, float progressRatio) {
		return geneSet[ rand() % geneSet.size() ];
	}
}
