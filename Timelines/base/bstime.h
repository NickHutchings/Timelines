#ifndef BSTIME_H
   #define BSTIME_H
#include "common.h"

#ifndef __BCplusplus__
using namespace std;
#endif

enum monthSymbol
{
   January,
   February,
   March,
   April,
   May,
   June,
   July,
   August,
   September,
   October,
   November,
   December
};
//!Manages the date information
class bsTime
{
   private:
	//!Day
      int  day;
      //!Month
      int  month;
      //!Year
      int  year;
      //!Julian Day number
      int  dayInYear;
      //!Day number in week
      int  dayInWeek;
      //!Day number relative to base year
      long longTime;
      //!Days in each month for normal and leap years
      int  tabDaysPerMonth[2][12];
      //!Number of days elapsed since 1900
      long AccumulatedDaysSince1900[500];
      //!Initialisation function
      void InitDaysPerMonth(void);

   public:
      //!Default constructor
      bsTime();
      //!Constructor with arguments
      /*!
      \param d day
      \param m month
      \param y year
       */
      bsTime(int d, int m, int y);
      //!constructor with arguments
      /*!
      \param f pointer to an instance of bsTime
      */
	  bsTime(const bsTime& f);
	  //!Set the time using numerical parameters
      /*!
      \param d day
      \param m month
      \param y year
       */
      void SetTime(int d, int m, int y);
      //!set the time using 3 string parameters
      /*!
       \param day Day
       \param month month
       \param year year
       */
      void SetTime(string day,string month, string year);
      //!Set the time using single string parameter
      /*!
       \param aTimeString the date as a string with format dd/mm/yyyy or dd-mm-yyyy
       */
	  void SetTime(string aTimeString);
	  //!Set the time using two numerical values
	  /*!
	   \param Year the year
	   \param JulianDay the Julian Day
	   */
		void SetTime(int Year, int JulianDay);
		//!Returns the year
		int GetYear() {return year;};
		//!Returns the month
	  int GetMonth() {return month;};
	  //!Returns the day
	  int GetDay() {return day;};
	  //!Returns the day in the year
	  int GetDayInYear() {return dayInYear;};
	  //!Prints the time to screen
	  void PrintTime(void);
	  //!At one day to the date
	  void AddOneDay(void);
	  //!Add one year to the date
	  void AddOneYear(void);
	  //!Return the name of the day in the week
	  char * WeekDay(void);
	  //!Return a pointer to the current date
	  bsTime * CurrentDay(void);
	  //!Return the date has a string with format dd/mm/yyyy
	  string GetString();
	  //!Return the date has a string with format dd-mm-yyyy
      string GetString2();
      //!Calculate the number of days between the current date and 1.1.1900
      void CalculateDateValue();
      //!Return the number of days between the current date and 1.1.1900
      long GetDateValue() {return longTime;};
      //!Returns true if the current date is the first day of the given month
      /*!
       \param monthNr number of month
       */
      bool FirstDay(int monthNr);
      //!Returns true if the current date is the first day of the current month
      bool FirstDayInMonth(void);
      //!Returns true if the current date is the last day of the current month
      bool LastDayInMonth(void);
      //!Returns true if the year is a leap year
      /*!
       \param  year number of year
       */
      bool IsLeapYear(int year);
      //!Returns the number of days in the year
      /*!
       \param  offset year number relative to the current year
       */
	  int daysInYear(int offset);
      bool operator == (bsTime time2);
      bool operator < (bsTime time2);
      bool operator <=(bsTime time2);
		bool operator > (bsTime time2);
		bool operator >= (bsTime time2);
		int operator - (bsTime time2);
      friend ostream& operator<< (ostream& os, const bsTime& aTime);
      //!At a given number of days to the date
      /*!
      \param nrOfDays number of days to add
       */
      void AddDays(int nrOfDays);
      //!Return the Julian Day for a given date
      /*!
      \param aday Day number
      \param amonth month number
      \param ayear year number
       */
		int GetJulianDay(int aday, int amonth, int ayear);
		//!Return the Julian Day number for the current state
      int GetJulianDay();
      //!Calculate the date that corresponds to a given Julian Day number
      /*!
      \param aJulianday the Julian Day for which the date is required
       */
		bool CalcDateFromJulianDay(int ayear, int aJulianday);
		//!Clones an instance of the class
      bsTime* clone() const;
};

extern bsTime theTime;
extern bsTime prepStopTime;
extern bsTime yearStopTime;
extern bsTime stopTime;

#endif

