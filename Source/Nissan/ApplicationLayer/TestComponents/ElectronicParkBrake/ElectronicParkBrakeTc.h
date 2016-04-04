//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Electronic Park Brake.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#ifndef ElectronicParkBrakeTc_h
#define ElectronicParkBrakeTc_h
//-----------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"

//-----------------------------------------------------------------------------
template <class ModuleType>
class ElectronicParkBrakeTc : public GenericTCTemplate<ModuleType>
{
public:
    ElectronicParkBrakeTc();
    virtual ~ElectronicParkBrakeTc();

protected:

    /**
     * Perform the ParkBrake test.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The wheel speed sensors will be compared to the roller speeds to ensure each sensor is within the allowed
     * tolerance.  Each roller will be rotated individually and each wheel checked to make sure the module knows the
     * correct wheel is being rotated.
     * <br><p>
     *
     * @return The result of validating the wheel speed sensors.
     *      <ul>
     *      <li> Pass - All wheel speed sensors are within tolerance. </li>
     *      <li> Fail - One or more wheel speed sensors are out of tolerance or wheel speeds could not be gathered
     *                  from the vehicle or system. </li>
     *      <li> Timeout - Target speed was not achieved in the time allowed. </li>
     *      <li> Skip - The wheel speed sensors do not need to be validated. </li>
     *      <li> SoftwareFail - A module exception occurred reading the wheel speed sensors. </li>
     *      </ul>
     */
    virtual string ParkBrakeTest(void);



private:

    virtual string ReadBrakeState(const string &requestedBrakeState, const int &brakeReadTime);

};
//-------------------------------------------------------------------------------------------------
#endif //ElectronicParkBrakeTc_h
