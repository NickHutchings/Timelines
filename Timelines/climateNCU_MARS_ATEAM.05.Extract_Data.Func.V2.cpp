
#include "base/message.h"
#include <stdio.h>
#include <string.h>
#include "climateNCU_MARS_ATEAM.05.Extract_Data.Func.V2.h"
#ifndef __BCplusplus__
using namespace std;
#else
	#include <fstream.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <dir.h>
#endif
#define maxpars 5

#include <math.h>

extern int verbosityLevel;

/*
LAST UPDATE 04/2009 (V1)
LAST UPDATE 06/2009 (V2)
 
This file reads data values from the binary climate files that are generated for the NitroEurope-IProject
on the basis of the MARS and ATEMA/CRU datasets and cover EU27+3 for the period 1901-2000.
The parameter included are Tmin [degree Celsius], Tmax [degree Celsius], Pre [mm], Vapour pressure [hPa], global radiation [MJ/m2/day].

All files are organized in groups of 10 years and 1500 NCUs. The reason is that each individual file should be kept at a
reasonable size (prefereably less than 100 MB) and the number of files a user needs should be limited for both applications
for a particular region or time slice.

Four parameters are stored together (Tmin, Tmax, Pre, Vap)
One parameter is stored extra (Rad)

Each set of files (for one time slice) is accompanied by a "map" file that stores the first bit for each NCU.
The loops are for each NCU: Loop(years,loop(days,loop(parameters))).
In a header of each climate_map, the following information are stored:

     numdim          --> Number of dimensions (four in this case: NCU, Years, Days, Parameters)

  1. Dimension: NCUs: 
     the first dimension is used to split the data-files, therefore a "gap" between the
     must be defined

     dim_beg[0]      --> Start Value (1)
     dim_end[0]      --> Last Value (4091)
     dim_stp[0]      --> Step (1)
     dim1_gap        --> The files are separated by the first dimension

 2. Dimension: Years
    Attention: there is no information on the map about differnt time slices; 
    this must be programmed

    dim_beg[1]      --> Start value (First year in the time slice)
    dim_end[1]      --> Last value (Last year in the time slice)
    dim_stp[1]      --> Step (1)

 3. Dimension: Days

    dim_beg[2]      --> Start value (1)
    dim_end[2]      --> Last value (365)
    dim_stp[2]      --> Step (1)
                    
 4. Dimension: Parameters
    Attention: the last dimension needs only the number of values

    dim_end[3]       -->  Number of parameters


 To read an individual datum, it must be defined by
 - File structure (location of climate data and place to put results), for example
   --
 - Data files-trunc for both sets of files, for example
   -- MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN for the files containing the first four parameters
   -- radCRU_NCU.i686-pc-cygwin_g++3.4.4_LITTLE_ENDIAN for the files containing the global radiation
 - Info on time slicing: absolute start year (1901) and size of each slice (10 years)
 - Identification of the numbers to read: NCU, Year, Day, Parameter
   Attention!!
   For each information you must give start and end value
   The program will cut out the n-dimensional array
 - In addition, a file is required that links NCU-numbers (from 1 to 4091) with the NCU-IDs.
   In some cases, only the NCU-ID will be known and this enables the program to find their numbers.
   We include this file as climateNCU_MARS_ATEAM.04.readbin_ncus.txt (name must be provided to the program)
 This information must be stored in a file called "climateNCU_MARS_ATEAM.05.extract_data.txt"


EXAMPLE OF A VALID INPUT FILE:
=============================================================================
4_Lncu_year
4_Lncu_year_test
MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN
radCRU_NCU.i686-pc-cygwin_g++3.4.4_LITTLE_ENDIAN
climateNCU_MARS_ATEAM.04.readbin_ncus.txt
1901
10
2036,2037
1982,1985
237,240
1,4
=============================================================================

... ANNOTATED (NOT VALID - ONLY HERE FOR EXPLANATION!!!)
=============================================================================
marsateam                                                  --> if you start the program at e.g. /home/nitroeurope/, the files are archived at /home/nitroeurope/marsateam
marsateam_test                                             --> if you start the program at e.g. /home/nitroeurope/, the files are archived at /home/nitroeurope/marsateam_test
MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN    --> for example for MARSATEAMNCU.x86_64-redhat-linux_g++4.1.2_LITTLE_ENDIAN.1901-1910.00001_01500.bin
radCRU_NCU.i686-pc-cygwin_g++3.4.4_LITTLE_ENDIAN           --> for example for radCRU_NCU.i686-pc-cygwin_g++3.4.4_LITTLE_ENDIAN.1991-2000.00001_01500.bin
climateNCU_MARS_ATEAM.04.readbin_ncus.txt                  --> this file must exist at the same location as the program is run!!
                                                               it is need in the case an NCU is called with the ID, for example
                                                               100249520 instead of the running number (in this case 1)

                                                           In the following the structure of the files is defined
1901                                                       --> first year considered
10                                                         --> number of years stored in one set of files

                                                           In the following the "window" for which the data should be extracted is defined
2036,2037                                                  --> NCUs (all between 2036 and 2037 incl.)
1982,1985                                                  --> years (all between 1982 and 1985 incl.)
237,240                                                    --> days (Julian days, all between 237 and 240 inc.)
1,5                                                        --> parameters (all between parameter #1 (Tmin) and #5 (Rad) incl.)
=============================================================================


Histoy of this file:
- first version april 2009
- second version june 2009, with extension to global-radiation files

*/

