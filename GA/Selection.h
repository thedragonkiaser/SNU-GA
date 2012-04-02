#if !defined(_SELECTION_H_)
#define _SELECTION_H_

#include "../lib/CmdLine.h"
#include "../lib/util.h"
#include "GA.h"

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
			typename S::QualityType best = population.back()->quality;
			typename S::QualityType worst = population.front()->quality;
			typename S::QualityType C = (worst - best) / (_threshold - 1);

			this->_sum = 0;
			this->_f.reserve(population.size());
			S::Vector::iterator it = population.begin();

			while (it != population.end()) {
				typename S::QualityType fitness = (worst - (*it)->quality) + C;
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
		typename S::QualityType _sum;
		std::vector<typename S::QualityType> _f;

		int _pick() {
			typename S::QualityType r = ((typename S::QualityType)rand() / (typename S::QualityType)RAND_MAX) * _sum;
			typename S::QualityType s = 0;

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
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-S", 1) );
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
			std::vector<int> indice;
			indice.reserve(nSize);
			for (int i=0; i<nSize; ++i)
				indice.push_back(i);

			for (int i=0; i<this->_exp; ++i) {
				std::vector<int> temp;
				temp.reserve( (int)pow((double)2, this->_exp - i - 1) );
				for (int k=0; k<(int)indice.size(); k+=2) {
					int r = rand() % 100;
					if (this->_threshold > r) {
						if (*population[k] < *population[k+1])
							temp.push_back(k+1);
						else
							temp.push_back(k); 
					}
					else {
						if (*population[k] < *population[k+1])
							temp.push_back(k);
						else
							temp.push_back(k+1); 
					}
				}
				indice = temp;
			}
			return indice.front();
		}
	};
}

#endif
