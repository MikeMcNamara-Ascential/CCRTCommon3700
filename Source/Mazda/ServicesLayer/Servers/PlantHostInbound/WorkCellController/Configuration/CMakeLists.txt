#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
	Hyundai.VehicleBuildRecord.xml
	Hyundai.WccConfig.xml
	Hyundai.Broadcast.WccConfig.xml
	Hyundai.File.WccConfig.xml
	Hyundai.WheelbaseConfig.xml
	VehicleBuildTable.xml
        WccProtocol.xml)
