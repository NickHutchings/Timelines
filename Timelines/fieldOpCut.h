// Class: fieldOp                     //ANSI C++

#ifndef __FIELDOPCUT_H  //Required for current class
  #define __FIELDOPCUT_H


#ifndef __FIELDOP_H
  #include "fieldOp.h"
#endif

//! Describes a cutting operation

class fieldOpCut: public fieldOp
{

private:

	double yield;
protected:


public:

		//Default constructor
fieldOpCut () {opCode = 4; yield =0.0;};

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
\param aYield optional value of grass yield (kg per ha dry weight)
*/
 virtual   void DefineOp (char * aName, int aNumber, int Year, int Month, int Day, double aYield=0.0);

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param JulianDay Julian day on which operation occurs
\param aYield optional value of grass yield (kg per ha dry weight)
*/
 virtual	void DefineOp(char * aName, int aNumber, int Year, int JulianDay, double aYield=0.0);

 //! Write details of the timing of the operation to a file
/*!
\param outfile pointer to the output file
\param thestatus integer indicating the success or failure of the operation to be performed as desired
\param printYear boolean which if true, prevents the year from being output
*/
 virtual  void outputOps (fstream * outfile, int thestatus, bool printYear)    ;

//!Write FASSET field operation files - this function will be overloaded in derived classes
  /*!
  \param outfile pointer to the output file
  \param number number of file ending (e.g. 2 will result in the file ending .f02)
   */
  virtual void outputFnnOps(ofstream *outfile, int number){};
  //!Read the operations from a NitroEurope field operation file
  /*!
  \param infile pointer to the input file
  \param theYear the year of the operation
  \param theJulianDay the Julian day of the operation
   */
	void ReadOpResults(ifstream * infile, int theYear, int theJulianDay);

};

#endif
