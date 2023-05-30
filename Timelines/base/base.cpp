/*!
This is a utility class, used to open and close files and manage the input of data
*/

#include "base.h"
#include "message.h"
//#include <common.h>
#include <stdio.h>
#include <string.h>
#ifdef __BCplusplus__
	#include <strstream>
#else
	#include <sstream>
	#include <sys/stat.h>
#endif


#ifdef BUILDER
	#include <dir.h>
#endif

/****************************************************************************\
 Default Constructor
\****************************************************************************/
base::base()
{
   InitVariables();
}

/****************************************************************************\
 Constructor with arguments
\****************************************************************************/
base::base(const char * aName, const int aIndex, const base * aOwner)
{
   InitVariables();
   strcpy(Name,aName);
   Index      = aIndex;
   Owner      = aOwner;
}

/****************************************************************************\
\****************************************************************************/
void base::InitVariables()
{
   strcpy(Name,"");
   Index      =-1;
   Owner      = NULL;
   cur_pos    = 0;
   file       = NULL;
   fileNme[0] = '\0';
   critical   = false;
   error      = false;
}

/****************************************************************************\
 Copy Constructor
 Does NOT copy "file", as there is no copy constructor for "fstream"
\****************************************************************************/
base::base(const base& aBase)
{
   strcpy(Name,aBase.Name);
   Index      = aBase.Index;
   Owner      = aBase.Owner;
   cur_pos    = aBase.cur_pos;
   file       = NULL;
   fileNme[0] = '\0';
   critical   = false;
   error      = false;
}

/****************************************************************************\
 Destructor
\****************************************************************************/
base::~base()
{
   if (file!=NULL)
   {
   	delete file;
   	file=NULL;
   }
}

/****************************************************************************\
 Assignment operator
\****************************************************************************/
base& base::operator = (const base& aBase)
{
   if(this==&aBase)
      return *this;
   strcpy(Name,aBase.Name);
   Index      = aBase.Index;
   Owner      = aBase.Owner;
   cur_pos    = aBase.cur_pos;
   file       = NULL;
   fileNme[0] = '\0';
   error      = false;
   critical   = false;
   return *this;
}

/****************************************************************************\
 Terminates the execution with an error message
\****************************************************************************/
void base::Terminate(const char * c1, const char * c2, const char * c3, const char * c4)
{
   if (strlen(c1)>0)
      cout << endl << c1 << c2 << c3 << c4 << endl;

   cout << endl << "Program terminated. " << endl;
#ifndef __ANSICPP__
   char ch=getchar();
   if (ch=='y' || ch=='Y' )
      exit(99);
#else
	exit(99);
#endif
}

/****************************************************************************\
 Upon call of this function, every error in parameter reading will cause an
 unconditional halt of the program. This halt will be accompagnied by
 explanatory text.
 "UnsetCritical()" ends this mode, as well as "CloseInputFile()"
\****************************************************************************/
void base::SetCritical()
{
   critical=true;
}

/****************************************************************************\
\****************************************************************************/
void base::UnsetCritical()
{
   critical=false;
}

/****************************************************************************\
\****************************************************************************/
bool base::OpenInputFile(char * fileName)
{
   cur_pos = 0;
   strcpy(fileNme,fileName);
   bool succes = false;
#ifdef __BCplusplus__
   char buffer[MAX_TXT];
#ifndef __ANSICPP__
   getcwd(buffer, MAX_TXT);
#else
   strncpy(buffer, fileName, MAX_TXT);
#endif

	if(file==NULL)
      file = new fstream;
#ifndef BUILDER
 #ifndef DEVCPP
   file->open(fileName, ios::in | ios::nocreate);
 #else
   file->open(fileName, ios::in);
 #endif
#else
   file->open(fileName, ios::in);
#endif
   if(!(*file) && (strlen(buffer)>4))
   {
   	chdir("..");                                 // check if file is in directory above
      file->close();
      succes = OpenInputFile(fileName);
   }
   else
   	return (*file);
   chdir(buffer);                                  // change to original directory
#else
   char baseInputDirectory[MAX_TXT];
   getcwd(baseInputDirectory, MAX_TXT);
//   cout << baseInputDirectory << endl;

	if(file==NULL)
	  file = new fstream();
   file->open(fileName, ios::in);
   if((!file->is_open()) && (strlen(baseInputDirectory)>4))
   {
      if (chdir("..")==0)                                 // check if file is in directory above
         file->close();
      else
      {
         if (critical)
         Terminate("File named [",fileName,"] not found");
      }
      file = NULL;
      succes = OpenInputFile(fileName);
   }
   else
   {
      if(file->is_open())
         return true;
      else
         return false;
   }
   chdir(baseInputDirectory);        // change to original directory
#endif
   if (!succes)
   {
	  error = true;
	  if (critical)
		 Terminate("File named [",fileName,"] not found");
   }
   return succes;
}

