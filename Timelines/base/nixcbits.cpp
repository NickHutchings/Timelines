#include "nixcbits.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "common.h"
using namespace std;
const double StephanBoltzmann =5.67E-8;
const double vonKarman = 0.41;
const double PI = 3.1415926;

//#pragma package(smart_init)
double GetvonKarman() {return vonKarman;};

double const N_to_protein()
{
  return 6.25;
}
double const GetstandardAirPressure()
{
   return 101325; //Pa
}
double const GetgravitationalAcceleration()
{
   return 9.81;
}
double const GetdynamicViscosityAir()
{
   return 1.72*1E-5;
}
double const GetGasConstant()
{
   return 8.31;   //K-1 mol-1
}
double const GetCconcInOrgmatter()
{
   return 0.52;
}

double GetFrictionVelocity(double height, double displacementHt, double zeroPlane, double windVelocity)
{
	double retVal=0.0;
   retVal=(windVelocity*vonKarman)/(log((height-displacementHt)/zeroPlane));
	return retVal;
}
void swapint(int& a , int& b)
{
   int t;
   t = a;
   a = b;
   b = t;
}

void swapdoub(double& a , double& b)
{
   double t;
   t = a;
   a = b;
   b = t;
}

void swappoint(void * &a, void * &b)
{
 void *t;
   t = a;
   a = b;
   b = t;
}

int partition(temp_double_array a, temp_int_array b, int left, int right)
//    { nested function inherits definitions of }
//    { "left" and "right" from enclosing procedure }
{
   double v ;            //{ pivot }
   int l, r ;
   v = a[right];  //{ select pivot }
   l = left;
   r = right-1;
   do
   {
		if ((a[l] < v)){do {l=l+1;}  while (a[l] < v);}
    	if ((a[r] >= v)&& (l < r)) {do {r = r-1;}  while ((a[r] >= v)&& (l < r));}
    	swapdoub(a[l],a[r]);
    	swapint(b[l],b[r]);
//    swap(a[l], a[r]);
   } while (l<r);
   swapdoub(a[l], a[r]);      // { undo that final swap }
   swapdoub(a[l], a[right]);
   swapint(b[l], b[r]);       //{ undo that final swap }
   swapint(b[l], b[right]);
   return  l;
}

void quicksort2(temp_double_array a, temp_int_array b, int left, int right)
{
   int p;
   if (left < right)
   {
   	p = partition(a,b, left, right);
    	quicksort2(a,b,left,p-1);
    	quicksort2(a,b,p+1,right);
   }
};

// sort with pointer as passenger
int partition(temp_double_array a, temp_ptr_array b, int left, int right)
//    { nested function inherits definitions of }
//    { "left" and "right" from enclosing procedure }
{
   double v ;            //{ pivot }
   int l, r ;
   v = a[right];  //{ select pivot }
   l = left;
   r = right-1;
   do
   {
    	if ((a[l] < v)){do {l=l+1;}  while (a[l] < v);}
    	if ((a[r] >= v)&& (l < r)) {do {r = r-1;}  while ((a[r] >= v)&& (l < r));}
    	swapdoub(a[l],a[r]);
    	swappoint(b[l],b[r]);
//    swap(a[l], a[r]);
   } while (l<r);
   swapdoub(a[l], a[r]);      // { undo that final swap }
   swapdoub(a[l], a[right]);
   swappoint(b[l], b[r]);       //{ undo that final swap }
   swappoint(b[l], b[right]);
   return  l;
}

void quicksort2(temp_double_array a, temp_ptr_array b, int left, int right)
{
   int p;
   if (left < right)
   {
    	p = partition(a,b, left, right);
    	quicksort2(a,b,left,p-1);
    	quicksort2(a,b,p+1,right);
   }
}

