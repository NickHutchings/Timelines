

// Class: NCUClass

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "NCUClass.h"
#include "climateNCU_MARS_ATEAM.05.Extract_Data.Func.V2.h"
#include "base/message.h"
#include "operationData.h"
#include "fieldOp.h"
#include "fieldOpTill.h"
#include "fieldOpSow.h"
#include "fieldOpFert.h"
#include "fieldOpHarv.h"
#include "fieldOpGrass.h"
#include "fieldOpCut.h"
#include "base/nixcbits.h"
#include "base/unix_util.h"
#ifdef __ANSICPP__
	#include <cmath>
	#include <istream>
	#include <iostream>
	#include <cstring>
#else
	#include <fstream.h>
	#include <iostream.h>

#endif
#include "tools/fileAccess.h"
#include "tools/convertData.h"
#include <vector>
#include <stdio.h>
#include <string.h>
extern int getNCU_ID(int,char[100]);
extern int getNCUfromHSMU(int,char[100]);
extern int verbosityLevel;
extern int routeNumber;

NCUClass::NCUClass ( )
{
 NCU=0;
 NCUI=0;
 N_added=0.0;
 slurryApplicMethod = 1;
 solidApplicMethod=1;
 ureaApplicMethod=1;
};

NCUClass:: ~ NCUClass ( )
{
	delete theYearPlans;
	delete theSeriesOperations;
};

void NCUClass::Initialise(string inputDir, int thenumFields, int aNCU, int aNCUI)
{
//load list of replacement crops to be used for those crops not present in CGMS data
	loadReplacements(inputDir);

   theYearPlans= new linkList<cropRotationClass>;
   theSeriesOperations = new linkList<cropSeriesClass>;
   maxFields = thenumFields;
   NCU=aNCU;
   NCUI=aNCUI;
}


int NCUClass::ReadData(bool isArable, int startDay,  int startYear,  int endDay,  int endYear,  int baseYear, int range,
		string metdataInputDir, string tpvStem, string radStem, string ncu_mars, string inputDir, fstream * rotationFile,
      bool readAll)
{

	int retVal=0;
	string aString;
   for (int year=startYear;year<(endYear+1); year++)
   {
      cropRotationClass * acropRotationClass = new cropRotationClass();
      acropRotationClass->setYear(year);
      theYearPlans->InsertLast(acropRotationClass);
   }

    string testString;

   if (verbosityLevel==1)
	   cout << " NCUI " << NCUI << endl;

   int fieldNum=0; //crop number


   //loop through crop series
   int startCol = startYear - baseYear + 1;
   while ((fieldNum<maxFields)&&(retVal==0))
   {
	   retVal=ReadCropSequence(isArable, fieldNum, startCol, rotationFile);
      fieldNum++;
   }

   if (retVal==0)
   {
   	if (routeNumber>2)	//met data not needed for Route 1
      {
      	//get met data
         for (int i=0; i<50;i++)
         for (int j=0;j<366;j++)
         for (int k=0; k<6; k++)
            metdata[i][j][k]=0.0;
         retVal = getMetData(NCU, startDay, startYear, endDay, endYear, metdataInputDir,
               tpvStem, radStem, ncu_mars, baseYear, range, metdata);
      }
   }
   return retVal;
}

int NCUClass::ReadCropSequence(bool isArable, int fieldNum, int startCol, fstream * rotationFile)
{
	int retVal=0;
#ifdef __BCplusplus__
	char dummy[5000];
#else
	string testString;
#endif
	char str10[10];

	  bool abort=false;
      int row=0;
      int maxRows; //number of variables per crop
      if (isArable)
    	  maxRows = 19;
      else
    	  maxRows=19;
      while (row<=maxRows)
      {
      	if (!abort)
      	{
#ifdef __BCplusplus__
	         rotationFile->getline(dummy,5000,'\n');
#else
      		testString.clear();
      		getline(*rotationFile, testString);
      		if (rotationFile->eof())
      		{
      			retVal=2;
      			return retVal;
      		}
#endif
      	}
		if (verbosityLevel==1)
		{
         	cout << "Crop sequence " << fieldNum << " Row number " << row << endl;
          	cout << testString << endl;
		}
#ifdef __BCplusplus__
		istrstream inputString(dummy,strlen(dummy));
#else
  		stringstream inputString(stringstream::in | stringstream::out);
   		inputString << testString;
#endif
        //find first year's data'
		int leadIn;
		if ((row==0)&&(fieldNum==0))
			leadIn =startCol-1;
		else
		{
			//first read and discard the NCUID
			int dumint;
			inputString >> dumint;
			leadIn=startCol-1;
		}
		for (int i=0; i<leadIn; i++)
		{
			inputString >> str10;
			if (verbosityLevel==1)
				cout << i << " " << str10;
		}
		int yearNum=0;
		while (yearNum<theYearPlans->NumOfNodes())
        {
			if (inputString.eof())
			{
				char tempChar[10];
				itoa(NCUI,tempChar,10);
				cout << "Error in year number " << yearNum << endl;
				inputString.unget();
				char test;
				inputString >> test;
				cout << "Last character is " << test << " ASCII code = " << (int(test)) << endl;
				theMessage->FatalError("Too few data in ", tempChar);
			}
			cropRotationClass * acropRotationClass = theYearPlans->ElementAtNumber(yearNum);
			NCUCropClass *aNCUCropClass;
			if (row==0)
			{
				aNCUCropClass = new NCUCropClass();
				aNCUCropClass->Initialise();
				if (verbosityLevel==1)
					cout << yearNum << " ";
				abort = aNCUCropClass->ReadData(&inputString, row, yearNum);
				if (abort)  //no cropping
				{
					delete aNCUCropClass;
					aNCUCropClass = NULL;
				}
				else
					acropRotationClass->gettheCrops()->InsertLast(aNCUCropClass);
			}
			else
			{
				if (!abort)
				{
					aNCUCropClass = acropRotationClass->gettheCrops()->ElementAtNumber(fieldNum);
					if (aNCUCropClass)
						aNCUCropClass->ReadData(&inputString, row, yearNum);
				}
			}
			if (isArable)
				aNCUCropClass->zeroGrazingDays();
			yearNum++;
        }
		row++;
		if (verbosityLevel==1)
			cout << endl;
      }
      return retVal;
}

