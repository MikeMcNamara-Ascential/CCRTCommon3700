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
        Tesla.Roadster2WD0BDriveCurve.xml
        Tesla.Roadster2WDMBDriveCurve.xml
        Tesla.Roadster4WD0BDriveCurve.xml
        Tesla.Roadster4WDMBDriveCurve.xml
	Tesla.Sedan2WD0BDriveCurve.xml
        Tesla.Sedan2WDMBDriveCurve.xml
        Tesla.Sedan4WD0BDriveCurve.xml
        Tesla.Sedan4WDMBDriveCurve.xml
	)