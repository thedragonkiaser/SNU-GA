#if !defined(_TSP_H_)
#define _TSP_H_

namespace TSP {
	struct Node {
		int id;
		float x, y;
	};

	extern float getCost(const std::vector<int>& sol, const std::vector< std::vector<float> >& distance);
	extern bool repairSolution(std::vector<int>& sol);
}
#endif
