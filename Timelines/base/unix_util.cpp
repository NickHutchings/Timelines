/****************************************************************************\
 $URL$
 $LastChangedDate$
 $LastChangedRevision$
 $LastChangedBy$
\****************************************************************************/

#include <string.h>
#include <ctype.h>
#include "unix_util.h"



void reverse( char* s )
{
  int c, i, j;

  for ( i=0, j=strlen(s)-1; i<j; i++, j-- ) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}



void itoa( int n, char* s, int maxcount )
{
  int i=0, sign;

  if ((sign = n) < 0 )
    n = -n;

  // Make room for 'end-of-string' marker.
  maxcount--;

  if ( sign<0 )
    // Make room for sign char.
    maxcount--;

  while ( maxcount-- > 0 ) {
    s[i++] = n % 10 + '0';
    if ( (n /= 10) <= 0 )
      break;
  }

  if ( sign<0 )
    s[i++] = '-';

  s[i] = '\0';
  reverse(s);
}
/*
 *
 */
string getLineFile(ifstream *files)
{

      char newline[1000];
      string line;
      if(!files->eof())
      {
		  files->getline (newline, 1000);
		  line=newline;
      }
      else {
    	  string tmp="NULL2";
    	  return tmp;
      }
	#ifdef UNIX
      if(line[line.size() - 1] == '\r')
      {
    	  line.resize(line.size() - 1);
      }
      #endif
      //if a line in a text file is empty cygwin returns false
#ifdef CYGWIN
      while(line.size()<=1)
#else
      while(line.empty())
#endif
      {
    	  if(!files->eof())
    	  {
			  files->getline (newline, 255);
			  string string1(newline);
			  line=string1;

    	  }
    	  else{
    		  string tmp="NULL1";
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
