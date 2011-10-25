#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	LossCompensationDriveCurve.xml
	BaseBrakeDriveCurve.xml
	Bosch9AbsDriveCurve.xml
	Changan.Beijing.507612.TevesMk70AbsDriveCurve.xml
	Changan.Beijing.507612.TrwAbsDriveCurve.xml
	TevesMk70AbsDriveCurve.xml
	TrwAbsDriveCurve.xml
	)
