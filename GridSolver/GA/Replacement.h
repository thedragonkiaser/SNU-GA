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
			Crowding,
			Strugle,
			Strugle2,
			Strugle3
		};
	public:
		ReplacementOp() {};
		virtual ~ReplacementOp() {};

		static ReplacementOp* create(CCmdLine& cmdLine);
		virtual void replace(Solution::Vector& offsprings, std::vector<Solution::Pair>& parents, Solution::Vector& population);

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population) = 0;
	};

	class ReplaceWorst : public ReplacementOp {
	public:
		ReplaceWorst(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceWorst() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceParent : public ReplacementOp {
	public:
		ReplaceParent(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceParent() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceWorstParent : public ReplacementOp {
	public:
		ReplaceWorstParent(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceWorstParent() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceCrowd : public ReplacementOp {
	public:
		ReplaceCrowd(CCmdLine& cmdLine);
		virtual ~ReplaceCrowd() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);

	protected:
		int _nCrowdSize;
		bool _bReplaceOnlyBetter;
	};

	class ReplaceStrugle : public ReplacementOp {
	public:
		ReplaceStrugle(CCmdLine& cmdLine) : ReplacementOp() {}
		virtual ~ReplaceStrugle() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceStrugle2 : public ReplaceStrugle {
	public:
		ReplaceStrugle2(CCmdLine& cmdLine) : ReplaceStrugle(cmdLine) {}
		virtual ~ReplaceStrugle2() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};

	class ReplaceStrugle3 : public ReplaceStrugle {
	public:
		ReplaceStrugle3(CCmdLine& cmdLine) : ReplaceStrugle(cmdLine) {}
		virtual ~ReplaceStrugle3() {}

	protected:
		virtual bool _remove(Solution::Ptr pOffspring, Solution::Pair& parents, Solution::Vector& population);
	};
}
