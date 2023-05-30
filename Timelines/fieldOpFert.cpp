

// Class: fieldOpFert

//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "fieldOpFert.h"

fieldOpFert::fieldOpFert ()
{
	opCode = 3;
	amountNH4N=0.0;
	amountNO3N=0.0;
	amountOrgN=0.0;
	amountC = 0.0;
	applicMethod=0;
	type=0;
};


void fieldOpFert::DefineOp(char * aName, int aNumber, int Year, int Month, int Day, int theType,
   		float theNH4N, float theNO3N, float theOrgN, float theC, int theappMeth)
{
	fieldOp::DefineOp(aName,  aNumber, Year, Month, Day);
	amountNH4N=theNH4N;
   amountNO3N=theNO3N;
   amountOrgN=theOrgN;
   amountC = theC;
   applicMethod=theappMeth;
   type=theType;
}

void fieldOpFert::DefineOp(char * aName, int aNumber, int Year, int JulianDay, int theType,
   		float theNH4N, float theNO3N, float theOrgN, float theC, int theappMeth)
{
	fieldOp::DefineOp(aName,  aNumber, Year, JulianDay);
	amountNH4N=theNH4N;
   amountNO3N=theNO3N;
   amountOrgN=theOrgN;
   amountC = theC;
   applicMethod=theappMeth;
   type=theType;
}


void fieldOpFert::outputOps(fstream * outfile, int status, bool printYear)
{
	fieldOp::outputOps(outfile, status, printYear);
	*outfile << type << "\t" << amountNH4N << "\t" << amountNO3N << "\t" << amountOrgN
   			<< "\t" << amountC << "\t" << applicMethod << "\t";
	if (verbosePrint)
		cout << type << "\t" << amountNH4N << "\t" << amountNO3N << "\t" << amountOrgN
   			<< "\t" << amountC << "\t" << applicMethod << "\t";
}

void fieldOpFert::outputFnnOps(ofstream *outfile, int number)
{
	char Num[3];
	float totalN = amountNH4N + amountNO3N + amountOrgN;
	sprintf(Num,"%d",number);
	*outfile << "FertilizerDate(" << Num << ")" << "\t" << gettheOptime()->GetDay() << "/" <<
			gettheOptime()->GetMonth() << "/" << gettheOptime()->GetYear() << endl;
	*outfile << "FertilizerType(" << Num << ")" << "\t" << "SLURRY" << endl;
	*outfile << "FertilizerN(" << Num << ")" << "\t" << totalN << endl;
	*outfile << "NH4_fraction(" << Num << ")" << "\t" << 0.5 << endl;

	if (verbosePrint)
   {
      cout << "FertilizerDate(" << Num << ")" << "\t" << gettheOptime()->GetDay() << "/" <<
            gettheOptime()->GetMonth() << "/" << gettheOptime()->GetYear() << endl;
      cout << "FertilizerType(" << Num << ")" << "\t" << "SLURRY" << endl;
      cout << "FertilizerN(" << Num << ")" << "\t" << totalN << endl;
      cout << "NH4_fraction(" << Num << ")" << "\t" << 0.5 << endl;
	}
}


void fieldOpFert::ReadOpResults(ifstream * infile, int theYear, int theJulianDay)
{
   fieldOp::ReadOpResults(infile, theYear, theJulianDay);
	*infile >> type >> amountNH4N >> amountNO3N >> amountOrgN >> amountC >> applicMethod;
   name = "Fertilisation";
}

double fieldOpFert::GetNApplied()
{
	double amountN = amountNH4N + amountNO3N + amountOrgN;
	return amountN;
}


void fieldOpFert::CheckContent()
{
	if ((amountNH4N==0.0) && (amountNO3N==0.0) && (amountOrgN==0.0))
	{
		cout << "type " << type << " amountNH4N " << amountNH4N << " amountNO3N " << amountNO3N << " amountOrgN " << amountOrgN << endl;;
	   	theMessage->FatalError("fieldOpFert::CheckContent - no N in fertiliser");
	}
   if ((type>1) &&  (amountC ==0.0))
   {
		cout << "type " << type << " amountC " << amountC << " applicMethod " << applicMethod << endl;;
	   	theMessage->FatalError("fieldOpFert::CheckContent - no C in manure");
   }


};

