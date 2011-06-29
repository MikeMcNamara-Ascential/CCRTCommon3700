//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/DataTagMap.cpp $
// $Author: Cward $
//   $Date: 1/16/04 4:51p $
//
// Description:
//	 Data type definitions used to map a tag name to an actual system data tag
//
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
//     $Log: /Core/ServicesLayer/Library/DataTagMap.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************

#include "DataTagMap.h"

DataTagMap::DataTagMap() : DataTagMap_t()
{
}

DataTagMap::~DataTagMap()
{
}

bool DataTagMap::AddName( const TagName_t &name, const DataTag_t &tag)
{
    bool				retVal = false;
    DataTagMapItr_t		itr;
    BepMutex			listLock( m_tagLock);

    if( listLock.Acquire() == EOK)
    {
        // If name already exists, replace with new value
        if( (itr=find( name)) != end())
        {
            itr->second = tag;
        }
        else
        {
            insert( DataTagMapVal_t( name, tag));
        }

        retVal = true;
    }
    else
    {
        printf( "\tUnable to lock tag list\n");
    }

    return( retVal);
}

bool DataTagMap::AddName( const char *name, const char *tag)
{
    std::string tagName( name), dataTag( tag);
    return( AddName( tagName, dataTag));
}

void DataTagMap::Clear()
{
    BepMutex	listLock( m_tagLock);

    if( listLock.Acquire() == EOK)
    {
        clear();
    }
}
