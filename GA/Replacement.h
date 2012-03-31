#if !defined(_REPLACEMENT_H_)
#define _REPLACEMENT_H_

#include "../lib/getopt_pp.h"
#include "GA.h"

namespace GA {
	template <typename S>
	class ReplacementOp {
	public:
		enum {
			Worst = 0,
		};
	public:
		virtual void replace(typename S::Vector& offsprings, typename S::Vector& population) = 0;

		static ReplacementOp<S>* Create(GetOpt::GetOpt_pp& ops) {
			return new ReplaceWorst<S>(ops);
		}
	};

	template <typename S>
	class ReplaceWorst : public ReplacementOp<S> {
	public:
		ReplaceWorst(GetOpt::GetOpt_pp& ops) {}
		virtual ~ReplaceWorst() {}

		void replace(typename S::Vector& offsprings, typename S::Vector& population) {}
	};
}

#endif