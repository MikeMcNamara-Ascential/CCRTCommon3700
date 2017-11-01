#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	Tesla.LossCompensationDriveCurve.xml
	Tesla.RoadsterDriveCurve.xml
	Tesla.SedanDriveCurve.xml
	Tesla.SedanNoAirbagDriveCurve.xml
	Tesla.SedanNoAirbagDualMotorDriveCurve.xml
	Tesla.BaseBrakeDriveCurve.xml
        Tesla.ModelXDriveCurve.xml
	Tesla.ModelSRWDDriveCurve.xml
        Tesla.ModelSDriveCurve.xml
	)