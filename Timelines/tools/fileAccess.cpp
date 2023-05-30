/*
 * fileAccess.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: JOVE
 */

#include "../tools/fileAccess.h"
#ifdef __BCplusplus__
#include <dir.h>
#endif
#ifdef BUILDER
	#include <dir.h>
#endif
#include <vector>
#include <string>
#ifndef VS2008
#include <dirent.h>
#else
#include <windows.h>
#include <conio.h>
#endif 
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <sys/stat.h>

#include "convertData.h"
#ifndef __BCplusplus__
	#ifndef BUILDER
		#ifndef VS2008
		#include <unistd.h>
#endif
	#endif
#endif


fileAccess::fileAccess()
{
}
void fileAccess::deleteEveryFileInFolder(string path)
{
#ifndef VS2008
	string oldPpath=getCurrentPath();
	changeDir(path);
	DIR           *d;
  struct dirent *dir;
  d = opendir(path.c_str());
  cout<<"deleting everything in" << path<<endl;
  if (d)
  {
	  dir = readdir(d);
	  while (dir!=NULL)
    {

    	string name=dir->d_name;

    	if(!name.compare(".svn")==0)
    	{

				if(!name.compare(".")==0)
				{

					if(!name.compare("..")==0)
					{
						remove(name.c_str( ));
					}
				}
       }
    	dir = readdir(d);
       }
   }

  changeDir(oldPpath);
#else
	int             iCount          = 0;
  std::string     strFilePath;          // Filepath
  std::string     strPattern;           // Pattern
  std::string     strExtension;         // Extension
  HANDLE          hFile;                // Handle to file
  WIN32_FIND_DATA FileInformation;      // File information


  strPattern = path + "\\*.*";
  hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    do
    {
		string name=FileInformation.cFileName;
		if(!name.compare(".svn")==0)
    	{
				
				if(!name.compare(".")==0)
				{
					cout<<"delete" <<name<<endl;
					if(!name.compare("..")==0)
					{
						char * cName=new char[200];
						strcpy(cName,path.c_str());
						strcat(cName,"\\");
						strcat(cName,name.c_str());
						cout<<cName<<endl;
						if( remove( cName) != 0 )
							cout<< "Error deleting file" <<endl;
						else
							cout<<"File successfully deleted" <<endl;
						delete cName;
					}
				}
       }
      
      
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);
  }
#endif
}
void fileAccess::openFile(string filename,bool seachback)
{

	char file[100];

	strcpy(file, filename.c_str());

	openFile(file, seachback);

}
char * filenames;
void fileAccess::openFile(char * filename,bool seachback)
{  
	filenames=filename;
	bool tmp=openFileWithoutExit(filename, seachback);

	if(!tmp)
	{

		char buffer[200];
		getcwd(buffer, 200);
		cout<<"Could not open file "<< filename << endl;
		cout << "In directory " << buffer <<endl;
		cout << "Press any key to terminate." << endl;
		cin.get();
		exit(1);
	}

}
bool fileAccess::openFileWithoutExit(string filename, bool seachback)
{
	char file[100];

	strcpy(file, filename.c_str());

	return openFileWithoutExit(file, seachback);
}
bool fileAccess::openFileWithoutExit(char * filename,bool seachback)
{
	string oldDir=getCurrentPath();
	while(true)
	{
		bool bad=true;
		bool fail=true;

		char * place=new char[400];
		strcpy(place,getCurrentPath().c_str());
#ifdef UNIX
		strcat(place,"/");
#else
		strcat(place,"\\");
#endif
		strcat(place,filename);

		files.open(place, fstream::in);
		if(files.bad())
		{
			bad= false;
		}
	    if(files.fail())
		{
	    	fail= false;
		}
	    if(bad==false || fail==false)
	    {
	    	if(seachback==true)
	    	{
				#ifdef UNIX
					if(getCurrentPath().compare("/")==0)
					{
											changeDir(oldDir);
											return false;
					}
				#endif
				#ifndef UNIX
					
					if(getCurrentPath().size()<=3)
					{
						changeDir(oldDir);
						return false;
					}
				#endif
					changeDir("..");
	    	}
	    	else
	    	{
	    		return false;
	    	}
	    }
	    else
	    {
	    	changeDir(oldDir);
	    	break;

	    }
	    delete place;
	}
   return true;
}
void fileAccess::openFileWrite(string filename){

	char file[100];

	strcpy(file, filename.c_str());

	openFileWrite(file);

}

