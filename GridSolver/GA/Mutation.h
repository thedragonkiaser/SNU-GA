#pragma once

#include "../lib/CmdLine.h"
#include "GA.h"
#include <vector>

namespace GA {
	class MutationOp {
	public:
		enum {
			Uniform = 0,
			NonUniform
		};
	public:
		MutationOp(CCmdLine& cmdLine);
		virtual ~MutationOp() {};

		static MutationOp* create(CCmdLine& cmdLine);
		virtual bool mutate(Solution::Ptr pSolution, int upperBound);

	protected:
		virtual int _mutate(int gene, int upperBound) = 0;

	protected:
		int _threshold;
	};

	class UniformMutation : public MutationOp {
	public:
		UniformMutation(CCmdLine& cmdLine) : MutationOp(cmdLine) {};
		virtual ~UniformMutation() {}

	protected:
		virtual int _mutate(int gene, int upperBound);
	};
}
