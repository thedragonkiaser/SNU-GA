#include "../lib/getopt_pp.h"
#include "../lib/util.h"
#include "../GA/GA.h"
#include "../GA/Selection.h"
#include "../GA/Crossover.h"
#include "../GA/Mutation.h"
#include "../GA/Replacement.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include "TSP.h"
#include "TSPSolver.h"

using namespace std;

typedef GA::Solution<int, float> Solution;
typedef GA::SelectionOp<Solution> SelectionOp;
typedef GA::CrossoverOp<Solution> CrossoverOp;
typedef GA::MutationOp<Solution> MutationOp;
typedef GA::ReplacementOp<Solution> ReplacementOp;

void parseInput(string sInputFile, TSP::Node& origin, vector<TSP::Node>& cities) {
	ifstream fin;
	fin.open(sInputFile);
	if (fin.is_open()) {
		string sLine;
		getline(fin, sLine);
		int nNodes = MyUtil::strTo<int>(sLine);
		cities.reserve(nNodes);

		getline(fin, sLine);
		size_t pos = sLine.find(" ");
		origin.id = 0;
		origin.x = MyUtil::strTo<float>(sLine.substr(0, pos));
		origin.y = MyUtil::strTo<float>(sLine.substr(pos + 1));
		
		for (int i=0; i<nNodes; ++i) {
			getline(fin, sLine);
			pos = sLine.find(" ");

			TSP::Node city;
			city.id = i + 1;
			city.x = MyUtil::strTo<float>(sLine.substr(0, pos));
			city.y = MyUtil::strTo<float>(sLine.substr(pos + 1));
			cities.push_back(city);
		}
			
		fin.close();
	}
}

void writeResult(string sOutputFile) {
	ofstream fout;
	fout.open(sOutputFile);
	if (fout.is_open()) {
//		for_each(cities.begin(), cities.end(), [&fout](TSP::Node& n) {
//			fout << n.id << " - " << n.x << ", " << n.y << endl;
//		});			
		fout.close();
	}
}

void preprocess(vector<TSP::Node>& cities) {
	sort(cities.begin(), cities.end(), [](TSP::Node& lhs, TSP::Node& rhs) {
		return lhs.x < rhs.x;
	});
}

int main(int argc, char* argv[]) {
	time_t tCurrent = MyUtil::getTime();
	srand(time(NULL));

	GetOpt::GetOpt_pp ops(argc, argv);	// parse cmd line arguments

	string sInputFile, sOutputFile;
	ops >> GetOpt::Option('i', "input", sInputFile)
		>> GetOpt::Option('o', "output", sOutputFile);

	TSP::Node origin;
	vector<TSP::Node> cities;
	parseInput(sInputFile, origin, cities);
//	preprocess(cities);
	
	SelectionOp* pSelector	= SelectionOp::Create(ops);
	CrossoverOp* pCrossover	= CrossoverOp::Create(ops);
	MutationOp* pMutator	= MutationOp::Create(ops);
	ReplacementOp* pReplacer= ReplacementOp::Create(ops);

	int nCitySize = cities.size();
	// set time limit		
	map<int, int> mTimeLimit;
	mTimeLimit[10] = 5500;
	mTimeLimit[20] = 19500;
	mTimeLimit[50] = 59500;
	mTimeLimit[100] = 179500;
	
	time_t tLimit = 0;
	map<int, int>::iterator it = mTimeLimit.find(nCitySize);
	if (it == mTimeLimit.end())
		ops >> GetOpt::Option('t', "time", tLimit);
	else
		tLimit = mTimeLimit[cities.size()];
	tLimit += tCurrent;

	// main routine
	int n, k, nRepair;
	ops >> GetOpt::Option('n', n, (int)(10 * nCitySize))
		>> GetOpt::Option('k', k, 1)
		>> GetOpt::Option('r', "repair", nRepair, 1);

	// generate initial solutions
	Solution::Vector population;
	population.reserve(n);
	for (int i=0; i<n; ++i) {
		Solution::Ptr p(new Solution);
		p->genotype.reserve(nCitySize);
		for (int m=0; m<nCitySize; ++m)
			p->genotype.push_back(m+1);
		TSP::scrambleSolution(p->genotype);
		p->quality = TSP::getQuality(p->genotype, origin, cities);
		population.push_back(p);
	}
	sort(population.begin(), population.end(), [](Solution::Ptr& lhs, Solution::Ptr& rhs) {
		return lhs->quality < rhs->quality;
	});

	// main loop
	while (true) {
		Solution::Vector offsprings;
		for (int i=0; i<k; ++i) {
			Solution::Pair parents = pSelector->select(population);
			Solution::Ptr pOffspring = pCrossover->crossover(parents);
			pOffspring->quality = TSP::getQuality(pOffspring->genotype, origin, cities);

			bool isMutated = pMutator->mutate(pOffspring);
			bool isRepaired = false;
			if (nRepair == 1)
				isRepaired = TSP::repairSolution(pOffspring->genotype);
			if (isMutated || isRepaired)
				pOffspring->quality = TSP::getQuality(pOffspring->genotype, origin, cities);
			offsprings.push_back(pOffspring);
		}
		pReplacer->replace(offsprings, population);

		if (MyUtil::getTime() > tLimit)
			break;
	}

	writeResult(sOutputFile);

	return 0;
}
