//*************************************************************************
// FILE:
//  Header: SoftwareId.h
//	Author: Chris Ward
//
// FILE DESCRIPTION:
//   This provides the methods to retrieve software version information.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/SoftwareId.h $
// 
// 1     5/12/04 9:54p Cward
// Initial version
//
//*************************************************************************
#ifndef SOFTWAREID_H
#define SOFTWAREID_H
//==============================================================================

#include <string>

//#include "CustomerId.h"

// setup default values if they are not alredy specified.
#ifndef IDCUSTNAM
#define IDCUSTNAM std::string("")
#endif

#ifndef IDCUSTBLD
#define IDCUSTBLD std::string("")
#endif


/**
 * Software ID class for providing the ability to identify Software versions
 * being used.
 *
 * @author Chris Ward
 */
class SoftwareId
{
public:
	/**
	 * SoftwareId class constructor.
	 *
	 * @param	custCode a string customer code/Id
	 * @param	custBld a string customer build Id
	 */
	SoftwareId(const std::string &custCode = IDCUSTNAM, const std::string &custBld = IDCUSTBLD);
	
	/**
	 * SoftwareId class destructor.
	 */
	virtual ~SoftwareId();

	/**
	 * Get Software ID.
	 * This method returns a string containing the
     * full software ID.
     *
     * @return  The software ID is in the form of:
     *          pppptYYMMDDRR-cccc-yymmddrr
     *          where:
     *          pppp = product ID
     *          t    = type of software (ie. R = real time, G = GUI, etc.)
     *          YY   = year (core build)
     *          MM   = month (core build)
     *          DD   = day (core build)
     *          RR   = run within day (core build)
     *          cccc = customer/plant identifier
     *          yy   = year (customer build)
     *          mm   = month (customer build)
     *          dd   = day (customer build)
     *          rr   = run within day (customer build)
	 */
	const std::string GetId(void);

	/**
	 * Get Partial Software ID.
	 * This method returns a string containing the
     * the requested part of the software ID.
     *
     *
     * @param req   The reqused type of data needed:
     *              "ProdID" returns pppp
     *              "Type"   returns t
     *              "CoreBuild" returns YYMMDDRR
     *              "CustID"    returns cccc
     *              "CustBuild" returns yymmddrr
     *              anything else retruns Null string
     * @return      string based on request parameter
	 */
	const std::string GetPartialId(const std::string &req);

protected:

private:

    /**
     * CustCode checks the input string to verify taht it is
     * in the proper form for a build code.
     *
     * @param val   The Customer code to be checked.
     *              It is expected to be of the following form:
     *              at least 3 charactes,and at most 8, preferrably 4
     *              all characters should be alphanumeric
     * @return      a string value customer code
     */
    const std::string CustCode(const std::string &val);

    /**
     * BuildCode checks the input string to verify that it is
     * in the proper form for a build code. If too short, it will default
	 * to the CORE build code, if too long it will be truncated to 8 characters.
     *
     * @param val   The build code to be checked.
     *              It is expected to be of the following form:
     *              yymmddrr
     *              where:
     *              yy   = year
     *              mm   = month
     *              dd   = day
     *              rr   = run within day
     * @return      a string value build code
     */
    const std::string BuildCode(const std::string &val);


	/**
     * Product ID.
     * an indicator of the product identifier
     */
    std::string m_prod;

    /**
     * Software Type indicator
     * An indicator of the software type. This
     * is used to differentiate different software
     * used on the same machine. As an example this
     * would be used to differentiate between the
     * realtime software and the GUI software.
     */
    std::string m_type;

	/**
	 * Core build ID.
     * ex. yymmddrr where:
     *     yy = year
     *     mm = month
     *     dd = day
     *     rr = run within day starting with 01
	 */
	std::string m_coreBuild;

	/**
	 * Customer ID.
     * a designator of Customer/Plant (usually 4 character string)
	 */
	std::string m_custId;

	/**
	 * Customer build ID.
     * ex. yymmddrr where:
     *     yy = year
     *     mm = month
     *     dd = day
     *     rr = run within day starting with 01
	 */
	std::string m_custBuild;

};

#endif // SOFTWAREID_H
