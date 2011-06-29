#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	RollTestDriveCurve.xml
	CamryBrakeTestDriveCurve.xml
	VenzaBrakeTestDriveCurve.xml
	MaintenanceDriveCurve.xml
	PreShiftCheckDriveCurve.xml
)

