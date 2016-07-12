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
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Library/AuxResultFieldList.cpp $
// 
// 2     3/09/06 12:58a Bmeinke
// Added a new m_visibility member (with getter and setter methods) to
// control whether or not a result field will be returned to clients when
// they read th test results
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 2     8/25/05 3:11p Bmeinke
// Fixed t he name of the include file
// 
// 1     8/25/05 3:09p Bmeinke
// Moved source from header to cpp file
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AuxResultFieldList.h"

AuxResultField::AuxResultField() : 
	m_fieldTag( ""), m_valueTag(""), m_visible( true)
{
}

/**
 * Construct a value field
 * 
 * @param tag    Tag of this field
 */
AuxResultField::AuxResultField( const std::string &tag) : 
	m_fieldTag( tag), m_valueTag( ""), m_visible( true)
{
}

/**
 * Construct a pass/fail flag field
 * 
 * @param tag        Tag for this field
 * @param valueField Tag of the value filed that this pass/fail flag refers to
 * @param visible    Flag indicating if this field is visible to clients reading results.
 *                   If true, this field will be returned to clients.
 */
AuxResultField::AuxResultField( const std::string &tag, const std::string &valueField, bool visible) :
	m_fieldTag( tag), m_valueTag( valueField), m_visible( visible)
{
}

/**
 * Construct a value field
 * 
 * @param tag    Tag of this field
 */
AuxResultField::AuxResultField( const AuxResultField &copy) : 
	m_fieldTag( copy.m_fieldTag), m_valueTag(copy.m_valueTag), m_visible( copy.m_visible)
{
}

AuxResultField::~AuxResultField()
{
}

/**
 * Return the tag for this field
 * 
 * @return The tag for this field
 */
const std::string& AuxResultField::GetFieldTag() const
{
	return( m_fieldTag);
}

/**
 * Sets the tag for this field
 * 
 * @param fieldTag The tag for this field
 */
void AuxResultField::SetFieldTag(const std::string& fieldTag)
{
	m_fieldTag = fieldTag;
}

/**
 * Check to see if this field represents a pass/fail flag
 * 
 * @return true if this field represents a pass/fail flag
 */
bool AuxResultField::IsPassFailFlag() const
{
	return( m_valueTag.empty() == false);
}

/**
 * For pass/fail flag fields, what value field does this field refer to
 * 
 * @return The value field does this field refer to
 */
const std::string& AuxResultField::GetValueTag() const
{
	return( m_valueTag);
}

/**
 * Marks this field as a pass/fail flag and associates it with 
 * the value field with the given tag
 * 
 * @param valueTag The value field this pass/fail flag field refers to
 */
void AuxResultField::SetValueTag(const std::string& valueTag)
{
	m_valueTag = valueTag;
}

/**
 * Returs the "visibility" of this field:
 *   "public": Returned to clients reading a result
 *   "private:: NOT Returned to clients reading a result
 * 
 * @return Returs the "visibility" of this field:
 *         "public": Returned to clients reading a result
 *         "private:: NOT Returned to clients reading a result
 */
bool AuxResultField::IsVisible() const
{
	return( m_visible);
}

/**
 * Returs the "visibility" of this field.
 * 
 * @param visible Desired field visibility:
 *                true: Returned to clients reading a result
 *                false: NOT Returned to clients reading a result
 */
void AuxResultField::SetVisible(bool visible)
{
	if( visible != m_visible)
	{
		m_visible = visible;
	}
}

