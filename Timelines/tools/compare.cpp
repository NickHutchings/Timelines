/*
 * compare.cpp
 *
 *  Created on: 11/05/2010
 *      Author: beer
 */

#include <math.h>
#include "compare.h"
 bool doubleEquals(double left, double right, double epsilon) {
   return (fabs(left - right) < epsilon);
 }

 bool doubleLess(double left, double right, double epsilon, bool orequal) {
   if (fabs(left - right) < epsilon) {
     return (orequal);
   }
   return (left < right);
 }

 bool doubleGreater(double left, double right, double epsilon, bool orequal) {
   if (fabs(left - right) < epsilon) {
     return (orequal);
   }
   return (left > right);
 }
