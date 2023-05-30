

#include "cropSeriesClass.h"
#include "base/message.h"
#include "base/nixcbits.h"
#include "fieldOpSow.h"
#include "fieldOpFert.h"

extern int verbosityLevel;

cropSeriesClass::cropSeriesClass (int theseriesNumber )
{
	seriesNumber = theseriesNumber;
	theOperations= new linkList<fieldOp>;
};

cropSeriesClass:: ~ cropSeriesClass ( )
{
	delete theOperations;
};

//! Sorts the field operations in order of starting date
void cropSeriesClass::sortOps()
{
	temp_double_array  doubleTime;
	temp_ptr_array ptrlist;
	int numPtrs=0;
	for (int i=0; i<theOperations->NumOfNodes(); i++)
	{
		fieldOp * afieldOp = theOperations->ElementAtNumber(i);
		doubleTime[i]=(double) afieldOp->gettheOptime()->GetDateValue();
		ptrlist[i]=(void*) theOperations->ElementAtNumber(i);
		numPtrs++;
	}

	// Below is the "Combsort" sorting algorithm, which is nearly as fast as
	// Quicksort, and does not require extra space on heap and stack, i.e. more
	// suited for this purpose than the implemented Quicksort (bmp).
	Comsort(doubleTime, ptrlist, numPtrs);
	theOperations->ForgetAll();
	for (int counter=0;counter<numPtrs;counter++)
	{
		theOperations->InsertLast((fieldOp *) ptrlist[counter]);
	}
}

void cropSeriesClass::OutputOps(fstream * outfile, int NCU)
{
	int year = 0;
	bool first = true;
	for (int i=0; i<theOperations->NumOfNodes(); i++)
	{
		fieldOp * afieldOp = theOperations->ElementAtNumber(i);
		int OpYear = afieldOp->gettheOptime()->GetYear();
		if (OpYear!=year)
		{
			if (first)
				first = false;
			else
				*outfile << endl;
			*outfile << NCU << "\t" << seriesNumber << "\t" << OpYear << "\t" << GetNumOutputOpsInYear(OpYear) << "\t";
			year = OpYear;
		}
		if (verbosityLevel>0)
			cout << OpYear << "\t";
		afieldOp->outputOps(outfile, true, false);
		if (verbosityLevel>0)
			cout << endl;
	}
	if (theOperations->NumOfNodes()>0)
		*outfile << endl;
};

int cropSeriesClass::GetNumOutputOpsInYear(int targetYear)
{
	int numOps = 0;
	bool gotit=false;
	int i=0;

	int OpYear = 0;
	linkList<fieldOp>::PS p;
	theOperations->PeekHead(p);
	while ((gotit==false)&&(p!=NULL))
	{

		OpYear = p->element->gettheOptime()->GetYear();
		if (OpYear==targetYear)
			gotit=true;
		else
			i++;
		theOperations->OneStep(p);
	}
	fieldOp * afieldOp;
	while ((OpYear==targetYear)&&(i<theOperations->NumOfNodes()))
	{
		afieldOp = theOperations->ElementAtNumber(i);
		OpYear = afieldOp->gettheOptime()->GetYear();
		if (OpYear==targetYear)
		{
			numOps++;
			i++;
		}
	}
	return numOps;
};

double cropSeriesClass::GetNOutput()
{
	double retVal = 0.0;
	for (int i=0; i<theOperations->NumOfNodes(); i++)
	{
		fieldOp * afieldOp = theOperations->ElementAtNumber(i);
		retVal+=afieldOp->GetNApplied();
	}
	return retVal;
}

