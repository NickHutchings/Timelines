// Class: fieldOpSow                     //ANSI C++

#include "base/linklist.h"

#ifndef __FIELDOPSOW_H  //Required for current class
  #define __FIELDOPSOW_H

                               //Required for base classes
#ifndef __FIELDOP_H
  #include "fieldOp.h"
#endif

//! Specialised field operation for sowing. See fieldOp for details.

class fieldOpSow  : public fieldOp
{

	//! Name of the crop to be sown
     string plantName; //Attribute data member
    //!amount of seed used (kg per ha)
     float amountSeed;
    //!true if the crop is a cover crop
     bool isCover;

private:

protected:

public:

			//Default constructor
  fieldOpSow () {opCode = 2;};

//! Defines the parameters of the sowing operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
\param aplantName name of the crop to be sown
\param cover if true, this is a cover crop
*/
   void DefineOp (char * aName, int aNumber, int Year, int Month, int Day, string aplantName, bool cover)    ;

/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param JulianDay Julian day no
\param aplantName name of the crop to be sown
\param cover if true, this is a cover crop
*/
   void DefineOp(char * aName, int aNumber, int Year, int JulianDay, string aplantName, bool cover);

   //!Return the name of the crop that is sown
  string getplantName() const  { return plantName;}

  //!Set the name of the crop that is to be sown
  /*!
  \param aplantName name of the crop to be sown
   */
  void setplantName (string aplantName)  { plantName = aplantName;}

  //! Write details of the timing of the operation to a file
  /*!
  \param outfile pointer to the output file
  \param status integer indicating the success or failure of the operation to be performed as desired
  \param printYear boolean which if true, prevents the year from being output
  */
   virtual  void outputOps (fstream * outfile, int status, bool printYear)    ;
   //!Write FASSET field operation files - this function will be overloaded in derived classes
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
	//!Return amount of seed to be sown per ha (kg)
   float GetamountSeed();
   //!return if crop is a cover crop
   bool getisCover() {return isCover;}

   //! write the contents of this operation to the screen
   void printToSceen();

};

#endif
