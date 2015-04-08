#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <errno.h>
#include <unistd.h>
#include "DataCollectorResultSender.h"


//------------------------------------------------------------------------------
DataCollectorSender::DataCollectorSender()
{
	m_reportPath = "/TestResults/Vehicle/";
	m_resultPath = getenv("FTP_ROOT");
}

//------------------------------------------------------------------------------
DataCollectorSender::~DataCollectorSender()
{
	m_resultFiles.clear();
}

//------------------------------------------------------------------------------
time_t DataCollectorSender::ConvertTime(string month, int day, int year)
{
	struct tm rawTime;
	rawTime.tm_sec = 0;
	rawTime.tm_min = 0;
	rawTime.tm_hour = 0;
	rawTime.tm_mday = day;
	rawTime.tm_mon = GetMonthNumber(month);
	rawTime.tm_year = year - 1900;
	return mktime(&rawTime);
}

//------------------------------------------------------------------------------
void DataCollectorSender::ReportResults(time_t &startDate, time_t &endDate)
{
	LoadResultFiles(startDate, endDate);
	string baseCommand = "SendCmd -ncore/PlantHostOutbound -tPublish -xTestResult,";
	for(int index = 0; index < m_resultFiles.size(); index++)
	{
		string command = baseCommand + m_resultFiles[index];
		cout << "Reporting: " << command << endl;
		system(command.c_str());
		delay(1000);
	}
}

//------------------------------------------------------------------------------
int DataCollectorSender::GetMonthNumber(string monthName)
{
	int month = 0;
	if((stricmp(monthName.c_str(), "january") == 0) || 
	   (stricmp(monthName.c_str(), "jan") == 0))
	{
		month = January;
	}
	else if((stricmp(monthName.c_str(), "february") == 0) || 
			(stricmp(monthName.c_str(), "feb") == 0))
	{
		month = February;
	}
	else if((stricmp(monthName.c_str(), "march") == 0) || 
			(stricmp(monthName.c_str(), "mar") == 0))
	{
		month = March;
	}
	else if((stricmp(monthName.c_str(), "april") == 0) || 
			(stricmp(monthName.c_str(), "apr") == 0))
	{
		month = April;
	}
	else if(stricmp(monthName.c_str(), "may") == 0)
	{
		month = May;
	}
	else if((stricmp(monthName.c_str(), "june") == 0) || 
			(stricmp(monthName.c_str(), "jun") == 0))
	{
		month = June;
	}
	else if((stricmp(monthName.c_str(), "july") == 0) || 
			(stricmp(monthName.c_str(), "jul") == 0))
	{
		month = July;
	}
	else if((stricmp(monthName.c_str(), "august") == 0) || 
			(stricmp(monthName.c_str(), "aug") == 0))
	{
		month = August;
	}
	else if((stricmp(monthName.c_str(), "september") == 0) || 
			(stricmp(monthName.c_str(), "sep") == 0))
	{
		month = September;
	}
	else if((stricmp(monthName.c_str(), "october") == 0) || 
			(stricmp(monthName.c_str(), "oct") == 0))
	{
		month = October;
	}
	else if((stricmp(monthName.c_str(), "november") == 0) || 
			(stricmp(monthName.c_str(), "nov") == 0))
	{
		month = November;
	}
	else if((stricmp(monthName.c_str(), "december") == 0) || 
			(stricmp(monthName.c_str(), "dec") == 0))
	{
		month = December;
	}
	return month;
}

//------------------------------------------------------------------------------
bool DataCollectorSender::IsFileInDateRange(time_t &startDate, 
											time_t &endDate, 
											time_t &fileDate)
{
/*
	cout << "\tStart date: " << ctime(&startDate);
	cout << "\tFile  date: " << ctime(&fileDate);
	cout << "\tEnd   date: " << ctime(&endDate);
*/	
	bool inRange = (difftime(fileDate, startDate) > 0) && (difftime(endDate, fileDate) > 0);
//	cout << "\t\tFile In Date Range: " << (inRange ? "True" : "False") << endl;
	return inRange;
}

//------------------------------------------------------------------------------
void DataCollectorSender::LoadResultFiles(time_t &startDate, time_t &endDate)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	m_resultFiles.clear();
	string dirPath = m_resultPath + m_reportPath;
	if((dir = opendir(dirPath.c_str())) != NULL)
	{
		while((entry = readdir(dir)) != NULL)
		{
			string name(entry->d_name);
			if(name.compare(".") && name.compare("..") && name.compare("TestResultIndex"))
			{   // Verify the file date is in range
				struct stat fileStat;
				string fileName = m_resultPath + m_reportPath + name;
				stat(fileName.c_str(), &fileStat);
//				cout << "Checking file dates for " << fileName << endl;
				if(IsFileInDateRange(startDate, endDate, fileStat.st_mtime))
				{
					m_resultFiles.push_back(m_reportPath + name);
					cout << "Adding result file: " << m_resultFiles.back() << endl;
				}
			}
		}
	}
	else
	{
		cout << endl << "ERROR: Could not open result folder: " << m_resultPath << " -- " << strerror(errno) << endl;
	}
}

