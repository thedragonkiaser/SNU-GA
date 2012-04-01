#if !defined(_TSP_H_)
#define _TSP_H_

namespace TSP {
	struct Node {
		int id;
		float x, y;
	};

	extern float getQuality(const std::vector<int>& sol, const TSP::Node& origin, const std::vector<TSP::Node>& cities);
	extern bool repairSolution(std::vector<int>& sol);
	extern void scrambleSolution(std::vector<int>& sol);
	extern void scrambleSolution(std::vector<int>& sol, int nStart, int nEnd);
}
#endif
