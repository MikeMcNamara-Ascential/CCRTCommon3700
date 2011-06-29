//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/DataAnalysis.cpp 2     1/23/04 9:25a Cward $
//
// FILE DESCRIPTION:
//  This class provides analysis methods for data.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/DataAnalysis.cpp $
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:42p Khykin
// Initial checkin.
//
// 1     4/22/03 9:39a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:23a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:01  tbochene
//
//    PooBag
//
//
// 2     2/08/03 9:43a Khykin
// Updated diagnostics.
//
// 4     1/14/03 4:34p Derickso
// Corrected compiler errors and warnings.
//
// 3     12/13/02 8:30a Derickso
// Modified ConvertData() method to use UINT16
// instead of unsigned char.
//
// 2     10/07/02 1:06p Derickso
// Not tested yet.
//
// 1     9/10/02 6:22p Tbochene
// Data analysis class
//
//*************************************************************************

#include "DataAnalysis.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Class constructor.
 * @since Version 1.0
 */
DataAnalysis::DataAnalysis()
{
}

/**
 * Class destructor.
 * @since Version 1.0
 */
DataAnalysis::~DataAnalysis()
{
}

const bool DataAnalysis::CompareData(const std::string &dataValue1, const std::string &dataValue2, const Comparison_t comparison)
{
	bool result = false;
	switch (comparison)
	{
	case GREATER:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) > 0;
		break;
	case GREATER_EQUAL:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) >= 0;
		break;
	case LESS:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) < 0;
		break;
	case LESS_EQUAL:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) <= 0;
		break;
	case EQUAL:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) == 0;
		break;
	case NOT_EQUAL:
		result = strcmp(dataValue1.c_str(), dataValue2.c_str()) != 0;
		break;
	default:
		printf("DataAnalysis::CompareData(string, string) -- Invalid comparison operation requested -- dataValue1:%s, dataValue2:%s, comparison:%d\n",
			   dataValue1.c_str(), dataValue2.c_str(), comparison);
		break;
	}
	return result;
}
const bool DataAnalysis::CompareData(const INT32 &dataValue1, const INT32 &dataValue2, const Comparison_t comparison)
{
	bool result = false;
	switch (comparison)
	{
	case GREATER:
		result = dataValue1 > dataValue2;
		break;
	case GREATER_EQUAL:
		result = dataValue1 >= dataValue2;
		break;
	case LESS:
		result = dataValue1 < dataValue2;
		break;
	case LESS_EQUAL:
		result = dataValue1 <= dataValue2;
		break;
	case EQUAL:
		result = dataValue1 == dataValue2;
		break;
	case NOT_EQUAL:
		result = dataValue1 != dataValue2;
		break;
	case LOGICAL_AND:
		result = dataValue1 && dataValue2;
		break;
	case LOGICAL_OR:
		result = dataValue1 || dataValue2;
		break;
	case ALL_BITS_SET:
		result = (dataValue1 & dataValue2) == dataValue2;
		break;
	case ANY_BITS_SET:
		result = dataValue1 & dataValue2;
		break;
	default:
		printf("DataAnalysis::CompareData(INT32, INT32) -- Invalid comparison operation requested -- dataValue1:%d, dataValue2:%d, comparison:%d\n",
			   dataValue1, dataValue2, comparison);
		break;
	}
	return result;
}

