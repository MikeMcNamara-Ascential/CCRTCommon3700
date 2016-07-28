#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES
	BaseBrakeDriveCurve.xml
        FrontAxleBaseBrakeDriveCurve.xml
        RearAxleBaseBrakeDriveCurve.xml
	Isuzu.DriveCurve.xml
	My13Isuzu.DriveCurve.xml
	My14Isuzu.DriveCurve.xml
        My17Isuzu.DriveCurve.xml
        My17Isuzu.SingleAxleDriveCurve.xml
	Isuzu.LossCompensationDriveCurve.xml
	Isuzu.StaticBrakeDriveCurve.xml
)