void NCUClass::calcCropManagement(string inputDir, string outputDir,int startYear, int implementationYear,
		bool springGrazing)
{
	//if (NCUI==711288310)
		//verbosityLevel=1;
	bool abort = false;
	int yearNo=0;
	int year = startYear;
	char inputFileName[256];
	int defaultspringBarleySowing =92;
	int defaultwinterWheatSowing = 279;

	//get timing of sowing of spring barley (used for timing of fertilisation of winter crops)
	//get crop rotation for required number of years
	while (yearNo<(theYearPlans->NumOfNodes()))
	{
		int defaultsowingDay;  //Europe-wide average date, for situations where there is no sowing date within a NUTS region
		year = startYear + yearNo;
		int lastDayOfYear;
		if (((year%4==0) && (year%100!=0)) || (year%400==0))
			lastDayOfYear = 366;
		else
			lastDayOfYear = 365;
		strcpy(inputFileName,"SpBarl.txt");
		springBarleySowing=getCropEventDate(inputDir, inputFileName, MARS, 1, true, year);
		if (springBarleySowing==-99)
			springBarleySowing=defaultspringBarleySowing;
		strcpy(inputFileName,"WinWheat.txt");
		winterWheatSowing=getCropEventDate(inputDir, inputFileName, MARS, 1, true, year);
		if (winterWheatSowing==-99)
			winterWheatSowing=defaultwinterWheatSowing;
		if ((winterWheatSowing-springBarleySowing)<100)  //if less than 100 days for growing season, assume that there is an error in the MARS file
		{
			springBarleySowing=defaultspringBarleySowing;
			winterWheatSowing=defaultwinterWheatSowing;
			string outstring = "Default sowing dates used for spring barley and winter wheat in NCU ";
			convertData aData;
			string tempstring = aData.IntToString(NCU);
			outstring.append(tempstring);
			outstring.append(" in year ");
			tempstring = aData.IntToString(year);
			outstring.append(tempstring);
			theMessage->WarningWithDisplay(outstring,"","");
		}
		int lengthOfGrowingSeason = winterWheatSowing - springBarleySowing + 21;
		cropRotationClass * acropRotationClass = theYearPlans->ElementAtNumber(yearNo);
		for (int cropNo = 0; cropNo<acropRotationClass->gettheCrops()->NumOfNodes(); cropNo++)
		{
			NCUCropClass * aNCUCropClass = acropRotationClass->gettheCrops()->ElementAtNumber(cropNo);
			if (aNCUCropClass)
			{
				string cropName = aNCUCropClass->getCropName();
				if (!(cropName.length()==4))
				{
					theMessage->FatalError("Crop name not 4 characters ",cropName.c_str());
				}
				//see if this crop needs to be replaced by one for which crop timing is available
				string * localName = new string(cropName);
				getReplacements(localName);
				if (!(localName->length()==4))
				{
					theMessage->FatalError("Crop name not 4 characters ",localName->c_str());
				}
				int sowingDay=0;
				int harvestingDate=0;
				int diff = 0;//default difference between sowing and harvesting date, for when harvesting date is missing
				bool gotit = false;
				if ((localName->compare("SWHE")==0) || (localName->compare("DWHE")==0))
				{
					if (routeNumber==1)
						strcpy(inputFileName,"SpWheat.csv");
					else
						strcpy(inputFileName,"SpWheat.txt");
					diff = 126;
					defaultsowingDay = 99;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("POTA")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"Potato.csv");
					else
						strcpy(inputFileName,"Potato.txt");
					diff = 137;
					defaultsowingDay=115;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("BARL")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"SpBarl.csv");
					else
						strcpy(inputFileName,"SpBarl.txt");
					diff = 128;
					defaultsowingDay=92;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("MAIF")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"FodMaize.csv");
					else
						strcpy(inputFileName,"FodMaize.txt");
					diff = 112;
					defaultsowingDay=124;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("MAIZ")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"GraMaize.csv");
					else
						strcpy(inputFileName,"GraMaize.txt");
					diff = 165;
					defaultsowingDay=117;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("RAPE")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"WinRape.csv");
					else
						strcpy(inputFileName,"WinRape.txt");
					diff = 306;
					defaultsowingDay=243;
					aNCUCropClass->setWinterCrop(true);
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("SUGB")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"SugBeet.csv");
					else
						strcpy(inputFileName,"SugBeet.txt");
					diff = 169;
					defaultsowingDay=108;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("SUNF")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"SunFlower.csv");
					else
						strcpy(inputFileName,"SunFlower.txt");
					diff = 150;
					defaultsowingDay=106;
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}
				if (localName->compare("WWHE")==0)
				{
					if (routeNumber==1)
						strcpy(inputFileName,"WinWheat.csv");
					else
						strcpy(inputFileName,"WinWheat.txt");
					diff = 283;
					defaultsowingDay=279;
					aNCUCropClass->setWinterCrop(true);
					aNCUCropClass->setminumumDaysFertManHarvest(21);
					gotit = true;
				}

				if (localName->compare("GRAP")==0) //permanent grassland
				{
					if (routeNumber==1)
						strcpy(inputFileName,"SpBarl.csv");
					else
						strcpy(inputFileName,"SpBarl.txt");
					diff = 128;
					defaultsowingDay=92;
					aNCUCropClass->setWinterCrop(false);
					aNCUCropClass->setisPermanent(true);
					aNCUCropClass->setminumumDaysFertManHarvest(0);
					gotit = true;
				}
				if ((localName->compare("SETA")==0)||(localName->compare("GRAS")==0))
				{
					if (routeNumber==1)
						strcpy(inputFileName,"WinWheat.csv");
					else
						strcpy(inputFileName,"WinWheat.txt");
					diff = 283;
					defaultsowingDay=279;
					aNCUCropClass->setWinterCrop(true);
					aNCUCropClass->setminumumDaysFertManHarvest(0);
					gotit = true;
				}
				//need values for winter barley, spring rape
				if (!gotit)
				{
					cout << "Error in year " << year << " and crop " << cropNo << endl;
					theMessage->FatalError("File not found for ",localName->c_str());
				}
				sowingDay=getCropEventDate(inputDir, inputFileName, MARS, 1, true, year);
				if (sowingDay == -99)
					sowingDay=defaultsowingDay;
				if ((!localName->compare("GRAP")==0)||(yearNo==0)) //do sowing for all crops except permanent grassland (for that too, in first year
				{
					aNCUCropClass->setSowingDay(sowingDay,0);
				}
				if (localName->compare("SETA")!=0)   //skip harvesting for set-aside
				{

					if ((localName->compare("GRAS")==0)||(localName->compare("GRAP")==0))
					{
						int earliestStart = 180 - round(lengthOfGrowingSeason/2);
						if (earliestStart<0)
							theMessage->FatalError("NCUClass - earliest start of grazing is negative");
						if ((localName->compare("GRAP")==0)&&(yearNo==0))
						{
							earliestStart=sowingDay + 30; //spare the permanent grass from grazing in the start of the first year
						}
						//default dates for start and stop of grazing dairy cattle
						int grazingDays;
						if ((springGrazing)&& (year>=implementationYear))
						{
							//dairy cattle
							grazingDays = round(aNCUCropClass->getGrazingDays(0));
							if (grazingDays>0)
							{
								if ((earliestStart+grazingDays)>lastDayOfYear)
								{
									aNCUCropClass->setGrazingStartDay(1,0,0);
									aNCUCropClass->setGrazingStopDay(lastDayOfYear,0,0);
								}
								else
								{
									aNCUCropClass->setGrazingStartDay(earliestStart,0,0);
									aNCUCropClass->setGrazingStopDay(earliestStart+grazingDays,0,0);
								}
							}
							//other cattle
							grazingDays = round(aNCUCropClass->getGrazingDays(1));
							if (grazingDays>0)
							{
								if ((earliestStart+grazingDays)>lastDayOfYear)
								{
									aNCUCropClass->setGrazingStartDay(1,1,0);
									aNCUCropClass->setGrazingStopDay(lastDayOfYear,1,0);
								}
								else
								{
									aNCUCropClass->setGrazingStartDay(earliestStart,1,0);
									aNCUCropClass->setGrazingStopDay(earliestStart+grazingDays,1,0);
								}
							}
							//sheep
							grazingDays = round(aNCUCropClass->getGrazingDays(2));
							if (grazingDays>0)
							{
								if ((earliestStart+grazingDays)>lastDayOfYear)
								{
									aNCUCropClass->setGrazingStartDay(1,2,0);
									aNCUCropClass->setGrazingStopDay(lastDayOfYear,2,0);
								}
								else
								{
									aNCUCropClass->setGrazingStartDay(earliestStart,2,0);
									aNCUCropClass->setGrazingStopDay(earliestStart+grazingDays,2,0);
								}
							}
						}
						else	//no spring grazing or is before implementation year
						{
							//dairy cattle
							grazingDays = round(aNCUCropClass->getGrazingDays(0));
							if (grazingDays>0)
							{
								aNCUCropClass->setGrazingStartDay(round(180.0-grazingDays/2),0,0);
								aNCUCropClass->setGrazingStopDay(round(180.0+grazingDays/2),0,0);
							}
							//other cattle
							grazingDays = round(aNCUCropClass->getGrazingDays(1));
							if (grazingDays>0)
							{
								aNCUCropClass->setGrazingStartDay(round(180.0-grazingDays/2),1,0);
								aNCUCropClass->setGrazingStopDay(round(180.0+grazingDays/2),1,0);
							}
							//sheep
							grazingDays = round(aNCUCropClass->getGrazingDays(2));
							if (grazingDays>0)
							{
								aNCUCropClass->setGrazingStartDay(round(180.0-grazingDays/2),2,0);
								aNCUCropClass->setGrazingStopDay(round(180.0+grazingDays/2),2,0);
							}
						}

						//assume 1 month between cuts
						int numberOfCuts = round(lengthOfGrowingSeason/30);
						if (numberOfCuts>aNCUCropClass->getmaxnumberHarvestCutDates())
							theMessage->FatalError("NCUClass::calcCropManagement - Too many grass cuts");
						double totalYield = aNCUCropClass->getYield(0);
						int cutDate = springBarleySowing + 28;
						double yield = round(totalYield/numberOfCuts);
						for (int cutNo=0; cutNo<numberOfCuts; cutNo++)
						{
							aNCUCropClass->setCuttingDay(cutDate,cutNo);
							aNCUCropClass->setYield(cutNo,yield); //yield of grass is divided equally between cuts
							cutDate+=28;
						}
						if ((aNCUCropClass->getCuttingDay(0)==0)&&(aNCUCropClass->getGrazingDays(0)==0))
							theMessage->Warning("NCUClass::calcCropManagement - Grass crop that is neither grazed nor cut");
					}
					else	//is not grass or SETA
					{
						if (!aNCUCropClass->getWinterCrop()||(yearNo>0))
						{
							harvestingDate=getCropEventDate(inputDir, inputFileName, MARS,2, true, year);
							if (harvestingDate<0)
							{
								harvestingDate=sowingDay+diff;
								if (harvestingDate>lastDayOfYear)
									harvestingDate-=lastDayOfYear;
							}
							aNCUCropClass->setHarvestingDay(harvestingDate,0);
						}
					}
				}// end not SETA
				delete localName;
			}  //end if aNCUCropClass not NULL
		}
		yearNo++;
	} //end of year loop

	yearNo=0;
	while (yearNo<theYearPlans->NumOfNodes())
	{
		year = startYear + yearNo;
		if ((year==2021)&& (NCUI==100250210))
				cout << "here";

		cropRotationClass * acropRotationClass = theYearPlans->ElementAtNumber(yearNo);
		for (int cropNo = 0; cropNo<acropRotationClass->gettheCrops()->NumOfNodes(); cropNo++)
		{
			NCUCropClass * aNCUCropClass = acropRotationClass->gettheCrops()->ElementAtNumber(cropNo);
			if (aNCUCropClass)
			{
				if (verbosityLevel==1)
					cout << " Year " << yearNo << " Field no " << cropNo << " Crop " << aNCUCropClass->getCropName() << endl;
				if (!aNCUCropClass->getisPermanent())
				{
					//tillage for all crops occurs just prior to sowing
					aNCUCropClass->setPloughingDay(aNCUCropClass->getSowingDay(0)-3,0);
					aNCUCropClass->setHarrowingDay(aNCUCropClass->getSowingDay(0)-2,0);
					//all solid manure is applied just prior to ploughing
					if (aNCUCropClass->getCattleSolid()>0.0)
						aNCUCropClass->setManureDay(aNCUCropClass->getSowingDay(0)-5,0,0);
					if (aNCUCropClass->getPigSolid()>0.0)
						aNCUCropClass->setManureDay(aNCUCropClass->getSowingDay(0)-5,2,0);
					if (aNCUCropClass->getPoultrySolid()>0.0)
						aNCUCropClass->setManureDay(aNCUCropClass->getSowingDay(0)-5,4,0);
					if (aNCUCropClass->getGrazingN()>0.0)
						aNCUCropClass->setManureDay(aNCUCropClass->getSowingDay(0)-5,6,0);
				}
				else //!is a permanent crop, so time operations to near end of growing season (=winterWheatSowing)
				{
					if (aNCUCropClass->getCattleSolid()>0.0)
						aNCUCropClass->setManureDay(winterWheatSowing,0,0);
					if (aNCUCropClass->getPigSolid()>0.0)
						aNCUCropClass->setManureDay(winterWheatSowing,2,0);
					if (aNCUCropClass->getPoultrySolid()>0.0)
						aNCUCropClass->setManureDay(winterWheatSowing,4,0);
					if (aNCUCropClass->getGrazingN()>0.0)
						aNCUCropClass->setManureDay(winterWheatSowing,6,0);
				}

				if (!aNCUCropClass->getWinterCrop()) //is a spring crop
				{
					int sowingDay;
					int harvestingDay;
					int growthPeriod;
					if (!aNCUCropClass->getisPermanent())
					{
						sowingDay = aNCUCropClass->getSowingDay(0);
						harvestingDay = aNCUCropClass->getHarvestingDay(0);
						growthPeriod = harvestingDay - sowingDay;
					}
					else
					{
						sowingDay = springBarleySowing;
						harvestingDay = winterWheatSowing;
						growthPeriod = harvestingDay - sowingDay;
					}
					//all fertilisation and liquid manuring of spring crops occurs just prior to sowing
					if (aNCUCropClass->getCattleSlurry()>0.0)
						aNCUCropClass->setManureDay(sowingDay-5,1,0);
					if (aNCUCropClass->getPigSlurry()>0.0)
						aNCUCropClass->setManureDay(sowingDay-5,3,0);
					if (aNCUCropClass->getPoultrySlurry()>0.0)
						aNCUCropClass->setManureDay(sowingDay-5,5,0);

					if (aNCUCropClass->getUreaFertiliser()>0.0)
					{
						aNCUCropClass->setFertDay(sowingDay-5,0,0);
						aNCUCropClass->setFertDay(round(sowingDay+0.2*growthPeriod),0,1);
					}
					if (aNCUCropClass->getNitrateFertiliser()>0.0)
					{
						aNCUCropClass->setFertDay(sowingDay-5,1,0);
						aNCUCropClass->setFertDay((sowingDay+0.2*growthPeriod),1,1);
					}
				}
				else
				{
					// fertilisation and liquid manuring of winter crops occurs in the spring (assumed to be at same time as spring barley would be sown)
					if (aNCUCropClass->getCattleSlurry()>0.0)
						aNCUCropClass->setManureDay(springBarleySowing,1,0);
					if (aNCUCropClass->getPigSlurry()>0.0)
						aNCUCropClass->setManureDay(springBarleySowing,3,0);
					if (aNCUCropClass->getPoultrySlurry()>0.0)
						aNCUCropClass->setManureDay(springBarleySowing,5,0);
					int harvestingDay = aNCUCropClass->getHarvestingDay(0);
					int growthPeriod = harvestingDay - springBarleySowing;
					if (aNCUCropClass->getUreaFertiliser()>0.0)
					{
						aNCUCropClass->setFertDay(springBarleySowing,0,0);
						aNCUCropClass->setFertDay((springBarleySowing + 0.2*growthPeriod),0,1);
					}
					if (aNCUCropClass->getNitrateFertiliser()>0.0)
					{
						aNCUCropClass->setFertDay(springBarleySowing,1,0);
						aNCUCropClass->setFertDay((springBarleySowing + 0.2*growthPeriod),1,1);
					}
				}
			}
		}
		yearNo++;
	}

};
struct years
{
	float JSOW;
	float JRIP;

};
struct product {
	vector <years> year;
	int marsCode;
} ;
struct File {
	char fileName[50];
	vector<product> fileContent;
} ;