void fileAccess::openFileWrite(char * filename)
{

	files.open(filename, fstream::out);
	files.precision(4);
	if(files.bad())
	{
			cout<<"could not open file "<< filename<<" and therefore halting"<<endl;
			exit(1);
	}
   if(files.fail())
		{
				cout<<"could not open file "<< filename<<" and therefore halting"<<endl;
				exit(1);
		}
}
void fileAccess::closeFile()
{
	files.close();
}

string fileAccess::getLineFile()
{
      char newline[4000]={'\0'};
      string line="NULL";
      if(!files.eof())
      {
		  files.getline (newline, 4000);
		  line=newline;
      }
      else {
    	  string tmp="NULL";
    	  return tmp;
      }
	#ifdef UNIX
      if(line[line.size() - 1] == '\r')
      {
    	  line.resize(line.size() - 1);
      }
      #endif
      //if a line in a text file is empty cygwin returns false
//      cout<<line<<endl;
      //cout<<" was just found"<<endl;
#ifdef CYGWIN
      while(line.size()<=1)
#else
      while(line.empty())
#endif
      {
    	  if(!files.eof())
    	  {
			  files.getline (newline, 255);
			  string string1(newline);
			  line=string1;
    	  }
    	  else{
    		  string tmp="NULL";
    		  return tmp;
    	  }
        #ifdef UNIX
    	  if(line[line.size() - 1] == '\r')
    	  {
    	     line.resize(line.size() - 1);
    	  }
        #endif
      }
      return line;
}
int fileAccess::changeDir(string dirName)
{
#ifndef VS2008
	char tmp[256];

	strcpy(tmp,dirName.c_str());

	int asci=tmp[dirName.size()-1];
	if(asci==13)
	{
		tmp[dirName.size()-1]='\0';
	}
	int ret_val=chdir(tmp);
	if (ret_val!=0)
	{
//		theMessage->FatalError("Directory not found")
		cout << tmp << endl;
		cout << "Directory not found"<<endl;
	}
	return ret_val;
#else
			char * tmp=new char[256];
			strcpy(tmp,dirName.c_str());
			int returnvalue=_chdir(tmp);
			return returnvalue ;
#endif
}
string fileAccess::getCurrentPath()
{
	string CurrentPath=" ";
    char path[FILENAME_MAX];

	getcwd(path,sizeof(path));
    CurrentPath=path;
    return CurrentPath;
}
void fileAccess::ignore2000()
{	char buffer[2000];
	files.getline(buffer,2000,'\n');
}

bool fileAccess::endOfFile()
{
	return files.eof();
}

void fileAccess::WriteS(string input)
{

	files<<input<<'\n';
}
void fileAccess::Write(double input)
{
	files<<input<<'\n';
}
void fileAccess::WriteNothing(string input)
{

	files<<input;
}
void fileAccess::WriteNothing(double input)
{

	files<<input;
}
void fileAccess::WriteWithTabS(string input)
{
	files<<input<<'\t';
}
void fileAccess::WriteWithTab(double input)
{

	files<<conv.DoubleToChar(input) << '\t';
}
void fileAccess::WriteNewLine()
{

	files<<endl;
}
bool fileAccess::NextSymbolEOF()
{
	if(files.peek()==EOF)
   	return true;
   else return false;

}

double fileAccess::GetDoubleFromFile()
{
	double val;
	files >> val;
	getLineFile();
	return val;
}

int fileAccess::GetIntFromFile()
{
	int val;
	files >> val;
	getLineFile();
	cout<<val<<endl;
	return val;
}

//added by NJH - needs checking by Jonas

int fileAccess::CreateDirectory(string directoryName)
{
	int retVal;
#ifdef __WINDOWS__
	#ifndef __BCplusplus__
		retVal=mkdir(directoryName.c_str(),'-p');
	#else
		retVal=mkdir(directoryName.c_str());
	#endif
	#else
		retVal=mkdir(directoryName.c_str(),0777);
#endif
		if (retVal==-1)
			cout << "Unable to create directory " << directoryName << endl;
	return retVal;
}


