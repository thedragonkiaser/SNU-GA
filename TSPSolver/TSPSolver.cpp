#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../lib/getopt_pp.h"
#include "../GA/GA.h"
#include "../GA/Selection.h"
#include "../GA/Crossover.h"
#include "../GA/Mutation.h"
#include "../GA/Replacement.h"
#include "TSP.h"
#include "TSPSolver.h"

using namespace std;

typedef GA::Solution<int, float> Solution;
typedef GA::SelectionOp<Solution> SelectionOp;
typedef GA::CrossoverOp<Solution> CrossoverOp;
typedef GA::MutationOp<Solution> MutationOp;
typedef GA::ReplacementOp<Solution> ReplacementOp;

#if defined(linux)
    typedef long long time_t;
	time_t getTime() {
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec * 1000000LL + tv.tv_usec;
	}
#elif defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib, "winmm.lib")
	#include <windows.h>

//	typedef DWORD time_t;
	time_t getTime() { return timeGetTime(); }
#endif

int str2Int(string& s) {
	istringstream ss(s);
	int val;
	ss >> val;
	return val;
}

float str2Float(string& s) {
	istringstream ss(s);
	float val;
	ss >> val;
	return val;
}

void parseInput(string sInputFile, TSP::Node& origin, vector<TSP::Node>& cities) {
	ifstream fin;
	fin.open(sInputFile);
	if (fin.is_open()) {
		string sLine;
		getline(fin, sLine);
		int nNodes = str2Int(sLine);
		cities.reserve(nNodes);

		getline(fin, sLine);
		size_t pos = sLine.find(" ");
		origin.id = 0;
		origin.x = str2Float(sLine.substr(0, pos));
		origin.y = str2Float(sLine.substr(pos + 1));
		
		for (int i=0; i<nNodes; ++i) {
			getline(fin, sLine);
			pos = sLine.find(" ");

			TSP::Node city;
			city.id = i + 1;
			city.x = str2Float(sLine.substr(0, pos));
			city.y = str2Float(sLine.substr(pos + 1));
			cities.push_back(city);
		}
			
		fin.close();
	}
}

void writeResult(string sOutputFile) {

}

void preprocess(vector<TSP::Node>& cities) {

}

int main(int argc, char* argv[]) {
	time_t tCurrent = getTime();

	GetOpt::GetOpt_pp ops(argc, argv);	// parse cmd line arguments

	string sInputFile, sOutputFile;
	ops >> GetOpt::Option('i', "input", sInputFile)
		>> GetOpt::Option('o', "output", sOutputFile);

	TSP::Node origin;
	vector<TSP::Node> cities;
	parseInput(sInputFile, origin, cities);
	preprocess(cities);

	SelectionOp* pSelector	= SelectionOp::Create(ops);
	CrossoverOp* pCrossover	= CrossoverOp::Create(ops);
	MutationOp* pMutator	= MutationOp::Create(ops);
	ReplacementOp* pReplacer= ReplacementOp::Create(ops);

	// set time limit		
	map<int, int> mTimeLimit;
	mTimeLimit[10] = 5500;
	mTimeLimit[20] = 19500;
	mTimeLimit[50] = 59500;
	mTimeLimit[100] = 179500;
	
	time_t tLimit = 0;
	map<int, int>::iterator it = mTimeLimit.find(cities.size());
	if (it == mTimeLimit.end())
		ops >> GetOpt::Option('t', "time", tLimit);
	else
		tLimit = mTimeLimit[cities.size()];
	tLimit += tCurrent;

	// main routine
	int n, k;
	ops >> GetOpt::Option('n', n)
		>> GetOpt::Option('k', k);

	Solution::Vector population;
	// generate random solutions;

	// main loop
	while (true) {
		Solution::Vector offsprings;
		for (int i=0; i<k; ++i) {
			Solution::Pair parents = pSelector->select(population);
			Solution::Ptr pOffspring = pCrossover->crossover(parents);
			// pOffspring->quality = ;
			// calculate qualitiy
			bool isMutated = pMutator->mutate(pOffspring);
			// if (isMutated)
			// pOffspring->quality = ;
			// calculate qualitiy
			//offspring = repair();
			offsprings.push_back(pOffspring);
		}
		pReplacer->replace(offsprings, population);

		if (getTime() > tLimit)
			break;
	}

	writeResult(sOutputFile);

	return 0;
}