int getNCU_ID(int,char[256]);
int getNCUfromHSMU(int,char[256]);
double MakkinkEvaporation(double globalRadiation,double temp);
double DeltaVaporPress(double temp);
double VaporPress(double temp);
int getMARS(int hsmuID,char Cmars[256]);
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
	string  tpvStem, string  radStem, string  ncu_mars, int baseYear, int range, double results[][366][6])
{
	FILE *clim_map1, *clim_map2, *clim_bin1, *clim_bin2;
    FILE *clim_write;
	char Cclim_map1[200], Cclim_map2[200], Cclim_bin1[200], Cclim_bin2[200], Cclim_out[200];
	char Cclim_name1[200], Cclim_name2[200], clim_inpath[200], clim_outpath[200];
    char Cncu[256];

	int hsmu,hsmuID,rhsmuID;
	int yearstart, yearsfile;
	int read_start[maxpars], read_end[maxpars];
	int dim_beg[maxpars],dim_stp[maxpars],dim_end[maxpars];
    int idummy1,idummy2;
    float fdummy;



	long int posmap1,posmap2,posbin1,posbin2,posclim;

    char pars[maxpars][5];
    sprintf(pars[0],"tmn");
    sprintf(pars[1],"tmx");
    sprintf(pars[2],"pre");
    sprintf(pars[3],"vap");
    sprintf(pars[4],"rad");
	char line[50000];


   if (verbosityLevel==2)
   {
      cout<<"inputDir: "<<inputDir<<"\n";
      cout<<"tpvStem: "<<tpvStem<<"\n";
      cout<<"radStem: "<<radStem<<"\n";
      cout<<"ncu: "<<ncu_mars<<"\n";
      cout<<"year_range_start: "<<startYear<<"\n";
      cout<<"year_range_end: "<<range<<"\n";
   }
    read_start[0] = read_end[0] = ID;
    read_start[1] = startYear;
    read_end[1]   = endYear;
    read_start[2]	= startDay;
    read_end[2]	= endDay;

    read_start[3]	=0;
	 read_end[3]	=4;

//    fclose(clim_map1);

    int numdim, dim1_gap,startpix,nnendpix;
    int ncomment,numyears, numdays, firstncunut,stepncunut,nncunut;

    //open map ... all maps are equally constructed, so it does not matter which one ...
    sprintf(Cclim_map1,"%s\%s.%d-%d.map",inputDir,tpvStem,baseYear,baseYear+range-1);
    if (verbosityLevel==2)
	    cout <<  "map file 1: " << Cclim_map1 << endl;
    sprintf(Cclim_map2,"%s\%s.%d-%d.map",inputDir,radStem,baseYear,baseYear+range-1);
    if (verbosityLevel==2)
    cout <<  "map file 2: " << Cclim_map2 << endl;
    clim_map1=fopen(Cclim_map1,"rb");  //rb
    clim_map2=fopen(Cclim_map2,"rb");  //rb
    if (clim_map1==NULL){theMessage->FatalError("getMetData: file ", Cclim_map1, " not found");}
    if (clim_map2==NULL){theMessage->FatalError("getMetData: file ", Cclim_map2, " not found");}
    fread(&numdim,4,1,clim_map1);
    fread(&dim_beg[0],4,1,clim_map1); // 1. Dimension: HSMUs
    fread(&dim_end[0],4,1,clim_map1);
    fread(&dim_stp[0],4,1,clim_map1);
    fread(&dim1_gap,4,1,clim_map1);     // The files are separated by the first dimension
    fread(&dim_beg[1],4,1,clim_map1);  // 2. Dimension: Years
    fread(&dim_end[1],4,1,clim_map1);
    fread(&dim_stp[1],4,1,clim_map1);
    fread(&dim_beg[2],4,1,clim_map1);  // 3. Dimension: Days
    fread(&dim_end[2],4,1,clim_map1);
    fread(&dim_stp[2],4,1,clim_map1);
    fread(&dim_end[3],4,1,clim_map1);  //  4. Dimension: Parameters

    numdays=(dim_end[2]-dim_beg[2]+1)/dim_stp[2];
    numyears=(dim_end[1]-dim_beg[1]+1)/dim_stp[1];

    for (int ncu=read_start[0];ncu<=read_end[0];ncu+=dim_stp[0])
    {
        //If NCU-ID is given, convert to NCU-number, otherwise get the ID
        int hsmuID=ncu;
        int hsmu=ncu;
        char *tempstring;
        strcpy(tempstring,ncu_mars.c_str());
        if (hsmu>dim_end[0]) {hsmu=getNCUfromHSMU(hsmu,tempstring);}
        if (hsmuID==hsmu) {hsmuID=getNCU_ID(hsmu,tempstring);}

        //Check position of first bit of the NCU in the climate-map.
        //Attention: a few NCUs are missing (small islands, not included in the NEU-data, so they should not occur)
        //           however, the program will not give any numbers for these.
        posmap1=1000+12*(hsmu-1);
        fseek(clim_map1,posmap1,SEEK_SET);
        fseek(clim_map2,posmap1,SEEK_SET);
        //Read Start-Bit for the NCU and calculated bit for the actual value
        fread(&idummy1,4,1,clim_map1);
        fread(&posbin1,8,1,clim_map1);
        fread(&idummy1,4,1,clim_map2);
        fread(&posbin2,8,1,clim_map2);

        if(idummy1!=hsmu || hsmuID==999)
        {
            cout<<"Attention: wrong NCU in climate-map. NCU="<<hsmu<<" and read: "<<idummy1<<"\n";
//            fprintf(clim_write,"\n%d,%d --> no data available",hsmu,hsmuID);
				return 2;
        }
        else
        {
            for (int year=read_start[1];year<=read_end[1];year+=dim_stp[1])
            {
                //Open Data-File!

                //First determine the time-slice
                int filey1=startYear;
                int filey2=filey1+range-1;
                while (filey1+range <= year)
                {
                    filey1+=range ;
                    filey2=filey1+range-1;
                }

                //File names have trailing zeros ... must be included here
                startpix=(hsmu-1)/dim1_gap;
                startpix=startpix*dim1_gap+1;
                nnendpix=startpix+dim1_gap-1;
                char zeroa[5], zeroe[5];
                if(startpix>=10000){sprintf(zeroa,"");};
                if(startpix<10000){sprintf(zeroa,"0");};
                if(startpix<1000){sprintf(zeroa,"00");};
                if(startpix<100){sprintf(zeroa,"000");};
                if(startpix<10){sprintf(zeroa,"0000");};
                if(nnendpix>=10000){sprintf(zeroe,"");};
                if(nnendpix<10000){sprintf(zeroe,"0");};
                if(nnendpix<1000){sprintf(zeroe,"00");};
                if(nnendpix<100){sprintf(zeroe,"000");};
                if(nnendpix<10){sprintf(zeroe,"0000");};

                sprintf(Cclim_bin1,"%s/%s.%d-%d.%s%d_%s%d.bin",inputDir,tpvStem,baseYear,baseYear+range-1,zeroa,startpix,zeroe,nnendpix);
                sprintf(Cclim_bin2,"%s/%s.%d-%d.%s%d_%s%d.bin",inputDir,radStem,baseYear,baseYear+range-1,zeroa,startpix,zeroe,nnendpix);

                clim_bin1=fopen(Cclim_bin1,"rb");
                clim_bin2=fopen(Cclim_bin2,"rb");
                if (clim_bin1==NULL)
                {
                	cout << "Cannot open " << Cclim_bin1 <<"\n";
                  return 3;
                }
                else
					    if (verbosityLevel==2)
               	 	cout << "Opened " << Cclim_bin1 <<"\n";
                if (clim_bin2==NULL && read_end[3]==4)
                {
                	cout << "Cannot open " << Cclim_bin2 <<"\n";
                  return 4;
                }
                else
					    if (verbosityLevel==2)
               	 	cout << "Opened " << Cclim_bin2 <<"\n";

                for (int day=read_start[2];day<=read_end[2];day+=dim_stp[2])
                {
//                    fprintf(clim_write,"\n%d,%d,%d,%d",hsmu,hsmuID,year,day);
//                    cout << hsmu << " " << year <<  " " << day << endl;
                    for (int para=read_start[3];para<=read_end[3];para++)
                    {

                        if(para<4)
                        {
                            posclim=posbin1+4*(

                                    +(hsmu-startpix)*(filey2-filey1+1)/dim_stp[1]*(dim_end[2]-dim_beg[2]+1)/dim_stp[2]*4  // full ncu's
                                    +(year-filey1)/dim_stp[1]*(dim_end[2]-dim_beg[2]+1)/dim_stp[2]*4                      // full years
                                    +(day-dim_beg[2])/dim_stp[2]*4                                                        // full days
                                    +para);
                            fseek(clim_bin1,posclim,SEEK_SET);
                            fread(&fdummy,4,1,clim_bin1);
                        }
                        if(para==4)
                        {
                            posclim=posbin2+4*(

                                    +(hsmu-startpix)*(filey2-filey1+1)/dim_stp[1]*(dim_end[2]-dim_beg[2]+1)/dim_stp[2]  // full ncu's
                                    +(year-filey1)/dim_stp[1]*(dim_end[2]-dim_beg[2]+1)/dim_stp[2]
                                    +(day-dim_beg[2])/dim_stp[2]

                                    );
                            fseek(clim_bin2,posclim,SEEK_SET);
                            fread(&fdummy,4,1,clim_bin2);
                        }
								results[year-read_start[1]][day-1][para]=fdummy;
                        if(para==4)
                        	results[year-read_start[1]][day-1][5]=MakkinkEvaporation(fdummy,results[year-read_start[1]][day-1][0]);

//                        fprintf(clim_write,",%f",fdummy);
                    }
                }
                fclose(clim_bin1);
                fclose(clim_bin2);
            }
        }
    }
    fclose(clim_map1);
    fclose(clim_map2);
	return 1;
 }