/****************************************************************************\
\****************************************************************************/
void base::DeleteInputFile()
{
   if(file!=NULL)
      delete file;
   file=NULL;
}

/****************************************************************************\
\****************************************************************************/
void base::CloseInputFile()
{
   if (file)
   	file->close();
   fileNme[0] = '\0';
   critical=false;
   error=false;
   cur_pos=0;
}

/****************************************************************************\
 Reads one line from 'f' into 's', max. 'maxlen'-1 charcters
\****************************************************************************/
void base::ReadLine(fstream * f,char * s,int maxlen)
{
   int i=-1;
   while (i<(maxlen-1))
   {
      i++;
      s[i]=f->peek();
      if (s[i]==EOF)
         break;
      f->get();
      if (s[i]=='\n')
         break;
      if (s[i]=='=') // Used in FARM-N internet interface for FASSET
         s[i]=' ';
   }
   s[i]='\0';
}

/****************************************************************************\
 Removes all spaces
\****************************************************************************/
void base::EatSpaces(char * s)
{
   int source=0;
   int dest=0;
   while (s[source]!='\0')
   {
      while (s[source]==' ')
         source++;
      s[dest]=s[source];
      dest++;
      source++;
   }
   s[dest]='\0';
}

/****************************************************************************\
\****************************************************************************/
void base::GetSectionNumbers(const char * sectionName, int * firstNum, int * numOfSections)
{
   if ((file->eof()>0) || (!(*file)))
	  theMessage->FatalError("base::GetSectionNumbers - previous attempt to read past end of file ",fileNme);

#ifdef __BCplusplus__
   char buffer[MAX_TXT];
   char txt[MAX_TXT];
   char section[MAX_TXT];
   bool searching = true;
   *numOfSections = 0;
   int high=-9999;
   *firstNum = 9999;
   strcpy(section,sectionName);
   file->seekg(0);
   while(*file && (file->peek()!=EOF)&& searching)
   {
      buffer[0]  = '\0';
      txt[0]     = '\0';
      ReadLine(file,buffer,MAX_TXT);
#ifdef __BCplusplus__
      istrstream inputString(buffer,strlen(buffer));
#else
      stringstream inputString;
      inputString << buffer;
#endif
      inputString >> txt >> ws;
      EatSpaces(txt);
      if(strstr(txt,"[end]"))
         searching=false;
      else
      {
         if ((*(txt)=='[') && (*(txt+strlen(txt)-1)==']') && strstr(txt,section) && (txt[strlen(section)+1]=='('))
         {
            int i = strlen(section)+2;
            while ((txt[i]>='0') && (txt[i]<='9'))
               i++;
            if ((txt[i]==')') && (txt[i+1]==']'))
            {
               (*numOfSections)++;
               char * ptr_char = &txt[strlen(section)+2];
               int nr = atoi(ptr_char);
               if(nr > high)
                  high = nr;
               if(nr < *firstNum)
                  *firstNum = nr;
            }
         }
      }
   }
#else
   string inputString;
   string paramString(sectionName);
   bool searching = true;
   *numOfSections = 0;
   int high=-9999;
   *firstNum = 9999;
   file->seekg(0);
   while(*file && (file->peek()!=EOF)&& searching)
   {
	  getline(*file,inputString,'\n');
	  int endchar = inputString.find_first_of(']');       //strip off any trailing comments
	  if ((endchar<0)) {
		  endchar=0;
	  }
	  inputString.assign(inputString.c_str(),endchar+1);
	  if (inputString.find("[end]",0)==0)
		 searching=false;
	  else
	  {
		if ((inputString[0]=='[') && (inputString[inputString.length()-1]==']')
				&& (inputString.find(paramString,0)==1) && (inputString[paramString.length()+1]=='('))
		{
			int i = paramString.length()+2;
			while ((inputString[i]>='0') && (inputString[i]<='9'))
			   i++;
			if ((inputString[i]==')') && (inputString[i+1]==']'))
			{
			   (*numOfSections)++;
			   string numstring = inputString.substr(paramString.length()+2,i);
			   int nr = atoi(numstring.c_str());
			   if(nr > high)
				  high = nr;
			   if(nr < *firstNum)
				  *firstNum = nr;
			}
		 }
	  }
   }
#endif
   if (*numOfSections==0)
   {
	  *firstNum = 0;
	  error = true;
	  if (critical)
	  {
		 cout << endl << endl << "No sections named [" << sectionName << "...] found";
		 if (fileNme[0]!='\0')
			cout << " in file '" << fileNme << "'";
		 Terminate();
	  }
   }
   else
	  if ((*numOfSections)!=(1+high-(*firstNum)))
	  {
		 error = true;
		 if (critical)
		 {
			cout << endl << endl << "Error in numbering of section [" << sectionName << "]";
			if (fileNme[0]!='\0')
			   cout << " in file '" << fileNme << "'";
			Terminate();
		 }
	}
   if (searching && critical)
   {
	  cout << endl << endl << "No [end] marker found";
	  if (fileNme[0]!='\0')
		 cout << " in file '" << fileNme << "'";
	  Terminate();
   }
   else
   	   if (file->peek()==EOF)
			file->clear();
}

