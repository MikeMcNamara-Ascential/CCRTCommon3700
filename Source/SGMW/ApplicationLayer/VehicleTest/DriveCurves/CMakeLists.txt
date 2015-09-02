#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	LossCompensationDriveCurve.xml
	BaseBrakeDriveCurve.xml
	Bosch9CanAbs15LDriveCurve.xml
	Bosch9CanAbs18LDriveCurve.xml
	Bosch9CanEsp15LDriveCurve.xml
	Bosch9CanEsp18LDriveCurve.xml
	Bosch9IsoAbsDriveCurve.xml
	DcuAbs18LDriveCurve.xml
	)
