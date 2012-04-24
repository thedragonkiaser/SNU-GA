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
	return rand() % pGridHelper->nn;
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
	bool bDynamicGap = cmdLine.HasSwitch("-dggap");

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
		cout << "Gen\tSize\tBest\tMed\tWorst\t"
			<< "CostAvg\tCostSTD\t"
			<< "DiffAvg\tDiffSTD\t"
			<< "XEffAvg\tXEffSTD"
			<< endl;
		cout.precision(4);
	}

	if (bDynamicGap)
		k = population.size() * 0.9;

	while (true) {
		if (bDynamicGap) {
			float remainRatio = (float)(tEnd - tCurrent) / (float)(tEnd - tStart);
			if (remainRatio > 0.9) remainRatio = 0.9;
			if (remainRatio < 0.1) remainRatio = 0.1;
			k = population.size() * remainRatio;
		}

		GA::Solution::Vector offsprings;
		offsprings.reserve(k);

		vector<GA::Solution::Pair> parentsVec;
		parentsVec.reserve(k);

		for (int i=0; i<k; ++i) {
			if (i==0)
				ga.generateFitness(population);
			GA::Solution::Pair parents = ga.select(population);
			parentsVec.push_back(parents);

			GA::Solution::Ptr pOffspring = ga.crossover(parents);

			ga.mutate(pOffspring, pGridHelper->nn);

			pOffspring->cost = pGridHelper->scoreGrid(pOffspring);
			offsprings.push_back(pOffspring);
		}
		ga.replace(offsprings, parentsVec, population);

		++nGenerations;

		if (bPlot && nGenerations % nPlotUnit == 0) {
			GA::Solution::Ptr pBest = population.front();
			GA::Solution::Ptr pMedian = population[population.size() / 2];
			GA::Solution::Ptr pWorst = population.back();

			long long nCostSum = 0;
			vector<int> solutionDiff;
			solutionDiff.reserve(population.size() * population.size());
			GA::Solution::Vector::iterator it = population.begin();
			for (; it != population.end(); ++it) {
				nCostSum += (*it)->cost;
				GA::Solution::Vector::iterator kit = population.begin();
				for (; kit != population.end(); ++kit)
					solutionDiff.push_back((*it)->getDistance(*kit));
			}

			vector<int> crossEffects;
			solutionDiff.reserve(offsprings.size());
			for (int i=0; i<offsprings.size(); ++i) {
				int effect = offsprings[i]->getDistance(parentsVec[i].first) + offsprings[i]->getDistance(parentsVec[i].second);
				crossEffects.push_back( effect / 2 );
			}

			float fCostAvg = (float)nCostSum / population.size();
			float fCostDev = 0;
			it = population.begin();
			for (; it != population.end(); ++it)
				fCostDev += pow(((*it)->cost - fCostAvg), 2);

			float fDiffAvg = accumulate(solutionDiff.begin(), solutionDiff.end(), 0) / solutionDiff.size();
			float fDiffDev = 0;
			for (int i=0; i<solutionDiff.size(); ++i)
				fDiffDev += pow((solutionDiff[i]- fDiffAvg) , 2);

			float fXEffectAvg = accumulate(crossEffects.begin(), crossEffects.end(), 0) / crossEffects.size();
			float fXEffectDev = 0;
			for (int i=0; i<crossEffects.size(); ++i)
				fXEffectDev += pow((crossEffects[i]- fXEffectAvg) , 2);
			
			cout << nGenerations << "\t" << population.size() << "\t" <<
				pBest->cost << "\t" << pMedian->cost << "\t" << pWorst->cost << "\t"
				<< fixed
				<< fCostAvg << "\t" << sqrt(fCostDev / population.size()) << "\t" 
				<< fDiffAvg << "\t" << sqrt(fDiffDev / solutionDiff.size()) << "\t"
				<< fXEffectAvg << "\t" << sqrt(fXEffectDev / crossEffects.size()) << endl;
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
