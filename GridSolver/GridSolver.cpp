#include "lib/CmdLine.h"
#include "lib/Utililty.h"

#include "GridHelper.h"
#include "GA/GA.h"

#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>

using namespace std;

GridHelper* pGridHelper;

int genGenes() {
	int idx = rand() % pGridHelper->values.size();
	return pGridHelper->values[idx];
}

GA::Solution::Ptr genSolution() {
	GA::Solution::Ptr p = make_shared<GA::Solution>(pGridHelper->columns, pGridHelper->rows);
	generate_n(back_inserter(p->genotype), p->width * p->height, genGenes);
	p->cost = pGridHelper->scoreGrid(p);
	return p;
}

int main(int argc, char* argv[]) {
	time_t tStart = Utility::getMilliSec();

	srand((unsigned int)time(NULL));

	//////// parse command line arguments
	CCmdLine cmdLine;
	cmdLine.SplitLine(argc, argv);

	string sInput( cmdLine.GetArgument("-i", 0) );
	string sOutput( cmdLine.GetArgument("-o", 0) );
	int n = cmdLine.HasSwitch("-n") ? Utility::strTo<int>( cmdLine.GetArgument("-n", 0) ) : 5;
	int k = cmdLine.HasSwitch("-k") ? Utility::strTo<int>( cmdLine.GetArgument("-k", 0) ) : 1;
	bool bRepair = !cmdLine.HasSwitch("-noRepair");	
	bool bPlot = cmdLine.HasSwitch("-plot");
	int nPlotUnit = bPlot ? Utility::strTo<int>( cmdLine.GetArgument("-plot", 0) ) : 0;

	time_t tDuration = cmdLine.HasSwitch("-t") ? Utility::strTo<int>( cmdLine.GetArgument("-t", 0) ) : 600000 - 5000;
	time_t tEnd = tStart + tDuration;

	//////// parse input file
	pGridHelper = new GridHelper;
	freopen(sInput.c_str(), "r", stdin);
	pGridHelper->readPoints();
	
	//////// prepare the initial population
	GA::Solution::Vector population;
	population.reserve(n);
	generate_n(back_inserter(population), n, genSolution);
	sort(population.begin(), population.end(), GA::SolutionPtrGreater());
	
	//////// main Loop
	GA::GAHelper ga(cmdLine);
	int nGenerations = 0;
	time_t tCurrent = tStart;

	if (bPlot) {
		cout << "Generation\tPopSize\tBest\tCostAvg\tCostSTD\tDiffAvg\tDiffSTD" << endl;
		cout.precision(4);
	}
	while (true) {
		GA::Solution::Vector offsprings;
		offsprings.reserve(k);

		vector<GA::Solution::Pair> parentsVec;
		parentsVec.reserve(k);

		for (int i=0; i<k; ++i) {
			GA::Solution::Pair parents = ga.select(population);
			parentsVec.push_back(parents);

			GA::Solution::Ptr pOffspring = ga.crossover(parents);

			ga.mutate(pOffspring, pGridHelper->values, (float)(tCurrent - tStart) / (float)tDuration);

			pOffspring->cost = pGridHelper->scoreGrid(pOffspring);
			offsprings.push_back(pOffspring);
		}
		ga.replace(offsprings, parentsVec, population);

		++nGenerations;

		if (bPlot && nGenerations % nPlotUnit == 0) {
			int nCostSum = 0;
			int nDiffSum = 0;
			GA::Solution::Ptr pBest = population.front();
			GA::Solution::Vector::iterator it = population.begin();
			for (; it != population.end(); ++it) {
				nCostSum += (*it)->cost;
				nDiffSum += pBest->getDistance(*it);
			}

			float fCostAvg = (float)nCostSum / population.size();
			float fDiffAvg = (float)nDiffSum / population.size();

			float fCostSum = 0;
			float fDiffSum = 0;
			it = population.begin();
			for (; it != population.end(); ++it) {
				fCostSum += pow(((*it)->cost - fCostAvg), 2);
				fDiffSum += pow((pBest->getDistance(*it) - fDiffAvg), 2);
			}

			float fCostSTD = sqrt(fCostSum / population.size());
			float fDiffSTD = sqrt(fDiffSum / population.size());

			cout << nGenerations << "\t" << population.size() << "\t" <<
				pBest->cost << "\t" << fixed <<
				fCostAvg << "\t" << fCostSTD << "\t" <<
				fDiffAvg << "\t" << fDiffSTD << endl;
		}

		tCurrent = Utility::getMilliSec();
		if (tCurrent > tEnd)
			break;
	}

	//////// output
	ofstream fout;
#if defined (_TEST_)
	fout.open(sOutput, fstream::out | fstream::app);
#else
	fout.open(sOutput, fstream::out | fstream::trunc);
#endif
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
		fout << "Generations: " << nGenerations << endl;
		fout << "Score : " << pSol->cost << endl;
		for (int i=0; i<argc; ++i)
			fout << argv[i] << " ";
		fout << endl;
		fout << "================================================================================" << endl;
		fout << endl;
#endif
		fout.close();
	}
}
