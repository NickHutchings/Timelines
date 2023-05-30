

// Class: fieldOp

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOp.h"
//! Describes a generalised or specific field operation

fieldOp::fieldOp()
{
	verbosePrint = true;
	Initialise();
}

fieldOp:: ~ fieldOp ( )
{
	delete theOptime;
};

void fieldOp::DefineOp(char * aName, int aopCode, int Year, int Month, int Day)
{
 name=aName;
 theOptime->SetTime(Day, Month, Year);
 opCode = aopCode;
}

void fieldOp::DefineOp(char * aName, int aopCode, int Year, int JulianDay)
{
 name=aName;
 theOptime->SetTime(Year,JulianDay);
 opCode = aopCode;
}
//! Initialise an instance
void fieldOp::Initialise()
{
 theOptime = new bsTime();
 name = "No operation" ;
}

void fieldOp::outputOps(fstream * outfile, int thestatus, bool printYear)
{
	status = thestatus;
	if (printYear)
	{
		*outfile << theOptime->GetYear() << "\t" ;
      if (verbosePrint)
			cout << theOptime->GetYear() << "\t";

	}
	*outfile << theOptime->GetJulianDay() << "\t" << opCode << "\t" << status << "\t";
	if (verbosePrint)
		cout << theOptime->GetJulianDay() << "\t" << opCode << "\t" << status << "\t";
}

void fieldOp::ReadOpResults(ifstream * infile, int theYear, int theJulianDay)
{
	theOptime->SetTime(theYear,theJulianDay);
	*infile >> status;
}

void fieldOp::ReadOpResults(ifstream * infile, int theYear, int theJulianDay, int anOpCode)
{
	ReadOpResults(infile, theYear, theJulianDay);
	if (opCode!=anOpCode)
		theMessage->FatalError("Input operation code does not match class operation code");
	name = "No operation" ;
}

void fieldOp::printToSceen()
{
	cout << theOptime->GetYear() << "\t" << theOptime->GetJulianDay() << "\t" << opCode << endl;
}

