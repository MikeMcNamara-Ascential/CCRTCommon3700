#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	LossCompensationDriveCurve.xml
	BaseBrakeDriveCurve.xml
	BaseBrakeDriveCurveSPDL.xml
	BaseBrake4wdDriveCurve.xml
	BaseBIABS4wdDriveCurve.xml
	BaseBIABSDriveCurve.xml
	BaseBIABSDriveCurveSPDL.xml
	)
