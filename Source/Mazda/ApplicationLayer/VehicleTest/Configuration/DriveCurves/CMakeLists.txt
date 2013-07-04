#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
        RollTestAutoDriveCurve.xml
        RollTestManualDriveCurve.xml
        BrakeTestDriveCurve.xml
        MaintenanceDriveCurve.xml
        PreShiftCheckDriveCurve.xml
)

