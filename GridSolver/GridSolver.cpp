#include "lib/CmdLine.h"
#include "lib/Utililty.h"

#include "GridHelper.h"
#include "GA/GA.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
	time_t tCurrent = Utility::getMilliSec();
	time_t tEnd = tCurrent + 6000000 - 1000;

	srand(time(NULL));

	//////// parse command line arguments
	CCmdLine cmdLine;
	cmdLine.SplitLine(argc, argv);

	string sInput( cmdLine.GetArgument("-i", 0) );
	string sOutput( cmdLine.GetArgument("-o", 0) );
	int n = cmdLine.HasSwitch("-n") ? Utility::strTo<int>( cmdLine.GetArgument("-n", 0) ) : 10;
	int k = cmdLine.HasSwitch("-k") ? Utility::strTo<int>( cmdLine.GetArgument("-k", 0) ) : 1;
	bool bRepair = !cmdLine.HasSwitch("-noRepair");	
	bool bPlot = cmdLine.HasSwitch("-plot");	

	//////// parse input file
	GridHelper grid;
	freopen(sInput.c_str(), "r", stdin);
	grid.readPoints();
	
	//////// prepare the initial population
	GA::Solution::Vector population;
	population.reserve(n);
	for (int i=0; i<n; ++i) {
		GA::Solution::Ptr p = make_shared<GA::Solution>(grid.columns, grid.rows);

		int size = p->width * p->height;
		for (int k=0; k<size; ++k) {
			int idx = rand() % grid.values.size();
			p->genotype.push_back(grid.values[idx]);
		}
		p->cost = grid.scoreGrid(p);
		population.push_back(p);
	}
	sort(population.begin(), population.end(), GA::SolutionPtrComp());
	
	GA::GAHelper ga(cmdLine);

	int nGenerations = 0;
	while (true) {
		GA::Solution::Vector offsprings;
		offsprings.reserve(k);

		vector<GA::Solution::Pair> parentsVec;
		parentsVec.reserve(k);

		for (int i=0; i<k; ++i) {
			GA::Solution::Pair parents = ga.select(population);
			parentsVec.push_back(parents);

//			GA::Solution::Ptr pOffspring = pCrossover->crossover(parents);

//			pMutator->mutate(pOffspring, grid.values, tCurrent, tEnd);

//			pOffspring->cost = grid.scoreGrid(pOffspring);
//			offsprings.push_back(pOffspring);
		}
//		pReplacer->replace(offsprings, parentsVec, population);

		++nGenerations;
#if defined (_PROFILE)

#endif
		tCurrent = Utility::getMilliSec();
		if (tCurrent > tEnd)
			break;
	}

	//////// output
	ofstream fout;
	fout.open(sOutput, fstream::out | fstream::trunc);
	if (fout.is_open()) {
		GA::Solution::Ptr pSol = population.front();
		int size = (int)pSol->genotype.size();
		for (int i=0; i<size; ++i) {
			if ( (i+1) % pSol->width == 0)
				fout << pSol->genotype[i] << endl;
			else
				fout << pSol->genotype[i] << " ";
		}

#if defined (_TEST_)
		fout << endl;
		fout << "Score : " << pSol->cost << endl;

		fout << "================================================================================" << endl;
		fout << endl;
#endif
		fout.close();
	}
}