static vector<File> fileList;
//! returns the date of sowing (1) or harvesting (2) for a particular MARS grid cell
int NCUClass::getCropEventDate(string inputDir, string MARSDateFileName, int MARScode, int dateID, bool recursive, int year)
{
	char inputFileName[256];
	strcpy(inputFileName,MARSDateFileName.c_str());
//!Check to make sure that the event requested is either Sowing or harvesting

   if ((dateID<1)||(dateID>2))
      theMessage->FatalError("NCUClass::getCropEventDate - dateID is not 1 or 2");
   bool found=false;
   int placeInVector=-1;

   //find the vector that corresponds to this file name (=crop name)
   for(int i=0;i<fileList.size();i++)	//step through file list
   {
	   for(int j=0;j<50;j++)	//step through characters in file name
	   {
		   int mars=MARSDateFileName[j];
		   int others=fileList.at(i).fileName[j];
		   if(mars==0||others==0)	//break if no data present
			   break;
		   if(fileList.at(i).fileName[j]==MARSDateFileName[j])	//character by character check
		   {
			   found=true;
			   placeInVector=i;
		   }
		   else
		   {
			   found=false;
			   break;
		   }
	   }
	   if(found==true)	//stop when correct element is found
		   break;
   }

   if(found==false)	//this crop has not been read into a vector yet, so get it
   {
	   File file;
	   for(int i=0;i<50;i++)
	   {
		   file.fileName[i]=MARSDateFileName[i];	//assign fileName to the crop file name
	   }
	   //find data in file and store it in vector
	   fileAccess hd;
	   hd.openFile(inputFileName);

	   int year=0;
	   hd.getLineFile();
	   while ( !hd.endOfFile())
	   {
		   string input=hd.getLineFile();
		   stringstream totalSString( input );
		   string nameOfItem;
		   totalSString >>nameOfItem;
		   if (verbosityLevel==2)
			   cout << nameOfItem << " ";
		   convertData converts;
		   product item;

		   item.marsCode=converts.StringToInt(nameOfItem);
		   for(int i=1971;i<2031;i++)
		   {
			   years yearItem;
			   totalSString >>nameOfItem;
			   if (verbosityLevel==2)
				   cout << i << " " << nameOfItem;
			   yearItem.JSOW=converts.StringToDouble(nameOfItem);
			   totalSString >>nameOfItem;
			   yearItem.JRIP=converts.StringToDouble(nameOfItem);
			   if (verbosityLevel==2)
				   cout << " " << nameOfItem << endl;
			   //! Round up if necessary
			   double temp2=yearItem.JSOW-floor(yearItem.JSOW);
			   if (temp2>=0.5)
				   yearItem.JSOW+=1.0;
			   temp2=yearItem.JRIP-floor(yearItem.JRIP);
			   if (temp2>=0.5)
				   yearItem.JRIP+=1.0;
			   item.year.push_back(yearItem);
		   }
		   file.fileContent.push_back(item);
		   year++;
	   }
	   hd.closeFile();
	   fileList.push_back(file);
   }
   if(found==false)	//data were not in vector array but should now be, so go look for it again
   {
	   for(int i=0;i<fileList.size();i++)
	      {
	   	   for(int j=0;j<50;j++)
	   	   {
	   		   if(fileList.at(i).fileName[j]==MARSDateFileName[j])
	   		   {
	   			   found=true;
	   			   placeInVector=i;
	   		   }
	   		   else
	   		   {
	   			   break;
	   		   }
	   	   }
	      }
   }

   int DayNo=-99;

   if(found==true)	//the data are now present
   {
	   File * item= &fileList.at(placeInVector);
	   for(int i=0;i<item->fileContent.size();i++)	//go through vector array of crops
	   {
		   if(item->fileContent.at(i).marsCode==MARScode)	//data corresponding to correct MARS code
		   {

			   if (dateID==1)//! get sowing date
				   DayNo=(int)item->fileContent.at(i).year[year-1971].JSOW;
			   if (dateID==2)//! get harvesting date
				   DayNo=(int)item->fileContent.at(i).year[year-1971].JRIP;
			   break;
		   }
	   }

   }
   else	//big problems
   {
	   cout <<  "Data for " << MARSDateFileName << " MARS code " << MARScode <<  " dataID " << dateID << "not found" << endl;
	   theMessage->FatalError("NCUClass::getCropEventDate: data not found");
   }

    //! No date was found, so we have to use the average for the NUTS region

    if ((DayNo==-99)&& (recursive))
    {
	   DayNo=getAverageCropEventDate(inputDir, MARSDateFileName, dateID, year);
    }
    return DayNo;
}

