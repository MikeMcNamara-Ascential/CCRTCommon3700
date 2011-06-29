#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	BaseBrakeDriveCurve.xml
	BoschDriveCurve.xml
	LossCompensationDriveCurve.xml
	Mk60DriveCurve.xml
	)
