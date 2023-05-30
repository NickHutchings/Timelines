// Class: fieldOpTill                     //ANSI C++

#include "base/linklist.h"

#ifndef __FIELDOPTILL_H  //Required for current class
  #define __FIELDOPTILL_H

                               //Required for base classes
#ifndef __FIELDOP_H
  #include "fieldOp.h"
#endif

//! Describes a tillage operation

class fieldOpTill  : public fieldOp
{

  //! Depth to which the soil should be tilled (metres)
     float depth; //Attribute data member (metres)

private:

protected:

public:

  //!Default constructor
  fieldOpTill () {opCode = 1;};

//! Defines the parameters of the tillage operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
\param tillageDepth depth of the tillage operation (metres)
*/
   void DefineOp(char * aName, int aNumber, int Year, int Month, int Day, float tillageDepth);

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param JulianDay Julian day on which operation occurs
\param tillageDepth depth of the tillage operation (metres)
*/
	void DefineOp(char * aName, int aNumber, int Year, int JulianDay, float tillageDepth);

   //!Returns the depth of cultivation (in metres)
  float getdepth() const  { return depth;}

  //!Set the depth of cultivation (in metres)
  /*!
  \param  adepth depth of cultivation in metres
   */
  void setdepth (float adepth)  { depth = adepth;}

  //! Write details of the timing of the operation to a file
  /*!
  \param outfile pointer to the output file
  \param status integer indicating the success or failure of the operation to be performed as desired
  \param printYear boolean which if true, prevents the year from being output
  */
   virtual void outputOps (fstream * outfile, int status, bool printYear)    ;
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

};

#endif
