#include "lib/CmdLine.h"
#include "lib/Utililty.h"

using namespace std;

int main(int argc, char* argv[]) {
	time_t tCurrent = Utility::getMilliSec();

	CCmdLine cmdLine;
	cmdLine.SplitLine(argc, argv);

	string sInput( cmdLine.GetArgument("-i", 0) );
	string sOutput( cmdLine.GetArgument("-o", 0) );

}