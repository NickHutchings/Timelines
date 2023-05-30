
 
// Class: fieldOpTill

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOpTill.h"   


void fieldOpTill::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, float tillageDepth)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
   depth=tillageDepth;
}

void fieldOpTill::DefineOp(char * aName, int aNumber, int Year, int JulianDay, float tillageDepth)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
   depth=tillageDepth;
}

void fieldOpTill::ReadOpResults(ifstream * infile, int theYear, int theJulianDay)
{
   fieldOp::ReadOpResults(infile, theYear, theJulianDay);
   *infile >> depth;
   int dumint;
	for (int i=0; i<5; i++)
		*infile >> dumint;
   name = "Tillage";
}


void fieldOpTill::outputOps(fstream * outfile, int status, bool printYear)
{
	fieldOp::outputOps(outfile, status, printYear);
	*outfile << depth << "\t" ;
	if (verbosePrint)
		cout << depth << "\t" ;
	for (int i=0; i<5; i++)
	{
		*outfile << "-999" << "\t";
      if (verbosePrint)
			cout << "-999" << "\t";
	}
}

void fieldOpTill::outputFnnOps(ofstream *outfile, int number)
{
	char Num[3];
	sprintf(Num,"%d",number);
	*outfile << "TillageDate(" << Num << ")" << "\t" << gettheOptime()->GetDay() << "/" <<
			gettheOptime()->GetMonth() << "/" << gettheOptime()->GetYear() << endl;
	if (verbosePrint)
		cout << "TillageDate(" << Num << ")" << "\t" << gettheOptime()->GetDay() << "/" <<
				gettheOptime()->GetMonth() << "/" << gettheOptime()->GetYear() << endl;
	if (depth>100.0)
	{
		*outfile << "TillageType(" << Num << ")" << "\t" << "PLOUGH" << endl;
		*outfile << "TillageIncorporation(" << Num << ")" << "\t" << 0.5 << endl;
	}
	else
	{
		*outfile << "TillageType(" << Num << ")" << "\t" << "HARROW" << endl;
		*outfile << "TillageIncorporation(" << Num << ")" << "/t" << 0.5 << endl;
	}
	*outfile << "TillageDepth(" << Num << ")" << "\t" << (depth * 100.0) << endl;//! note that FASSET requires tillage steps in millimetres
}
