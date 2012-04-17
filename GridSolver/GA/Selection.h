#pragma once

#include "../lib/CmdLine.h"
#include "GA.h"

namespace GA {	
	class SelectionOp {
	public:
		enum {
			RouletteWheel = 0,
			Tournament,
			RankBased,
			Sharing
		};
	public:
		SelectionOp() {};
		virtual ~SelectionOp() {};

		static SelectionOp* Create(CCmdLine& cmdLine);
		virtual Solution::Pair select(Solution::Vector& population);

	protected:
		virtual int _select(Solution::Vector& population) = 0;
	};

	class RouletteWheelSelector : public SelectionOp {
	public:
		RouletteWheelSelector(CCmdLine& cmdLine);
		virtual ~RouletteWheelSelector() {}

		virtual Solution::Pair select(Solution::Vector& population);

	protected:
		virtual int _select(Solution::Vector& population);

	protected:
		float _threshold;
		
		std::vector<float> _fitness;
		float _sumFitness;
	};

	class TournamentSelector : public SelectionOp {
	public:
		TournamentSelector(CCmdLine& cmdLine);
		virtual ~TournamentSelector() {}

	protected:
		virtual int _select(Solution::Vector& population);

	protected:
		float _threshold;
		int _exp;
	};

	class RankBasedSelector : public RouletteWheelSelector {
	public:
		RankBasedSelector(CCmdLine& cmdLine);
		virtual ~RankBasedSelector() {}

		virtual Solution::Pair select(Solution::Vector& population);

	protected:
		float _maxFitness;
		float _minFitness;
	};
}
