//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/DataAnalysis.h 1     12/21/03 6:26p Cward $
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
//    $Log: /Core/ServicesLayer/Include/DataAnalysis.h $
// 
// 1     12/21/03 6:26p Cward
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:08p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:35  tbochene
//
//    PooBag
//
// 
// 4     1/14/03 4:49p Derickso
// Corrected compile errors and warnings
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
#ifndef DataAnalysis_h
#define DataAnalysis_h
//=============================================================================

#include <string>
#include <vector>
#include <algorithm>
#include "BepDefs.h"

/**
 * Valid data conversion operations.
 * @since Version 1.0
 */
typedef enum
{
	ADD,				  // +
	SUBTRACT,			  // -
	MULTIPLY,			  // *
	DIVIDE,				  // /
	SET_BITS,        
	CLEAR_BITS,
	TOGGLE_BITS,		  // ^
	BITWISE_INVERT		  // ~
} Conversion_t;
/**
 * Valid data comparison operations.
 * @since Version 1.0
 */
typedef enum
{
	GREATER,			  // >
	GREATER_EQUAL,		  // >=
	LESS,				  // <
	LESS_EQUAL,			  // <=
	EQUAL,				  // ==
	NOT_EQUAL,			  // !=
	LOGICAL_AND,		  // &&
	LOGICAL_OR,			  // ||
	ALL_BITS_SET,		  // & and =	
	ANY_BITS_SET		  // &
} Comparison_t;


