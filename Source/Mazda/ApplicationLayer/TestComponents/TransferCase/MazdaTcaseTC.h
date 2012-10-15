//****************************************************************************
// DESCRIPTION:
//    Test specific to the Mazda AWD.
//
//============================================================================
//
//    Copyright (c) 2010- Burke E. Porter Machinery Company
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//*************************************************************************
#ifndef MazdaTcaseTC_h
#define MazdaTcaseTC_h
//-------------------------------------------------------------------------------------------------
#include "GenericTcaseTC.h"
//-------------------------------------------------------------------------------------------------
class MazdaTcaseTC : public GenericTcaseTC
{
public:
    MazdaTcaseTC();
    virtual ~MazdaTcaseTC();

protected:
    /**
     * Validate the axle ratio.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * The front to rear axle ratio will be verified to ensure the axles are within limits.  If an incorrect axle is
     * installed in the vehicle, this test will detect that situation and fail the vehicle.  This test is best performed
     * when the t-case is in 4-Lo, 4-Hi or 4-Lock, or any mode where the front to rear ratio should be 1:1.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TcaseTool::CheckAxleRatio() </li>
     *      </ul>
     *
     * @return Result of validating the front to rear axle ratio is within tolerance.
     */
    virtual string CheckAxleRatio(void);
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaTcaseTC_h