struct product1 {
	int nutsCode;
	int marsCode;
} ;

static vector<product1> productList;

//!Returns the average date within a NUTS region on which an operation is performed
int NCUClass::getAverageCropEventDate(string inputDir, string MARSDateFileName, int dateID, int year)
{
	int retVal=-99;
//!create linklist of ops data
   linkList<operationData> *theOpsData;
   theOpsData = new linkList<operationData>;
	char inputFileName[256];
	strcpy(inputFileName,inputDir.c_str());
	strcat(inputFileName,"NCU_MARS.txt");
   //! NUTS = first three digits of NCU ID
   int NUTS = NCUI/1000000;


/****************************************************************/

	   if(productList.size()==0)	//data have not been read into a vector yet, so get it
	   {
		   //productList aproductList;
		   //find data in file and store it in vector
			FILE *afile;
			afile=fopen(inputFileName, "r");
			//!

		    int idummy1, idummy2;
		    //!read thru NCU MARS data correspondence file and find each place where this NUTS region is represented by a MARS grid point
		    if (afile==NULL)
		    	theMessage->FatalError("NCUClass::getAverageCropEventDate: file ", inputFileName, " not found");
			//idummy1 contains NCUI code, idummy2 contains MARS code
		    while ( fscanf(afile, "%d %d", &idummy1,&idummy2) == 2)
		    {
		    	int tempNUTS = idummy1/1000000;//! truncate idummy1 (NCUI) to obtain the NUTS region
				product1 item;
				item.nutsCode=tempNUTS;
				item.marsCode=idummy2;
				productList.push_back(item);
		    }

			fclose(afile);
	   }

       operationData * aoperationData;
	   for(int i=0;i<productList.size();i++)	//step through product list
	   {
		   int productNUTS = productList.at(i).nutsCode;
		   if (productNUTS==NUTS)	//have correct row
		   {
			   bool gotit = true;
			   int productMARS = productList.at(i).marsCode;
			   for (int i=0;i<theOpsData->NumOfNodes(); i++)
		       {
		       	aoperationData=theOpsData->ElementAtNumber(i);
		          if (aoperationData->GetMARSno()==productMARS)  //!one MARS grid point may cover more than one NUTS region. We only want one
		          	gotit=false;
		       }
		      if (gotit)
		       {
		      	//! get the crop event date relating to the target MARS code (i.e. is within the target NUTS region), immobilise recursive calls.
		      	int adate=getCropEventDate(inputDir, MARSDateFileName, productMARS, dateID, false, year);
		        if (adate!=-99)//! 	we found a valid date. Load the MARS and operation date
		         {
		            aoperationData = new operationData();
		            aoperationData->SetMARSno(productMARS);//! productMARS = MARS code
		            aoperationData->SetdayNo(adate);
		            theOpsData->InsertLast(aoperationData);
		            //! print out the details to screen, if that is what we want
		//			  if (verbosityLevel==1)
		//          	  cout<<idummy1<<","<<idummy2<<endl;
		         }
		       }
		   }
		   if (productNUTS>NUTS)	//have reached end of data for this NUTS
			   break;
	   }
	   //! if no data was found at the NUTS region, return -99
	   if (theOpsData->NumOfNodes()==0)
	   {
	   	delete theOpsData;
	   	return retVal;
	   }
	   else  //calculate average date
	   {
	      double aveDate=0.0;
	      for (int i=0;i<theOpsData->NumOfNodes(); i++)
	      {
	         operationData * aoperationData;
	         aoperationData=theOpsData->ElementAtNumber(i);
	         aveDate+=aoperationData->GetdayNo();
	      }
	      aveDate/=theOpsData->NumOfNodes();
	      theOpsData->Reset();
	      delete theOpsData;
	      //! round up or down, as appropriate
	      if ((aveDate-floor(aveDate))>0.5)
	         retVal=ceil(aveDate);
	      else
	         retVal=floor(aveDate);
	      return retVal;
	   }
}


