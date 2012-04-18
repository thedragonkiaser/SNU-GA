#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "../lib/CmdLine.h"

using namespace std::rel_ops;

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

		// Fields
		ChromosomeType genotype;
		int width, height;
		int cost;

		// Methods
		int get(int x, int y);
		void set(int x, int y, int val);
		int getDistance(const Solution& sol);
		int getDistance(Ptr pSol);

		// Operators
		bool operator ==(const Solution& other) const;
		bool operator ==(Ptr p) const;
		bool operator <(const Solution& r) const;
		bool operator <(Ptr p) const;
	};

	struct SolutionPtrComp : std::unary_function<Solution::Ptr, bool> {
		bool operator()(const Solution::Ptr& lhs, const Solution::Ptr& rhs) const {
			return *lhs > *rhs;
		}
	};

	class SelectionOp;
	class ReplacementOp;

	class GAHelper {
	public:
		GAHelper(CCmdLine& cmdLine);
		Solution::Pair select(Solution::Vector& population);
		void replace(Solution::Vector& offsprings, std::vector<Solution::Pair>& parents, Solution::Vector& population);

	protected:
		SelectionOp* _selector;
		ReplacementOp* _replacer;
	};
}
