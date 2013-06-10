#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
        RollTestAutoDriveCurve.xml
        RollTestManualDriveCurve.xml
        BaseBrakeTestDriveCurve.xml
        MaintenanceDriveCurve.xml
        PreShiftCheckDriveCurve.xml
)

