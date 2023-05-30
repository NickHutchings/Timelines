// Class: fieldOp                     //ANSI C++

#ifndef __FIELDOP_H  //Required for current class
  #define __FIELDOP_H


#include "base/bstime.h"        //Required for include files
#include "base/common.h"
#include "base/message.h"

//! Describes a generalised or specific field operation

class fieldOp
{

private:

protected:

  //! Date on which the operation can be performed
     bsTime * theOptime; //Attribute data member
   //! Name of the operation
     string name; //Attribute data member
   //! Identity number of the operation
     int opCode; //Attribute data member
   //!Indicates whether the operation was performed normally or was forced
   	int status;
   //!If true, there will be a detailed dialogue printed to screen
     bool verbosePrint;

public:

			//!Default constructor
  fieldOp ();

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param Month month
\param Day day
*/
 virtual   void DefineOp (char * aName, int aNumber, int Year, int Month, int Day);

//! Defines the parameters of the operation
/*!
\param aName name of the operation
\param aNumber identity of the operation
\param Year year
\param JulianDay Julian day on which operation occurs
*/
 virtual	void DefineOp(char * aName, int aNumber, int Year, int JulianDay);

 //! Initialise an instance
 virtual   void Initialise ()    ;

//! Write details of the timing of the operation to a file
/*!
\param outfile pointer to the output file
\param thestatus integer indicating the success or failure of the operation to be performed as desired
\param printYear boolean which if true, prevents the year from being output
*/
 virtual  void outputOps (fstream * outfile, int thestatus, bool printYear)    ;

//!Destructor
  virtual  ~ fieldOp ( );

  //!Returns a pointer to the instance of bsTime that holds the date of the operation
  bsTime * gettheOptime() const  { return theOptime;}

//!returns the name of the operation
  string getname() const  { return name;}

  //!Returned the operation code
  int getopCode() const  { return opCode;}

  //!Set the date of the operation
  /*!
  \param atheOptime pointer to an instance of bsTime that holds the date of the operation
   */
  void settheOptime (bsTime * atheOptime)  { theOptime = atheOptime;}

  //!Set the name of the operation
  /*!
  \param aname the name of the operation
   */
  void setname (string aname)  { name = aname;}
  //!set the operation code
  /*!
  \param aopCode the operation code
   */
  void setopCode (int aopCode)  { opCode = aopCode;}
//! Set if details should be printed to the screen
  /*!
   \param aVal set verbose printing on or off
   */
  void SetverbosePrint(bool aVal) {verbosePrint = aVal;}
//!Write FASSET field operation files - this function will be overloaded in derived classes
  /*!
  \param outfile pointer to the output file
  \param number number of file ending (e.g. 2 will result in the file ending .f02)
   */
  virtual void outputFnnOps(ofstream *outfile, int number)
  {theMessage->FatalError("FieldOp: call to base class");};
  //!Read the operations from a NitroEurope field operation file
  /*!
  \param infile pointer to the input file
  \param theYear the year of the operation
  \param theJulianDay the Julian day of the operation
   */
	virtual void ReadOpResults(ifstream * infile, int theYear, int theJulianDay);

  //!Read the operations from a NitroEurope field operation file
  /*!
  \param infile pointer to the input file
  \param theYear the year of the operation
  \param theJulianDay the Julian day of the operation
  \param anOpCode operation code
   */
	virtual void ReadOpResults(ifstream * infile, int theYear, int theJulianDay, int anOpCode);
	virtual double GetNApplied(){return 0.0;}

	virtual void CheckContent(){};
	virtual void printToSceen();

};

#endif
