#ifndef __ANSICPP__
   #include <windows.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <iostream>
   #include <iomanip>
   #include <fstream>
   #include <conio.h>
   #include <dir.h>
   #include <math.h>
   #include <assert.h>
#include <stdio.h>
#include <string>

#else
   #define MAXPATH 1024
	#include <cstdlib>
	#include <stdlib.h>
   #include <cstring>
   #include <string>
   #include <iostream>
   #include <iomanip>
   #include <fstream>
   #include "unix_util.h"
	#include <sstream>

#endif // __ANSICPP__
#ifdef __BCplusplus__
#include <string.h>
#endif
#ifdef __ANSICPP__
using namespace std;
#endif
