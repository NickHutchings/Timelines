

#include "cropRotationClass.h"
#include "base/message.h"

cropRotationClass::cropRotationClass ( )
{
	theCrops= new linkList<NCUCropClass>;
};

cropRotationClass:: ~ cropRotationClass ( )
{
	delete theCrops;
};

bool cropRotationClass::Initialise(fstream *afile)
{
/*	Year =0;
	char str6[6];
   afile->getline(str6,6,'\n');
   Year = atoi(str6);
	if (Year==0)
   	return false;
   else    */
      return true;
}