/**
* DataAnalysis class.
* This class provides methods to analyze the data that is passed to it.
* 
* @author Dan Erickson
* @version Version 1.0
*/
class DataAnalysis
{
public:
	/**
	 * Class constructor.
	 * @since Version 1.0
	 */
	DataAnalysis();
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~DataAnalysis();
	/**
	 * Compares two data values and returns the result of the comparison.
	 * 
	 * @param dataValue1 Left data operand.
	 * @param dataValue2 Right data operand.
	 * @param comparison Comparison operation to be performed.
	 * @return Result of the comparison operation.
	 * @since Version 1.0
	 */
	const bool CompareData(const std::string &dataValue1, const std::string &dataValue2, const Comparison_t comparison);
	/**
	 * Compares two data values and returns the result of the comparison.
	 * 
	 * @param dataValue1 Left data operand.
	 * @param dataValue2 Right data operand.
	 * @param comparison Comparison operation to be performed.
	 * @return Result of the comparison operation.
	 * @since Version 1.0
	 */
	const bool CompareData(const INT32 &dataValue1, const INT32 &dataValue2, const Comparison_t comparison);
	/**
	 * Compares two data values and returns the result of the comparison.
	 * 
	 * @param dataValue1 Left data operand.
	 * @param dataValue2 Right data operand.
	 * @param comparison Comparison operation to be performed.
	 * @return Result of the comparison operation.
	 * @since Version 1.0
	 */
	const bool CompareData(const UINT32 &dataValue1, const UINT32 &dataValue2, const Comparison_t comparison);
	/**
	 * Compares two data values and returns the result of the comparison.
	 * 
	 * @param dataValue1 Left data operand.
	 * @param dataValue2 Right data operand.
	 * @param comparison Comparison operation to be performed.
	 * @return Result of the comparison operation.
	 * @since Version 1.0
	 */
	const bool CompareData(const unsigned char &dataValue1, const unsigned char &dataValue2, const Comparison_t comparison);
	/**
	 * Compares two data values and returns the result of the comparison.
	 * 
	 * @param dataValue1 Left data operand.
	 * @param dataValue2 Right data operand.
	 * @param comparison Comparison operation to be performed.
	 * @return Result of the comparison operation.
	 * @since Version 1.0
	 */
	const bool CompareData(const double &dataValue1, const double &dataValue2, const Comparison_t comparison);
	/**
	 * Convert a given data value according to the conversion operation specified.
	 * Valid conversions are: Add, Subtract, Multiply, Divide.
	 * 
	 * @param dataValue1 Raw data to be converted.
	 * @param dataValue2 Conversion factor to use.
	 * @param conversion Conversion operation.
	 * @return Converted data.
	 * @since Version 1.0
	 */
	const double ConvertData(const UINT16 &dataValue1, const double &dataValue2, const Conversion_t conversion);
	/**
	 * Convert a given data value according to the conversion operation specified.
	 * 
	 * @param dataValue1 Raw data to be converted.
	 * @param dataValue2 Conversion factor to use.
	 * @param conversion Conversion operation.
	 * @return Converted data.
	 * @since Version 1.0
	 */
	const unsigned char ConvertData(const unsigned char dataValue1, const unsigned char dataValue2, const Conversion_t conversion);
	/**
	 * Convert a given data value according to the conversion operation specified.
	 * 
	 * @param dataValue1 Raw data to be converted.
	 * @param dataValue2 Conversion factor to use.
	 * @param conversion Conversion operation.
	 * @return Converted data.
	 * @since Version 1.0
	 */
	const UINT32 ConvertData(const UINT32 &dataValue1, const UINT32 &dataValue2, const Conversion_t conversion);
	/**
	 * Convert a given data value according to the conversion operation specified.
	 * 
	 * @param dataValue1 Raw data to be converted.
	 * @param dataValue2 Conversion factor to use.
	 * @param conversion Conversion operation.
	 * @return Converted data.
	 * @since Version 1.0
	 */
	const INT32 ConvertData(const INT32 &dataValue1, const INT32 &dataValue2, const Conversion_t conversion);
	/**
	 * Convert a given data value according to the conversion operation specified.  Valid operations
	 * include: Add, Subtract, Multiply and Divide.
	 * 
	 * @param dataValue1 Raw data to be converted.
	 * @param dataValue2 Conversion factor to use.
	 * @param conversion Conversion operation.
	 * @return Converted data.
	 * @since Version 1.0
	 */
	const double ConvertData(const double &dataValue1, const double &dataValue2, const Conversion_t conversion);
	/**
	 * Averages a set of values.
	 * 
	 * @param dataArray Data values to be averaged.
	 * @param numberOfValues
	 *                  Total number of values in the average.
	 * @return The average of the data values.
	 * @since Version 1.0
	 */
	const unsigned char Average(const unsigned char *dataArray, const INT32 &numberOfValues);
	/**
	 * Averages a set of values.
	 * 
	 * @param dataArray Data values to be averaged.
	 * @param numberOfValues
	 *                  Total number of values in the average.
	 * @return The average of the data values.
	 * @since Version 1.0
	 */
	const UINT32 Average(const UINT32 *dataArray, const INT32 &numberOfValues);
	/**
	 * Averages a set of values.
	 * 
	 * @param dataArray Data values to be averaged.
	 * @param numberOfValues
	 *                  Total number of values in the average.
	 * @return The average of the data values.
	 * @since Version 1.0
	 */
	const INT32 Average(const INT32 *dataArray, const INT32 &numberOfValues);
	/**
	 * Averages a set of values.
	 * 
	 * @param dataArray Data values to be averaged.
	 * @param numberOfValues
	 *                  Total number of values in the average.
	 * @return The average of the data values.
	 * @since Version 1.0
	 */
	const double Average(const double *dataArray, const INT32 &numberOfValues);
	/**
	 * Searches the data array for the minimum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Minimum data value in the specified array.
	 * @since Version 1.0
	 */
	const unsigned char FindMinValue(const unsigned char *dataArray, const INT32 &numberOfValues); 
	/**
	 * Searches the data array for the minimum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Minimum data value in the specified array.
	 * @since Version 1.0
	 */
	const UINT32 FindMinValue(const UINT32 *dataArray, const INT32 &numberOfValues); 
	/**
	 * Searches the data array for the minimum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Minimum data value in the specified array.
	 * @since Version 1.0
	 */
	const INT32 FindMinValue(const INT32 *dataArray, const INT32 &numberOfValues); 
	/**
	 * Searches the data array for the minimum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Minimum data value in the specified array.
	 * @since Version 1.0
	 */
	const double FindMinValue(const double *dataArray, const INT32 &numberOfValues); 
	/**
	 * Searches the data array for the minimum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Minimum data value in the specified array.
	 * @since Version 1.0
	 */
	const float FindMinValue(const float *dataArray, const INT32 &numberOfValues); 
	/**
	 * Searches the data array for the maximum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Maximum data value in the specified array.
	 * @since Version 1.0
	 */
	const unsigned char FindMaxValue(const unsigned char *dataArray, const INT32 &numberOfValues);
	/**
	 * Searches the data array for the maximum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Maximum data value in the specified array.
	 * @since Version 1.0
	 */
	const UINT32 FindMaxValue(const UINT32 *dataArray, const INT32 &numberOfValues);
	/**
	 * Searches the data array for the maximum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Maximum data value in the specified array.
	 * @since Version 1.0
	 */
	const INT32 FindMaxValue(const INT32 *dataArray, const INT32 &numberOfValues);
	/**
	 * Searches the data array for the maximum value.
	 * 
	 * @param dataArray Data values.
	 * @param numberOfValues
	 *                  Total elements in the array.
	 * @return Maximum data value in the specified array.
	 * @since Version 1.0
	 */
	const double FindMaxValue(const double *dataArray, const INT32 &numberOfValues);
    /**
     * Searches the data array for the maximum value.
     * 
     * @param dataArray Data values.
     * @param numberOfValues
     *                  Total elements in the array.
     * @return Maximum data value in the specified array.
     * @since Version 1.0
     */
    const float FindMaxValue(const float *dataArray, const INT32 &numberOfValues);
};

#endif //DataAnalysis_h
