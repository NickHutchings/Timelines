

// Class: NCUCropClass                     //ANSI C++

#include "base/common.h"
#include "base/linklist.h"

#ifndef __NCUCROPCLASS_H  //Required for current class
  #define __NCUCROPCLASS_H



#ifdef __BCplusplus__
	#include <strstream>
#else
	#include <sstream>
#endif
const int numberManureTypes = 8;
const int numberFertTypes = 2;
const int numberGrazingTypes = 3;
const int numberHarvestCutDates=10;
const int numberGrazingDates = 5;
const int numberFertManDates = 4;
const int numberOtherOpDates = 5;

class NCUCropClass
{

	string CropName; //Attribute data member

	double UreaFertiliser; //Attribute data member

	double NitrateFertiliser; //Attribute data member

	double CattleSlurry; //Attribute data member

	double CattleSolid; //Attribute data member

	double PigSlurry; //Attribute data member

	double PigSolid; //Attribute data member

	double PoultrySlurry; //Attribute data member

	double PoultrySolid; //Attribute data member

	double GrazingN; //Attribute data member

	double CattleSlurryC; //Attribute data member

	double CattleSolidC; //Attribute data member

	double PigSlurryC; //Attribute data member

	double PigSolidC; //Attribute data member

	double PoultrySlurryC; //Attribute data member

	double PoultrySolidC; //Attribute data member

	double GrazingC; //Attribute data member

	double AtmosphericNdep;

	double GrazingDays[numberGrazingTypes];

	double Yield[numberHarvestCutDates];

	int SowingDay[numberOtherOpDates];
	int HarvestingDay[numberHarvestCutDates];
	//!
	int PloughingDay[numberOtherOpDates];
	int HarrowingDay[numberOtherOpDates];
	//!Manure application dates; first index - 0= Cattle solid, 1=cattle slurry, 2=pig solid, 3=pig slurry, 4=poultry solid, 5=poultry slurry, 6=grazing
	//!second index - n = application times in order
	int ManureDay[numberManureTypes][numberFertManDates];
	//!Fertiliser application dates; 0= urea, 1=nitrate
	int FertDay[numberFertTypes][numberFertManDates];
	//!Day when grazing starts
	int GrazingStartDay[numberGrazingTypes][numberGrazingDates];
	//!Day when grazing stops
	int GrazingStopDay[numberGrazingTypes][numberGrazingDates];
	//!Day when cutting occurs
	int CuttingDay[numberHarvestCutDates];
	//!true if is winter crop
	bool WinterCrop;
	//!proportion of mineral fertiliser applied at each application time
	double propFert[2];
	//! true if the crop is permanent (must not be sown)
	bool isPermanent;
	//!
	int numLeadSpaces;
	//! minimum number of days between last fertilisation and harvesting
	int minumumDaysFertManHarvest;


private:

protected:

public:

	//Default constructor
	NCUCropClass () {};

	void Initialise ()    ;

	~ NCUCropClass ( );  //Destructor - Delete any pointer data members that used new in constructors

	int getmaxnumberHarvestCutDates(){ return numberHarvestCutDates;}

	//! return number of FertTypes
	int getnumberFertTypes(){return numberFertTypes;}

	//! return number of FertTypes
	int getnumberManureTypes(){return numberManureTypes;}

	//! return number of FertManDates
	int getnumberFertManDates(){return numberFertManDates;}

	//!Get accessor function for non-static attribute data member
	double getNitrateFertiliser() const  { return NitrateFertiliser;}
	//!Get accessor function for non-static attribute data member
	double getUreaFertiliser() const  { return UreaFertiliser;}

	//!Get accessor function for non-static attribute data member
	double getCattleSlurry() const  { return CattleSlurry;}

	//!Get accessor function for non-static attribute data member
	double getCattleSolid() const  { return CattleSolid;}

	//!Get accessor function for non-static attribute data member
	double getPigSlurry() const  { return PigSlurry;}

	//!Get accessor function for non-static attribute data member
	double getPigSolid() const  { return PigSolid;}

	//!Get accessor function for non-static attribute data member
	double getPoultrySlurry() const  { return PoultrySlurry;}

	//!Get accessor function for non-static attribute data member
	double getPoultrySolid() const  { return PoultrySolid;}

	//!Get accessor function for non-static attribute data member
	double getGrazingN() const  { return GrazingN;}

	double getCattleSlurryC() const  { return CattleSlurryC;}

	//!Get accessor function for non-static attribute data member
	double getCattleSolidC() const  { return CattleSolidC;}

	//!Get accessor function for non-static attribute data member
	double getPigSlurryC() const  { return PigSlurryC;}

	//!Get accessor function for non-static attribute data member
	double getPigSolidC() const  { return PigSolidC;}

	//!Get accessor function for non-static attribute data member
	double getPoultrySlurryC() const  { return PoultrySlurryC;}

	//!Get accessor function for non-static attribute data member
	double getPoultrySolidC() const  { return PoultrySolidC;}

	//!Get accessor function for non-static attribute data member
	double getGrazingC() const  { return GrazingC;}

	double getAtmosphericNdep(){return AtmosphericNdep;}


	//!Get accessor function for non-static attribute data member
	string getCropName() const  { return CropName;}

	//!Get accessor function for non-static attribute data member