bool cropSeriesClass::CheckOperationIntegrity()
{
	bool retVal=true;
	bool blockNextSowing=false;
	bool blockNextTillage=false;
	string cropName;
	int currentNodeNum=0;
	int lastFertilisation=0;//! date of last fertilisation
	int minimumFertHarvGap=0;//! minimum number of days between last application fertiliser or manure and a harvest operation
	while ((currentNodeNum<theOperations->NumOfNodes())&&(retVal))
	{
		fieldOp * afieldOp = theOperations->ElementAtNumber(currentNodeNum);
		switch (afieldOp->getopCode())
		{
		case 1://tillage
		if (verbosityLevel>0)
			cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " " << afieldOp->getopCode() << endl;
		if (blockNextTillage)
		{
			cout << "Series No " << getseriesNumber() << " Crop name " << cropName << endl;
			if (verbosityLevel>0)
				theMessage->FatalError("Tillage operation before old crop has been harvested");
			else
				theMessage->WarningWithDisplay("Tillage operation before old crop has been harvested");
			retVal=false;
		}
		else
			blockNextSowing=false;
		break;
		case 2: //sowing
			if (verbosityLevel>0)
				cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " " << afieldOp->getopCode()
				<< " " << ((fieldOpSow *) afieldOp)->getplantName() << endl;
			cropName=((fieldOpSow *) afieldOp)->getplantName();
			if ((cropName=="GRAS")||(cropName=="GRAP"))
				minimumFertHarvGap=0;
			else
				minimumFertHarvGap=21;
			if (blockNextSowing)
			{
				cout << "Crop name " << cropName << endl;
				if (verbosityLevel>0)
					theMessage->FatalError("Sowing of new crop before old crop has been harvested or incorporated");
				else
				{
					cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " "
							<< afieldOp->gettheOptime()->GetJulianDay() << " " << afieldOp->getopCode()
									<< " " << ((fieldOpSow *) afieldOp)->getplantName() << endl;
					theMessage->WarningWithDisplay("Sowing of new crop before old crop has been harvested or incorporated");
				}
				retVal=false;
			}
			else
			{
				blockNextSowing=true;
				if ((((fieldOpSow *) afieldOp)->getplantName()=="SETA")
						|| (((fieldOpSow *) afieldOp)->getisCover()))
					blockNextTillage=false;
				else
					blockNextTillage=true;
			}
			break;
		case 3: //fertilisation and manuring
			lastFertilisation = afieldOp->gettheOptime()->GetDateValue();
			if (verbosityLevel>0)
				cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " "
				<< afieldOp->gettheOptime()->GetJulianDay() << " " << afieldOp->getopCode()
				<< " " << ((fieldOpFert *) afieldOp)->gettype() << endl;
			break;
		case 4://cutting
		case 5://grazing

			blockNextTillage=false;
			blockNextSowing=false;
			if (verbosityLevel>0)
				cout << *afieldOp->gettheOptime() << " " << afieldOp->getopCode() << endl;
			break;
		case 9://harvesting
			blockNextTillage=false;
			blockNextSowing=false;
			if (verbosityLevel>0)
				cout << *afieldOp->gettheOptime() << " " << afieldOp->getopCode() << endl;
			int currentDay = afieldOp->gettheOptime()->GetDateValue();
			if ((currentDay-lastFertilisation)<minimumFertHarvGap)
			{
				cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " "
						<< afieldOp->gettheOptime()->GetJulianDay() << " " << afieldOp->getopCode()
								<< " " << cropName << endl;
				theMessage->WarningWithDisplay("Fertilisation within three weeks of harvesting");
				retVal=false;
			}
			else
				lastFertilisation=0;
			break;
		}
		currentNodeNum++;
	}
	if (!retVal)
	{
		int numStartLastCodes = 0;
		if (currentNodeNum>30)
			numStartLastCodes=currentNodeNum-30;
		for (int i=numStartLastCodes; i<currentNodeNum; i++)
		{
			fieldOp * afieldOp = theOperations->ElementAtNumber(i);
			if (afieldOp->getopCode()==2)
				cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " " << afieldOp->getopCode()
				<< " " << ((fieldOpSow *) afieldOp)->getplantName() << endl;
			else
				cout << *afieldOp->gettheOptime() << " " << afieldOp->gettheOptime()->GetDateValue() << " " << afieldOp->getopCode() << endl;
		}
	}
	return retVal;
}
