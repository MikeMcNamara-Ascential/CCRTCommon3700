//******************************************************************************
// Description:
//	Defines the diagnostic item object.
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
//     $Log: /Core/ServicesLayer/Include/DiagItem.h $
// 
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 2     8/06/03 3:29p Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/26/03 1:28p Khykin
// Updated with Brians latest changes.
//
// 2     5/29/03 2:02p Bmeinke
// Added processNamer argument to constructor (default to "")
// Changed the logic for determining whether a diagnostic item's limits
// are inclusize or exclusive
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:36  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 8     10/15/02 10:55a Bmeinke
// Forgot to remove a printf...ooops
//
// 7     10/14/02 7:34p Bmeinke
// Added IFaultServer* data member: used to publish fault data
// Added  IFaultServer* argument to CheckTrigger() method
// Changed logic on fault inclusivity (was m_low > m_high...now m_low >=
// m_high)
// Added std::string m_processName data member (needed for fault
// publication)
//
// 6     8/01/02 4:12p Bmeinke
// Removed compiler warnings
//
// 5     11/06/01 12:29a Brian.meinke
// Changed CheckTrigger() to properly process exclusive fault
// Added IsInclusive() accessor method
// Added ShouldPublish() and ShouldClear virtual methods to allow derived
// classes to implement fault hysterisis
// Call ShouldPublish() from PublishFault()
// Call ShouldClear() from ClearFault()
//
// 4     11/05/01 11:49a Brian.meinke
// Added DIAG_CHANNEL_TAG define
// Added more implementation to CheckTrigger
// Allow for inclusive/exclusive trigger ranges
// If min trigger > max trigger, trigger range is exclusive
//
// 3     10/26/01 3:06p Brian.meinke
// Added m_status data member to maintain current "active" status for the
// diag object
// Added DiagItem::GetStatus()/DiagItem::SetStatus() methods to set/get
// current "active" status for the diag object
// Added virtual method CheckTrigger() which will also send fault
// information to the Fault Server (when completed)
// Added DiagItem::PublishFault() and DiagItem::ClearFault() methods to
// send fault information to the Fault Server (when completed)
//
// 2     10/25/01 2:09p Brian.meinke
// Added XML tag definitions
// Added copy constructor
//
// 1     10/24/01 11:36a Brian.meinke
// Initial check in
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _DIAG_ITEM_H_
#define _DIAG_ITEM_H_

#include <string>

#include "XmlDefs.h"
#include "IFaultServer.h"

/**
 * XML node tag for parent diagnostic node
 */
#define DIAG_NODE_TAG		XML_T("Diag")

/**
 * XML node tag used to define which data tag a diagnostic data item
 * will be associated with
 */
#define	DIAG_CHANNEL_TAG	XML_T("channelTag")

/**
 * XML node tag for low trip point
 */
#define DIAG_MIN_TAG		XML_T("min")

/**
 * XML node tag for high trip point
 */
#define DIAG_MAX_TAG		XML_T("max")

/**
 * XML node tag for fault severity
 */
#define DIAG_LEVEL_TAG		XML_T("level")

/**
 * XML node tag for fault description
 */
#define DIAG_DESC_TAG		XML_T("desc")

/**
 * XML node value to indicate an informational diagnostic level
 */
#define DIAG_INF_TAG		XML_T( "Informational")
/**
 * XML node value to indicate an warning diagnostic level
 */
#define DIAG_WARN_TAG		XML_T( "Warning")
/**
 * XML node value to indicate an fatal diagnostic level
 */
#define DIAG_FATAL_TAG		XML_T( "Fatal")

/**
 * Diagnostic warning levels
 * 	DIAG_NONE=0,
 * 	DIAG_INF,
 * 	DIAG_WARN,
 * 	DIAG_FATAL=9
 */
typedef enum
{
	/**
	 * No diagnostic level
	 */
	DIAG_NONE=0,

	/**
	 * Diagnostic informational
	 */
	DIAG_INF,

	/**
	 * Diagnostic warning
	 */
	DIAG_WARN,

	/**
	 * Fatal diagnostic warning
	 */
	DIAG_FATAL=9
} DiagLevel_t;


