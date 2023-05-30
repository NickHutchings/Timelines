#include "modelControlClass.h"
#include "base/message.h"
#include "NCUCropClass.h"
#include "tools/fileAccess.h"
#include <stdio.h>
#include <string.h>

//! used to send messages to screen or file
message * theMessage;
//! keeps the time and date
bsTime theTime;
//!If true, detailed output is sent to the screen
int verbosityLevel = 0;

//!Choice of algorithm for deriving timing of field operations
int routeNumber = 2;

//!
extern int getNCU(int,char[100]);
extern int getNCUfromHSMU(int,char[100]);



int main(int argc,char *argv[])
{
	char ncu_mars[]	= "climateNCU_MARS_ATEAM.ncus.txt";	//! name of the file containing the NCU number x MARS identifier matrix
	int baseYear	= 1971;//! base year for the meteorological data
	int range	= 10;//! number of years with each block of meteorological data
	bool zeroTillage=false;//! set to true is zero tillage should be  used for those crops where this technique is possible
	bool coverCropping=false;//! set to true if grass should be sown  in the autumn prior to a spring crop
	bool springGrazing=false;//! set to true if the timing of grazing should be skewed  towards the spring
	fileAccess hd;
	//! open file containing the parameters for the simulation. An error will be generated if this file does not exist or cannot be opened
	string commandFileName = "command.txt";
	hd.openFile(commandFileName,false);
	//! the name of input directory
	string input=hd.getLineFile();
	char inputDir[2000] ;
	strcpy(inputDir,input.c_str());
	int asci=inputDir[input.size()-1];
	if(asci==13)
	{
		inputDir[input.size()-1]='\0';
	}
	//! the name of output directory
	char outputDir[2000] ;
	string output=hd.getLineFile();
	strcpy(outputDir,output.c_str());
	asci=outputDir[output.size()-1];
	if(asci==13)
	{
		outputDir[output.size()-1]='\0';
	}
	//! initialise files for reporting  errors etc
	typedef char string100[256];
	string100 FN1,FN2, FN3, FN4, FN5, OutputDirectory;
	strcpy(OutputDirectory, outputDir);
	strcpy(FN1,OutputDirectory);
	strcat(FN1,"warnings.txt");
	strcpy(FN2,OutputDirectory);
	strcat(FN2,"logfile.txt");
	strcpy(FN3,OutputDirectory);
	strcat(FN3,"debug.txt");      //debug file name
	theMessage = new message();
	if (!theMessage->InitMessage(FN1,FN2,FN3,false))
		cout << "Error opening message files" << endl;
	//! determine whether this is an arable or grazed grassland rotation
	int dumint=0;
	//! if arableRotation is set to true, any grassland will be cut and not grazed
	bool arableRotation;
	dumint=hd.GetIntFromFile();
	if (dumint==1)
		arableRotation=true;
	else
		arableRotation=false;

	//! read number of crop sequences per NCU record
	int  numOfFields=hd.GetIntFromFile();

	//! the name and open file containing the crop rotation information
	string atempname;
	atempname=hd.getLineFile();
	char croprotFileName[2000];
	strcpy(croprotFileName,atempname.c_str());
	asci=croprotFileName[atempname.size()-1];
	if(asci==13)
	{
		croprotFileName[atempname.size()-1]='\0';
	}
	//! the name of output file
	char outfileName[256];
	atempname=hd.getLineFile();
	strcpy(outfileName,atempname.c_str());
	asci=outfileName[atempname.size()-1];
	if(asci==13)
	{
		outfileName[atempname.size()-1]='\0';
	}
	//! the number of the first and last NCU for which timelines should be generated
	//! if one or other of these has a value of -1,all the NCUs in the file will be read
	int startNCUI = hd.GetIntFromFile();
	int endNCUI = hd.GetIntFromFile();

	int startDay = hd.GetIntFromFile();
	cout<<startDay<<endl;
	int startYear = hd.GetIntFromFile();
	cout<<startYear<<endl;
	if (startYear<baseYear)
		theMessage->FatalError("Start year is before start of dataset");
	int endDay = hd.GetIntFromFile();
	int endYear = hd.GetIntFromFile();
	if (startYear>endYear)
		theMessage->FatalError("Start year is greater than end year");
	//! Read if zero tillage should be applied
	dumint=hd.GetIntFromFile();
	if (dumint==1)
		zeroTillage=true;
	//! Read if cover cropping should be used
	dumint=hd.GetIntFromFile();
	if (dumint==1)
		coverCropping=true;
	//! Read if spring grazing should be used
	dumint=hd.GetIntFromFile();
	if (dumint==1)
		springGrazing=true;
	int implementationYear=0;
	implementationYear=hd.GetIntFromFile();
	if ((zeroTillage)||(coverCropping)||(springGrazing))
		if ((implementationYear<startYear)||(implementationYear>endYear))
			theMessage->FatalError("Implementation year for measures is out of range");

	verbosityLevel=hd.GetIntFromFile();

	hd.closeFile();
	hd.changeDir(input);
	//! the following information is not currently used
#ifdef __PORTABLE__
	char metdataInputDir[] = "D:\\";
	char tpvStem[] 	= "MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN";
	char radStem[]		= "radCRU_NCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN";
#else
	char metdataInputDir[] = "C:\\Datastore\\JRCMetdatastore\\";
	char tpvStem[] 	= "tpv\\MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN";
	char radStem[]		= "rad\\radCRU_NCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN";
#endif

	//! open the file containing the crop rotation information
	char LongcroprotFileName[2000];
	sprintf(LongcroprotFileName,"%s\%s",inputDir,croprotFileName);
	fstream *rotationFile = new fstream();
	rotationFile->open(LongcroprotFileName,fstream::in);
#ifdef __BCplusplus__
	if (!rotationFile)
#else
	if (!rotationFile->is_open())
#endif
			//	      theMessage->FatalError("NCUClass: crop rotation file ", croprotFileName.c_str(), " not found");
			theMessage->FatalError("NCUClass: crop rotation file ", croprotFileName, " not found");

	bool readAll;
	int maxNUCs = 500000;
	if ((startNCUI>0)&&(endNCUI>0))
		readAll = false;  //set true if NCUI is to be read from file
	else readAll = true;

	modelControlClass *aControlClass = new modelControlClass();

	aControlClass->processCropManagement(arableRotation, startDay,  startYear,  endDay,  endYear, maxNUCs, numOfFields, baseYear, range,
			startNCUI,  endNCUI, metdataInputDir, tpvStem, radStem, ncu_mars, inputDir, outputDir, rotationFile, outfileName, readAll,
			verbosityLevel,implementationYear,  zeroTillage, coverCropping, springGrazing);

	cout << "Finished" << endl;
	delete aControlClass;
	rotationFile->close();
	delete rotationFile;
	delete theMessage;
}

