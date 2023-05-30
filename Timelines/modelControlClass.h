// Class: modelControlClass                     //ANSI C++

#include "base/common.h"

#ifndef __MODELCONTROLCLASS_H  //Required for current class
  #define __MODELCONTROLCLASS_H

#include "base/base.h"
#include "base/linklist.h"
#include "base/bstime.h"
#include "NCUClass.h"

class modelControlClass  : public base        
{                       
			
     bsTime* startTime; //! starting date for the simulation
   		
     bsTime* endTime; //! ending data for the simulation
   		
     fstream* outputFile;  //! pointer to output file

private:

protected:

public:

   modelControlClass ();
	//! simulate crop management
   /*!
    * \param isArable if true, any grass in the crop rotations should be cut and not grazed
    * \param startDay first Julian day of the simulation
    * \param startYear first year of the simulation
    * \param endDay last day of the simulation
    * \param endYear last year the simulation
    * \param maxNCUs maximum number of NCUs that should be processed
    * \param numFields number of crop series (fields) simulated for each NCU
    * \param baseYear base year for the meteorological data
    * \param range number of years in each block of meteorological data
    * \param startNCUI NCUID of the first NCU to be simulated
    * \param endNCUI NCUID of the last NCU tto be simulated
    * \param metdataInputDir name of the directory containing the meteorological data
    * \param tpvStem name of the subdirectory containing the temperature and vapour pressure data
    * \param radStem name the subdirectory containing the radiation data
    * \param ncu_mars name of the file containing the NCU identifier x MARS identifier matrix
    * \param inputDir name of the input directory for the crop rotation
    * \param outputDir name at the output directory for the field operations
    * \param infile pointer to the file containing the crop rotations
    * \param outfileName name of the file to each field operations should be output
    * \param readAll if true,  all NCUs will be read
    * \param verbosityLevel level of reporting to the screen (0 = minimal reporting, 1 = medium, 2 = maximum)
    * \param implementationYear calendar year from which mitigation measures will be applied
    * \param usezeroTillage if true, zero tillage will be used where possible
    * \param usecoverCropping if true, a cover crop will be sown where possible
    * \param springGrazing if true, the timing of  any grazing will be skewed towards the spring
    */
	void processCropManagement(bool isArable, int startDay,  int startYear, int endDay,  int endYear,  int maxNCUs,  int numFields, int baseYear,
			 int range,	int startNCUI,  int endNCUI, string metdataInputDir, string tpvStem, string radStem, string ncu_mars, string inputDir,
			 string outputDir, fstream* infile, string outfileName, bool readAll, int verbosityLevel, int implementationYear, bool usezeroTillage, bool usecoverCropping,
			 bool springGrazing);
	//!Destructor - Delete any pointer data members that used new in constructors
   ~ modelControlClass ( );

                       //!Get date on which  simulation should start
  bsTime* getstartTime() const  { return startTime;}

                       //!Get date on which simulation should finish
  bsTime* getendTime() const  { return endTime;}

                       //!Get pointer to output file
  fstream* getoutputFile() const  { return outputFile;}
                       
                       //!Set date for start of simulation
  void setstartTime (bsTime* astartTime)  { startTime = astartTime;}

                       //!Set date for ending simulation
  void setendTime (bsTime* aendTime)  { endTime = aendTime;}

                       //!Set pointer to output file
  void setoutputFile (fstream* aoutputFile)  { outputFile = aoutputFile;}

};

#endif
