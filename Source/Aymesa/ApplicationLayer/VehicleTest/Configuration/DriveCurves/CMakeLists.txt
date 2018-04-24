#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	LossCompensationDriveCurve.xml
	BaseBrakeDriveCurve.xml
	BoschAbsDriveCurve.xml
	MandoAbsDriveCurve.xml
	MandoMgh60KwpAbsDriveCurve.xml
	MandoMgh80AbsDriveCurve.xml  
	)
