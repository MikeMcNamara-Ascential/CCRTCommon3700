#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "DataCollectorResultSender.h"

int main(void)
{
	DataCollectorSender dcSender;
	string startMonth, endMonth;
	int startDay, endDay;
	int startYear, endYear;

	// Prompt the user for the starting date
	cout << endl << endl << "Enter start date (mmm dd yyyy): ";
	cin >> startMonth >> startDay >> startYear;
	cout << endl << "Enter end date (mmm dd yyyy): ";
	cin >> endMonth >> endDay >> endYear;

	time_t startDate = dcSender.ConvertTime(startMonth, startDay, startYear);
	time_t endDate = dcSender.ConvertTime(endMonth, endDay, endYear);

	dcSender.ReportResults(startDate, endDate);
	cout << endl << endl << "Reporting Complete, Have a Nice Day!" << endl << endl;

	return 0;
}
