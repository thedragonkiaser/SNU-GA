#include "Selection.h"

#include "../lib/Utililty.h"

#include <set>
#include <vector>
#include <cmath>
#include <utility>
#include <cstdlib>

using namespace std;

namespace GA {
	//////////////////////////// SelectionOp ////////////////////////////
	SelectionOp* SelectionOp::create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-S", 0) );

		switch (mode) {
		case RouletteWheel: return new RouletteWheelSelector(cmdLine);
		case Tournament:	return new TournamentSelector(cmdLine);
		case RankBased:		return new RankBasedSelector(cmdLine);
		}

		return NULL;
	}

	Solution::Pair SelectionOp::select(Solution::Vector& population) {
		Solution::Ptr p1 = population[this->_select(population)];
		Solution::Ptr p2 = population[this->_select(population)];
		while (p1 == p2)
			p2 = population[this->_select(population)];

		return make_pair(p1, p2);
	}

	//////////////////////////// FinessBasedSelector ////////////////////////////
	void FitnessBasedSelector::generateFitness(Solution::Vector& population) {
		this->_generateFitness(population);
		if (this->_isSharing) this->_sharingFitness(population);
	}

	void FitnessBasedSelector::_sharingFitness(Solution::Vector& population) {
		if (!this->_isSharing)
			return;

		int size = (int)population.size();

		int nLongestDist = 0;
		vector<int> distances(size * size, 0);
		for (int i=0; i<size; ++i) {
			for (int k=0; k<size; ++k) {
				int dist = 0;
				if (k < i)
					dist = distances[k*size + i];
				else if (k > i)
					dist = population[i]->getDistance(population[k]);
				else
					dist = 0;

				if (dist > nLongestDist)
					nLongestDist = dist;
				distances[i*size + k] = dist;
			}
		}
		if (nLongestDist == 0) nLongestDist = 1;
		
		this->_fitnessSum = 0;
		for (int i=0; i<size; ++i) {
			Solution::Ptr pSol = population[i];

			float sum = 0;
			for (int k=0; k<size; ++k) {
				int nDist = distances[i*size + k];
				float ratio = (float)nDist / (float)nLongestDist;
				sum += (1 - (float)pow(ratio, this->_sharingFactor));
			}
			if (sum == 0) sum = 1;
			this->_fitness[i] /= sum;
			this->_fitnessSum += this->_fitness[i];
		}
	}

	int FitnessBasedSelector::_select(Solution::Vector& population) {
		float r = ((float)rand() / (float)RAND_MAX) * this->_fitnessSum;
		float s = 0;

		int nLen = this->_fitness.size();
		for (int i=0; i<nLen; ++i) {
			s += this->_fitness[i];
			if (r<=s) return i;
		}
		return 0;
	}

	//////////////////////////// RouletteWheelSelector ////////////////////////////
	RouletteWheelSelector::RouletteWheelSelector(CCmdLine& cmdLine)
		: FitnessBasedSelector() {
		this->_threshold = Utility::strTo<float>( cmdLine.GetArgument("-S", 1) );
		this->_isSharing = ("1" == cmdLine.GetArgument("-S", 2));
		if (this->_isSharing)
			this->_sharingFactor = Utility::strTo<int>( cmdLine.GetArgument("-S", 3) );
	}

	void RouletteWheelSelector::_generateFitness(Solution::Vector& population) {
		long long best = population.front()->cost;
		long long worst = population.back()->cost;
		float C = abs((worst - best) / (this->_threshold - 1));

		this->_fitnessSum = 0;
		this->_fitness.clear();
		this->_fitness.reserve(population.size());

		Solution::Vector::iterator it = population.begin();
		for (;it != population.end(); ++it) {
			float f = abs( (worst - (*it)->cost) + C );
			this->_fitnessSum += f;
			this->_fitness.push_back(f);
		}
	}
	
	//////////////////////////// RankBasedSelector ////////////////////////////
	RankBasedSelector::RankBasedSelector(CCmdLine& cmdLine)
		: FitnessBasedSelector() {
		this->_maxFitness = Utility::strTo<float>( cmdLine.GetArgument("-S", 1) );
		this->_minFitness = Utility::strTo<float>( cmdLine.GetArgument("-S", 2) );
		this->_isSharing = ("1" == cmdLine.GetArgument("-S", 3));
		if (this->_isSharing)
			this->_sharingFactor = Utility::strTo<int>( cmdLine.GetArgument("-S", 4) );
	}

	void RankBasedSelector::_generateFitness(Solution::Vector& population) {
		int size = (int)population.size();
		this->_fitnessSum = 0;
		this->_fitness.clear();
		this->_fitness.reserve(population.size());

		float C = (this->_minFitness - this->_maxFitness) / (size - 1);
		for (int i=0; i<size; ++i) {
			float f = abs(this->_maxFitness + i * C);
			this->_fitnessSum += f;
			this->_fitness.push_back(f);
		}
	}

	//////////////////////////// TournamentSelector ////////////////////////////
	TournamentSelector::TournamentSelector(CCmdLine& cmdLine)
		: SelectionOp() {
		this->_threshold = Utility::strTo<float>( cmdLine.GetArgument("-S", 1) ) * 100;
		this->_exp = Utility::strTo<int>( cmdLine.GetArgument("-S", 2) );
	}

	int TournamentSelector::_select(Solution::Vector& population) {
		int nSize = (int)pow((double)2, this->_exp);
		while (nSize > (int)population.size()) {
			this->_exp--;
			nSize = (int)pow((double)2, this->_exp);
		}

		std::set<int> indice;
		while (indice.size() != nSize)
			indice.insert( rand() % population.size() );

		for (int i=0; i<this->_exp; ++i) {
			std::set<int> temp;
			std::set<int>::iterator it = indice.begin();
			while (it != indice.end()) {
				int idx1 = *it;	++it;
				int idx2 = *it; ++it;
				int r = rand() % 100;

				if (this->_threshold > r)
					temp.insert( (*population[idx1] < *population[idx2]) ? idx2 : idx1 );
				else
					temp.insert( (*population[idx1] < *population[idx2]) ? idx1 : idx2 );
			}
			indice = temp;
		}
		return *indice.begin();
	}
}
