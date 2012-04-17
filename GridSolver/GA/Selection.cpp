#pragma once

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
	SelectionOp* SelectionOp::Create(CCmdLine& cmdLine) {
		int mode = Utility::strTo<int>( cmdLine.GetArgument("-S", 0) );

		switch (mode) {
		case RouletteWheel: return new RouletteWheelSelector(cmdLine);
		case Tournament:	return new TournamentSelector(cmdLine);
		case RankBased:		return new RankBasedSelector(cmdLine);
		case Sharing: return NULL;
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

	//////////////////////////// RouletteWheelSelector ////////////////////////////
	RouletteWheelSelector::RouletteWheelSelector(CCmdLine& cmdLine)
		: SelectionOp() {
		this->_threshold = Utility::strTo<float>( cmdLine.GetArgument("-S", 1) );
	}

	Solution::Pair RouletteWheelSelector::select(Solution::Vector& population) {
		int best = population.front()->cost;
		int worst = population.back()->cost;
		float C = abs((worst - best) / (this->_threshold - 1));

		this->_sumFitness = 0;
		this->_fitness.clear();
		this->_fitness.reserve(population.size());

		Solution::Vector::iterator it = population.begin();
		for (;it != population.end(); ++it) {
			float f = abs( (worst - (*it)->cost) + C );
			this->_sumFitness += f;
			this->_fitness.push_back(f);
		}

		return SelectionOp::select(population);
	}

	int RouletteWheelSelector::_select(Solution::Vector& population) {
		float r = ((float)rand() / (float)RAND_MAX) * this->_sumFitness;
		float s = 0;

		int nLen = this->_fitness.size();
		for (int i=0; i<nLen; ++i) {
			s += this->_fitness[i];
			if (r<=s) return i;
		}
		return 0;
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

	RankBasedSelector::RankBasedSelector(CCmdLine& cmdLine)
		: RouletteWheelSelector(cmdLine) {
		this->_maxFitness = Utility::strTo<float>( cmdLine.GetArgument("-S", 1) );
		this->_minFitness = Utility::strTo<float>( cmdLine.GetArgument("-S", 2) );
	}

	Solution::Pair RankBasedSelector::select(Solution::Vector& population) {
		this->_sumFitness = 0;
		int size = (int)population.size();

		this->_fitness.clear();
		this->_fitness.reserve(size);
		float C = (this->_minFitness - this->_maxFitness) / (size - 1);
		for (int i=0; i<size; ++i) {
			float f = this->_maxFitness + i * C;;
			this->_sumFitness += f;
			this->_fitness.push_back(f);
		}

		return SelectionOp::select(population);
	}
}