const bool DataAnalysis::CompareData(const UINT32 &dataValue1, const UINT32 &dataValue2, const Comparison_t comparison)
{
	bool result = false;
	switch (comparison)
	{
	case GREATER:
		result = dataValue1 > dataValue2;
		break;
	case GREATER_EQUAL:
		result = dataValue1 >= dataValue2;
		break;
	case LESS:
		result = dataValue1 < dataValue2;
		break;
	case LESS_EQUAL:
		result = dataValue1 <= dataValue2;
		break;
	case EQUAL:
		result = dataValue1 == dataValue2;
		break;
	case NOT_EQUAL:
		result = dataValue1 != dataValue2;
		break;
	case LOGICAL_AND:
		result = dataValue1 && dataValue2;
		break;
	case LOGICAL_OR:
		result = dataValue1 || dataValue2;
		break;
	case ALL_BITS_SET:
		result = (dataValue1 & dataValue2) == dataValue2;
		break;
	case ANY_BITS_SET:
		result = dataValue1 & dataValue2;
		break;
	default:
		printf("DataAnalysis::CompareData(UINT32, UINT32) -- Invalid comparison operation requested -- dataValue1:%d, dataValue2:%d, comparison:%d\n",
			   dataValue1, dataValue2, comparison);
		break;
	}
	return result;
}

const bool DataAnalysis::CompareData(const unsigned char &dataValue1, const unsigned char &dataValue2, const Comparison_t comparison)
{
	bool result = false;
	switch (comparison)
	{
	case GREATER:
		result = dataValue1 > dataValue2;
		break;
	case GREATER_EQUAL:
		result = dataValue1 >= dataValue2;
		break;
	case LESS:
		result = dataValue1 < dataValue2;
		break;
	case LESS_EQUAL:
		result = dataValue1 <= dataValue2;
		break;
	case EQUAL:
		result = dataValue1 == dataValue2;
		break;
	case NOT_EQUAL:
		result = dataValue1 != dataValue2;
		break;
	case LOGICAL_AND:
		result = dataValue1 && dataValue2;
		break;
	case LOGICAL_OR:
		result = dataValue1 || dataValue2;
		break;
	case ALL_BITS_SET:
		result = (dataValue1 & dataValue2) == dataValue2;
		break;
	case ANY_BITS_SET:
		result = dataValue1 & dataValue2;
		break;
	default:
		printf("DataAnalysis::CompareData(unsigned char, unsigned char) -- Invalid comparison operation requested -- dataValue1:0x%X, dataValue2:0x%X, comparison:%d\n",
			   dataValue1, dataValue2, comparison);
		break;
	}
	return result;
}

const bool DataAnalysis::CompareData(const double &dataValue1, const double &dataValue2, const Comparison_t comparison)
{
	bool result = false;
	switch (comparison)
	{
	case GREATER:
		result = dataValue1 > dataValue2;
		break;
	case GREATER_EQUAL:
		result = dataValue1 >= dataValue2;
		break;
	case LESS:
		result = dataValue1 < dataValue2;
		break;
	case LESS_EQUAL:
		result = dataValue1 <= dataValue2;
		break;
	case EQUAL:
		result = dataValue1 == dataValue2;
		break;
	case NOT_EQUAL:
		result = dataValue1 != dataValue2;
		break;
	case LOGICAL_AND:
		result = dataValue1 && dataValue2;
		break;
	case LOGICAL_OR:
		result = dataValue1 || dataValue2;
		break;
	default:
		printf("DataAnalysis::CompareData(double, double) -- Invalid comparison operation requested -- dataValue1:%f, dataValue2:%f, comparison:%d\n",
			   dataValue1, dataValue2, comparison);
		break;
	}
	return result;
}

const double DataAnalysis::ConvertData(const UINT16 &dataValue1, const double &dataValue2, const Conversion_t conversion)
{
	double result = 0.0;
	switch (conversion)
	{
	case ADD:
		result = dataValue1 + dataValue2;
		break;
	case SUBTRACT:
		result = dataValue1 - dataValue2;
		break;
	case MULTIPLY:
		result = dataValue1 * dataValue2;
		break;
	case DIVIDE:
		result = dataValue1 / dataValue2;
		break;
	default:
		printf("DataAnalysis::ConvertData(unsigned char, double) -- Invalid conversion requested -- dataValue1:0x%02X, dataValue2:%f, conversion:%d\n",
			   dataValue1, dataValue2, conversion);
		break;
	}
	return result;
}

