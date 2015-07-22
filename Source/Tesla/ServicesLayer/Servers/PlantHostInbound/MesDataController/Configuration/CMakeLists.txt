#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
	Tesla.VehicleBuildRecord.xml
	Tesla.WccConfig.xml
	Tesla.Tilburg.WccConfig.xml
	Tesla.BepTroy.508155.WccConfig.xml
	Tesla.WheelbaseConfig.xml
	VehicleBuildTable.xml
        WccProtocol.xml)
