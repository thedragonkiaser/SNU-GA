#if !defined(_CROSSOVER_H_)
#define _CROSSOVER_H_

#include "../lib/getopt_pp.h"
#include "GA.h"

namespace GA {
	template <typename S>
	class CrossoverOp {
	public:
		enum {
			Cycle = 0,
		};
	public:
		virtual typename S::Ptr crossover(typename S::Pair& parents) = 0;

		static CrossoverOp<S>* Create(GetOpt::GetOpt_pp& ops) {
			return new CycleCrossover<S>(ops);
		}
	};

	template <typename S>
	class CycleCrossover : public CrossoverOp<S> {
	public:
		CycleCrossover(GetOpt::GetOpt_pp& ops) {}
		virtual ~CycleCrossover() {}

		typename S::Ptr crossover(typename S::Pair& parents) {
			return NULL;
		}
	};
}

#endif