/**
 * Diagnostic item. Describes the properties of a generic diagnostic item.
 *
 * @author Brian Meinke
 * @version 1.0
 */
template< class valType>
class DiagItem
{
public:
	DiagItem( valType min, valType max, const string &faultTag,
			  const string &faultDesc, const string &processName="", DiagLevel_t level=DIAG_WARN) :
		m_faultServer( NULL), m_faultTag( faultTag), m_low( min), m_high( max),
		m_level( level), m_faultDesc( faultDesc), m_status( false), m_inclusive( true),
		m_processName(processName)
	{
		if( m_low > m_high)	m_inclusive = false;
		else				m_inclusive = true;
	};

	DiagItem( const DiagItem& copy) : m_faultServer( copy.m_faultServer),
		m_faultTag( copy.m_faultTag), m_low( copy.m_low), m_high( copy.m_high),
		m_level( copy.m_level), m_faultDesc( copy.m_faultDesc),
		m_status( copy.m_status),  m_inclusive( copy.m_inclusive),
		m_processName(copy.m_processName)
	{
	};

	virtual ~DiagItem()	{};

	/**
	 * Returns the warning level for the diagnostic object
	 *
	 * @return The warning level for the diagnostic object
	 * @see DiagLevel_t
	 */
	DiagLevel_t GetLevel() const				{ return( m_level);	};

	/**
	 * Returns the low trip limit for the diagnostic object
	 *
	 * @return The low trip limit for the diagnostic object
	 */
	valType GetMin() const						{ return( m_low);	};

	/**
	 * Returns the high trip limit for the diagnostic object
	 *
	 * @return The high trip limit for the diagnostic object
	 */
	valType GetMax() const						{ return( m_high);	};

	/**
	 * Returns the fault tag for the diagnostic object
	 *
	 * @return The fault tag for the diagnostic object
	 */
	const string& GetFaultTag() const			{ return( m_faultTag);	};

	/**
	 * Sets the fault description for the diagnostic object
	 */
	const string& GetDesc() const				{ return( m_faultDesc);	};

	/**
	 * Check the current status of this diagnostic object
	 *
	 * @return true if fault is currently active, false otherwise
	 */
	bool GetStatus() const						{ return( m_status);	};

	/**
	 * Returns the name of the process managing this fault item
	 *
	 * @return The name of the process managing this fault item
	 */
	const std::string& GetProcessName() const	{	return( m_processName); };

	/**
	 * Checks a value for diagnostic level
	 *
	 * @param value     Value to check
	 * @param faultSrvr Pointer fault server interface object. Used to publish fault data
	 *                  to the fault server and propogate throughout the system
	 * @return Diagnostic level for the given value
	 * @see DiagLevel_t
	 */
	virtual DiagLevel_t CheckTrigger( const valType &value, IFaultServer *faultSrvr=NULL)
	{
		DiagLevel_t	retVal = DIAG_NONE;

		// Check for valid fault server interface object
		if( faultSrvr != NULL)	m_faultServer = faultSrvr;

		// If inclusive (good if between min/max) and outside min/max
		if( ((m_inclusive) && ((value <= m_low) || (value >= m_high)) ) ||
		// or exclusive (good if outside min/max) and between min/max
			((!m_inclusive) && ((value >= m_high) && (value <= m_low))) )
		{
			// If this diag level is higher than the last
			if( m_level > retVal)
			{
				// Return the hightest diag warning level
				retVal = m_level;
				PublishFault( value);
			}
			retVal = m_level;
		}
		else
		{
			// Clear fault status
			ClearFault( value);
		}

		return( retVal);
	};

	/**
	 * Returns the "inclusive" property for this diagnostic item. A diag item is
	 * "inclusive" if the item is triggered when low <= value <= max. Exclusive
	 * is defined as: trigger if val <= low or val >= max.
	 *
	 * @return The "inclusive" property for this diagnostic item
	 */
	bool IsInclusive() { return( m_inclusive);	};

protected:

	/**
	 * Method to give derived classes more control over when a fault should be
	 * published once it is tripped
	 *
	 * @return true to publish fault, false to ignore
	 */
	virtual bool ShouldPublish()					{ return( true);	};