const unsigned char DataAnalysis::ConvertData(const unsigned char dataValue1, const unsigned char dataValue2, const Conversion_t conversion)
{
	unsigned char result = 0;
	switch (conversion)
	{
	case ADD:
		result = dataValue1 + dataValue2;
		break;
	case SUBTRACT:
		result = dataValue1 - dataValue2;
		break;
	case MULTIPLY:
		result = dataValue1 * dataValue2;
		break;
	case DIVIDE:
		result = dataValue1 / dataValue2;
		break;
	case SET_BITS:
		result = dataValue1 | dataValue2;
		break;
	case CLEAR_BITS:
		result = dataValue1 & (~dataValue2);
		break;
	case TOGGLE_BITS:
		result = dataValue1 ^ dataValue2;
		break;
	case BITWISE_INVERT:
		result = ~dataValue1;
		break;
	default:
		printf("DataAnalysis::ConvertData(unsigned char, unsigned char) -- Invalid conversion requested"
			   " -- dataValue1:0x%02X, dataValue2:0x%02X, conversion:%d\n",
			   dataValue1, dataValue2, conversion);
		break;
	}
	return result;
}

const UINT32 DataAnalysis::ConvertData(const UINT32 &dataValue1, const UINT32 &dataValue2, const Conversion_t conversion)
{
	UINT32 result = 0;
	switch (conversion)
	{
	case ADD:
		result = dataValue1 + dataValue2;
		break;
	case SUBTRACT:
		result = dataValue1 - dataValue2;
		break;
	case MULTIPLY:
		result = dataValue1 * dataValue2;
		break;
	case DIVIDE:
		result = dataValue1 / dataValue2;
		break;
	case SET_BITS:
		result = dataValue1 | dataValue2;
		break;
	case CLEAR_BITS:
		result = dataValue1 & (~dataValue2);
		break;
	case TOGGLE_BITS:
		result = dataValue1 ^ dataValue2;
		break;
	case BITWISE_INVERT:
		result = ~dataValue1;
		break;
	default:
		printf("DataAnalysis::ConvertData(UINT32, UINT32) -- Invalid conversion requested -- "
			   "dataValue1:0x%04X, dataValue2:0x%04X, conversion:%d\n", dataValue1, dataValue2, conversion);
		break;
	}
	return result;
}

const INT32 DataAnalysis::ConvertData(const INT32 &dataValue1, const INT32 &dataValue2, const Conversion_t conversion)
{
	INT32 result = 0;
	switch (conversion)
	{
	case ADD:
		result = dataValue1 + dataValue2;
		break;
	case SUBTRACT:
		result = dataValue1 - dataValue2;
		break;
	case MULTIPLY:
		result = dataValue1 * dataValue2;
		break;
	case DIVIDE:
		result = dataValue1 / dataValue2;
		break;
	case SET_BITS:
		result = dataValue1 | dataValue2;
		break;
	case CLEAR_BITS:
		result = dataValue1 & (~dataValue2);
		break;
	case TOGGLE_BITS:
		result = dataValue1 ^ dataValue2;
		break;
	case BITWISE_INVERT:
		result = ~dataValue1;
		break;
	default:
		printf("DataAnalysis::ConvertData(INT32, INT32) -- Invalid conversion requested -- "
			   "dataValue1:%d, dataValue2:%d, conversion:%d\n", dataValue1, dataValue2, conversion);
		break;
	}
	return result;
}

const double DataAnalysis::ConvertData(const double &dataValue1, const double &dataValue2, const Conversion_t conversion)
{
	double result = 0;
	switch (conversion)
	{
	case ADD:
		result = dataValue1 + dataValue2;
		break;
	case SUBTRACT:
		result = dataValue1 - dataValue2;
		break;
	case MULTIPLY:
		result = dataValue1 * dataValue2;
		break;
	case DIVIDE:
		result = dataValue1 / dataValue2;
		break;
	default:
		printf("DataAnalysis::ConvertData(double, double) -- Invalid conversion requested -- "
			   "dataValue1:%f, dataValue2:%f, conversion:%d\n", dataValue1, dataValue2, conversion);
		break;
	}
	return result;
}