/****************************************************************************\
Given arrays x[n] and y[n] containing ordered function tabulations, this
rutine returns an array y2[n] that contains the second derivatives of the
interpolating functions at the tabulated points.
\****************************************************************************/
void Spline(double * x, double * y, int n, double * y2)
{
   int i,k;
   double p, sig;
   double u[100];
   y2[0] = 0.0;
   u[0] = 0.0;
   for (i=1;i<n-1;i++)
   {
      sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
      p = sig*y2[i-1]+2.0;
      y2[i] = (sig-1.0)/p;
      u[i] = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
      u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
   }
   y2[n-1] = 0;
   for (k=n-2;k>=0;k--)
      y2[k] = y2[k]*y2[k+1]+u[k];
}

/****************************************************************************\
Given the arrays xa[n] and ya[n], containing ordered function tabulations,
and given the array y2a[n], which is the output of from the Spline() routine,
this routine returns a cubic-spline interpolated value.
\****************************************************************************/
double Splint(double * xa, double * ya, double * y2a, int n, double x)
{
   int klo, khi, k;
   double h, b, a;
   klo = 0;
   khi = n-1;
   while (khi-klo > 1)
   {
      k = (khi+klo)/2;
      if (xa[k] > x)
         khi = k;
      else
         klo = k;
   }
   h = xa[khi]-xa[klo];
   a = (xa[khi]-x)/h;
   b = (x-xa[klo])/h;
   return a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

/****************************************************************************\
Linear Interpolation of ordered x data
\****************************************************************************/
double LinearInterpolation(double *x, double *y,int n, double x0)
{
	int i=0;
   while (i<n && x[i]<x0)
   	i++;

   if (i==n) i--;
   if (i==0)
   	return y[0]+(y[1]-y[0])/(x[1]-x[0])*(x0-x[0]);
   else
   	return y[i-1]+(y[i]-y[i-1])/(x[i]-x[i-1])*(x0-x[i-1]);
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  CUBIC.C - Solve a cubic polynomial
**  public domain by Ross Cottrell
*/


void SolveCubic(double  a,
                double  b,
                double  c,
                double  d,
                int    *solutions,
                double *x)
{
      long double    a1 = b/a, a2 = c/a, a3 = d/a;
      long double    Q = (a1*a1 - 3.0*a2)/9.0;
      long double R = (2.0*a1*a1*a1 - 9.0*a1*a2 + 27.0*a3)/54.0;
      double    R2_Q3 = R*R - Q*Q*Q;

      double    theta;

      if (R2_Q3 <= 0)
      {
            *solutions = 3;
            theta = acos(R/sqrt(Q*Q*Q));
            x[0] = -2.0*sqrt(Q)*cos(theta/3.0) - a1/3.0;
            x[1] = -2.0*sqrt(Q)*cos((theta+2.0*PI)/3.0) - a1/3.0;
            x[2] = -2.0*sqrt(Q)*cos((theta+4.0*PI)/3.0) - a1/3.0;
      }
      else
      {
            *solutions = 1;
            x[0] = pow(sqrt(R2_Q3)+fabs(R), 1/3.0);
            x[0] += Q/x[0];
            x[0] *= (R < 0.0) ? 1 : -1;
            x[0] -= a1/3.0;
      }
}


double GetAirPressure(double altitude, double temperature) //Pa    //Does not work!!!!
{
	double ret_val = 101325*exp(GetgravitationalAcceleration()*altitude
   					/(GetGasConstant() * (temperature+273.0)));
	return ret_val;
}

double GetspecificHeatCapAir() {return 1200.0;}; //J per K per kg - specific heat capacity of dry air

double GetdensityDryAir(double airPressure, double temperature) // air pressure in Pa, temperature in K
{
	temperature-=273.15;
   if (temperature<-100.0)
    	cout << "Air temperature probably in Celsius not Kelvin" << endl;
   double ret_val=1000.0 * airPressure/(287.05*(temperature+273.15));
	return ret_val;
}; //kg per cubic meter - density of air

double GetdensityAir(double airPressure, double temperature, double vapourPressure)
{
	airPressure/=1000.0;
   vapourPressure/=1000.0;
	double ret_val = GetdensityDryAir(airPressure,temperature)
   		*(1-(vapourPressure*(1-0.622)/airPressure));
	return ret_val;
}; //kg per cubic meter - density of air

double GetsaturatedWaterVapourPressure(double airTemperature)  //returns value in Pa
{
 airTemperature-=273.3;
 if (airTemperature<-100.0)
 	cout << "Air temperature probably in Celsius not Kelvin" << endl;
 double ret_val;
 if (airTemperature>0.0)
  	ret_val = 1000.0 * 0.61078*exp((17.269*airTemperature)/(airTemperature+237.3));
 else
	ret_val = 1000.0 * 0.61078*exp((21.87456 * airTemperature)/(airTemperature + 265.5));

 return ret_val;
}

double GetlongWaveDown(double airTemp)
{
 double ret_val = -119.0 + 1.06*StephanBoltzmann*pow(airTemp,4);
 return ret_val;
}

double  GetlongWaveUp(double emissivity, double aTemperature)
{
  double ret_val = emissivity*StephanBoltzmann*pow(aTemperature,4);
  return ret_val;
}

double GetKHenry(double temperature)
{
 return pow(10,(-1.69+1447.7/(273.15+temperature)));
}

double GetDissociationConst(double temperature)
{
 return exp(-177.95292-(1843.22/(273+temperature))+(31.4335*log(273+temperature))-0.0544943*(273+temperature));
}

// astronomical daylength - latitude in degrees, dayNo counted from Jan 1
double GetdayLength(double latitude, int dayNo)
{
 double sinLD=sin(GetDeclination(dayNo))* sin(latitude*PI/180);
 double cosLD =cos(GetDeclination(dayNo))* cos(latitude*PI/180);
 double length = 12 + 24*asin(sinLD/cosLD)/PI;
 return length;
}

// latitude in degrees, dayNo counted from Jan 1, offset is difference in hours between local time and astronomical time
double GettimeFromDarkness(double latitude, int dayNo, int hour, int mins, double offset)
{
 double ret_val;
 double time = hour + mins/60 - offset;
 double daylength = GetdayLength(latitude, dayNo);
 double diff = fabs(time - 12);
/* if (diff>=daylength/2)
 	ret_val=0.0;
 else*/
 	ret_val = (daylength/2) - diff;
 return ret_val;
}

double GettimeFromNoon(double latitude, int dayNo, int hour, int mins, double offset)
{
 double timeFromDarkness =	GettimeFromDarkness(latitude, dayNo, hour, mins, offset);
 double daylength = GetdayLength(latitude, dayNo);
 double timeFromNoon = (daylength/2)-timeFromDarkness;
 return timeFromNoon;
}

// solar declination in radians
double GetDeclination(int dayNo)
{
 double declination = -asin(sin(23.45*PI/180)*cos(2*PI*(dayNo+10)/365));
// if (declination<0.0) declination = 0.0;
 return declination;
}

double GetzenithAngel(double declination, double latitude, double timeToNoon)
{
 double theta = 2*PI*timeToNoon/24; //radians
 double zenithAngle= acos(sin(latitude/(180.0))*sin(declination) +
                       cos(latitude/180.0)*cos(declination)*cos(theta));
 return zenithAngle;
}

double GetMaxSolar(double latitude, int dayNo, int hour, int mins, double offset)
{
 double ret_val=0.0;
 if (GettimeFromDarkness(latitude, dayNo, hour, mins, offset)<=0.0)
   return ret_val;
 else
 {
    double solarConst=1370.0;
    double solarConstToday =  solarConst*(1+0.033*cos(2*PI*dayNo/365));
    double declination = GetDeclination(dayNo);
    double timeFromNoon = GettimeFromNoon(latitude, dayNo, hour, mins, offset);
    double zenithAngle = GetzenithAngel(declination, latitude, timeFromNoon);
    double ret_val = 0.75 * solarConstToday * cos(zenithAngle);
    return ret_val;
 }
}

/**********************************************
Returns root of a quadratic
**********************************************/
bool SolveQuadratic(bool posRoot, double a, double b, double c, double *x)
{
   if ((b*b-4*a*c)<0)
   {
   	return false;
   }
	else if (posRoot)
	   *x = (-b+sqrt(b*b-4*a*c))/(2*a);
   else
   	*x = (-b-sqrt(b*b-4*a*c))/(2*a);
   return true;
};

// in kJ per kg
double GetLatentHeatVaporisationWater(double temperature)
{
	double ret_val=2560.75-2.43557* temperature;
	return ret_val;
}

double GetSpecificHumidity(double airPressure, double waterVapourPressure)
{
	double ret_val=0.622*waterVapourPressure/(airPressure-waterVapourPressure);
	return ret_val;
}

int newGap(int gap)
{
  gap = (gap * 10) / 13;
  if (gap == 9 || gap == 10)
    gap = 11;
  if (gap < 1)
    gap = 1;
  return gap;
}

void Comsort(temp_double_array double_array, temp_ptr_array ptr_array, int Hi)
{
   int gap = Hi;
   while (true)
   {
      gap = newGap(gap);
      bool swapped=false;
      for (int k = 0; k < Hi-gap; k++)
      {
         int j = k + gap;
         if (double_array[k] > double_array[j])
         {
            double a=double_array[k];
            double_array[k]=double_array[j];
            double_array[j]=a;
            void * p=ptr_array[k];
            ptr_array[k]=ptr_array[j];
            ptr_array[j]=p;
            swapped = true;
         }
      }
      if (gap == 1 && !swapped)
         break;
   }
}

/*****************************
Guess at FE from organic N content in DM (from values in feed table)
*****************************/
double N_to_FE(double orgNcontent)
{
	double ret_val;
   ret_val = 0.6 + 1000.0 * orgNcontent * N_to_protein() * 0.001583;
   if (ret_val>1.1)
   	ret_val=1.1;
   return ret_val;
}


double GetWindAtHeight(double origHt, double origWind, double newHt, double roughnessLength)
{
	return origWind * log(newHt/roughnessLength)/log(origHt/roughnessLength);
};

double GetGrossEnergyinDryMatter(){return 18.4;}; //MJ per kg



/*
// root of linear equation c1 * x + c0 = 0
// if successful, returns ret.value[0] = x
void calc_root_of_linear(retval & ret, double c1, double c0)
{
  if (c1 == 0)
    ret.num_val = 0;
  else
  {
    ret.num_val = 1;
    *(ret.value) = -c0/c1;
  }
}

// distinct real roots x1, x2 of quadratic equation:
//   c2 * x^2 + c1 * x + c0 = 0
// if successful, returns ret.value[0] = x1 & ret.value[1] = x2
// if roots are equal, ret.value[0] = root
void calc_roots_of_quadratic(retval & ret, double c2, double c1, double c0)
{
  double d;

  if (c2 == 0)
    calc_root_of_linear(ret, c1, c0);
  else
  {
    d = c1 * c1 - 4 * c2 * c0;

    if (d == 0)
    {
      ret.num_val = 1;
      *(ret.value) = -c1/( 2 * c2);
    }
    else
    {
      if (d > 0)
      {
        ret.num_val = 2;
        *(ret.value) = (-c1 + sqrt(d))/( 2 * c2);
        ret.value[1] = (-c1 - sqrt(d))/( 2 * c2);
      }
      else
        ret.num_val = 0;
    }
  }
}

// real roots of cubic equation:
//   c3 * x^3 + c2 * x^2 + c1 + c0 = 0
// if successful, returns real roots in ret.value[]
void calc_roots_of_cubic(retval & ret, double c3, double c2, double c1, double c0)
{
  static retval quad_rts;
  double k, p, q, w, cos3a, t, p_d3_e3, a;
  int i;

  if (c3 == 0)
    calc_roots_of_quadratic(ret, c2, c1, c0);
  else
  {
    if (c2 == 0 && c1 == 0)
    {
      ret.num_val = 1;
      *(ret.value) = pow((-c0/c3),-3);
    }
    else
    {
      if (c0 == 0)
      {
        ret.num_val = 1;
        *(ret.value) = 0;
        calc_roots_of_quadratic(quad_rts, c3, c2, c1);
        for (i = 0; i < quad_rts.num_val; i++)
          if (quad_rts.value[i] != 0)
            ret.value[ret.num_val++] = quad_rts.value[i];
      }
      else
      {
        if (c2 != 0)
        {
          k = -c2/( 3 * c3);
          p = (3 * c3 * c1 - c2 * c2)/(-3 * c3 * c3);
          q = (2 * c2 * c2 * c2 - 9 * c3 * c2 * c1 + 27 * c3 * c3 * c0)/( -27 * c3 * c3 * c3);
        }
        else
        {
          k = 0;
          p = -c1/ c3;
          q = -c0/ c3;
        }

        p_d3_e3 = p * p / 27 * p;
        w = q / 4 * q - p_d3_e3;

        if (w < 0)
        {
          cos3a = q/( 2 * sqrt(p_d3_e3));
          a = acos(cos3a) / 3;
          t = sqrt(p * 1.33333333333333);
          ret.num_val = 3;
          *(ret.value) = t * cos(a) + k;
          ret.value[1] = t * cos(a + RAD_120) + k;
          ret.value[2] = t * cos(a + RAD_240) + k;
        }
        else
        {
          if (w == 0)
          {
            ret.num_val = 1;
            *(ret.value) = 2 * cbrt(q / 2) + k;
          }
          else
          {
            ret.num_val = 1;
            *(ret.value) = cbrt(q / 2 + sqrt(w)) + cbrt(q / 2 - sqrt(w)) + k;
          }
        }
      }
    }
  }
}

// real roots of quartic equation:
//   c4 * x^4 + c3 * x^3 + c2 * x^2 + c1 + c0 = 0
// if successful, returns real roots in ret.value[]
void calc_roots_of_quartic(retval & ret, double c4, double c3, double c2, double c1, double c0)
{
  static retval subpoly_rts[2];
  double m, b, a3, a2, a1, a0;
  int i, j;

  if (c4 == 0)
    calc_roots_of_cubic(ret, c3, c2, c1, c0);
  else
  {
    if (c3 == 0 && c2 == 0 && c1 == 0)
      calc_fourth_root(ret, fdiv(-c0, c4));
    else
    {
      if (c0 == 0)
      {
        ret.num_val = 1;
        *(ret.value) = 0;
        calc_roots_of_cubic(subpoly_rts[0], c4, c3, c2, c1);
        for (i = 0; i < subpoly_rts[0].num_val; i++)
          if (subpoly_rts[0].value[i] != 0)
            ret.value[ret.num_val++] = subpoly_rts[0].value[i];
      }
      else
      {
        a3 = fdiv(c3, c4);
        a2 = fdiv(c2, c4);
        a1 = fdiv(c1, c4);
        a0 = fdiv(c0, c4);

        retval cb_rts;
        double hi_root;

        calc_roots_of_cubic(cb_rts, -8, 4 * a2, 8 * a0 - 2 * a1 * a3, a1 * a1 - a0 * (4 * a2 - a3 * a3));

        hi_root = cb_rts.value[0];

        if (cb_rts.num_val == 2)
        {
          if (cb_rts.value[1] > hi_root)
            hi_root = cb_rts.value[1];
        }
        else if (cb_rts.num_val == 3)
        {
          if (cb_rts.value[1] > cb_rts.value[2])
          {
            if (cb_rts.value[1] > hi_root)
              hi_root = cb_rts.value[1];
          }
          else
          {
            if (cb_rts.value[2] > hi_root)
              hi_root = cb_rts.value[2];
          }
        }

        ret.num_val = 0;

        if ((a2 = hi_root * hi_root) > a0)
        {
          b = sqrt(a2 - a0);
          m = fdiv(a3 * hi_root - a1, 2 * b);

          calc_roots_of_quadratic(subpoly_rts[0], 1, a3 / 2 + m, hi_root + b);
          calc_roots_of_quadratic(subpoly_rts[1], 1, a3 / 2 - m, hi_root - b);

          for (i = 0; i < 2; i++)
            for (j = 0; j < subpoly_rts[i].num_val; j++)
              ret.value[ret.num_val++] = subpoly_rts[i].value[j];
        }
      }
    }
  }
}
*/
