#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"
using namespace std;
//!Manages output to the screen and to log files etc
class message
{
private:
	static ofstream * fw;
	static ofstream * fl;
	static ofstream * fd; //debug text file
	static char * WarnFileName;
	static char * LogFileName;
	static char * DebugFileName;
	static int Warnings;
	static int Copies;
	bool showTime;
	message& operator=(const message& m); // Dissable the compilers generation of default assignment operator.
	message(const message& m);            // Dissable the compilers generation of default copy constructor.

	char * AssignString(char * c);

public:
	message();
	~message();
	bool InitMessage(char * WarnFN,char * LogFN, char *DebugFN, bool showTheTime = true);
	void ChangeLogfilename(char * cp);
	void Warning(const char * cp1,const char * cp2="",const char * cp3="");
	void Warning(const string st1, const string st2="", const string st3="");
	void Warning(const string st1, int anint);
	void WarningWithDisplay(const string st1, const int anint);
	void WarningWithDisplay(const string st1, const string st2, const string st3);
	void Warning(const char * cp1, const double aNumber, const char * cp3="");
	void WarningWithDisplay(const char * cp1,const char * cp2="",const char * cp3="");
	void WarningWithDisplay(const char * cp1, const double aNumber, const char * cp3="");
	void WarningWithDisplay(const char * cp1, const int aNumber, const char * cp3="");
	void FatalError(const char * cp1,int anInt,const char * cp2);
	void FatalError(const char * cp1,const char * cp2="",const char * cp3="");
	void LogEvent(const char * cp1);
	void LogEvent(const char * cp, const double number);
	void LogEvent(const char * cp, const int number);
	int NumOfWarnings();
	ofstream * GiveHandle();
	void CloseHandle();
	ofstream * GiveDebugHandle();
	void CloseDebugHandle();
	void LogEvent(const char * cp1, int anint, const char *cp2, double number);
	void LogEventWithTime(const char * cp1, int anint, const char *cp2, double number);
	void LogEventWithTime(const char * cp1);
	void LogEventWithTime(const char * cp, const double number);
};

extern message * theMessage;

#endif
