#include "lib/CmdLine.h"
#include "lib/Utililty.h"
#include "GridHelper.h"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;
using namespace GA;

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
	Solution::Vector population;
	population.reserve(n);
	for (int i=0; i<n; ++i) {
		Solution::Ptr p = make_shared<Solution>(grid.columns, grid.rows);

		int size = p->width * p->height;
		for (int k=0; k<size; ++k) {
			int idx = rand() % grid.values.size();
			p->genotype.push_back(grid.values[idx]);
		}
		p->cost = grid.scoreGrid(p);
		population.push_back(p);
	}

	int nGenerations = 0;
/*
	while (true) {
		Solution::Vector offsprings;
		offsprings.reserve(k);

		for (int i=0; i<k; ++i) {

		}

		++nGenerations;
#if defined (_PROFILE)

#endif
		if (Utility::getMilliSec() > tEnd)
			break;
	}
*/

	//////// output
	ofstream fout;
	fout.open(sOutput, fstream::out | fstream::trunc);
	if (fout.is_open()) {
		Solution::Ptr pSol = population.front();
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