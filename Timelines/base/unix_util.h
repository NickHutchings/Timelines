/****************************************************************************\
 $URL$
 $LastChangedDate$
 $LastChangedRevision$
 $LastChangedBy$
\****************************************************************************/

#ifndef UNIX_UTIL_H
#define UNIX_UTIL_H

#include <fstream>
#include <string>
using namespace std;

//utility functions - avoids having to rely on compiler having them
extern void reverse( char* s );
extern void itoa( int n, char* s, int maxcount );
string getLineFile(ifstream *files);
#endif // UNIX_UTIL_H
