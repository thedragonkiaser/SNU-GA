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

GridHelper* g_pGridHelper;

int genGenes() {
	return g_pGridHelper->values[ rand() % g_pGridHelper->values.size() ];
}

GA::Solution::Ptr genSolution() {
	GA::Solution::Ptr p = make_shared<GA::Solution>(g_pGridHelper->nColumns, g_pGridHelper->nRows);
	generate_n(back_inserter(p->genotype), p->width * p->height, genGenes);
	p->cost = g_pGridHelper->scoreGrid(p);
	return p;
}

struct Config {
	string sInput;
	string sOutput;
	int nPopulation;
	float fGenerationGap;

	bool bPlot;
	int nPlot;
	time_t tDuration;

	int nMode;

	enum {
		HYBRID_GA = 0,
		LOCAL_OP,
		MULTI_START,
		PURE_GA,
	};

	Config() {};
	Config(CCmdLine cmdLine) {
		sInput = ( cmdLine.GetArgument("-i", 0) );
		sOutput = ( cmdLine.GetArgument("-o", 0) );
		nPopulation = Utility::strTo<int>( cmdLine.GetArgument("-n", 0) );
		fGenerationGap = Utility::strTo<float>( cmdLine.GetArgument("-gap", 0) );
		bPlot = cmdLine.HasSwitch("-plot");
		nPlot = bPlot ? Utility::strTo<int>( cmdLine.GetArgument("-plot", 0) ) : 0;
		tDuration = cmdLine.HasSwitch("-t") ? Utility::strTo<int>( cmdLine.GetArgument("-t", 0) ) : 360000 - 2500;

		nMode = Utility::strTo<int>( cmdLine.GetArgument("-mode", 0) );
	}
};

int main(int argc, char* argv[]) {
	srand((unsigned int)time(NULL));
	
	//////////////////////////////////////////////////////// 
	//////// parse command line arguments
	CCmdLine cmdLine;
	cmdLine.SplitLine(argc, argv);
	Config config(cmdLine);

	time_t tStart = Utility::getMilliSec();
	time_t tCurrent = tStart;
	time_t tEnd = tStart + config.tDuration;

	//////////////////////////////////////////////////////// 
	//////// parse input file
	g_pGridHelper = new GridHelper;
	freopen(config.sInput.c_str(), "r", stdin);
	g_pGridHelper->readPoints();
	
	////////////////////////////////////////////////////////
	//////// prepare the initial population
	GA::Solution::Vector population;
	population.reserve(config.nPopulation);
	generate_n(back_inserter(population), config.nPopulation, genSolution);
	sort(population.begin(), population.end(), GA::SolutionPtrGreater());
	
	////////////////////////////////////////////////////////
	//////// main Loop
	GA::GAHelper ga(cmdLine);
	int nGeneration = 0;

	switch (config.nMode) {
	case Config::HYBRID_GA: {
		if (config.bPlot) {
			cout << "Gen\tSize\tBest\tMed\tWorst\t"
			<< "CostAvg\tCostSTD\t"
			<< "DiffAvg\tDiffSTD\t"
			<< endl;
			cout.precision(4);
		}

		int k = (int)(config.nPopulation * config.fGenerationGap);
		if ( k < 1 ) k = 1;
		while (true) {
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

				ga.mutate(pOffspring, g_pGridHelper);

				pOffspring->cost = g_pGridHelper->scoreGrid(pOffspring);
				g_pGridHelper->localOptimization(pOffspring);

				offsprings.push_back(pOffspring);
			}
			ga.replace(offsprings, parentsVec, population);

			++nGeneration;

			////////////////////////////////////////////////////////
			//////// plot
			if (config.bPlot && nGeneration % config.nPlot == 0) {
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

				float fCostAvg = (float)nCostSum / population.size();
				float fCostDev = 0;
				it = population.begin();
				for (; it != population.end(); ++it)
					fCostDev += (((*it)->cost - fCostAvg) * ((*it)->cost - fCostAvg));

				float fDiffAvg = accumulate(solutionDiff.begin(), solutionDiff.end(), 0) / solutionDiff.size();
				float fDiffDev = 0;
				for (int i=0; i<solutionDiff.size(); ++i)
					fDiffDev += ((solutionDiff[i]- fDiffAvg) * (solutionDiff[i]- fDiffAvg));

				cout << nGeneration << "\t" << population.size() << "\t" <<
					pBest->cost << "\t" << pMedian->cost << "\t" << pWorst->cost << "\t"
					<< fixed
					<< fCostAvg << "\t" << sqrt(fCostDev / population.size()) << "\t" 
					<< fDiffAvg << "\t" << sqrt(fDiffDev / solutionDiff.size()) << "\t"
					<< endl;
			}

			tCurrent = Utility::getMilliSec();
			if (tCurrent > tEnd)
				break;
		}
	}
	break;

	case Config::PURE_GA: {
		int k = (int)(config.nPopulation * config.fGenerationGap);
		if ( k < 1 ) k = 1;
		while (true) {
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
				ga.mutate(pOffspring, g_pGridHelper);
				pOffspring->cost = g_pGridHelper->scoreGrid(pOffspring);
				offsprings.push_back(pOffspring);
			}
			ga.replace(offsprings, parentsVec, population);

			++nGeneration;
			tCurrent = Utility::getMilliSec();
			if (tCurrent > tEnd)
				break;
		}
	}
	break;

	case Config::LOCAL_OP: {
		population.clear();

		for (int i=0; i<100; ++i) {
			GA::Solution::Ptr pSol = genSolution();
			pSol->cost = g_pGridHelper->scoreGrid(pSol);
			g_pGridHelper->localOptimization(pSol);
			population.push_back(pSol);
		}
	}
	break;

	case Config::MULTI_START: {
		population.clear();
		population.push_back( genSolution() );
		while (true) {
			GA::Solution::Ptr pSol = genSolution();
			pSol->cost = g_pGridHelper->scoreGrid(pSol);
			g_pGridHelper->localOptimization(pSol);

			if ( pSol->cost > population[0]->cost )
				population[0] = pSol;
			
			tCurrent = Utility::getMilliSec();
			if (tCurrent > tEnd)
				break;
		}
	}
	break;

	}

	////////////////////////////////////////////////////////
	//////// output
	ofstream fout;
#if defined (_TEST_)
	fout.open(config.sOutput, fstream::out | fstream::app);
#else
	fout.open(config.sOutput, fstream::out | fstream::trunc);
#endif
	if (fout.is_open()) {
		if (config.nMode == Config::LOCAL_OP) {
			long long nCostSum = 0;
			GA::Solution::Vector::iterator it = population.begin();
			for (; it != population.end(); ++it)
				nCostSum += (*it)->cost;

			float fCostAvg = (float)nCostSum / population.size();

			float fCostDev = 0;
			it = population.begin();
			for (; it != population.end(); ++it)
				fCostDev += (((*it)->cost - fCostAvg) * ((*it)->cost - fCostAvg));
			fout << fCostAvg << "\t" << sqrt(fCostDev / population.size()) << "\t" << endl;
		}
		else {
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
			fout << "Generations: " << nGeneration << endl;
			fout << "Score : " << pSol->cost << endl;
			for (int i=0; i<argc; ++i)
				fout << argv[i] << " ";
			fout << endl;
			fout << "================================================================================" << endl;
			fout << endl;
	#endif
		}

		fout.close();
	}

	delete g_pGridHelper;
}
