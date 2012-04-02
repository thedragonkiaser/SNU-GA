#include <vector>
#include <math.h>
#include "TSP.h"

using namespace std;

namespace TSP {
	float getCost(const vector<int>& sol, const Node& origin, const vector<Node>& cities) {
		float cost = 0.0;

		float last_x = origin.x;
		float last_y = origin.y;
		vector<int>::const_iterator it = sol.begin();
		while(it < sol.end()) {
			const Node& city = cities[*it - 1];
			float diff_x = city.x - last_x;
			float diff_y = city.y - last_y;
			cost += sqrt(diff_x * diff_x + diff_y * diff_y);

			last_x = city.x;
			last_y = city.y;
			++it;
		}

		float diff_x = last_x - origin.x;
		float diff_y = last_y - origin.y;
		cost += sqrt(diff_x * diff_x + diff_y * diff_y);

		return cost;
	}
	
	bool repairSolution(vector<int>& sol) {
		return false;
	}

	void scrambleSolution(vector<int>& sol) {
		scrambleSolution(sol, 0, sol.size());
	}
	
	void scrambleSolution(vector<int>& sol, int nStart, int nEnd) {
		int nLen = nEnd - nStart;
		for (int i=nStart; i<nEnd; ++i) {
			int nRan = rand();
			if ( (nRan % 100) > 50 ) {
				int k = rand() % nLen;
				int temp = sol[i];
				sol[i] = sol[k];
				sol[k] = temp;
			}
		}
	}
}
