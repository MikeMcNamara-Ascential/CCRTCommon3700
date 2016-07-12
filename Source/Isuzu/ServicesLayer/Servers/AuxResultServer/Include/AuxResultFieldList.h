//******************************************************************************
// Description:
//  Object used to hold an ordered list of result fields as received from 
//	an external source (i.e. HLA and side slip testers)
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Include/AuxResultFieldList.h $
// 
// 2     3/09/06 12:58a Bmeinke
// Added a new m_visibility member (with getter and setter methods) to
// control whether or not a result field will be returned to clients when
// they read th test results
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 3     8/25/05 3:09p Bmeinke
// Moved source from header to cpp file
// 
// 2     8/22/05 2:08p Bmeinke
// Added a comment block showing an ordered list of the HLA result fields
// 
// 1     8/22/05 1:52p Bmeinke
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _AUX_RESULT_FIELD_LIST_H_INCLUDED_
#define _AUX_RESULT_FIELD_LIST_H_INCLUDED_

#include <string>
#include <vector>

/**
 * This is the ordered list of results we receive from the HLA tester
 * 	VIN
 * 	VehicleType
 * 	Operator
 * 	TestDate
 * 	LeftWheelLeftAngle
 * 	LeftWheelRightAngle
 * 	RightWheelLeftAngle
 * 	RightWheelRightAngle
 * 	LeftWheelLeftAngleJudgement
 * 	LeftWheelRightAngleJudgement
 * 	RightWheelLeftAngleJudgement
 * 	RightWheelRightAngleJudgement
 * 	LeftLampLuminousIntensity
 * 	LeftLampUpDownDeviationAngle
 * 	LeftLampLeftRightDeviationAngle
 * 	LeftLampHeight
 * 	RightLampLuminousIntensity
 * 	RightLampUpDownDeviationAngle
 * 	RightLampLeftRightDeviationAngle
 * 	RightLampHeight
 */

/**
 * Object used to represent the individual fields of the
 * Aux result string
 */
class AuxResultField
{
public:
	/**
	 * Default constructor
	 */
	AuxResultField();

	/**
	 * Construct a value field
	 * 
	 * @param tag    Tag of this field
	 */
	AuxResultField( const std::string &tag);

	/**
	 * Construct a pass/fail flag field
	 * 
	 * @param tag        Tag for this field
	 * @param valueField Tag of the value filed that this pass/fail flag refers to
	 * @param visible    Flag indicating if this field is visible to clients reading results.
	 *                   If true, this field will be returned to clients.
	 */
	AuxResultField( const std::string &tag, const std::string &valueField, bool visible=true);

	/**
	 * Construct a value field
	 * 
	 * @param tag    Tag of this field
	 */
	AuxResultField( const AuxResultField &copy);
	
	/**
	 * Destructor
	 */
	~AuxResultField();

	/**
	 * Return the tag for this field
	 * 
	 * @return The tag for this field
	 */
	const std::string& GetFieldTag() const;

	/**
	 * Sets the tag for this field
	 * 
	 * @param fieldTag The tag for this field
	 */
	void SetFieldTag(const std::string& fieldTag);

	/**
	 * Check to see if this field represents a pass/fail flag
	 * 
	 * @return true if this field represents a pass/fail flag
	 */
	bool IsPassFailFlag() const;

	/**
	 * For pass/fail flag fields, what value field does this field refer to
	 * 
	 * @return The value field does this field refer to
	 */
	const std::string& GetValueTag() const;

	/**
	 * Marks this field as a pass/fail flag and associates it with 
	 * the value field with the given tag
	 * 
	 * @param valueTag The value field this pass/fail flag field refers to
	 */
	void SetValueTag(const std::string& valueTag);

	/**
	 * Returns the "visibility" of this field:
	 *   true: Returned to clients reading a result
	 *   false: NOT Returned to clients reading a result
	 * 
	 * @return Returns the "visibility" of this field:
	 *         true: Returned to clients reading a result
	 *         false: NOT Returned to clients reading a result
	 */
	bool IsVisible() const;

	/**
	 * Returs the "visibility" of this field.
	 * 
	 * @param visible Desired field visibility:
	 *                true: Returned to clients reading a result
	 *                false: NOT Returned to clients reading a result
	 */
	void SetVisible(bool visible);

protected:
	/**
	 * The name of this result field
	 */
	std::string	m_fieldTag;
	/**
	 * If this is a pass/fail flag, which field does the flag belong to
	 */
	std::string	m_valueTag;
	/**
	 * Flag indicating if this field is visible to clients reading results.
	 */
	bool		m_visible;
};

/**
 * Object used to store the list of tags used for reporting results
 */
typedef std::vector< AuxResultField>			AuxResultFieldList_t;
typedef AuxResultFieldList_t::iterator			AuxResultFieldListItr_t;
typedef AuxResultFieldList_t::const_iterator	AuxResultFieldListCItr_t;

#endif // _AUX_RESULT_FIELD_LIST_H_INCLUDED_
