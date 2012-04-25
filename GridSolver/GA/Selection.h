#pragma once

#include "../lib/CmdLine.h"
#include "GA.h"

namespace GA {	
	class SelectionOp {
	public:
		enum {
			RouletteWheel = 0,
			Tournament,
			RankBased
		};
	public:
		SelectionOp() {};
		virtual ~SelectionOp() {};

		static SelectionOp* create(CCmdLine& cmdLine);

		virtual void generateFitness(Solution::Vector& population) {};
		virtual Solution::Pair select(Solution::Vector& population, Solution::Pair& parents);

	protected:
		virtual int _select(Solution::Vector& population) = 0;
	};

	class FitnessBasedSelector : public SelectionOp {
	public:
		FitnessBasedSelector() : SelectionOp(), _fitnessSum(0), _isSharing(false), _sharingFactor(1) {};
		virtual ~FitnessBasedSelector() {};

		virtual void generateFitness(Solution::Vector& population);

	protected:
		virtual void _generateFitness(Solution::Vector& population) = 0;
		virtual void _sharingFitness(Solution::Vector& population);

		virtual int _select(Solution::Vector& population);

	protected:
		std::vector<float> _fitness;
		float _fitnessSum;
		
		bool _isSharing;
		int _sharingFactor;
	};

	class RouletteWheelSelector : public FitnessBasedSelector {
	public:
		RouletteWheelSelector(CCmdLine& cmdLine);
		virtual ~RouletteWheelSelector() {}
		
	protected:
		virtual void _generateFitness(Solution::Vector& population);

	protected:
		float _threshold;
	};

	class RankBasedSelector : public FitnessBasedSelector {
	public:
		RankBasedSelector(CCmdLine& cmdLine);
		virtual ~RankBasedSelector() {}

	protected:
		virtual void _generateFitness(Solution::Vector& population);

	protected:
		float _maxFitness;
		float _minFitness;
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
}
