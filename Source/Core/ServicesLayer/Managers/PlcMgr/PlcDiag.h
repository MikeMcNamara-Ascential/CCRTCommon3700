//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcDiag.h $
// $Author:: Cward                                                            $
//   $Date:: 4/13/06 4:09a                                                    $
//
// Description:
//	PLC Diagnostic Data Types
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcDiag.h $
// 
// 2     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:19p Bmeinke
// Beautified
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 2     5/29/03 2:25p Bmeinke
// Added processName argument to constructor (defaults to PLC)
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 5     10/14/02 7:39p Bmeinke
// Added override for SetProcessName()
// Removed PublishFault()/ClearFault() (just use base class versions)
// Fixed ReadLimitsXml() (was looking fro wrong XML tag)
// Changed PlcDiagMap_t from map< long, PlcDiagItem> to map< uint64_t,
// PlcDiagItem>
//
// 4     8/01/02 5:14p Bmeinke
// Removed compiler warnings
//
// 3     11/02/01 1:56p Brian.meinke
// No longer need to overload the CheckTrigger() method
//
// 2     11/02/01 1:55p Brian.meinke
// Moved DIAG_CHANNEL_TAG define to DiagItem.h
//
// 1     10/31/01 2:09p Brian.meinke
// Data type definitions for PLC diagnostic items
//
//******************************************************************************

#ifndef _PLC_DIAG_H_INCLUDED_
#define _PLC_DIAG_H_INCLUDED_

class PlcDiagItem;

#include "BepDefs.h"
#include "DiagItem.h"
#include "BepXmlUtils.h"
#include "XmlNode.h"

#include <map>

typedef DiagItem< UINT32>	PlcDiagItem_t;

/**
 * Class used to describe a PLC Diagnostic data item. A PLC diagnostic
 * item has a fault tag, min and max trigger points, a fault level and
 * a fault description. The fault will be triggered if the value of the
 * associated data item is between (inclusively) the min and max trigger
 * points
 *
 * @author Brian Meink
 * @version 1.0
 * @see DiagItem
 */
class PlcDiagItem : public PlcDiagItem_t
{
public:

	PlcDiagItem( UINT32 min=0, UINT32 max=0, const string &faultTag="",
				  const string &faultDesc="", const string &processName="PLC", DiagLevel_t level=DIAG_WARN) :
		PlcDiagItem_t( min, max, faultTag, faultDesc, processName, level)
	{
	};

	PlcDiagItem( const PlcDiagItem& copy) : PlcDiagItem_t( copy)
	{
	};

	~PlcDiagItem()
	{
	};

	/**
	 * Sets the warning level for the diagnostic object
	 *
	 * @see DiagLevel_t
	 */
	void SetLevel(  DiagLevel_t newLevel)
	{
		PlcDiagItem_t::SetLevel( newLevel);
	};

	/**
	 * Sets the low trip limit for the diagnostic object
	 */
	void SetMin( float newLow)
	{
		PlcDiagItem_t::SetMin( newLow);
	};

	/**
	 * Sets the high trip limit for the diagnostic object
	 */
	void SetMax( float newHigh)
	{
		PlcDiagItem_t::SetMax( newHigh);
	};

	/**
	 * Sets the fault description for the diagnostic object
	 */
	void SetDesc( const string &newDesc)
	{
		PlcDiagItem_t::SetDesc( newDesc);
	};

	/**
	 * Set the current status of this diagnostic object
	 *
	 * @return true if fault is currently active, false otherwise
	 */
	void SetStatus( bool newStatus)
	{
		PlcDiagItem_t::SetStatus( newStatus);
	};

	/**
	 * Sets the name of the process managing this fault item
	 *
	 * @param newName The name of the process managing this fault item
	 */
	void SetProcessName(const std::string& newName)
	{
		PlcDiagItem_t::SetProcessName( newName);
	};

	/**
	 * Reads diagnostic configuration from an XML node.
	 *
	 * @param diagNode Node containing a single diagnostic item description to be read
	 * @param dataTag  Data tag (from DataMap) to associate with this diag item
	 * @return PlcDiagItem populated from the given XML node
	 */
	static PlcDiagItem ReadXmlItem( const XmlNode *diagNode, XmlString& dataTag)
	{
		PlcDiagItem	retVal;

		const XmlNodeMap &attribs = diagNode->getAttributes();

		retVal.SetFaultTag( XmlToAscii( diagNode->getName()));
		retVal.ReadLimitsXml( attribs);
		retVal.ReadLevelXml( attribs);
		retVal.ReadDescXml( attribs);
		dataTag = diagNode->getAttributes().getNode( DIAG_CHANNEL_TAG)->getValue();
		return( retVal);
	};

protected:

	/**
	 * Reads the trigger limits for a diagnostic range from an XML node
	 *
	 * @param attribs Attributes which describe the diagnostic item being parsed
	 */
	void ReadLimitsXml( const XmlNodeMap& attribs)
	{
		UINT32	faultVal;

		// Read min trigger level
		faultVal = BposReadInt( attribs.getNode( XML_T("value"))->getValue().c_str());

		SetMin( faultVal);
		SetMax( faultVal);

	};

	/**
	 * Reads the fault level for a diagnostic range from an XML node
	 *
	 * @param attribs Attributes which describe the diagnostic item being parsed
	 */
	void ReadLevelXml( const XmlNodeMap& attribs)
	{
		XmlString level(DIAG_INF_TAG);

		try
		{
			level = attribs.getNode( DIAG_LEVEL_TAG)->getValue();
		}
		catch( XmlException &err)
		{
		}

		if( level == DIAG_INF_TAG)			SetLevel( DIAG_INF);
		else if( level == DIAG_WARN_TAG)	SetLevel( DIAG_WARN);
		else if( level == DIAG_FATAL_TAG)	SetLevel( DIAG_FATAL);
		else								SetLevel( DIAG_NONE);
	};

	/**
	 * Reads the fault tag for a diagnostic range from an XML node
	 *
	 * @param attribs Attributes which describe the diagnostic item being parsed
	 */
	void ReadDescXml( const XmlNodeMap& attribs)
	{
		string desc( "");
		try
		{
			desc = XmlToAscii(attribs.getNode( DIAG_DESC_TAG)->getValue());
		}
		catch( XmlException &err)
		{
		}
		SetDesc( desc);
	};
};


/**
 * Mapping of PLC Diagnostic channel ID to a diagnostic tag. The channel ID
 * is calculated based on channel index and bit index: channel*15+bit.
 */
typedef std::map< uint64_t, PlcDiagItem>	PlcDiagMap_t;
typedef PlcDiagMap_t::iterator				PlcDiagMapItr_t;
typedef PlcDiagMap_t::const_iterator		PlcDiagMapCItr_t;
typedef PlcDiagMap_t::value_type			PlcDiagMapVal_t;

#endif // _PLC_DIAG_H_INCLUDED_