/****************************************************************************\
\****************************************************************************/
bool base::FindSection(const string sectionName)
{
	return FindSection((char*) sectionName.c_str());
}

/****************************************************************************\
\****************************************************************************/
bool base::FindSection(const char * sectionName)
{
   file->seekg(0);
   if (file->eof()>0 || (!(*file)))
		theMessage->FatalError("base::FindSection - previous attempt to read past end of file ",fileNme);

#ifdef __BCplusplus__
   char buffer[MAX_TXT];
   char txt[MAX_TXT];
   file->seekg(0);
   while(*file && (file->peek()!=EOF))
   {
      buffer[0] = '\0';
      txt[0]    = '\0';
      ReadLine(file,buffer,MAX_TXT);
#ifndef DEVCPP
      istrstream inputString(buffer,strlen(buffer));
#else
      stringstream inputString;
      inputString << buffer;
#endif
      inputString >> txt >> ws;
      EatSpaces(txt);
      if(strstr(txt,"[end]"))
		{
         cur_pos = file->tellg();
         error = true;
         if (critical)
         {
            cout << endl << endl << "Section named '" << sectionName << "' not found";
            if (fileNme[0]!='\0')
               cout << " in file '" << fileNme << "'";
            Terminate();
         }
         return false;
		}
      if ((*(txt)=='[') && (*(txt+strlen(sectionName)+1)==']') && strstr(txt,sectionName))
      {
         cur_pos = file->tellg();
         return true;
      }
   }
#else
   string inputString;
   string paramString(sectionName);
   while(*file && (file->peek()!=EOF))
   {
	  getline(*file,inputString,'\n');
	  int endchar = inputString.find_first_of(']');
	  if ((endchar<0)) {
		  endchar=0;
	  }
	  inputString.assign(inputString.c_str(),endchar+1);
	  if (inputString.find("[end]",0)==0)
		{
		 cur_pos = file->tellg();
		 error = true;
		 if (critical)
		 {
			cout << endl << endl << "Section named '" << sectionName << "' not found";
			if (fileNme[0]!='\0')
			   cout << " in file '" << fileNme << "'";
			Terminate();
		 }
		 return false;
		}
//		cout << "vec " << inputString << endl;
      if ((inputString[0]=='[') && (inputString[inputString.length()-1]==']')
			&& (inputString.find(paramString,0)==1))
	  {
		 cur_pos = file->tellg();
		 return true;
	  }
   }
   error = true;
#endif
   if (critical)
   {
	  cout << endl << endl << "Section named '" << sectionName << "' not found";
	  if (fileNme[0]!='\0')
         cout << " in file '" << fileNme << "'";
      Terminate();
   }
   else
   	   if (file->peek()==EOF)
			file->clear();

   return false;
}

/****************************************************************************\
\****************************************************************************/
bool base::FindSection(const char * sectionName, int index)
{
   char s_section[81];
	s_section[0] = '\0';
   sprintf(s_section,"%s(%d)",sectionName,index);
   return FindSection(s_section);
}