int getNCU_ID(int hsmu,char Cncu[256])
{
    FILE *ncu;
    //char Cncu[100];
    ncu=fopen(Cncu, "r");
    int hsmuID=999, idummy1, idummy2;
    if (ncu==NULL){theMessage->FatalError("getMetData: file ", Cncu, " not found");}
    while ( fscanf(ncu, "%d %d", &idummy1,&idummy2) == 2 && hsmuID==999 )
    {
        if(idummy2==hsmu){hsmuID=idummy1;}
    }
    fclose(ncu);
    return hsmuID;
}
int getNCUfromHSMU(int hsmuID,char Cncu[256])
{
    FILE *ncu;
    //char Cncu[100];
    ncu=fopen(Cncu, "r");
    int hsmu=99999, idummy1, idummy2;
    if (ncu==NULL){theMessage->FatalError("getMetData: file ", Cncu, " not found");}
    while ( fscanf(ncu, "%d %d", &idummy1,&idummy2) == 2 && hsmu==99999 )
    {
        if(idummy1==hsmuID)
        {
        	hsmu=idummy2;
         if (verbosityLevel==2)
				cout<<hsmu;
        }
        //cout<<idummy1<<","<<hsmuID<<"\n";
    }
    if (hsmu==99999)
    {
    	theMessage->WarningWithDisplay("getMARS - no MARS code for this NCU ",hsmuID);
    }
    fclose(ncu);
    return hsmu;
}

