
 
// Class: fieldOpGrass

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOpGrass.h"   
#include "base/message.h"

fieldOpGrass::fieldOpGrass (int theGrazingType,int anOpCode)
{
	switch (anOpCode)
	{
	case 5: name = "Start grazing";
	break;
	case 6: name = "Stop grazing";
	break;
	default:		theMessage->FatalError("Input operation code does not match grazing operation code");
	}
	opCode =anOpCode;
	grazingType=theGrazingType;
}


//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
\param LAI leaf area index after grazing
 */

void fieldOpGrass::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, int theGrazingType, double Height)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
	grazingHeight=Height;
	name = "Start grazing";
	grazingType = theGrazingType;
	opCode = 5;
}

void fieldOpGrass::DefineOp(char * aName, int aNumber, int Year, int JulianDay, int theGrazingType, double Height)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
	grazingHeight=Height;
	name = "Start grazing";
	grazingType = theGrazingType;
	opCode = 5;
}
void fieldOpGrass::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, int theGrazingType)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
	name = "Stop grazing";
	grazingType = theGrazingType;
	opCode = 6;
}

void fieldOpGrass::DefineOp(char * aName, int aNumber, int Year, int JulianDay, int theGrazingType)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
	name = "Stop grazing";
	grazingType = theGrazingType;
	opCode = 6;
}

void fieldOpGrass::outputOps(fstream * outfile, int status, bool printYear)
{
	fieldOp::outputOps(outfile, status, printYear);
	int numDumint;
	if (opCode==5)
	{
		*outfile << grazingType << "\t" << grazingHeight << "\t";
		if (verbosePrint)
			cout << grazingType << "\t" << grazingHeight << "\t";
		numDumint=4;
	}
	else
	{
		*outfile << grazingType << "\t";
		if (verbosePrint)
			cout << grazingType << "\t";
		numDumint=5;
	}
	for (int i=0; i<numDumint; i++)
	{
		*outfile << "-999" << "\t";
		if (verbosePrint)
			cout << "-999" << "\t";
	}
}
void fieldOpGrass::ReadOpResults(ifstream * infile, int theYear, int theJulianDay, int anOpCode)
{
	fieldOp::ReadOpResults(infile, theYear, theJulianDay, anOpCode);
	int numDumint;
	if (opCode==5)
	{
		*infile >> grazingType >> grazingHeight;
		numDumint=4;
	}
	else
	{
		numDumint=5;
		*infile >> grazingType;
	}
	int dumint;
	for (int i=0; i<numDumint; i++)
		*infile >> dumint;
	switch (anOpCode)
	{
	case 5: name = "Start grazing";
	break;
	case 6: name = "Stop grazing";
	break;
	default:		theMessage->FatalError("Input operation code does not match grazing operation code");
	}
}


