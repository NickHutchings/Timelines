
 
// Class: fieldOpCut

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOpCut.h"

void fieldOpCut::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, double aYield)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
	yield =aYield;
}

void fieldOpCut::DefineOp(char * aName, int aNumber, int Year, int JulianDay, double aYield)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
	yield =aYield;
}

void fieldOpCut::outputOps(fstream * outfile, int status, bool printYear)
{
	fieldOp::outputOps(outfile, status, printYear);
	*outfile << yield << "\t";
    if (verbosePrint)
			cout << yield << "\t";
	for (int i=0; i<5; i++)
	{
		*outfile << "-999" << "\t";
      if (verbosePrint)
			cout << "-999" << "\t";
	}
}
void fieldOpCut::ReadOpResults(ifstream * infile, int theYear, int theJulianDay)
{
   fieldOp::ReadOpResults(infile, theYear, theJulianDay);
   int dumint;
	for (int i=0; i<6; i++)
		*infile >> dumint;
   name = "Cutting";
}


