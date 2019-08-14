// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

#include<iostream>
#include<fstream>
#include<deque>
#include<string.h>

using namespace std;

#ifdef _MSC_VER
inline int strcasecmp(const char *s1, const char *s2) {
	return _stricmp(s1, s2);
}
#endif // _MSC_VER

struct LogEntry {
	string timeStampStr;//either store it in logentry, or convert it everytime 
	string cat;
	string message;
	long long int timeStamp;
	int EntryID;
};


struct sortMasterLog {
	bool operator() (const LogEntry &left, const LogEntry &right) const {
		if (left.timeStamp != right.timeStamp) {
			return left.timeStamp < right.timeStamp;
		}
		else if ((strcasecmp(left.cat.c_str(), right.cat.c_str()))) {//figure out another way without having to use c_str(). extremely slow
			return left.cat < right.cat;
		}
		return left.EntryID < right.EntryID;
	}
};

struct compareTSLOWER {
	bool operator() (const LogEntry &entry, long long int time) const {
		if (entry.timeStamp < time) {
			return true;
		}
		return false;
	}
};

struct compareTSUPPER {//IS IT GREATER OR GREATER THAN EQUAL
	bool operator() (long long int time,const LogEntry &entry) const {
		if (time < entry.timeStamp) {
			return true;
		}
		return false;
	}
};