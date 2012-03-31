#if !defined(_SELECTION_H_)
#define _SELECTION_H_

#include "../lib/getopt_pp.h"
#include "GA.h"

namespace GA {
	template <typename S>
	class SelectionOp {
	public:
		enum {
			RouletteWheel = 0,
		};
	public:
		virtual typename S::Pair select(typename S::Vector& population) = 0;

		static SelectionOp<S>* Create(GetOpt::GetOpt_pp& ops) {
			return new RouletteWheelSelector<S>(ops);
		}
	};

	template <typename S>
	class RouletteWheelSelector : public SelectionOp<S> {
	public:
		RouletteWheelSelector(GetOpt::GetOpt_pp& ops) {}
		virtual ~RouletteWheelSelector() {}

		typename S::Pair select(typename S::Vector& population) {
			S::Ptr pFather = NULL;
			S::Ptr pMother = NULL;

			return make_pair(pFather, pMother);
		}
	private:
		float _threshold;
	};
}

#endif
