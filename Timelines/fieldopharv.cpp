
 
// Class: fieldOpTill

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOpHarv.h"

fieldOpHarv::fieldOpHarv ()
{
	opCode = 9;
   collectStraw=false;
   Yield = 0.0;
};


void fieldOpHarv::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, bool CollectStraw, double cropYield)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
   collectStraw=CollectStraw;
   Yield = cropYield;
}

void fieldOpHarv::DefineOp(char * aName, int aNumber, int Year, int JulianDay, bool CollectStraw, double cropYield)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
   collectStraw=CollectStraw;
   Yield = cropYield;
}


void fieldOpHarv::outputOps(fstream * outfile, int status, bool printYear)
{
	fieldOp::outputOps(outfile, status, printYear);
	int dumint=0;
	if (collectStraw)
		dumint=1;
	*outfile << collectStraw << "\t" << Yield << "\t" ;
	if (verbosePrint)
		cout << collectStraw << "\t" << Yield << "\t";
	for (int i=0; i<4; i++)
	{
		*outfile << "-999" << "\t";
      if (verbosePrint)
			cout << "-999" << "\t";
	}
}

void fieldOpHarv::outputFnnOps(ofstream *outfile, int number)
{
	char Num[3];
	sprintf(Num,"%d",number);
	int dumint=0;
	if (collectStraw)
		dumint=1;
	*outfile << "HarvestDate(" << Num << ")" << "\t" << gettheOptime()->GetDay() << "/" <<
			gettheOptime()->GetMonth() << "/" << gettheOptime()->GetYear() << endl;
	*outfile << "HarvestRemoveStraw(" << Num << ")" << "\t" << dumint << endl;
}

void fieldOpHarv::ReadOpResults(ifstream * infile, int theYear, int theJulianDay)
{
   fieldOp::ReadOpResults(infile, theYear, theJulianDay);
   int dumint;
   *infile >> dumint;
   if (dumint==1)
   	collectStraw=true;
   else
   	collectStraw=false;
	for (int i=0; i<5; i++)
		*infile >> dumint;
   name = "Harvesting";
}

