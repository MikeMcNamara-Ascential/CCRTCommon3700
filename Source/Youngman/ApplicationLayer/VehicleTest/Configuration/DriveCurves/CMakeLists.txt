#------------------------------------------------------------------------------------------
# Tell CMake what files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
    BaseBrakeDriveCurve.xml
    BaseBrake4wdDriveCurve.xml
    BoschAbs4wdDriveCurve.xml
    LossCompensationDriveCurve.xml
    BoschAbsDriveCurve.xml)
