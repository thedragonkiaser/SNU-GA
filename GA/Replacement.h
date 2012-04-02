#if !defined(_REPLACEMENT_H_)
#define _REPLACEMENT_H_

#include "../lib/CmdLine.h"
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

		static ReplacementOp<S>* Create(CCmdLine& cmdLine) {
			return new ReplaceWorst<S>(cmdLine);
		}
	};

	template <typename S>
	class ReplaceWorst : public ReplacementOp<S> {
	public:
		ReplaceWorst(CCmdLine& cmdLine) {}
		virtual ~ReplaceWorst() {}

		void replace(typename S::Vector& offsprings, typename S::Vector& population) {}
	};
}

#endif