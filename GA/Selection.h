#if !defined(_SELECTION_H_)
#define _SELECTION_H_

#include "../lib/CmdLine.h"
#include "../lib/util.h"
#include "GA.h"
#include <set>
#include <vector>
#include <cmath>
#include <utility>
#include <cstdlib>

namespace GA {
	template <typename S>
	class BaseSelector {
	public:
		BaseSelector() {}

		virtual typename S::Pair select(typename S::Vector& population) = 0;
	};

	template <typename S>
	class RouletteWheelSelector : public BaseSelector<S> {
	public:
		RouletteWheelSelector(CCmdLine& cmdLine) : BaseSelector<S>() {
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) );
		}
		virtual ~RouletteWheelSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			typename S::CostType best = population.front()->cost;
			typename S::CostType worst = population.back()->cost;
			typename S::CostType C = (worst - best) / (_threshold - 1);

			this->_sum = 0;
			this->_f.clear();
			this->_f.reserve(population.size());

			typename S::Vector::iterator it = population.begin();
			for (;it != population.end(); ++it) {
				typename S::CostType fitness = (worst - (*it)->cost) + C;
				this->_sum += fitness;
				this->_f.push_back(fitness);
			}

			typename S::Ptr pFather = population[this->_pick()];
			typename S::Ptr pMother = population[this->_pick()];

			return make_pair(pFather, pMother);
		}
	protected:
		float _threshold;
		typename S::CostType _sum;
		std::vector<typename S::CostType> _f;

		int _pick() {
			typename S::CostType r = ((typename S::CostType)rand() / (typename S::CostType)RAND_MAX) * _sum;
			typename S::CostType s = 0;

			int nLen = this->_f.size();
			for (int i=0; i<nLen; ++i) {
				s += this->_f[i];
				if (r<=s) return i;
			}
			return 0;
		}
	};

	template <typename S>
	class TournamentSelector : public BaseSelector<S> {
	public:
		TournamentSelector(CCmdLine& cmdLine) : BaseSelector<S>() {
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) ) * 100;
			_exp = MyUtil::strTo<int>( cmdLine.GetArgument("-S", 2) );
		}
		virtual ~TournamentSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			typename S::Ptr pFather = population[this->_pick(population)];
			typename S::Ptr pMother = population[this->_pick(population)];

			return make_pair(pFather, pMother);
		}
	protected:
		float _threshold;
		int _exp;

		int _pick(typename S::Vector& population) {
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
	};

	template <typename S>
	class RankBasedSelector : public RouletteWheelSelector<S> {
	public:
		RankBasedSelector(CCmdLine& cmdLine) : RouletteWheelSelector<S>(cmdLine) {
			this->_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) );
			this->_maxFitness = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 2) );
			this->_minFitness = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 3) );
		}
		virtual ~RankBasedSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			this->_sum = 0;
			this->_f.clear();
			this->_f.reserve(population.size());

			int pSize = (int)population.size();
			typename S::CostType C = (this->_maxFitness - this->_minFitness) / (pSize - 1);
			for (int i=0; i<pSize; ++i) {
				typename S::CostType fitness = this->_maxFitness + i * C;
				this->_sum += fitness;
				this->_f.push_back(fitness);
			}

			typename S::Ptr pFather = population[this->_pick()];
			typename S::Ptr pMother = population[this->_pick()];

			return make_pair(pFather, pMother);
		}
	};

	template <typename S>
	class SelectionOp {
	public:
		enum {
			RouletteWheel = 0,
			Tournament,
			RankBased,
			Sharing
		};
	public:
		static BaseSelector<S>* Create(CCmdLine& cmdLine) {
			int mode = MyUtil::strTo<int>( cmdLine.GetArgument("-S", 0) );
			switch (mode) {
				case RouletteWheel: return new RouletteWheelSelector<S>(cmdLine);
				case Tournament:	return new TournamentSelector<S>(cmdLine);
				case RankBased:		return NULL;
				case Sharing: return NULL;
			}
			return NULL;
		}
	};
}

#endif
