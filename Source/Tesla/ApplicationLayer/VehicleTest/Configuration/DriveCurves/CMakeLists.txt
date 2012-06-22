#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	Tesla.LossCompensationDriveCurve.xml
	Tesla.RoadsterDriveCurve.xml
	Tesla.SedanDriveCurve.xml
	Tesla.BaseBrakeDriveCurve.xml
	)