#pragma once

#include "../lib/CmdLine.h"
#include "GA.h"
#include <vector>

namespace GA {
	class ReplacementOp {
	public:
		enum {
			Worst = 0,
			Parent,
			WorstParent,
			Crowding
		};
	public:
		ReplacementOp() {};
		virtual ~ReplacementOp() {};

		static ReplacementOp* create(CCmdLine& cmdLine);
		virtual void replace(Solution::Vector& offsprings, std::vector<Solution::Pair>& parents, Solution::Vector& population);

	protected:
		virtual void _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) = 0;
	};

	class ReplaceWorst : public ReplacementOp {
	public:
		ReplaceWorst(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceWorst() {}

	protected:
		virtual void _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceParent : public ReplacementOp {
	public:
		ReplaceParent(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceParent() {}

	protected:
		virtual void _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceWorstParent : public ReplacementOp {
	public:
		ReplaceWorstParent(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceWorstParent() {}

	protected:
		virtual void _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceCrowd : public ReplacementOp {
	public:
		ReplaceCrowd(CCmdLine& cmdLine);
		virtual ~ReplaceCrowd() {}

	protected:
		virtual void _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);

	protected:
		int _nCrowdSize;
	};
}