int NCUClass::generateOps(int implementationYear)
{
	int retVal=0;
	for (int fieldNo=0; fieldNo<maxFields; fieldNo++)
	{
		cropSeriesClass * acropSeriesClass = new cropSeriesClass(fieldNo);
		cout << "Processing crop series " << fieldNo << endl;

		for (int yearNum=0;yearNum<theYearPlans->NumOfNodes();yearNum++)
		{
			cropRotationClass * acropRotationClass = theYearPlans->ElementAtNumber(yearNum);	//get the rotation for this year
			int calendarYear= acropRotationClass->getYear();

//			if ((calendarYear==2021)&& (NCUI==100250210)&&(fieldNo==2))
	//				cout << "here";
			cropRotationClass * nextcropRotationClass;
			if (yearNum<(theYearPlans->NumOfNodes()-1))
				nextcropRotationClass = theYearPlans->ElementAtNumber(yearNum+1);	//get the rotation for following year, if relevant
			else
				nextcropRotationClass=NULL;
			NCUCropClass * aNCUCropClass = acropRotationClass->gettheCrops()->ElementAtNumber(fieldNo);	//get the crop grown on this field in the current year
			NCUCropClass * nextCropClass;
			if (yearNum<(theYearPlans->NumOfNodes()-1))
				nextCropClass = nextcropRotationClass->gettheCrops()->ElementAtNumber(fieldNo);	//get the crop grown on this field in the following year
			else
				nextCropClass=NULL;
			if (aNCUCropClass)	//do operations on the current year's crop
			{
				if (!((aNCUCropClass->getWinterCrop())&&(yearNum==0))) //do not do spring and summer operations on winter crops in first year - there is no crop
					retVal=doCurrentSpringSummerCropOperations(acropSeriesClass,aNCUCropClass, nextCropClass, calendarYear);
			}
			if (nextCropClass)
			{
				if ((zeroTillage)&&(calendarYear>=implementationYear)&&(aNCUCropClass))
					doZeroTillage(acropSeriesClass, aNCUCropClass, nextCropClass, calendarYear);
				if (nextCropClass->getWinterCrop())	//if next year's crop is a winter crop, do the operations that need to be done in the current year
				{
					if (retVal==0)
						retVal=doNextCropAutumnOperations(acropSeriesClass, nextCropClass, calendarYear);
				}
				else
					if ((coverCropping)&&(calendarYear>=implementationYear)&&(aNCUCropClass))
						doCoverCrop(acropSeriesClass, aNCUCropClass, calendarYear);
			}
		}
		//should now have all the operations loaded, so sort them into chronological order
		acropSeriesClass->sortOps();
		for (int j=0; j<acropSeriesClass->gettheOperations()->NumOfNodes(); j++)
		{
			fieldOp *afieldOp = acropSeriesClass->gettheOperations()->ElementAtNumber(j);
			//			cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " " << afieldOp->getopCode() << endl;
			if (verbosityLevel>0)
				afieldOp->SetverbosePrint(true);
			else
				afieldOp->SetverbosePrint(false);
		}
		//store series
		theSeriesOperations->InsertLast(acropSeriesClass);
	}
	return retVal;
}

