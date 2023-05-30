// Class: fieldOpFert                     //ANSI C++

#include "base/linklist.h"

#ifndef __FIELDOPFERT_H  //Required for current class
  #define __FIELDOPFERT_H

                               //Required for base classes
#ifndef __FIELDOP_H
  #include "fieldOp.h"
#endif

//! Specialised field operation for the application of fertiliser or manure

class fieldOpFert  : public fieldOp
{

  //! Amount of fertiliser or manure ammonium nitrogen to be applied (kilogram/hectare).
     float amountNH4N; //Attribute data member
  //! Amount of fertiliser or manure nitrate nitrogen to be applied (kilogram/hectare).
     float amountNO3N; //Attribute data member
  //! Amount of manure organic nitrogen to be applied (kilogram/hectare).
     float amountOrgN; //Attribute data member
   //! Type of manure to be applied. Optional parameter.
     int type; //Attribute data member
     //! depth into soil to which fertiliser or manure is applied
     float depth;
     //! Amount of fertiliser or manure carbon to be applied (kilogram/hectare).
     float amountC; //Attribute data member
     //! Application method used to apply fertiliser or manure. 1 = surface application, 2 = injection, 3 = band/trailing shoe
     int applicMethod;

private:

protected:

public:

  fieldOpFert ();

//! Defines the parameters of a fertiliser operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
\param theType the type of fertiliser or manure applied. Note: optional parameter
\param theNH4N the amount of ammonium nitrogen in the fertiliser or manure applied (kilograms per hectare).
\param theNO3N the amount of nitrate nitrogen in the fertiliser or manure applied (kilograms per hectare).
\param theOrgN the amount of organic nitrogen in the manure applied (kilograms per hectare).
\param theC the amount of carbon in the manure applied (kilograms per hectare).
\param theappMeth the method used to apply the fertiliser or manure.
*/
   void DefineOp (char * aName, int aNumber, int Year, int Month, int Day, int theType,
   		float theNH4N, float theNO3N, float theOrgN, float theC, int theappMeth)    ;

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param JulianDay Julian day on which operation occurs
\param theType the type of fertiliser or manure applied. Note: optional parameter
\param theNH4N the amount of ammonium nitrogen in the fertiliser or manure applied (kilograms per hectare).
\param theNO3N the amount of nitrate nitrogen in the fertiliser or manure applied (kilograms per hectare).
\param theOrgN the amount of organic nitrogen in the manure applied (kilograms per hectare).
\param theC the amount of carbon in the manure applied (kilograms per hectare).
\param theappMeth the method used to apply the fertiliser or manure.
*/
	void DefineOp(char * aName, int aNumber, int Year, int JulianDay, int theType,
   		float theNH4N, float theNO3N, float theOrgN, float theC, int theappMeth);

  //! Returns amount of fertiliser or manure ammonium nitrogen to be applied (kilogram/hectare).
     float getamountNH4N() {return amountNH4N;}
  //! Returns amount of fertiliser or manure nitrate nitrogen to be applied (kilogram/hectare).
     float getamountNO3N() {return amountNO3N;} //Attribute data member
  //! Returns amount of manure organic nitrogen to be applied (kilogram/hectare).
     float getamountOrgN() {return amountOrgN;} //Attribute data member
   //! Returns type of manure to be applied. Optional parameter.
     int gettype() {return type;} //Attribute data member
     //! Returns depth into soil to which fertiliser or manure is applied (mm)
     float getdepth() {return depth;}
     //! Returns amount of fertiliser or manure carbon to be applied (kilogram/hectare).
     float getamountC() {return amountC;} //Attribute data member
     //! Returns application method used to apply fertiliser or manure. 1 = surface application, 2 = injection, 3 = band/trailing shoe
     int getapplicMethod() {return applicMethod;}

   //! Write details of the timing of the operation to a file
   /*!
   \param outfile pointer to the output file
   \param status integer indicating the success or failure of the operation to be performed as desired
   \param printYear boolean which if true, prevents the year from being output
   */
   virtual void outputOps (fstream * outfile, int status, bool printYear);

   //!Write FASSET field operation files
     /*!
     \param outfile pointer to the output file
     \param number number of file ending (e.g. 2 will result in the file ending .f02)
      */
   virtual void outputFnnOps(ofstream *outfile, int number);

   //!Read the operations from a NitroEurope field operation file
   /*!
   \param infile pointer to the input file
   \param theYear the year of the operation
   \param theJulianDay the Julian day of the operation
    */
	void ReadOpResults(ifstream * infile, int theYear, int theJulianDay);

	virtual double GetNApplied();

//!Checks various parts of the output for logical errors
	virtual void CheckContent();

};

#endif
