

// Class: modelControlClass

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "modelControlClass.h"
#include "NCUClass.h"
#include "climateNCU_MARS_ATEAM.05.Extract_Data.Func.V2.h"
#include "base/unix_util.h"
#include <stdlib.h>
#include "base/message.h"
#ifdef __ANSICPP__
	#include "base/nixcbits.h"
#endif
#include <stdio.h>
#include <string.h>

extern int getMARS(int hsmuID,char Cmars[256]);

modelControlClass::modelControlClass ( )
{
};

modelControlClass:: ~ modelControlClass ( )
{
};

void modelControlClass::processCropManagement(bool isArable, int startDay,  int startYear, int endDay,  int endYear,  int maxNCUs, int numFields, int baseYear,
 int range,	int startNCUI,  int endNCUI, string metdataInputDir, string tpvStem, string radStem, string ncu_mars, string inputDir,
 string outputDir, fstream* infile, string outfileName, bool readAll, int verbosityLevel,  int implementationYear, bool usezeroTillage, bool usecoverCropping,
 bool springGrazing)
{

	string inputFileName;
	inputFileName = inputDir;
	inputFileName+="NCU_MARS.txt";

	string outputFileName;
	outputFileName=outputDir;
	outputFileName+=outfileName;
	fstream *outfile = new fstream();
	cout << "Output file name " << outputFileName << endl;
	string outString = "Output file name ";
	outString.append(outputFileName);
	theMessage->LogEvent(outString.c_str());
	outfile->open(outputFileName.c_str(),fstream::out);
	if (!outfile->is_open())
		theMessage->FatalError("modelControlClass::processCropManagement could not open output file ", outputFileName.c_str());

	char tempChar[10];
	bool keepGoing = true;
	int ncu=-1;
	int count = 0;
	bool passOver=true;
	while ((keepGoing)&& (count<=maxNCUs))
	{
		int retCode=1; // NCUClass::Initialise will set this to 0 if the NOCR code is found (no cropping)
		int NCUI;
		*infile >> NCUI;
		if (!infile->is_open())
			return;
		char tempString[256];
		strcpy(tempString,ncu_mars.c_str());
		ncu = getNCUfromHSMU(NCUI,tempString);
		if (!readAll)
		{
			if (NCUI==startNCUI)
				passOver=false;
			if (NCUI>endNCUI)
				retCode=2;
		}
		else
			passOver=false;
		NCUClass * aNCUClass = new NCUClass();
		if (retCode==1)
		{
			aNCUClass->Initialise(inputDir, numFields, ncu, NCUI);
			retCode=aNCUClass->ReadData(isArable, startDay,  startYear,  endDay,  endYear,  baseYear, range,
					metdataInputDir, tpvStem, radStem, ncu_mars, inputDir, infile, readAll);
		}
		if ((retCode==0)&&(ncu==99999))
			retCode=1;
		switch (retCode)
		{
		case 0:
		{
			cout << "Count " << count << " Reading ncu " << ncu << " NCUI " << aNCUClass->getNCUI() << endl;
			if (verbosityLevel>0)
			{
				outString.clear();
				itoa(ncu,tempChar,10);
				outString = "Reading ";
				outString.append(tempChar);
				outString.append(" NCUI ");
				itoa(aNCUClass->getNCUI(),tempChar,10);
				outString.append(tempChar);
				theMessage->LogEvent(outString.c_str());
			}
			break;
		}
		case 1:
		{
			cout << "Skipping " << ncu << " NCUI " << aNCUClass->getNCUI() << endl;
			itoa(ncu,tempChar,10);
			outString.clear();
			outString.append("Skipping ncu ");
			outString.append(tempChar);
			outString.append(" NCUI ");
			itoa(aNCUClass->getNCUI(),tempChar,10);
			outString.append(tempChar);
			theMessage->WarningWithDisplay(outString.c_str());
		}
		break;
		case 2:
			keepGoing=false;
			break;
		default:
			theMessage->FatalError("NCUClass - read value on return is not in range 0 to 2");
		}
		if (!passOver)
		{
			char *tempstring=new char[inputFileName.size()];
			strcpy(tempstring,inputFileName.c_str());
			int MARS = getMARS(aNCUClass->getNCUI(),tempstring);
			if (MARS==99999)
			{
				cout << "Skipping " << ncu << " NCUI " << aNCUClass->getNCUI() << " due to missing MARS code" << endl;
				itoa(ncu,tempChar,10);
				outString.clear();
				outString.append("Skipping ncu due to missing MARS code ");
				outString.append(tempChar);
				outString.append(" NCUI ");
				itoa(aNCUClass->getNCUI(),tempChar,10);
				outString.append(tempChar);
				theMessage->WarningWithDisplay(outString.c_str());
				retCode=1;
			}
			if (retCode==0)
			{
				cout << "Processing " << count << " NCUI " << aNCUClass->getNCUI() << endl;
				aNCUClass->setcoverCropping(usecoverCropping);
				aNCUClass->setzeroTillage(usezeroTillage);
				aNCUClass->setMARS(MARS);
				aNCUClass->calcCropManagement(inputDir, outputDir,startYear,implementationYear, springGrazing);
				retCode=aNCUClass->generateOps(implementationYear);
				aNCUClass->CheckOperationIntegrity();
				if (retCode==0)
					aNCUClass->outputOps(outfile);
			}
		}
		else
			cout << "Passing over " << count << " NCUI " << aNCUClass->getNCUI() << endl;
		delete aNCUClass;
		count++;
		ncu++;
	}
	cout << "Finished after " << count << " NCUs" << endl;
	itoa(count,tempChar,10);
	outString.clear();
	outString = "Finished after  ";
	outString.append(tempChar);
	outString.append(" NCUs ");
	theMessage->LogEvent(outString.c_str());
	outfile->close();
	delete outfile;
}