/****************************************************************************\
 Gives the first index and the numbers of indexed parameters in the section
 in question
\****************************************************************************/
void base::GetVectorInfo(const char * parameter,int * firstNum,int * vectorSize)
{
   if (file->eof()>0 || (!(*file)))
	theMessage->FatalError("base::GetVectorInfo - previous attempt to read past end of file ",fileNme);

#ifdef __BCplusplus__
   char txt[MAX_TXT];
   char string[MAX_TXT];
   char buffer[MAX_TXT];
   txt[0] = '\0';
   int high  =-32000;
   *firstNum = 32000;
   file->seekg(cur_pos);
   while ((*file) && (file->peek()!=EOF) && (*(txt)!='['))
   {
      ReadLine(file,buffer,MAX_TXT);
#ifndef DEVCPP
      istrstream inputString(buffer,strlen(buffer));
#else
      stringstream inputString;
      inputString << buffer;
#endif
      inputString >> txt >> string >> ws;
      char * char_ptr = strchr(txt,'(');
      if (char_ptr && (*(txt+strlen(txt)-1)==')') && (*(txt+strlen(parameter))=='(') && strstr(txt,parameter))
      {
         char_ptr++;
         int nr = atoi(char_ptr);
         if (nr>high)
            high=nr;
         if (nr<(*firstNum))
            *firstNum=nr;
      }
   }
#else
   string inputString;
   string paramString(parameter);
   string vectorString;
   bool searching = true;
   int high  =-32000;
   *firstNum = 32000;
   file->seekg(cur_pos);
   while ((*file) && (file->peek()!=EOF) && (inputString[0]!='[') && searching)
   {
	  getline(*file,inputString,'\n');
	  if (inputString.find("[end]",0)<MAX_TXT)
		 searching=false;
	  else
	  {
	   int endchar = inputString.find_first_of(')');       //strip off any trailing characters
	   if ((endchar<0))
			  endchar=0;
	   vectorString.assign(inputString.c_str(),endchar+1);
	   if ((vectorString.find(paramString,0)<MAX_TXT))
	   {
		   if ((vectorString[paramString.length()]=='(') && (vectorString[vectorString.length()-1]==')'))
			{
				int i = paramString.length()+1;
				while ((vectorString[i]>='0') && (vectorString[i]<='9'))
				   i++;
				if (vectorString[i]==')')
				{
				   string numstring = vectorString.substr(paramString.length()+1,i-1);
				   int nr = atoi(numstring.c_str());
				   if(nr > high)
					  high = nr;
				   if(nr < *firstNum)
					  *firstNum = nr;
				}
			 }
	   }
	  }
   }
#endif
   *vectorSize=1+high-(*firstNum);
   if (*vectorSize<0)
   {
	  *vectorSize=0;
	  *firstNum = 0;
	  error = true;
	  if (critical)
	  {
		 cout << endl << endl << "Missing vector parameters";
		 if (fileNme[0]!='\0')
			cout << " in file '" << fileNme << "'";
		 Terminate();
	  }
	  else
		   file->clear();  //clears end of file marker if set.
   }
   else
	   file->clear();  //clears end of file marker if set.
}

/****************************************************************************\
\****************************************************************************/
string base::GetLongName()
{
   base * p=this;
   char s1[255];
	s1[0] = '\0';
   string s;
   while (p)
   {
      char s2[255];
      s2[0]='\0';
      strcat(s2,p->GetName());
      if (s2[0]!='\0')
      {
         sprintf(s1,"%s(%d)",s2,p->GetIndex());
         s="/"+(string)s1+s;
         p=(base*)p->Owner;
      }
      else
         p=NULL;
   }
   return s;
}

