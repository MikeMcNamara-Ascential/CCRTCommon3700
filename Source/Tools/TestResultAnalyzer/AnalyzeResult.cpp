#include <stdio.h>
#include <stdlib.h>
#include "TestResultAnalyzer.h"

int main(void)
{
	TestResultAnalyzer analyzer;
	// Prompt the user for a vin file
	string vin;
	cout << endl << endl << "Please enter the last 8 characters of the VIN to analyze: ";
	cin >> vin;
	// Load the requested file
	analyzer.LoadTestResult(vin);
	// Analyze the results
	int numberOfTests = 0;
	int testPassCount = 0;
	float percentPass = 0.0;
	analyzer.GetTestResultStats(numberOfTests, testPassCount, percentPass);
	// Display the results to the user
	cout << endl;
	cout << "\tNumber Of Tests Performed: " << numberOfTests << endl;
	cout << "\tNumber Of Tests Passed:    " << testPassCount << endl;
	cout << "\tPercent of Tests Passed:   " << percentPass << endl;
	cout << endl << "Have a nice day!" << endl << endl;
	return 0;
}
