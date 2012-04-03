#include <vector>
#include <algorithm>
#include "TSP.h"

using namespace std;

namespace TSP {
	float getCost(const vector<int>& sol, const vector< vector<float> >& distance) {
		float cost = 0.0;
		int nFrom = 0;
		vector<int>::const_iterator it = sol.begin();
		for (; it != sol.end(); ++it) {
			cost += distance[nFrom][*it];
			nFrom = *it;
		}
		cost += distance[nFrom][0];
		return cost;
	}
	
	bool repairSolution(vector<int>& sol) {
		return false;
	}
}
