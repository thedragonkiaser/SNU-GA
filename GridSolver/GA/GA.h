#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "../lib/CmdLine.h"

using namespace std::rel_ops;

class GridHelper;
namespace GA {
	struct Solution {
		// Type Definitions
		typedef std::vector<int> ChromosomeType;
		typedef std::shared_ptr<Solution> Ptr;
		typedef std::pair<Ptr, Ptr> Pair;
		typedef std::vector<Ptr> Vector;

		// Constructors
		Solution();
		Solution(int w, int h);
		Solution(const Solution& sol);
		Solution(Ptr pSol);

		// Fields
		ChromosomeType genotype;
		int width, height;
		long long cost;

		// Methods
		int get(int x, int y);
		void set(int x, int y, int val);
		int getDistance(const Solution& sol);
		int getDistance(Ptr pSol);

		// Operators
		Solution& operator =(const Solution& other);
		bool operator ==(const Solution& other) const;
		bool operator ==(Ptr p) const;
		bool operator <(const Solution& r) const;
		bool operator <(Ptr p) const;
	};

	struct SolutionPtrLess : std::binary_function<Solution::Ptr, Solution::Ptr, bool> {
		bool operator()(const Solution::Ptr& lhs, const Solution::Ptr& rhs) const {
			return *lhs < *rhs;
		}
	};

	struct SolutionPtrGreater : std::binary_function<Solution::Ptr, Solution::Ptr, bool> {
		bool operator()(const Solution::Ptr& lhs, const Solution::Ptr& rhs) const {
			return *lhs > *rhs;
		}
	};

	struct SolutionPtrEqual : std::binary_function<Solution::Ptr, Solution::Ptr, bool> {
		bool operator()(const Solution::Ptr& lhs, const Solution::Ptr& rhs) const {
			return *lhs == *rhs;
		}
	};

	class SelectionOp;
	class CrossoverOp;
	class ReplacementOp;
	class MutationOp;

	class GAHelper {
	public:
		GAHelper(CCmdLine& cmdLine);

		void generateFitness(Solution::Vector& population);
		Solution::Pair select(Solution::Vector& population);
		Solution::Ptr crossover(Solution::Pair& parents);
		bool mutate(Solution::Ptr pSolution, GridHelper *pHelper);
		void replace(Solution::Vector& offsprings, std::vector<Solution::Pair>& parents, Solution::Vector& population);

	protected:
		SelectionOp*	_selector;
		CrossoverOp*	_crossover;
		MutationOp*		_mutator;
		ReplacementOp*	_replacer;
	};
}