	/**
	 * Method to give derived classes more control over when a fault should be
	 * cleared once it is no longer tripped
	 *
	 * @return true to clear fault, false to leave active
	 */
	virtual bool ShouldClear()						{ return( true);	};

	/**
	 * Sets the warning level for the diagnostic object
	 *
	 * @see DiagLevel_t
	 */
	void SetLevel(	DiagLevel_t newLevel)			{ m_level = newLevel;	};

	/**
	 * Sets the low trip limit for the diagnostic object
	 */
	void SetMin( const valType& newLow)
	{
		m_low = newLow;
		if( m_low >= m_high)	m_inclusive = false;
		else					m_inclusive = true;
	};

	/**
	 * Sets the high trip limit for the diagnostic object
	 */
	void SetMax( const valType& newHigh)	{
		m_high = newHigh;
		if( m_low >= m_high)	m_inclusive = false;
		else					m_inclusive = true;
	};

	/**
	 * Returns the fault tag for the diagnostic object
	 *
	 * @return The fault tag for the diagnostic object
	 */
	void SetFaultTag( const string &newTag)			{ m_faultTag = newTag;	};

	/**
	 * Sets the fault description for the diagnostic object
	 */
	void SetDesc( const string &newDesc)			{ m_faultDesc = newDesc;	};

	/**
	 * Set the current status of this diagnostic object
	 *
	 * @return true if fault is currently active, false otherwise
	 */
	void SetStatus( bool newStatus)					{ m_status = newStatus;	};

	/**
	 * Sets the name of the process managing this fault item
	 *
	 * @param newName The name of the process managing this fault item
	 */
	void SetProcessName(const std::string& newName) { m_processName = newName; };

	/**
	 * Sends a fault to the fault server
	 *
	 * @param val    Current value causing the fault
	 * @return true if published, false otherwise
	 */
	virtual bool PublishFault( const valType& val)
	{
		std::string		response;
		bool 			retVal = false;

		// If currently not active
		if( GetStatus() == false)
		{
			if( ShouldPublish())
			{
				// Activate
				SetStatus( true);
				if( m_faultServer != NULL)
				{
					printf( "Fault '%s': '%s', severity %d sent to fault server from process '%s'\n",
							m_faultTag.c_str(), m_faultDesc.c_str(), m_level, m_processName.c_str());

					m_faultServer->SetFault( m_faultTag,
											 m_faultDesc,
											 m_processName,
											 response,
											 true,
											 m_level);
				}
				retVal = true;
			}
		}
		return( retVal);
	};

	/**
	 * Clears a fault
	 *
	 * @param val    Current value causing the fault
	 * @return true if cleared, false otherwise
	 */
	virtual bool ClearFault( const valType& val)
	{
		std::string		response;
		bool retVal = false;
		// IF currently active
		if( GetStatus() == true)
		{
			if( ShouldClear())
			{
				// Set to not-active
				SetStatus( false);

				if( m_faultServer != NULL)
				{
					m_faultServer->ClearFault(m_faultTag,
											 response,
											 true);
					printf( "Clearing Fault '%s'\n", m_faultTag.c_str());
				}

				retVal = true;
			}
		}
		return( retVal);
	};

protected:

	/**
	 * Fault server interface used to publich and clear faults in the
	 * system
	 */
	IFaultServer	*m_faultServer;


private:

	/**
	 * Fault tag for the diagnostic object
	 */
	string			m_faultTag;

	/**
	 * Low trip limit for the diagnostic object
	 */
	valType			m_low;

	/**
	 * High trip limit for the diagnostic object
	 */
	valType			m_high;

	/**
	 * warning level for the diagnostic object
	 *
	 * @see DiagLevel_t
	 */
	DiagLevel_t		m_level;

	/**
	 * Fault descriptino for the diagnostic object
	 */
	string			m_faultDesc;

	/**
	 * Flag to indicate if the fault is already active
	 */
	bool			m_status;

	/**
	 * Flag used to indicate if a good value (i.e. non-faul value)
	 * is represented by min < value < max (inclusive) or if a good
	 * value is represented by min > value < max (exclusive)
	 */
	bool			m_inclusive;

	/**
	 * Name of the process managing this fault item
	 */
	std::string		m_processName;
};


#endif 	// _DIAG_ITEM_H_
