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

	Config() {};
	Config(CCmdLine cmdLine) {
		sInput = ( cmdLine.GetArgument("-i", 0) );
		sOutput = ( cmdLine.GetArgument("-o", 0) );
		nPopulation = Utility::strTo<int>( cmdLine.GetArgument("-n", 0) );
		fGenerationGap = Utility::strTo<float>( cmdLine.GetArgument("-gap", 0) );
		bPlot = cmdLine.HasSwitch("-plot");
		nPlot = bPlot ? Utility::strTo<int>( cmdLine.GetArgument("-plot", 0) ) : 0;
		tDuration = cmdLine.HasSwitch("-t") ? Utility::strTo<int>( cmdLine.GetArgument("-t", 0) ) : 360000 - 2500;
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
	
	if (config.bPlot) {
		cout << "Gen\tSize\tBest\tMed\tWorst\t"
			<< "CostAvg\tCostSTD\t"
			<< "SelAvg\tSelSTD\t"
			<< "DiffAvg\tDiffSTD\t"
			<< "XEffAvg\tXEffSTD"
			<< endl;
		cout.precision(4);
	}

	////////////////////////////////////////////////////////
	//////// main Loop
	GA::GAHelper ga(cmdLine);
	int nGeneration = 0;
	int k = config.nPopulation * config.fGenerationGap;
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

			long long nSelCostSum = 0;
			for (int i=0; i<parentsVec.size(); ++i) {
				nSelCostSum += parentsVec[i].first->cost;
				nSelCostSum += parentsVec[i].second->cost;
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

			float fSelCostAvg = (float)nSelCostSum / (parentsVec.size() * 2);
			float fSelCostDev = 0;
			for (int i=0; i<parentsVec.size(); ++i) {
				fSelCostDev += pow((parentsVec[i].first->cost - fSelCostAvg), 2);
				fSelCostDev += pow((parentsVec[i].second->cost - fSelCostAvg), 2);
			}

			float fDiffAvg = accumulate(solutionDiff.begin(), solutionDiff.end(), 0) / solutionDiff.size();
			float fDiffDev = 0;
			for (int i=0; i<solutionDiff.size(); ++i)
				fDiffDev += pow((solutionDiff[i]- fDiffAvg) , 2);

			float fXEffectAvg = accumulate(crossEffects.begin(), crossEffects.end(), 0) / crossEffects.size();
			float fXEffectDev = 0;
			for (int i=0; i<crossEffects.size(); ++i)
				fXEffectDev += pow((crossEffects[i]- fXEffectAvg) , 2);
			
			cout << nGeneration << "\t" << population.size() << "\t" <<
				pBest->cost << "\t" << pMedian->cost << "\t" << pWorst->cost << "\t"
				<< fixed
				<< fCostAvg << "\t" << sqrt(fCostDev / population.size()) << "\t" 
				<< fSelCostAvg << "\t" << sqrt(fSelCostDev / (parentsVec.size() * 2)) << "\t" 
				<< fDiffAvg << "\t" << sqrt(fDiffDev / solutionDiff.size()) << "\t"
				<< fXEffectAvg << "\t" << sqrt(fXEffectDev / crossEffects.size()) << endl;
		}

		tCurrent = Utility::getMilliSec();
		if (tCurrent > tEnd)
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
		fout.close();
	}

	delete g_pGridHelper;
}
