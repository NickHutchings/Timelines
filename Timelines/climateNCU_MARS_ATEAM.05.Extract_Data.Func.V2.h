//! Extract data from MARS met data set
/*
/param ID MARS identifier code
/param startDay first day no to extract data
/param startYear first year to extract data
/param endDay last day to extract data
/param endYear last year to extract data
/param inputDir name of input directory
/param tpvStem name of subfolder with tpv (temperature, precipitation & vapour pressure) data
/param radStem name of subfoloder with radiation data
/param ncu_mars name of file containing NCU to MARS correspondence table
/param baseYear base year for calculation (normally 19x1)
/param range range of years per dataset (normally 10)
/param results[][366][6] array containing met data (air temp mean, air temp max, precipitation, vapour pressure, radiation
 */

int getMetData(int ID, int startDay, int startYear, int endDay, int endYear, string  inputDir,
	string  tpvStem, string  radStem, string  ncu_mars, int baseYear, int range, double results[][366][6]);
//! Returned the NCU number that corresponds with the HSMU identifier
/*!
 * \param hsmuID the HSMU identifier for which the NCU number is required
 * \param Cncu name of the file containing the HSMU identifier x NCU number matrix
 */
int getNCUfromHSMU(int hsmuID,char Cncu[256]);

