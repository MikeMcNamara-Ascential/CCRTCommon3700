#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	BaseBrakeDriveCurve.xml
	Isuzu.DriveCurve.xml
	My13Isuzu.DriveCurve.xml
	Isuzu.LossCompensationDriveCurve.xml
	Isuzu.StaticBrakeDriveCurve.xml
)