const unsigned char DataAnalysis::Average(const unsigned char *dataArray, const INT32 &numberOfValues)
{
	unsigned char result = 0;
	for (INT32 ii = 0; ii < numberOfValues; ii++)
		result += dataArray[ii];
	return (result/numberOfValues);
}

const UINT32 DataAnalysis::Average(const UINT32 *dataArray, const INT32 &numberOfValues)
{
	UINT32 result = 0;
	for (INT32 ii = 0; ii < numberOfValues; ii++)
		result += dataArray[ii];
	return (result/numberOfValues);
}

const INT32 DataAnalysis::Average(const INT32 *dataArray, const INT32 &numberOfValues)
{
	INT32 result = 0;
	for (INT32 ii = 0; ii < numberOfValues; ii++)
		result += dataArray[ii];
	return (result/numberOfValues);
}

const double DataAnalysis::Average(const double *dataArray, const INT32 &numberOfValues)
{
	double result = 0;
	for (INT32 ii = 0; ii < numberOfValues; ii++)
		result += dataArray[ii];
	return (result/numberOfValues);
}

const unsigned char DataAnalysis::FindMinValue(const unsigned char *dataArray, const INT32 &numberOfValues)
{
	unsigned char result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the smallest number we have seen
		result = dataArray[ii] < result ? dataArray[ii] : result;
	}
	return result;
}

const UINT32 DataAnalysis::FindMinValue(const UINT32 *dataArray, const INT32 &numberOfValues)
{
	UINT32 result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the smallest number we have seen
		result = dataArray[ii] < result ? dataArray[ii] : result;
	}
	return result;
}

const INT32 DataAnalysis::FindMinValue(const INT32 *dataArray, const INT32 &numberOfValues)
{
	INT32 result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the smallest number we have seen
		result = dataArray[ii] < result ? dataArray[ii] : result;
	}
	return result;
}

const double DataAnalysis::FindMinValue(const double *dataArray, const INT32 &numberOfValues)
{
	double result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the smallest number we have seen
		result = dataArray[ii] < result ? dataArray[ii] : result;
	}
	return result;
}

const float DataAnalysis::FindMinValue(const float *dataArray, const INT32 &numberOfValues)
{
	float result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the smallest number we have seen
		result = dataArray[ii] < result ? dataArray[ii] : result;
	}
	return result;
}

const unsigned char DataAnalysis::FindMaxValue(const unsigned char *dataArray, const INT32 &numberOfValues)
{
	unsigned char result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the largest number we have seen
		result = dataArray[ii] > result ? dataArray[ii] : result;
	}
	return result;
}

const UINT32 DataAnalysis::FindMaxValue(const UINT32 *dataArray, const INT32 &numberOfValues)
{
	UINT32 result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the largest number we have seen
		result = dataArray[ii] > result ? dataArray[ii] : result;
	}
	return result;
}

const INT32 DataAnalysis::FindMaxValue(const INT32 *dataArray, const INT32 &numberOfValues)
{
	INT32 result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the largest number we have seen
		result = dataArray[ii] > result ? dataArray[ii] : result;
	}
	return result;
}

const double DataAnalysis::FindMaxValue(const double *dataArray, const INT32 &numberOfValues)
{
	double result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the largest number we have seen
		result = dataArray[ii] > result ? dataArray[ii] : result;
	}
	return result;
}

const float DataAnalysis::FindMaxValue(const float *dataArray, const INT32 &numberOfValues)
{
	float result = dataArray[0];
	for (INT32 ii = 1; ii < numberOfValues; ii++)
	{   // Check to see if this is the largest number we have seen
		result = dataArray[ii] > result ? dataArray[ii] : result;
	}
	return result;
}
