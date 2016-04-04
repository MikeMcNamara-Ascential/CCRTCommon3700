#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
        BaseBrakeDriveCurve11.xml	
	Nissan.Changzhou.BaseBrakeDriveCurve.xml
        BaseBrakeDriveCurve-soundlevel.xml
        BaseBrakeDriveCurve.xml	
	Bosch9AbsDriveCurve.xml
        LossCompensationDriveCurve.xml
	Nissan.Changzhou.508517.TevesMk70DriveCurve.xml
	Nissan.Changzhou.508517.TrwAbsDriveCurve.xml
	TevesMk70DriveCurve.xml
	TrwAbsDriveCurve.xml
	)
