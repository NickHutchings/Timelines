// Class: NCUClass                     //ANSI C++

#include "base/common.h"
#include "base/linklist.h"
#include <fstream>
using namespace std;
#ifndef __CROPROTCLASS_H
  #define __CROPROTCLASS_H


                               //Required for 1:M aggregation (part) classes
#ifndef __NCUCROPCLASS_H
  #include "NCUCropClass.h"
#endif

class cropRotationClass
{
   linkList<NCUCropClass> *theCrops; //! container for pointers to the crops present in a particular year

private:

     int Year; //! the calendar year to which this crop rotation relates

protected:

public:

			//Default constructor
  cropRotationClass ();

   ~ cropRotationClass ( );  //Destructor - Delete any pointer data members that used new in constructors
   //! return the calendar year to which this crop rotation relates
  int getYear() const  { return Year;}
  //! set the calendar year to which this crop rotation relates
  void setYear (int aYear)  { Year = aYear;}
  //! returned pointer to the container of pointers to the crop is present in this year
  linkList <NCUCropClass>* gettheCrops()  { return theCrops;  }
  //! initialise the class
  bool Initialise(fstream *afile);
  //! returns the amount of nitrogen applied to the crops in this rotation (kilograms per hectare)
  double GetNOutput();

};

#endif
