#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
	Mazda.VehicleBuildRecord.xml
	Mazda.WccConfig.xml
	Mazda.Broadcast.WccConfig.xml
	Mazda.File.WccConfig.xml
	Mazda.WheelbaseConfig.xml
	VehicleBuildTable.xml
        WccProtocol.xml)
