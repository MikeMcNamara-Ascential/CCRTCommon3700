#------------------------------------------------------------------------------------------
# Tell CMake where to install the config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
    Mazda.Braketest.RemoteVehicleBuildServer.xml
    Mazda.Rolltest.RemoteVehicleBuildServer.xml)
