

// Class: NCUCropClass

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "NCUCropClass.h"
#include "base/message.h"
#include "fieldOp.h"
#include "fieldOpFert.h"
#include "fieldOpSow.h"
#include "fieldOpTill.h"
#include "base/unix_util.h"
extern int verbosityLevel;

NCUCropClass:: ~ NCUCropClass ( )
{

};

void NCUCropClass::Initialise()
{
     UreaFertiliser = 0.0;
     NitrateFertiliser = 0.0;
     CattleSlurry= 0.0;;
     CattleSolid= 0.0;
     PigSlurry= 0.0;
     PigSolid= 0.0;
     PoultrySlurry= 0.0;
     PoultrySolid= 0.0;
     CattleSlurryC= 0.0;;
     CattleSolidC= 0.0;
     PigSlurryC= 0.0;
     PigSolidC= 0.0;
     PoultrySlurryC= 0.0;
     PoultrySolidC= 0.0;
     GrazingN = 0.0;
     AtmosphericNdep=0.0;
     CropName.clear();
     WinterCrop=false;
     isPermanent = false;
	  for (int i=0; i<numberFertTypes;i++)
		  for (int j=0; j<numberFertManDates;j++)
			  FertDay[i][j]=0;
	  for (int i=0; i<numberOtherOpDates;i++)
     	SowingDay[i]=0;
	  for (int i=0; i<numberHarvestCutDates;i++)
	  {
     	HarvestingDay[i]=0;
        Yield[i] = 0.0;
	  }
	  for (int i=0; i<numberOtherOpDates;i++)
     	PloughingDay[i]=0;
	  for (int i=0; i<numberOtherOpDates;i++)
     	HarrowingDay[i]=0;
	  for (int i=0; i<numberHarvestCutDates;i++)
		  CuttingDay[i]=0;
	  zeroGrazingDays();
	  zeroGrazingPeriods();
	  for (int i=0; i<numberManureTypes;i++)
		  for (int j=0; j<numberFertManDates;j++)
			  ManureDay[i][j]=0;
	  propFert[0]=0.2;
	  propFert[1]=0.8;
	  numLeadSpaces=0;
}

#ifdef __BCplusplus__	   
	bool NCUCropClass::ReadData(istrstream *instring, int row, int year)
#else
	bool NCUCropClass::ReadData(stringstream *instring, int row, int year)
#endif

{
		bool abort = false;
		double indata;
		string aString;
		if (row==0)
			*instring >>aString;
		else
		{
			*instring >>indata;
			if (verbosityLevel>0)
#ifdef __BCplusplus__
				cout << indata << " ";
#else
				cout << indata << " ";
#endif
		}
		char yearStr[5];
		itoa(year,yearStr,4);
		switch (row) {
		case 0:  if (aString == "NOCR")
			abort = true;
		if ((aString == "WWHE")||(aString == "RAPE"))
			setWinterCrop(true);
		if(!(aString.length()==4))
		{
			cout << "Error in year " << year << endl;
			if(aString.empty())
				cout<<"got a null problem"<<endl;
			theMessage->FatalError("Crop name not 4 characters ",aString.c_str());
		}
		setCropName(aString);
		break;
		case 1: setNitrateFertiliser(indata);
		break;
		case 2: setUreaFertiliser(indata);
		break;
		case 3: setCattleSlurry(indata);
		break;
		case 4: setCattleSolid(indata);
		break;
		case 5: setPigSlurry(indata);
		break;
		case 6: setPigSolid(indata);
		break;
		case 7: setPoultrySolid(indata);
		break;
		case 8: setGrazingN(indata);
		break;
		case 9: setCattleSlurryC(indata);
		break;
		case 10: setCattleSolidC(indata);
		break;
		case 11: setPigSlurryC(indata);
		break;
		case 12: setPigSolidC(indata);
		break;
		case 13: setPoultrySolidC(indata);
		break;
		case 14: setGrazingC(indata);
		break;
		case 15: setAtmosphericNdep(indata);
		break;
		case 16: setYield(0,indata);
		break;
		case 17:
			if (indata>366)
				theMessage->FatalError("NCUCropClass: grazing days exceed 366 in year number ", yearStr);
			setGrazingDays(0,indata);;
			break;
		case 18:
			if (indata>366)
				theMessage->FatalError("NCUCropClass: grazing days exceed 366 in year number ", yearStr);
			setGrazingDays(1,indata);
			break;
		case 19:
			if (indata>366)
				theMessage->FatalError("NCUCropClass: grazing days exceed 366 in year number ", yearStr);
			setGrazingDays(2,indata);
			break;

		default:
			theMessage->FatalError("NCUCropClass: error in reading crop rotation data in year number ", yearStr);
		}
		return abort;
}

int NCUCropClass::getNumManureApps(int index)
{
	int retVal=0;
	for (int i=0; i<=numberFertManDates; i++)
		if (ManureDay[index][i]>0.0)
			retVal++;
	return retVal;
}
int NCUCropClass::getNumFertApps(int index)
{
	int retVal=0;
	for (int i=0; i<=numberFertManDates; i++)
		if (FertDay[index][i]>0.0)
			retVal++;
	return retVal;
}

void NCUCropClass::zeroGrazingPeriods()
{
	for (int j=0; j<numberGrazingTypes;j++)
		for (int i=0; i<numberGrazingDates;i++)
			GrazingStartDay[j][i]=0;
	for (int j=0; j<numberGrazingTypes;j++)
		for (int i=0; i<numberGrazingDates;i++)
			GrazingStopDay[j][i]=0;
}

void NCUCropClass::zeroGrazingDays()
{
	 for (int j=0; j<numberGrazingTypes;j++)
		  GrazingDays[j] = 0;
}


