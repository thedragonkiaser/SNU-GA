#if !defined(_MUTATION_H_)
#define _MUTATION_H_

#include "../lib/CmdLine.h"
#include "GA.h"

namespace GA {
	template <typename S>
	class MutationOp {
	public:
		enum {
			Uniform = 0,
		};
	public:
		virtual bool mutate(typename S::Ptr pSolution) = 0;

		static MutationOp<S>* Create(CCmdLine& cmdLine) {
			return new UniformMutator<S>(cmdLine);
		}
	};

	template <typename S>
	class UniformMutator : public MutationOp<S> {
	public:
		UniformMutator(CCmdLine& cmdLine) {}
		virtual ~UniformMutator() {}

		bool mutate(typename S::Ptr pSolution) { return false; }
	private:
		float _threshold;
	};
}

#endif
