#if !defined(_SELECTION_H_)
#define _SELECTION_H_

#include "../lib/CmdLine.h"
#include "../lib/util.h"
#include "GA.h"
#include <set>
#include <vector>

namespace GA {
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
		virtual typename S::Pair select(typename S::Vector& population) = 0;

		static SelectionOp<S>* Create(CCmdLine& cmdLine) {
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

	template <typename S>
	class RouletteWheelSelector : public SelectionOp<S> {
	public:
		RouletteWheelSelector(CCmdLine& cmdLine) {
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) );
		}
		virtual ~RouletteWheelSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			typename S::CostType worst = population.front()->cost;
			typename S::CostType best = population.back()->cost;
			typename S::CostType C = (worst - best) / (_threshold - 1);

			this->_sum = 0;
			this->_f.reserve(population.size());
			S::Vector::iterator it = population.begin();

			while (it != population.end()) {
				typename S::CostType fitness = (worst - (*it)->cost) + C;
				this->_sum += fitness;
				this->_f.push_back(fitness);
				it++;
			}

			typename S::Ptr pFather = population[this->_pick()];
			typename S::Ptr pMother = population[this->_pick()];

			return make_pair(pFather, pMother);
		}
	private:
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
	class TournamentSelector : public SelectionOp<S> {
	public:
		TournamentSelector(CCmdLine& cmdLine) {
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) ) * 100;
			_exp = MyUtil::strTo<int>( cmdLine.GetArgument("-S", 2) );
		}
		virtual ~TournamentSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			typename S::Ptr pFather = population[this->_pick(population)];
			typename S::Ptr pMother = population[this->_pick(population)];

			return make_pair(pFather, pMother);
		}
	private:
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
}

#endif