/****************************************************************************\
\****************************************************************************/
bool base::GetParameter(const char * parameter, char * p_string, int index)
{
/*    if (*file)
        cout << " file ok " << endl;
   if (file->peek()!=EOF)

    cout << " file->peek()!=EOF ok " << endl;
*/
#if __ANSICPP__
   if (file->eof()>0 || (!file->is_open()))
#else
   if (file->eof()>0 || (!(*file)))
#endif
   {
	  theMessage->FatalError("base::GetParameter - previous attempt to read past end of file ",fileNme);
   }
#ifdef __BCplusplus__
   char buffer[MAX_TXT];
   char txt[MAX_TXT];
   char string[MAX_TXT];
   char parm[MAX_TXT];
   if (index>-1)
      sprintf(parm,"%s(%d)",parameter,index);
   else
      strcpy(parm,parameter);
   txt[0] = '\0';
   file->seekg(cur_pos);
   while ((*file) && (file->peek()!=EOF) && (*(txt)!='['))
   {
      buffer[0] = '\0';
      txt[0]    = '\0';
      string[0] = '\0';
      ReadLine(file,buffer,MAX_TXT);
#ifndef DEVCPP
      istrstream inputString(buffer,strlen(buffer));
#else
      stringstream inputString;
      inputString << buffer;
#endif
      inputString >> txt >> string >> ws;
      if(!strcmp(parm,txt))
      {
         strcpy(p_string,string);
         return true;
      }
   }
#else
//not Borland C++ 5.02
   string inputString("  ");
   char parm[MAX_TXT];
   if (index>-1)
	  sprintf(parm,"%s(%d)",parameter,index);
   else
	  strcpy(parm,parameter);
   string paramString(parm);
   file->seekg(cur_pos);
   bool searching = true;
   while ((*file) && (file->peek()!=EOF) && (inputString[0]!='[') && searching)
   {
	  getline(*file,inputString);
	  if (inputString.find("[end]",0)==0)
		 searching=false;
	  else
	  {
//		  cout << " paramString " << paramString << " instring " << inputString << endl;
		  if(inputString.find(paramString)==0)
		  {
			   string string1, string2;
			   istringstream totalSString( inputString );
			   totalSString >> string1 >> string2;
//	cout << "s1 " <<string1 << " s2 " << string2 << endl;
			   strcpy(p_string,string2.c_str());
			 return true;
		  }
	  }
   }
#endif
//   if (*file)
//   cout << " file ok " << endl;
//   if (file->peek()!=EOF)
//   cout << " file->peek()!=EOF ok " << endl;
//   if	((inputString[0]!='['))
//   cout << " (inputString[0]!='[') ok " << endl;
   error = true;
   if (critical)
   {
		cout << endl << "Base::GetParameter - ";
	  if (fileNme[0]!='\0')
		 cout << "Error in file " << fileNme << " ";
#ifdef __BCplusplus__
      theMessage->FatalError(parm, " missing in a section ",GetName());
#else
	  theMessage->FatalError(paramString.c_str(), " missing in a section ",GetName());
#endif
	}
   else
   {
	   if (file->peek()==EOF)
			file->clear();
	   file->seekg(cur_pos);
   }
   return false;
}

/****************************************************************************\
\****************************************************************************/
bool base::GetParameter(const char * parameter, int * p_value, int index)
{
   char aCharArray[MAX_TXT] = "";
   bool val = GetParameter(parameter,aCharArray,index);
   if (val)
	  *p_value = atoi(aCharArray);
   return val;
}

/****************************************************************************\
\****************************************************************************/
bool base::GetParameter(const char * parameter, double * p_value, int index)
{
   char aCharArray[MAX_TXT] = "";
   bool val = GetParameter(parameter,aCharArray,index);
   if (val)
	  *p_value = atof(aCharArray);
   return val;
}

