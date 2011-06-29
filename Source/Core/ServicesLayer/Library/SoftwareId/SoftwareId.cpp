//*************************************************************************
// Description:
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
//    $Log: /Core/ServicesLayer/Library/SoftwareId/SoftwareId.cpp $
// 
// 2     6/24/04 1:01a Cward
// Removed pound if 0 block. UPdated the header block comments to the new
// standard.
//
// 1     5/12/04 9:55p Cward
// Initial version
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================

#include <ctype.h>
#include <stdlib.h>	// for atoi()

#include "SoftwareId.h"
#include "CoreId.h"


#define PRODID_STR  "ProdID"
#define TYPE_STR    "Type"
#define COREBLD_STR "CoreBuild"
#define CUSTID_STR  "CustID"
#define CUSTBLD_STR "CustBuild"

//using namespace std;


SoftwareId::SoftwareId(const std::string &custCode, const std::string &custBld)
	: m_prod(IDPROD),
	m_type(IDTYPE),
	m_coreBuild(IDCOREBLD),
	m_custId(CustCode(custCode)),
	m_custBuild(BuildCode(custBld))
{
	
}

SoftwareId::~SoftwareId()
{
}

const std::string SoftwareId::GetId(void)
{
    std::string Id;

    Id = m_prod + m_type + m_coreBuild + "-" + m_custId + "-" + m_custBuild;

    return( Id );
}

const std::string SoftwareId::GetPartialId(const std::string &req)
{
    if( req == PRODID_STR )
    {
        return(m_prod);
    }
    else if(req == TYPE_STR)
    {
        return(m_type);
    }
    else if(req == COREBLD_STR)
    {
        return(m_coreBuild);
    }
    else if(req == CUSTID_STR)
    {
        return(m_custId);
    }
    else if(req == CUSTBLD_STR)
    {
        return(m_custBuild);
    }
    else
    {
        return("");
    }
}


const std::string SoftwareId::CustCode(const std::string &val)
{
	std::string retval;
	if(val.size() < 3 )
	{
		retval = "CORE";
	}
	else if(val.size() > 8)
	{
		retval = val.substr(0, 8);
	}
	else // val is between 3 and 8 characters
	{
		retval = val;
	}

	char chr;
	for (unsigned int idx = 0; idx < retval.size(); idx++ )
	{
		chr = retval[idx];
		if(isalnum(chr))
		{
			chr = toupper(chr);
		}
		else
		{
			chr = '_';
		}
		retval[idx] = chr;
	}

    return(retval);
}


const std::string SoftwareId::BuildCode(const std::string &val)
{
	std::string retval;

	if(val.size() < 8)  // especially size 0
	{
		retval = IDCOREBLD;
	}
	else if(val.size() == 8)	// make sure it's not too large
	{
		retval = val;
	}
	else
	{
		retval = val.substr(0, 8);
	}

	char chr;
	for (unsigned int idx = 0; idx < retval.size(); idx++ )
	{
		chr = retval[idx];
		if(isalnum(chr))
		{
			chr = toupper(chr);
		}
		else
		{
			chr = '_';
		}
		retval[idx] = chr;
	}


    return(retval);
}

