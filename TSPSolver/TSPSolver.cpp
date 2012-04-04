#include "../lib/CmdLine.h"
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
typedef GA::BaseSelector<Solution> SelectionOp;
typedef GA::BaseCrossover<Solution> CrossoverOp;
typedef GA::BaseMutator<Solution> MutationOp;
typedef GA::BaseReplacer<Solution> ReplacementOp;

int parseInput(string sInputFile, vector< vector<float> >& matrix) {
	int nNodes = 0;
	vector<TSP::Node> cities;

	ifstream fin;
	fin.open(sInputFile);
	if (fin.is_open()) {
		string sLine;
		getline(fin, sLine);
		nNodes = MyUtil::strTo<int>(sLine);
		cities.reserve(nNodes);

		getline(fin, sLine);
		size_t pos = sLine.find(" ");
		TSP::Node city;
		city.id = 0;
		city.x = MyUtil::strTo<float>(sLine.substr(0, pos));
		city.y = MyUtil::strTo<float>(sLine.substr(pos + 1));
		cities.push_back(city);
		
		for (int i=0; i<nNodes; ++i) {
			getline(fin, sLine);
			pos = sLine.find(" ");

			city.id = i + 1;
			city.x = MyUtil::strTo<float>(sLine.substr(0, pos));
			city.y = MyUtil::strTo<float>(sLine.substr(pos + 1));
			cities.push_back(city);
		}
			
		fin.close();
	}

	int nSize = (int)cities.size();
	matrix.reserve(nSize);
	for (int i=0; i<nSize; ++i) {
		vector<float> v(nSize);
		TSP::Node& city = cities[i];
		for (int k=0; k<nSize; ++k) {
			float diff_x = cities[k].x - city.x;
			float diff_y = cities[k].y - city.y;
			v[k] = sqrt(diff_x * diff_x + diff_y * diff_y);
		}
		matrix.push_back(v);
	}

	return nNodes;
}

//#define _TEST_
void writeResult(string sOutputFile, Solution::Ptr pSol, int argc, char* argv[], int nGenerations) {
	ofstream fout;
#if defined(_TEST_)
	fout.open(sOutputFile, fstream::out | fstream::app);
#else
	fout.open(sOutputFile, fstream::out | fstream::trunc);
#endif
	if (fout.is_open()) {
#if defined(_TEST_)
		for (int i=0; i<argc; ++i)
			fout << argv[i] << " ";
		fout << endl;
		fout << nGenerations << " Generations" << endl;
#endif
		fout.precision(4);
		fout << fixed << pSol->cost << endl;

		for_each(pSol->genotype.begin(), pSol->genotype.end(), [&fout](Solution::GeneType g) {
			fout << g << " ";
		});
		fout << endl;

		fout.close();
	}
}

void preprocess(vector<TSP::Node>& cities) {
//	sort(cities.begin(), cities.end(), [](TSP::Node& lhs, TSP::Node& rhs) {
//		return lhs.x < rhs.x;
//	});
}

int main(int argc, char* argv[]) {
	time_t tCurrent = MyUtil::getTime();
	srand(time(NULL));

	CCmdLine cmdLine;
	cmdLine.SplitLine(argc, argv);

	string sInputFile( cmdLine.GetArgument("-i", 0) );
	string sOutputFile( cmdLine.GetArgument("-o", 0) );

	vector< vector<float> > distance;
	int nCities = parseInput(sInputFile, distance);
//	preprocess(cities);
	
	SelectionOp* pSelector	= GA::SelectionOp<Solution>::Create(cmdLine);
	CrossoverOp* pCrossover	= GA::CrossoverOp<Solution>::Create(cmdLine);
	MutationOp* pMutator	= GA::MutationOp<Solution>::Create(cmdLine);
	ReplacementOp* pReplacer= GA::ReplacementOp<Solution>::Create(cmdLine);

	// set time limit
	time_t tLimit = 0;
	if (cmdLine.HasSwitch("-t")) {
		tLimit = MyUtil::strTo<time_t>( cmdLine.GetArgument("-t", 0) );
	}
	else {
		map<int, int> mTimeLimit;
		mTimeLimit[10] = 5700;
		mTimeLimit[20] = 19700;
		mTimeLimit[50] = 59700;
		mTimeLimit[100] = 179700;
		
		map<int, int>::iterator it = mTimeLimit.find(nCities);
		if (it != mTimeLimit.end())
			tLimit = mTimeLimit[nCities];
	}
	tLimit += tCurrent;

	// main routine
	int n = 50;
	if (cmdLine.HasSwitch("-n"))
		n = MyUtil::strTo<int>( cmdLine.GetArgument("-n", 0) );
	int k = 1;
	if (cmdLine.HasSwitch("-k"))
		k = MyUtil::strTo<int>( cmdLine.GetArgument("-k", 0) );
	bool bRepair = !cmdLine.HasSwitch("-noRepair");	
	bool bPlot = cmdLine.HasSwitch("-plot");	

	// generate initial solutions
	Solution::Vector population;
	population.reserve(n);
	for (int i=0; i<n; ++i) {
		Solution::Ptr p(new Solution);
		p->genotype.reserve(nCities);
		for (int m=0; m<nCities; ++m)
			p->genotype.push_back(m+1);
		random_shuffle(p->genotype.begin(), p->genotype.end());
		p->cost = TSP::getCost(p->genotype, distance);
		population.push_back(p);
	}
	sort(population.begin(), population.end(), GA::SolutionPtrComp<Solution>());

	// main loop
	int nGenerations = 0;
	while (true) {
		Solution::Vector offsprings;
		offsprings.reserve(k);

		vector<Solution::Pair> parentsVec;
		parentsVec.reserve(k);

		for (int i=0; i<k; ++i) {
			Solution::Pair parents = pSelector->select(population);
			parentsVec.push_back(parents);

			Solution::Ptr pOffspring = pCrossover->crossover(parents);
			pOffspring->cost = TSP::getCost(pOffspring->genotype, distance);

			bool isMutated = pMutator->mutate(pOffspring);
			bool isRepaired = false;
			if (bRepair)
				isRepaired = TSP::repairSolution(pOffspring->genotype);
			if (isMutated || isRepaired)
				pOffspring->cost = TSP::getCost(pOffspring->genotype, distance);
			offsprings.push_back(pOffspring);
		}
		pReplacer->replace(offsprings, parentsVec, population);
		++nGenerations;
		//cout << nGenerations << " " << population.size() << endl;
		if (bPlot && nGenerations % 1000 == 0) {
			float sum = 0;
			for (int it=0; it<population.size(); ++it)
				sum += population[it]->cost;
			cout.precision(4);
			cout << fixed << nGenerations << " " << population.front()->cost << " " << (float)(sum / population.size()) << endl;
		}
		if (MyUtil::getTime() > tLimit)
			break;
	}

	writeResult(sOutputFile, population.front(), argc, argv, nGenerations);

	return 0;
}
