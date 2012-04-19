#pragma once

#include "../lib/CmdLine.h"
#include "GA.h"

namespace GA {
	class CrossoverOp {
	public:
		enum {
			BlockUniform = 0,
			Geographic,
		};
	public:
		static CrossoverOp* create(CCmdLine& cmdLine);

		virtual Solution::Ptr crossover(Solution::Pair& parents);

	protected:
		virtual void _crossover(Solution::Pair& parents, Solution::Ptr p) = 0;
	};
	
	class BlockUniformCrossover : public CrossoverOp {
	public:
		BlockUniformCrossover(CCmdLine& cmdLine);
		virtual ~BlockUniformCrossover() {}

	protected:
		virtual void _crossover(Solution::Pair& parents, Solution::Ptr p);

	protected:
		int _nRowCuts;
		int _nColCuts;
	};

	class GeographicCrossover : public CrossoverOp {
	public:
		GeographicCrossover(CCmdLine& cmdLine) : CrossoverOp() {};
		virtual ~GeographicCrossover() {}

	protected:
		virtual void _crossover(Solution::Pair& parents, Solution::Ptr p);
	};
}
