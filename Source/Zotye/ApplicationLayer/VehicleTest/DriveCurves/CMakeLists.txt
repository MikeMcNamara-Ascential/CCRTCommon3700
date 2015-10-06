#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	LossCompensationDriveCurve.xml
	BaseBrakeDriveCurve.xml
	Bosch9AbsDriveCurve.xml
	Bosch9EspDriveCurve.xml
	BwiDbcAbsDriveCurve.xml
	)
