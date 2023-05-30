// Class: NCUClass                     //ANSI C++

#include "base/common.h"
#include "base/linklist.h"

#ifndef __CROPSERIESCLASS_H
#define __CROPSERIESCLASS_H

#ifndef __FIELDOP_H
#include "fieldOp.h"
#endif
//! Container class for the series of field operations to be performed at a given location (field)

class cropSeriesClass
{

private:

	int seriesNumber;//! the number of the series (starts at zero)
	linkList<fieldOp> *theOperations; //! list of pointers to all the field operations performed on this field

protected:

public:

	//!constructor
	cropSeriesClass (int theseriesNumber);

	~ cropSeriesClass ( );  //!Destructor - Delete any pointer data members that used new in constructors
	//! return a pointer to the list of field operations
	linkList<fieldOp> * gettheOperations() {return theOperations;}
	//! sort the field operations into the order of the date on which they will be performed
	void sortOps();
	//! output all the field operations in crop series to a file
	/*!
	 * \param outfile pointer to output file
	 * \param NCU NCU identifier
	 */
	void OutputOps(fstream * outfile, int NCU);
	//! returned the number of operations to be performed within a given calendar year
	/*!
	 * \param targetYear calendar year for which the number of operations is required
	 */
	int GetNumOutputOpsInYear(int targetYear);
	//! returns the amount of nitrogen applied by field operations within the series (kilograms per hectare)
	double GetNOutput();
	//! performs quality control checks on  the field operations
	bool CheckOperationIntegrity();
	//! return number of the series
	int getseriesNumber(){return seriesNumber;}
};

#endif