/****************************************************************************\
\****************************************************************************/
bool base::GetParameter(const char * parameter, bool * p_value, int index)
{
   char aCharArray[MAX_TXT] = "";
   bool val = GetParameter(parameter,aCharArray,index);
   if (val)
   {
	  int i=-1;

	  if(strcmp("0",aCharArray)==0)
		 i=0;
	  if(strcmp("FALSE",aCharArray)==0)
		 i=0;
	  if(strcmp("False",aCharArray)==0)
		 i=0;
	  if(strcmp("false",aCharArray)==0)
		 i=0;
	  if(strcmp("F",aCharArray)==0)
		 i=0;
	  if(strcmp("f",aCharArray)==0)
		 i=0;
	  if(strcmp("NO",aCharArray)==0)
		 i=0;
	  if(strcmp("No",aCharArray)==0)
		 i=0;
	  if(strcmp("no",aCharArray)==0)
		 i=0;
	  if(strcmp("N",aCharArray)==0)
		 i=0;
	  if(strcmp("n",aCharArray)==0)
		 i=0;
	  if(strcmp("1",aCharArray)==0)
		 i=1;
	  if(strcmp("TRUE",aCharArray)==0)
		 i=1;
	  if(strcmp("True",aCharArray)==0)
		 i=1;
	  if(strcmp("true",aCharArray)==0)
		 i=1;
	  if(strcmp("T",aCharArray)==0)
		 i=1;
	  if(strcmp("t",aCharArray)==0)
		 i=1;
	  if(strcmp("YES",aCharArray)==0)
		 i=1;
	  if(strcmp("Yes",aCharArray)==0)
		 i=1;
	  if(strcmp("yes",aCharArray)==0)
		 i=1;
	  if(strcmp("Y",aCharArray)==0)
		 i=1;
	  if(strcmp("y",aCharArray)==0)
		 i=1;

	  if (i<0)
	  {
		 cout << endl << endl << "Parameter not legal for bool type";
		 if (fileNme[0]!='\0')
			cout << " in file '" << fileNme << "'";
		 Terminate();
	  }
	  *p_value=(i==1);

   }
   return val;
}

/****************************************************************************\
\****************************************************************************/
bool base::GetParameter(const char * parameter, string * aString, int index)
{
   char aCharArray[MAX_TXT] = "";
   bool val = GetParameter(parameter,aCharArray,index);
   if (val)
   {
   	if (!aString)
      	theMessage->FatalError("base::GetParameter - no string has been constructed");
      aString->append(aCharArray);
   }
   else
   	delete aString;
   return val;
}

/****************************************************************************\
\****************************************************************************/
void base::SetSignPositive()
{
	sign = 1;
}

/****************************************************************************\
\****************************************************************************/
void base::SetSignNegative()
{
	sign = -1;
}


/****************************************************************************\
\****************************************************************************/
void base::UnsetSign()
{
	sign = 0;
}

/****************************************************************************\
\****************************************************************************/
bool base::CheckSign(double *aVal)
{
	bool ret_val = false;
   switch (sign)
   {
   	case 0:  ret_val = true;
      			break;
      case -1: if (!((*aVal==0) || (*aVal<0)))
		            theMessage->FatalError(GetName()," :Paramater must be negative");
               break;
      case 1: if (!((*aVal==0) || (*aVal>0)))
		            theMessage->FatalError(GetName()," :Paramater must be positive");
               break;
   }
   return ret_val;
}

/****************************************************************************\
\****************************************************************************/
bool base::CheckSign(int *aVal)
{
	bool ret_val = false;
   switch (sign)
   {
   	case 0:  ret_val = true;
      			break;
      case -1: if (!((sign<0) && (*aVal<0)))
		            theMessage->FatalError(GetName()," :Paramater must be negative");
               break;
      case 1: if (((sign>0) && (*aVal>0)))
		            theMessage->FatalError(GetName()," :Paramater must be positive");
               break;
   }
   return ret_val;
}

/****************************************************************************\
\****************************************************************************/
bool base::OpenOutputFile(char * fileName)
{
	if(!file)
      file = new fstream;
   else
   {
      cout << "Base: output file still open";
	   Terminate();
   }

   file->open(fileName, ios::out);
   if(!(*file)) return 0;
   else return 1;
}

/****************************************************************************\
\****************************************************************************/
void base::CloseOutputFile()
{
   if(file)
      file->close();
   fileNme[0] =  '\0';
   critical=false;
   error=false;
   cur_pos=0;
   file = NULL;
}

bool base::FileExists(string strFilename) {
#ifndef __BCplusplus__

  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }

  return(blnReturn);
    #else
    return false;
    #endif
}

bool base::GetParameter(const string * parameter, char   * p_string, int index)
{
	return GetParameter(parameter->c_str(), p_string, index);
};
bool base::GetParameter(const string *parameter, int    * p_value,  int index)
{
	return GetParameter(parameter->c_str(), p_value,  index);
};
bool base::GetParameter(const string * parameter, double * p_value,  int index)
{
	return GetParameter(parameter->c_str(), p_value,  index);
};
bool base::GetParameter(const string * parameter, bool   * p_value,  int index)
{
	return GetParameter(parameter->c_str(), p_value,  index);
};
bool base::GetParameter(const string * parameter, string * aString,  int index)
{
	return GetParameter(parameter->c_str(), aString,  index);
};