int getMARS(int hsmuID,char Cmars[256])
{
    FILE *mars;
    //char Cncu[100];
    mars=fopen(Cmars, "r");
    int marsID=99999, idummy1, idummy2;
    if (mars==NULL){theMessage->FatalError("getMetData: file ", Cmars, " not found");}
    while ( fscanf(mars, "%d %d", &idummy1,&idummy2) == 2 && marsID==99999 )
    {
        if(idummy1==hsmuID)
        {
        	marsID=idummy2;
			if (verbosityLevel==2)
	         cout << "MARS ID " << marsID;
        }
        //cout<<idummy1<<","<<hsmuID<<"\n";
    }
    fclose(mars);
    if (marsID==99999)
    {
    	theMessage->WarningWithDisplay("getMARS - no MARS code for this HMSU ",hsmuID);
    }
    return marsID;
}

/****************************************************************************\
\****************************************************************************/
double VaporPress(double temp)
{
	double ret_val;
   if (temp<0)
   	ret_val = 6.11*exp(log(10.)*9.5*temp/(temp+265.5));
   else
   	ret_val = 6.11*exp(log(10.)*7.5*temp/(temp+237.3));
   return ret_val;
}

/****************************************************************************\
Returns slope of vapour pressure curve (in hPa/Celsius)
Input temp - mean daily air temperature in Celsuis
/****************************************************************************/

double DeltaVaporPress(double temp)
{
	double ret_val;
   ret_val = VaporPress(temp)*(4098.0259/((temp+237.3)*(temp+237.3)));
   return ret_val;
}



/****************************************************************************\
Calculates the potential evaporation after the Makkink method.
Input globalRadiation - MJ/sq metre/day, temp - mean daily air temperature in Celsuis
Output potential evapotranspiration (mm/day)
\****************************************************************************/

double MakkinkEvaporation(double globalRadiation,double temp)

{
   const double evpHtWat=2.465;
   const double psykrConst=0.667;
   double result =0.7*(DeltaVaporPress(temp)*globalRadiation/
                      (evpHtWat*(DeltaVaporPress(temp)+psykrConst)));
   return result;
}


void calcTemperatureSums(int ID, int startYear, int endYear, string  inputDir,
	string  tpvStem, string  radStem, string  ncu_mars, int baseYear, int range, double baseTemp)
{
	double metdata[50][366][6];
	double results[50][366][6];
	int startDay=1;
	int endDay = 366;
	getMetData(ID, startDay, startYear, endDay, endYear, inputDir, tpvStem, radStem, ncu_mars, baseYear, range, metdata);
	//for (int start)
}