int NCUClass::doCurrentSpringSummerCropOperations(cropSeriesClass * acropSeriesClass,NCUCropClass *aNCUCropClass, NCUCropClass * nextCropClass, int calendarYear)
{
	int retVal=0;
	int firstOpNextCrop=999;
	if (nextCropClass)
	{
		if (nextCropClass->getWinterCrop())//check to make sure that the current operations do not interfere with next crop operations
		{
			int tillageDate = 999;
			if (nextCropClass->getPloughingDay(0)>0)
				tillageDate=nextCropClass->getPloughingDay(0);
			int sowingDate = 999;
			if (nextCropClass->getSowingDay(0)>0)
				sowingDate = nextCropClass->getSowingDay(0);
			if (sowingDate<tillageDate)
				firstOpNextCrop=sowingDate;
			else
				firstOpNextCrop=tillageDate-5;
		}
	}
	if (!aNCUCropClass->getWinterCrop())
	{
		//start with ploughing
		// get first ploughing date
		if ((!zeroTillage)||(aNCUCropClass->getCropName()=="SUGB")||(aNCUCropClass->getCropName()=="POTA"))
		{
			if (aNCUCropClass->getPloughingDay(0)>0)
			{
				fieldOpTill *afieldOpTill = new fieldOpTill();
				afieldOpTill->DefineOp("Ploughing", 1, calendarYear, aNCUCropClass->getPloughingDay(0), 0.25);
				if (verbosityLevel==2)
					afieldOpTill->printToSceen();
				acropSeriesClass->gettheOperations()->InsertLast(afieldOpTill);
			}
		}
		if (aNCUCropClass->getSowingDay(0)>0)
		{
			fieldOpSow *afieldOpSow = new fieldOpSow();
			afieldOpSow->DefineOp("Sowing", 2, calendarYear, aNCUCropClass->getSowingDay(0), aNCUCropClass->getCropName(),false);
			if (verbosityLevel==2)
				afieldOpSow->printToSceen();
			acropSeriesClass->gettheOperations()->InsertLast(afieldOpSow);
		}

		retVal=doSolidManureOperations(acropSeriesClass, aNCUCropClass, calendarYear);
	}
	//the following operations are always performed in the current year

	if (retVal==0)
		retVal=doSlurryOperations(acropSeriesClass, aNCUCropClass, calendarYear);

	if (aNCUCropClass->getHarvestingDay(0)>0)  //harvesting
	{
		if (aNCUCropClass->getHarvestingDay(0)>=firstOpNextCrop)
			aNCUCropClass->setHarvestingDay(firstOpNextCrop-1,0); //move forward harvesting date, if it conflicts with the first operation of next crop
		bool collectStraw = true;
		double cropYield = aNCUCropClass->getYield(0);
		if ((cropYield==0.0)&&(aNCUCropClass->getCropName()!="SETA"))
		{
			string outstring;
			outstring="No crop yield in NCUI ";
			convertData aData;
			string tempstring = aData.IntToString(NCUI);
			outstring.append(tempstring);
			tempstring = ", series ";
			outstring.append(tempstring);
			tempstring = aData.IntToString(acropSeriesClass->getseriesNumber());
			outstring.append(tempstring);
			tempstring =" and crop ";
			tempstring.append(aNCUCropClass->getCropName());
			outstring.append(tempstring);
			tempstring =" in year ";
			outstring.append(tempstring);
			theMessage->WarningWithDisplay(outstring,calendarYear);
			retVal=1;
		}
		fieldOpHarv *afieldOpHarv = new fieldOpHarv();
		afieldOpHarv->DefineOp("Harvesting", 9, calendarYear, aNCUCropClass->getHarvestingDay(0), collectStraw, cropYield);
		if (verbosityLevel==2)
			afieldOpHarv->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpHarv);
	}

	if (retVal==0)
		doFertiliserOperations(acropSeriesClass, aNCUCropClass, calendarYear); //define fertilisation here, so that the dates can be adjusted if the harvesting date has moved

	if (aNCUCropClass->getAtmosphericNdep()>0)  //atmospheric deposition
	{
		double amountN = aNCUCropClass->getAtmosphericNdep();
		AddN(amountN);
		double theNH4N =0.5 * amountN;
		double theNO3N = 0.5 * amountN;;
		double theOrgN = 0.0;
		double theamountC = 0.0;
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("AtmosphericN", 10, calendarYear, 1,1, theNH4N, theNO3N, theOrgN, theamountC, 0);
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	else
	{
		char outstring1[40];
		strcpy(outstring1,"No atmospheric N in NCUI ");
		char outstring2[40];
		sprintf(outstring2, " %i ", NCUI);
		strcat(outstring2," in year ");
		strcat(outstring1,outstring2);
		theMessage->WarningWithDisplay(outstring1,calendarYear);
		retVal=1;
	}
	if (aNCUCropClass->getManureDay(6,0)>0)  //excreta
	{
		double amountN = aNCUCropClass->getGrazingN();
		AddN(amountN);
		double theNH4N =0.5 * amountN;
		double theNO3N = 0.5 * amountN;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC =aNCUCropClass->getGrazingC();
		if (theamountC==0.0)
			theMessage->WarningWithDisplay("excreta without C in ", NCUI);
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("Excreta", 3, calendarYear, aNCUCropClass->getManureDay(6,0), 10, theNH4N, theNO3N, theOrgN,  theamountC, solidApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	for (int grazingType=0;grazingType<3; grazingType++)
	{
		if (aNCUCropClass->getGrazingStartDay(grazingType,0)>0)  //grazing starts
		{
			int grazingStartDate = aNCUCropClass->getGrazingStartDay(grazingType,0);
			if (firstOpNextCrop<grazingStartDate) //stop grazing if they will conflict with tillage or sowing operations for next crop
				grazingStartDate=firstOpNextCrop-1;
			fieldOpGrass *afieldOpGrass = new fieldOpGrass();
			afieldOpGrass->DefineOp("StartGrazing", 5, calendarYear, grazingStartDate ,grazingType, 0.07);  //default postgrazing heigh = 0.07 metres
			if (verbosityLevel==2)
				afieldOpGrass->printToSceen();
			acropSeriesClass->gettheOperations()->InsertLast(afieldOpGrass);
		}
		if (aNCUCropClass->getGrazingStopDay(grazingType,0)>0)  //grazing stops
		{
			int grazingStopDate = aNCUCropClass->getGrazingStopDay(grazingType,0);
			if (firstOpNextCrop<grazingStopDate)//stop grazing if they will conflict with tillage or sowing operations for next crop
				grazingStopDate=firstOpNextCrop-1;

			fieldOpGrass *afieldOpGrass = new fieldOpGrass();
			afieldOpGrass->DefineOp("StopGrazing", 6, calendarYear, grazingStopDate, grazingType);  //LAI not used
			if (verbosityLevel==2)
				afieldOpGrass->printToSceen();
			acropSeriesClass->gettheOperations()->InsertLast(afieldOpGrass);

		}
	}
	for (int i=0;i<aNCUCropClass->getmaxnumberHarvestCutDates();i++)
	{
		int cuttingDay=aNCUCropClass->getCuttingDay(i);
		if (cuttingDay>0)  //cutting will occur
		{
			if (firstOpNextCrop>cuttingDay) //stop cutting if it will conflict with tillage or sowing operations for next crop
			{
				fieldOpCut *afieldOp = new fieldOpCut();
				double yield = aNCUCropClass->getYield(i);
				afieldOp->DefineOp("Cutting", 4, calendarYear, cuttingDay, yield);
				if (verbosityLevel==2)
					afieldOp->printToSceen();
				acropSeriesClass->gettheOperations()->InsertLast(afieldOp);
			}
		}
	}
}


int NCUClass::doNextCropAutumnOperations(cropSeriesClass * acropSeriesClass,NCUCropClass *nextCropClass, int calendarYear)
{
	int retVal=0;
	if (!zeroTillage)
	{
		if (nextCropClass->getPloughingDay(0)>0)
		{
			fieldOpTill *afieldOpTill = new fieldOpTill();
			afieldOpTill->DefineOp("Ploughing", 1, calendarYear, nextCropClass->getPloughingDay(0), 0.25);
			if (verbosityLevel==2)
				afieldOpTill->printToSceen();
			acropSeriesClass->gettheOperations()->InsertLast(afieldOpTill);
		}
	}
	if (nextCropClass->getSowingDay(0)>0)
	{
		fieldOpSow *afieldOpSow = new fieldOpSow();
		afieldOpSow->DefineOp("Sowing", 2, calendarYear, nextCropClass->getSowingDay(0), nextCropClass->getCropName(),false);
		if (verbosityLevel==2)
			afieldOpSow->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpSow);
	}
	retVal=doSolidManureOperations(acropSeriesClass, nextCropClass, calendarYear);
	return retVal;
}

int NCUClass::doSolidManureOperations(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	int retVal=0;
	if (aNCUCropClass->getManureDay(0,0)>0)  //cattle solid - first and only application
	{
		double amountN = aNCUCropClass->getCattleSolid();
		AddN(amountN);
		double theNH4N =0.25 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC = aNCUCropClass->getCattleSolidC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("cattle solid manure without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("CattleSolid", 3, calendarYear, aNCUCropClass->getManureDay(0,0), 3, theNH4N, theNO3N, theOrgN, theamountC, solidApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	if (aNCUCropClass->getManureDay(2,0)>0)  //pig solid - first and only application
	{
		double amountN = aNCUCropClass->getPigSolid();
		AddN(amountN);
		double theNH4N =0.25 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC =aNCUCropClass->getPigSolidC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("pig solid manure without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("PigSolid", 3, calendarYear, aNCUCropClass->getManureDay(2,0), 5, theNH4N, theNO3N, theOrgN, theamountC, solidApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	if (aNCUCropClass->getManureDay(4,0)>0)  //poultry solid - first and only application
	{
		double amountN = aNCUCropClass->getPoultrySolid();
		AddN(amountN);
		double theNH4N =0.25 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC =aNCUCropClass->getPoultrySolidC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("poultry solid manure without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("PoultrySolid", 3, calendarYear, aNCUCropClass->getManureDay(4,0), 7, theNH4N, theNO3N, theOrgN,  theamountC, solidApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	return retVal;
}

int NCUClass::doSlurryOperations(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	int retVal=0;
	checkSlurryOperations(acropSeriesClass, aNCUCropClass, calendarYear);
	if (aNCUCropClass->getManureDay(1,0)>0)  //cattle slurry - first and only application
	{
		double amountN = aNCUCropClass->getCattleSlurry();
		AddN(amountN);
		double theNH4N =0.5 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC =aNCUCropClass->getCattleSlurryC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("cattle slurry without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("CattleSlurry", 3, calendarYear, aNCUCropClass->getManureDay(1,0), 2, theNH4N, theNO3N, theOrgN, theamountC, slurryApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	if (aNCUCropClass->getManureDay(3,0)>0)  //pig slurry - first and only application
	{
		double amountN = aNCUCropClass->getPigSlurry();
		AddN(amountN);
		double theNH4N =0.7 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC = aNCUCropClass->getPigSlurryC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("pig slurry without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("PigSlurry", 3, calendarYear, aNCUCropClass->getManureDay(3,0), 4, theNH4N, theNO3N, theOrgN, theamountC, slurryApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}

	if (aNCUCropClass->getManureDay(5,0)>0)  //poultry slurry - first and only application
	{
		double amountN = aNCUCropClass->getPoultrySlurry();
		AddN(amountN);
		double theNH4N =0.90 * amountN;
		double theNO3N = 0.0;
		double theOrgN = amountN - (theNH4N + theNO3N);
		double theamountC = aNCUCropClass->getPoultrySlurryC();
		if (theamountC==0.0)
		{
			theMessage->WarningWithDisplay("poultry slurry without C in NCUI ", NCUI);
			cout << "Calendar year = " << calendarYear << endl;
			retVal=1;
		}
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("PoultrySlurry", 3, calendarYear, aNCUCropClass->getManureDay(5,0), 6, theNH4N, theNO3N, theOrgN, theamountC, slurryApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}
	return retVal;
}

int NCUClass::checkSlurryOperations(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	int harvestingDate=aNCUCropClass->getHarvestingDay(0);
	if (harvestingDate>0) //this crop is not grass
	{
		for (int i=0; i<aNCUCropClass->getnumberManureTypes();i++)
		{
			if ((i==1)||(i==3)||(i==5))//only for slurries
			{
				int minDays= aNCUCropClass->getminumumDaysFertManHarvest();
				if ((harvestingDate-aNCUCropClass->getManureDay(i,0))<minDays)
				{
					string outstring;
					string tempstring;
					switch (i)
					{
					case 1:outstring="Cattle";
					break;
					case 2:outstring="Pig";
					break;
					case 3:outstring="Poultry";
					break;
					}
					int newDate = harvestingDate-minDays;
					outstring.append(tempstring);
					tempstring=" slurry date adjusted in NCUI ";
					outstring.append(tempstring);
					convertData aData;
					tempstring = aData.IntToString(NCUI);
					outstring.append(tempstring);
					tempstring = ", series ";
					outstring.append(tempstring);
					tempstring = aData.IntToString(acropSeriesClass->getseriesNumber());
					outstring.append(tempstring);
					tempstring =" and crop ";
					tempstring.append(aNCUCropClass->getCropName());
					outstring.append(tempstring);
					tempstring =" in year ";
					outstring.append(tempstring);
					tempstring = aData.IntToString(calendarYear);
					outstring.append(tempstring);
					tempstring =" to day ";
					outstring.append(tempstring);
					theMessage->WarningWithDisplay(outstring,newDate);
					aNCUCropClass->setManureDay(newDate,i,0);
				}
			}
		}
	}

}


void NCUClass::doFertiliserOperations(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	checkFertiliserOperations(acropSeriesClass, aNCUCropClass, calendarYear);
	if (aNCUCropClass->getFertDay(0,0)>0)  //fertilising with urea fertiliser - first application
	{
		double amountN = aNCUCropClass->getUreaFertiliser() * aNCUCropClass->getpropFert(0);
		AddN(amountN);
		double theNH4N = amountN;
		double theNO3N = 0.0;
		double theOrgN = 0.0;
		double theamountC =(12/28) * amountN;
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("Urea", 3, calendarYear, aNCUCropClass->getFertDay(0,0), 1, theNH4N, theNO3N, theOrgN,theamountC, ureaApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}

	if (aNCUCropClass->getFertDay(0,1)>0)  //fertilising with urea fertiliser - second application
	{
		double amountN = aNCUCropClass->getUreaFertiliser() * aNCUCropClass->getpropFert(1);
		AddN(amountN);
		double theNH4N = amountN;
		double theNO3N = 0.0;
		double theOrgN = 0.0;
		double theamountC =(12/28) * amountN;
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("Urea", 3, calendarYear, aNCUCropClass->getFertDay(0,1), 1, theNH4N, theNO3N, theOrgN,theamountC, ureaApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}

	if (aNCUCropClass->getFertDay(1,0)>0)  //fertilising with nitrate fertiliser - first application
	{
		double amountN = aNCUCropClass->getNitrateFertiliser() * aNCUCropClass->getpropFert(0);
		AddN(amountN);
		double theNH4N = 0.0;
		double theNO3N = amountN;
		double theOrgN = 0.0;
		double theamountC =0.0;
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("Nitrate", 3, calendarYear, aNCUCropClass->getFertDay(1,0), 1, theNH4N, theNO3N, theOrgN,theamountC, ureaApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}

	if (aNCUCropClass->getFertDay(1,1)>0)  //fertilising with nitrate fertiliser - second application
	{
		double amountN = aNCUCropClass->getNitrateFertiliser() * aNCUCropClass->getpropFert(1);
		AddN(amountN);
		double theNH4N = 0.0;
		double theNO3N = amountN;
		double theOrgN = 0.0;
		double theamountC =0.0;
		fieldOpFert *afieldOpFert = new fieldOpFert();
		afieldOpFert->DefineOp("Nitrate", 3, calendarYear, aNCUCropClass->getFertDay(1,1), 1, theNH4N, theNO3N, theOrgN,theamountC, ureaApplicMethod);
		if (verbosityLevel==2)
			afieldOpFert->printToSceen();
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpFert);
	}

}

void NCUClass::checkFertiliserOperations(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	int harvestingDate=aNCUCropClass->getHarvestingDay(0);
	if (harvestingDate>0) //this crop is not grass
	{
		for (int i=0; i<aNCUCropClass->getnumberFertTypes();i++)
		{
			for (int j=0; j<aNCUCropClass->getnumberFertManDates(); j++)
			{
				int minDays= aNCUCropClass->getminumumDaysFertManHarvest()+j; //to avoid possibility of two applications on same day
				if (aNCUCropClass->getFertDay(i,j)>0)
				{
				if ((harvestingDate-aNCUCropClass->getFertDay(i,j))<minDays)
				{
					string outstring;
					string tempstring;
					switch (j)
					{
					case 0:outstring="First";
					break;
					case 1:outstring="Second";
					break;
					case 2:outstring="Third";
					break;
					case 3:outstring="Fourth";
					}
					switch (i)
					{
					case 0:tempstring=" urea ";
					break;
					case 1:outstring=" nitrate ";
					break;
					}
					int newDate = harvestingDate-minDays;
					outstring.append(tempstring);
					tempstring="fertiliser date adjusted in NCUI ";
					outstring.append(tempstring);
					convertData aData;
					tempstring = aData.IntToString(NCUI);
					outstring.append(tempstring);
					tempstring = ", series ";
					outstring.append(tempstring);
					tempstring = aData.IntToString(acropSeriesClass->getseriesNumber());
					outstring.append(tempstring);
					tempstring =" and crop ";
					tempstring.append(aNCUCropClass->getCropName());
					outstring.append(tempstring);
					tempstring =" in year ";
					outstring.append(tempstring);
					tempstring = aData.IntToString(calendarYear);
					outstring.append(tempstring);
					tempstring =" to day ";
					outstring.append(tempstring);
					theMessage->WarningWithDisplay(outstring,newDate);
					aNCUCropClass->setFertDay(newDate,i,j);
				}
				}
			}
		}
	}
}
void NCUClass::outputOps(fstream * outfile)
{
	for (int fieldNo=0; fieldNo<theSeriesOperations->NumOfNodes(); fieldNo++)
   {
	if (verbosityLevel==1)
		cout << NCUI << " " << fieldNo << " " << endl;
	cropSeriesClass * acropSeriesClass = theSeriesOperations->ElementAtNumber(fieldNo);
	if (acropSeriesClass)
	  acropSeriesClass->OutputOps(outfile, NCU);
   }
	CheckNBalance();
}

bool NCUClass::CheckNBalance()
{
	double NOut=0.0;
	for (int fieldNo=0; fieldNo<theSeriesOperations->NumOfNodes(); fieldNo++)
   {
	cropSeriesClass * acropSeriesClass = theSeriesOperations->ElementAtNumber(fieldNo);
	if (acropSeriesClass)
		NOut+=acropSeriesClass->GetNOutput();
   }
	if (!N_added==NOut)
	{
		cout << "N In " << N_added << " N output " << NOut << endl;
		theMessage->FatalError("NCUClass: N balance error");
	}
	else
		cout << "N balance OK" << endl;
	return true;
}
void NCUClass::loadReplacements(string inputDir)
{
   string inputFileName;
	inputFileName=inputDir + "replacements.txt";
	fstream *afile = new fstream();
   afile->open(inputFileName.c_str(),fstream::in);
   if (afile==NULL)
    	theMessage->FatalError("NCUClass::loadReplacements: file ", inputFileName.c_str(), " not found");
   int i=0;
   char dummy[5000];
   string wantCrop;
   string repCrop;
   bool finished = false;
   while (!finished)
   {
	   afile->getline(dummy,5000,'\n');
      if (strlen(dummy)==0)
      	finished=true;
      else
      {
#ifdef __BCplusplus__	    
	      istrstream inputString(dummy,strlen(dummy));
#else      
  	   stringstream inputString(stringstream::in | stringstream::out);
		inputString << dummy;
#endif
	   inputString >> wantCrop >> repCrop;
	     replacements[i][0] =wantCrop;
	     replacements[i][1] = repCrop;
	     i++;
      }
   }
   afile->close();
   delete afile;
}

void NCUClass::getReplacements(string *aCrop)
{
	int i=0;
	while (replacements[i][0].length()>0)
	{
		if (replacements[i][0].compare(*aCrop)==0)
		{
			if (verbosityLevel>0)
				cout << "Want " << aCrop << " Get " << replacements[i][0] << endl;
			*aCrop=replacements[i][1];
		}
		i++;
	}
}


double NCUClass::getAnnualPrecip(int yearNo)
{
	double ret_val=0.0;
	for (int i=0;i<366; i++)
		ret_val+=getDailyPrecip(yearNo,i);
	return ret_val;
}

double NCUClass::getAnnualMet(int variableNo, int yearNo)
{
	double ret_val=0.0;
	switch (variableNo)
	{
	case 0:
	case 1:theMessage->FatalError("NCUClass::getAnnualMet: not implemented for this variable");
	case 2:ret_val=getAnnualPrecip(yearNo);
		break;
	default:theMessage->FatalError("NCUClass::getAnnualMet: not implemented for this variable");
	}
return ret_val;
}

void NCUClass::writeAnnualMet(fstream *outfileMet)
{
	for (int yearNum=0;yearNum<theYearPlans->NumOfNodes();yearNum++)
	*outfileMet << NCUI << '\t' << getAnnualMet(2, yearNum);
}

void NCUClass::doZeroTillage(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, NCUCropClass *nextCrop, int calendarYear)
{
	if ((aNCUCropClass->getCropName()=="GRAS")||(aNCUCropClass->getCropName()=="GRAP")||(aNCUCropClass->getCropName()=="SETA"))
		 //insert topping cut to remove excess plant material
	{
		bool collectStraw = true;
	   fieldOpHarv *afieldOpHarv = new fieldOpHarv();
	   afieldOpHarv->DefineOp("Harvesting", 9, calendarYear, nextCrop->getPloughingDay(0)-5, collectStraw);
	   acropSeriesClass->gettheOperations()->InsertLast(afieldOpHarv);
	}
}

void NCUClass::doCoverCrop(cropSeriesClass * acropSeriesClass, NCUCropClass *aNCUCropClass, int calendarYear)
{
	string cropName = aNCUCropClass->getCropName();
	if ((!cropName.compare("SETA")==0)&&(!cropName.compare("GRAS")==0)&&(!cropName.compare("GRAP")==0))
	{
		int lastDayOfYear;
		if (((calendarYear%4==0) && (calendarYear%100!=0)) || (calendarYear%400==0))
			lastDayOfYear = 366;
		else
			lastDayOfYear = 365;

		if (!zeroTillage) //prepare seedbed
		{
			fieldOpTill *afieldOpTill = new fieldOpTill();
			if (aNCUCropClass->getHarvestingDay(0)>0)
			{
				if ((aNCUCropClass->getHarvestingDay(0)+2)>lastDayOfYear)
					afieldOpTill->DefineOp("Ploughing", 1, calendarYear+1, (aNCUCropClass->getHarvestingDay(0)+2-lastDayOfYear), 0.25);
				else
					afieldOpTill->DefineOp("Ploughing", 1, calendarYear, aNCUCropClass->getHarvestingDay(0)+2, 0.25);
			}
			acropSeriesClass->gettheOperations()->InsertLast(afieldOpTill);
		}
		//sow grass as cover crop
		fieldOpSow *afieldOpSow = new fieldOpSow();
		if (aNCUCropClass->getHarvestingDay(0)+3>lastDayOfYear)
			afieldOpSow->DefineOp("Sowing", 2, calendarYear+1, (aNCUCropClass->getHarvestingDay(0)+3-lastDayOfYear), "GRAS",true);
		else
			afieldOpSow->DefineOp("Sowing", 2, calendarYear, aNCUCropClass->getHarvestingDay(0)+3, "GRAS",true);
		acropSeriesClass->gettheOperations()->InsertLast(afieldOpSow);
	}
}

bool NCUClass::CheckOperationIntegrity()
{
	bool retVal=true;

	for (int fieldNo=0; fieldNo<theSeriesOperations->NumOfNodes(); fieldNo++)
	{
		cropSeriesClass * acropSeriesClass = theSeriesOperations->ElementAtNumber(fieldNo);
		if (acropSeriesClass)
		{
			if (verbosityLevel==1)
				cout << "NCUI " << NCUI << " Field no " << fieldNo << endl;
			retVal=acropSeriesClass->CheckOperationIntegrity();
			if (!retVal)
			{
				cout << "NCUI " << NCUI << " crop series no " << fieldNo << " failure in operation integrity " << endl;
				theMessage->FatalError("NCUClass: CheckOperationIntegrity - error");
			}
		}
	}
	return retVal;
}
