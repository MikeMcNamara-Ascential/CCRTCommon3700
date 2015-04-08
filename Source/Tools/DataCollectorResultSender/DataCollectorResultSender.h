#ifndef DataCollectorSender_h
#define DataCollectorSender_h
//-------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <vector>
#include <string>
#include "XmlNode.h"
#include "XmlParser.h"
//-------------------------------------------------------------------------------------------------
class DataCollectorSender
{
	typedef enum MONTHS
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

public:
	DataCollectorSender();
	virtual ~DataCollectorSender(); 

	time_t ConvertTime(string month, int day, int year);
	void ReportResults(time_t &startDate, time_t &endDate);



private:
	int GetMonthNumber(string monthName);
	bool IsFileInDateRange(time_t &startDate, time_t &endDate, time_t &fileDate);
	void LoadResultFiles(time_t &startDate, time_t &endDate);

	vector<string> m_resultFiles;
	string m_resultPath;
	string m_reportPath;
};
//-------------------------------------------------------------------------------------------------
#endif //DataCollectorSender_h