	int getSowingDay(int index){return SowingDay[index];}
	int getHarvestingDay(int index){return HarvestingDay[index];}
	bool getWinterCrop(){return WinterCrop;};
	int getPloughingDay(int index) {return PloughingDay[index];}
	int getHarrowingDay(int index) {return HarrowingDay[index];}
	int getManureDay(int index, int appNo) {return ManureDay[index][appNo];}
	int getFertDay(int index, int appNo) {return FertDay[index][appNo];}
	int getGrazingStartDay(int type, int index) {return GrazingStartDay[type][index];}
	int getGrazingStopDay(int type, int index) {return GrazingStopDay[type][index];}
	int getCuttingDay(int index) {return CuttingDay[index];}
	bool getisPermanent(){return isPermanent;}

	//!Set accessor function for non-static attribute data member
	void setNitrateFertiliser (double aNitrateFertiliser)  { NitrateFertiliser = aNitrateFertiliser;}
	//!Set accessor function for non-static attribute data member
	void setUreaFertiliser (double aUreaFertiliser)  { UreaFertiliser = aUreaFertiliser;}

	//!Set accessor function for non-static attribute data member
	void setCattleSlurry (double aCattleSlurry)  { CattleSlurry = aCattleSlurry;}

	//!Set accessor function for non-static attribute data member
	void setCattleSolid (double aCattleSolid)  { CattleSolid = aCattleSolid;}

	//!Set accessor function for non-static attribute data member
	void setPigSlurry (double aPigSlurry)  { PigSlurry = aPigSlurry;}

	//!Set accessor function for non-static attribute data member
	void setPigSolid (double aPigSolid)  { PigSolid = aPigSolid;}

	//!Set accessor function for non-static attribute data member
	void setPoultrySlurry (double aPoultrySlurry)  { PoultrySlurry = aPoultrySlurry;}

	//!Set accessor function for non-static attribute data member
	void setPoultrySolid (double aPoultrySolid)  { PoultrySolid = aPoultrySolid;}

	//!Set accessor function for non-static attribute data member
	void setGrazingN (double aGrazingN)  { GrazingN = aGrazingN;}

	//!Set accessor function for non-static attribute data member
	void setCattleSlurryC (double aCattleSlurryC)  { CattleSlurryC = aCattleSlurryC;}

	//!Set accessor function for non-static attribute data member
	void setCattleSolidC (double aCattleSolidC)  { CattleSolidC = aCattleSolidC;}

	//!Set accessor function for non-static attribute data member
	void setPigSlurryC (double aPigSlurryC)  { PigSlurryC = aPigSlurryC;}

	//!Set accessor function for non-static attribute data member
	void setPigSolidC (double aPigSolidC)  { PigSolidC = aPigSolidC;}

	//!Set accessor function for non-static attribute data member
	void setPoultrySlurryC (double aPoultrySlurryC)  { PoultrySlurryC = aPoultrySlurryC;}

	//!Set accessor function for non-static attribute data member
	void setPoultrySolidC (double aPoultrySolidC)  { PoultrySolidC = aPoultrySolidC;}

	//!Set accessor function for non-static attribute data member
	void setGrazingC (double aGrazingC)  { GrazingC = aGrazingC;}

	void setAtmosphericNdep(double aVal) {AtmosphericNdep = aVal;}

	void setminumumDaysFertManHarvest(int aVal){minumumDaysFertManHarvest=aVal;}

	int getminumumDaysFertManHarvest(){return minumumDaysFertManHarvest;}

	//!Set accessor function for non-static attribute data member
	void setCropName (string aCropName)  { CropName = aCropName;}

	double getGrazingDays(int index){return GrazingDays[index];}
	void setGrazingDays(int index, double aVal){GrazingDays[index]=aVal;};

	double getYield(int num){return Yield[num];}
	void setYield(int num,double aVal){Yield[num] = aVal;};


	void setSowingDay(int aVal, int index) {SowingDay[index]=aVal;}
	void setHarvestingDay(int aVal, int index) {HarvestingDay[index]=aVal;}
	void setWinterCrop(bool aVal){WinterCrop = aVal;}
	void setPloughingDay(int aVal, int index){PloughingDay[index]=aVal;}
	void setHarrowingDay(int aVal, int index){HarrowingDay[index]=aVal;}
	void setManureDay(int aVal, int index, int appNo){ManureDay[index][appNo]=aVal;}
	void setFertDay(int aVal, int index, int appNo){FertDay[index][appNo]=aVal;}
	void setGrazingStartDay(int aVal, int type, int index){GrazingStartDay[type][index]=aVal;}
	void setGrazingStopDay(int aVal, int type, int index){GrazingStopDay[type][index]=aVal;}
	void setCuttingDay(int aVal, int index){CuttingDay[index]=aVal;}
	int getNumManureApps(int index);
	int getNumFertApps(int index);
	double getpropFert(int index){return propFert[index];}
	bool setisPermanent(bool aVal){isPermanent=aVal;}
#ifdef __BCplusplus__
	bool ReadData(istrstream *instring, int row);
#else
	bool ReadData(stringstream *instring, int row, int year);
#endif

//! zeros all grazing periods
	void zeroGrazingPeriods();
//! zero all grazing days (used to stop grazing, even if there are grazing days in the input file
	void zeroGrazingDays();
};

/*
1 - tillage
2 - sowing
3 - fertilisation
4 - cutting
5 - start grazing
6 - end grazing
7 - start irrigation
8 - stop irrigation
9 - harvesting
*/
#endif
